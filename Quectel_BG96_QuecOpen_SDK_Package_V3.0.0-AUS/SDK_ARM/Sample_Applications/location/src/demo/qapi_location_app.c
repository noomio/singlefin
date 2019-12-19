/*****************************************************************************
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#include "txm_module.h"
#include "qapi_location.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qapi_fs.h"
#include "qapi_timer.h"
#include "qflog_utils.h"

/*==========================================================================
Structures for various tests
===========================================================================*/
/* Tracking */
typedef struct {
    uint32_t minInterval;
    uint32_t minDistance;
    uint32_t noFixes;
    uint32_t timeoutSeconds;
} Tracking_t;

/* Batching */
typedef struct {
    uint32_t minInterval;
    uint32_t minDistance;
    uint32_t noBatches;
    uint32_t timeoutSeconds;
} Batching_t;

/* Geofence */
typedef struct {
    qapi_Geofence_Info_t gfInfo;
    qapi_Geofence_Option_t gfOptions;
    uint32_t noBreaches;
    uint32_t timeoutSeconds;
} Geofence_t;

/* Singleshot */
typedef struct {
    qapi_Location_Power_Level_t power_level;
} Singleshot_t;

/* GnssData */
typedef struct {
    uint32_t noFixes;
    uint32_t timeoutSeconds;
} GnssData_t;

Tracking_t gTracking;
Batching_t gBatching;
Geofence_t gGeofence;
Singleshot_t gSingleshot;
GnssData_t gGnssData;

/*==========================================================================
Signals used for waiting in test app for callbacks
===========================================================================*/
static TX_EVENT_FLAGS_GROUP *gSignalInit;
static TX_EVENT_FLAGS_GROUP *gSignalGeofenceResponse;
static TX_EVENT_FLAGS_GROUP *gSignalResponse;
static TX_EVENT_FLAGS_GROUP *gSignalLocation;
static TX_EVENT_FLAGS_GROUP *gSignalGeofenceBreach;

/*==========================================================================
Local state variables
===========================================================================*/
static UINT gGpsVerifyTestInProgress = 0;

#define USER_BUFFER_SIZE 4096

static uint32_t clientId;
static qapi_Location_Callbacks_t sLocationCallbacks;

//#define QAPI_BUGS_FIXED

#ifdef QAPI_BUGS_FIXED
static uint8_t userBuffer[USER_BUFFER_SIZE];
#else
__declspec(align(4)) static uint8_t userBuffer[USER_BUFFER_SIZE];
uint32_t geofence_id;
#endif

// Just for printing
#define MAX_FRAC_DIGITS 10000000

#define LINE_SIZE 128

// LOGGING MACROS
#define LOC_LOG_QFLOG(fmt, ...) QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, fmt "\n", ##__VA_ARGS__)

/* Function to initialize all the parameters */
static void parameters_init();

/* Function to get a line from the config file */
static int fgetline(char* s, int fd);

/* Functions to parse each command once it's identified */
static void parse_tracking(char* str);
static void parse_batching(char* str);
static void parse_geofence(char* str);
static void parse_singleshot(char* str);
static void parse_data(char* str);

/* Functions to run each command */
static void process_tracking();
static void process_batching();
static void process_geofence();
static void process_singleshot();
static void process_data();

/*---------------------------------------------
  Wrappers around QAPI functions
 ---------------------------------------------*/
/* Init/Deinit */
static int location_init();
static int location_deinit();

/* Tracking */
static uint32_t location_start_tracking(uint32_t minInterval, uint32_t minDistance);
static int location_update_tracking_options(uint32_t id, uint32_t minInterval, uint32_t minDistance);
static int location_stop_tracking(uint32_t id);

/* Batching */
static uint32_t location_start_batching(uint32_t minInterval, uint32_t minDistance);
static int location_update_batching_options(uint32_t id, uint32_t minInterval, uint32_t minDistance);
static int location_stop_batching(uint32_t id);
static int location_get_batched_locations(uint32_t id, size_t count);

/* Geofence */
static uint32_t* location_add_geofences(size_t count,
    qapi_Geofence_Option_t* options,
    qapi_Geofence_Info_t* datas);
static int location_modify_geofences(
    size_t count, uint32_t* ids, qapi_Geofence_Option_t* options);
static int location_pause_geofences(size_t count, uint32_t* ids);
static int location_resume_geofences(size_t count, uint32_t* ids);
static int location_remove_geofences(size_t count, uint32_t* ids);

/* Single-shot */
static uint32_t location_get_single_shot(uint32_t powerLevel);
static int location_cancel_single_shot(uint32_t id);

/* GNSS Data */
static uint32_t location_start_get_gnss_data();
static int location_stop_get_gnss_data(uint32_t id);

/* QAPI callback functions */
static void location_capabilities_callback(
    qapi_Location_Capabilities_Mask_t capabilitiesMask);
static void location_response_callback(
    qapi_Location_Error_t err, uint32_t id);
static void location_collective_response_callback(size_t count,
    qapi_Location_Error_t* err,
    uint32_t* ids);

static void location_tracking_callback(qapi_Location_t location);
static void location_batching_callback(
    size_t count, qapi_Location_t* pLocation);
static void location_geofence_breach_callback(
    qapi_Geofence_Breach_Notification_t notification);
static void location_single_shot_callback(qapi_Location_t location,
    qapi_Location_Error_t err);
static void location_gnss_data_callback(qapi_Gnss_Data_t gnssData);

/* Timer start/stop functions */
static qapi_TIMER_handle_t start_app_timer(uint32_t id,
    uint32_t timeoutSeconds, void* sigs_func_ptr);
static void stop_app_timer(qapi_TIMER_handle_t timer_handle);

/* Timer callback functions */
static void timer_init_cb(uint32_t id);
static void timer_tracking_cb(uint32_t id);
static void timer_batching_cb(uint32_t id);
static void timer_geofence_cb(uint32_t id);
static void timer_gnssdata_cb(uint32_t id);

/* Utility functions */
static void log_location(qapi_Location_t location);
static double qapi_loc_util_string_to_double(char* str);

/*
This application demonstrates the use of QAPI Location API.
The use of the following APIs is illustrated:
qapi_Loc_Init

The application will parse a configuration file called loc_app_data
and will execute the commands with the associated parameters from this file.
The possible commands are:
T <minInterval> <minDistance> <numberOfFixes> <timeout>
B <minInterval> <minDistance> <numberOfBatches> <timeout>
G <breachTypeMask> <responsiveness> <dwellTime> <latitude> <longitude> <radius> <numberOfBreaches> <timeout>
S <powerLevel>
D <numberOfFixes> <timeout>
The configuration file can have any number of these commands in any order.
Any line that starts with the character # in the configuration file is ignored.
When the application starts it will invoke the following APIs:
qapi_Loc_Init
qapi_Loc_Set_User_Buffer

When the application ends it will invoke the following API:
qapi_Loc_Deinit

Here is a detailed description of each command from the configuration file:
T <minInterval> <minDistance> <numberOfFixes> <timeout>
This command illustrates tracking. It invokes the following APIs:
qapi_Loc_Start_Tracking
qapi_Loc_Stop_Tracking

*/

/* module entry point */
void location_app()
{
    int fd = -1;
    char str[LINE_SIZE];
    uint32 offset;
    int retvalue;
    qapi_FS_Offset_t seek_status = 0;
    ULONG set_signal = 0;
    qapi_TIMER_handle_t timer_handle;

    txm_module_object_allocate(&gSignalInit, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(gSignalInit, "gSignalInit");

    LOC_LOG_QFLOG("Location Demo Application!");

    /* Wait here for 10 seconds before starting
       Since the app starts at bootup time we allow
       some time here for the user to start the logging */
    LOC_LOG_QFLOG("10 seconds delay");
    timer_handle = start_app_timer(0, 10, &timer_init_cb);

    LOC_LOG_QFLOG("Waiting for Init (10 seconds)...");
    tx_event_flags_get(gSignalInit,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    stop_app_timer(timer_handle);

    /* Initialize all the parameters here.
       In case the config file cannot be read these 
       values will be used */
    parameters_init();
    if (location_init() == 0) {
        LOC_LOG_QFLOG("Location Init Success");
    }
    else {
        LOC_LOG_QFLOG("Location Init Failure !");
        return;
    }

    qapi_FS_Open("/datatx/loc_app_data", QAPI_FS_O_RDONLY_E, &fd);
    if (fd >= 0) {
        /* In the following loop we parse the config file line by line
        fgetline function returns the next line to be parsed in the file
        Based on the first character in the line we call the appropriate
        function for further parsing */
        offset = 0;
        do {
            qapi_FS_Seek(fd, offset, QAPI_FS_SEEK_SET_E, &seek_status);
            retvalue = fgetline(str, fd);
            LOC_LOG_QFLOG("string=%s", str);
            if (retvalue > 0) {
                str[retvalue - 1] = '\0';
                switch (str[0]) {
                case 'T':
                case 't':
                    parse_tracking(str);
                    process_tracking();
                    break;

                case 'B':
                case 'b':
                    parse_batching(str);
                    process_batching();
                    break;

                case 'G':
                case 'g':
                    parse_geofence(str);
                    process_geofence();
                    break;

                case 'S':
                case 's':
                    parse_singleshot(str);
                    process_singleshot();
                    break;

                case 'D':
                case 'd':
                    parse_data(str);
                    process_data();
                    break;

                case '#':
                default:
                    break;
                }
                offset += (retvalue + 1);
            }
        } while (retvalue > 0);
        qapi_FS_Close(fd);
    }
    else {
        LOC_LOG_QFLOG("Error in opening loc_app_data!");
        /* We couldn't read the config file 
           We run the default test cases with default parameters */
        process_tracking();
        process_batching();
        process_geofence();
        process_singleshot();
        process_data();
    }

    if (location_deinit() == 0) {
        LOC_LOG_QFLOG("Location Deinit Success");
    }
    else {
        LOC_LOG_QFLOG("Location Deinit Failure !");
    }
    tx_event_flags_delete(gSignalInit);
}

/* Function to initialize all the parameters */
static void parameters_init()
{
    /* Tracking */
    gTracking.minInterval = 1000;
    gTracking.minDistance = 0;
    gTracking.noFixes = 20;
    gTracking.timeoutSeconds = 240;

    /* Batching */
    gBatching.minInterval = 1000;
    gBatching.minDistance = 0;
    gBatching.noBatches = 2;
    gBatching.timeoutSeconds = 240;

    /* Geofence */
    gGeofence.gfInfo.size = sizeof(gGeofence.gfInfo);
    gGeofence.gfInfo.latitude = 0.0;
    gGeofence.gfInfo.longitude = 0.0;
    gGeofence.gfInfo.radius = 100.0;
    gGeofence.gfOptions.size = sizeof(gGeofence.gfOptions);
    gGeofence.gfOptions.breachTypeMask = QAPI_GEOFENCE_BREACH_ENTER_BIT;
    gGeofence.gfOptions.responsiveness = 3;
    gGeofence.gfOptions.dwellTime = 5;
    gGeofence.noBreaches = 1;
    gGeofence.timeoutSeconds = 240;

    /* Singleshot */
     gSingleshot.power_level = QAPI_LOCATION_POWER_HIGH;

    /* GnssData */
    gGnssData.noFixes = 20;
    gGnssData.timeoutSeconds = 240;
}

/* Function to get a line from the config file */
static int fgetline(char* s, int fd)
{
    char file_contents[LINE_SIZE] = "";
    int ch;
    int  read_length = 0;
    int i;

    qapi_FS_Read(fd, file_contents, LINE_SIZE, &read_length);

    if (read_length != 0) {
        for (i = 0; i < read_length; i++) {
            ch = *(file_contents + i);
            *(s + i) = ch;
            if ('\n' == ch || EOF == ch) {
                *(s + i) = '\0';
                return i;
            }
        }
        *(s + i) = '\0';
        return i;
    }
    return -1;
}

/* Functions to parse each command once it's identified */
static void parse_tracking(char* str)
{
    char* pch;
    char* saveptr;
    int i;

    pch = strtok_r(str + 1, " ", &saveptr);
    i = 0;
    while (NULL != pch) {
        switch (i) {
        case 0:
            gTracking.minInterval = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 1:
            gTracking.minDistance = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 2:
            gTracking.noFixes = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 3:
            gTracking.timeoutSeconds = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        default:
            pch = NULL;
            break;
        }
        pch = strtok_r(NULL, " ", &saveptr);
        i++;
    }
}

static void parse_batching(char* str)
{
    char* pch;
    char* saveptr;
    int i;

    pch = strtok_r(str + 1, " ", &saveptr);
    i = 0;

    while (NULL != pch) {
        switch (i) {
        case 0:
            gBatching.minInterval = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 1:
            gBatching.minDistance = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 2:
            gBatching.noBatches = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 3:
            gBatching.timeoutSeconds = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        default:
            pch = NULL;
            break;
        }
        pch = strtok_r(NULL, " ", &saveptr);
        i++;
    }
}

static void parse_geofence(char* str)
{
    char* pch;
    char* saveptr;
    int i;

    pch = strtok_r(str + 1, " ", &saveptr);
    i = 0;

    while (NULL != pch) {
        switch (i) {
        case 0:
            gGeofence.gfOptions.breachTypeMask = (uint8_t)qapi_loc_util_string_to_double(pch);
            break;

        case 1:
            gGeofence.gfOptions.responsiveness = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 2:
            gGeofence.gfOptions.dwellTime = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 3:
            gGeofence.gfInfo.latitude = qapi_loc_util_string_to_double(pch);
            break;

        case 4:
            gGeofence.gfInfo.longitude = qapi_loc_util_string_to_double(pch);
            break;

        case 5:
            gGeofence.gfInfo.radius = qapi_loc_util_string_to_double(pch);
            break;

        case 6:
            gGeofence.noBreaches = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 7:
            gGeofence.timeoutSeconds = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        default:
            pch = NULL;
            break;
        }
        pch = strtok_r(NULL, " ", &saveptr);
        i++;
    }
}

static void parse_singleshot(char* str)
{
    char* pch;
    char* saveptr;
    int i;

    pch = strtok_r(str + 1, " ", &saveptr);
    i = 0;

    while (NULL != pch) {
        switch (i) {
        case 0:
            gSingleshot.power_level = (qapi_Location_Power_Level_t)qapi_loc_util_string_to_double(pch);
            break;

        default:
            pch = NULL;
            break;
        }
        pch = strtok_r(NULL, " ", &saveptr);
        i++;
    }
}

static void parse_data(char* str)
{
    char* pch;
    char* saveptr;
    int i;

    pch = strtok_r(str + 1, " ", &saveptr);
    i = 0;

    while (NULL != pch) {
        switch (i) {
        case 0:
            gGnssData.noFixes = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        case 1:
            gGnssData.timeoutSeconds = (uint32_t)qapi_loc_util_string_to_double(pch);
            break;

        default:
            pch = NULL;
            break;
        }
        pch = strtok_r(NULL, " ", &saveptr);
        i++;
    }
}

/* Functions to run each command. The parameters
   are either set in parameters_init or from
   parsing the config file */
static void process_tracking()
{
    int i;
    uint32_t id;
    ULONG set_signal = 0;
    qapi_TIMER_handle_t timer_handle;

    LOC_LOG_QFLOG("minInterval = %u", gTracking.minInterval);
    LOC_LOG_QFLOG("minDistance = %u", gTracking.minDistance);
    LOC_LOG_QFLOG("noFixes = %u", gTracking.noFixes);
    LOC_LOG_QFLOG("timeoutSeconds = %u", gTracking.timeoutSeconds);

    gGpsVerifyTestInProgress = 1;

    id = location_start_tracking(gTracking.minInterval, gTracking.minDistance);

    if (gTracking.timeoutSeconds > 0) {
        timer_handle = start_app_timer(id, gTracking.timeoutSeconds, &timer_tracking_cb);
        if (id > 1000) {
            LOC_LOG_QFLOG("location_start_tracking() returned suspicious ID (%u), "
                "check logs for failure !", id);
        }
        else {
            LOC_LOG_QFLOG("Tracking started, ID = %u", id);
        }
    }

    for (i = 0; i < gTracking.noFixes; i++) {
        LOC_LOG_QFLOG("Waiting for Location...");
        tx_event_flags_get(gSignalLocation,
            1, TX_OR_CLEAR,
            &set_signal, TX_WAIT_FOREVER); 	// Wait for ever
        if (0 == gGpsVerifyTestInProgress) {
            break;
        }
    }
    if (gTracking.timeoutSeconds > 0) {
        stop_app_timer(timer_handle);
    }
    location_stop_tracking(id);
}

static void process_batching()
{
    int i;
    uint32_t id;
    ULONG set_signal = 0;
    qapi_TIMER_handle_t timer_handle;

    LOC_LOG_QFLOG("minInterval = %u", gBatching.minInterval);
    LOC_LOG_QFLOG("minDistance = %u", gBatching.minDistance);
    LOC_LOG_QFLOG("noBatches = %u", gBatching.noBatches);
    LOC_LOG_QFLOG("timeoutSeconds = %u", gBatching.timeoutSeconds);

    gGpsVerifyTestInProgress = 1;

    id = location_start_batching(gBatching.minInterval, gBatching.minDistance);
    if (gBatching.timeoutSeconds > 0) {
        timer_handle = start_app_timer(id, gBatching.timeoutSeconds, &timer_batching_cb);
        if (id > 1000) {
            LOC_LOG_QFLOG("location_start_batching() returned suspicious ID (%u), "
                "check logs for failure !", id);
        }
        else {
            LOC_LOG_QFLOG("Batching started, ID = %u", id);
        }
    }

    for (i = 0; i < gBatching.noBatches; i++) {
        LOC_LOG_QFLOG("Waiting for Location...");
        tx_event_flags_get(gSignalLocation,
            1, TX_OR_CLEAR,
            &set_signal, TX_WAIT_FOREVER); 	// Wait for ever
        if (0 == gGpsVerifyTestInProgress) {
            break;
        }
    }

    if (gBatching.timeoutSeconds > 0) {
        stop_app_timer(timer_handle);
    }

    location_stop_batching(id);
}

static void process_geofence()
{
    int i;
    uint32_t id;
    ULONG set_signal = 0;
    qapi_TIMER_handle_t timer_handle;
    uint32_t* idArray;

    LOC_LOG_QFLOG("breachTypeMask = 0x%X", gGeofence.gfOptions.breachTypeMask);
    LOC_LOG_QFLOG("responsiveness = %u", gGeofence.gfOptions.responsiveness);
    LOC_LOG_QFLOG("dwellTime = %u", gGeofence.gfOptions.dwellTime);
    LOC_LOG_QFLOG("latitude: %d.%d", (int)gGeofence.gfInfo.latitude, (abs((int)(gGeofence.gfInfo.latitude*MAX_FRAC_DIGITS))) % MAX_FRAC_DIGITS);
    LOC_LOG_QFLOG("longitude: %d.%d", (int)gGeofence.gfInfo.longitude, (abs((int)(gGeofence.gfInfo.longitude*MAX_FRAC_DIGITS))) % MAX_FRAC_DIGITS);
    LOC_LOG_QFLOG("radius: %d.%d", (int)gGeofence.gfInfo.radius, (abs((int)(gGeofence.gfInfo.radius*MAX_FRAC_DIGITS))) % MAX_FRAC_DIGITS);
    LOC_LOG_QFLOG("noBreaches = %u", gGeofence.noBreaches);
    LOC_LOG_QFLOG("timeoutSeconds = %u", gGeofence.timeoutSeconds);

    gGpsVerifyTestInProgress = 1;

    idArray = location_add_geofences(1, &gGeofence.gfOptions, &gGeofence.gfInfo);

#ifdef QAPI_BUGS_FIXED
    if (idArray == NULL) {
        LOC_LOG_QFLOG("NULL ID Array returned, Failure !");
        return;
    }

    id = *idArray;
    LOC_LOG_QFLOG("idArray = 0x%p id = %u", idArray, id);
#else
    id = geofence_id;
    LOC_LOG_QFLOG("id = %u", id);
#endif

    if (gGeofence.timeoutSeconds > 0) {
        timer_handle = start_app_timer(id, gGeofence.timeoutSeconds, &timer_geofence_cb);
    }

    for (i = 0; i < gGeofence.noBreaches; i++) {
        LOC_LOG_QFLOG("Waiting for Breach...");
        tx_event_flags_get(gSignalGeofenceBreach,
            1, TX_OR_CLEAR,
            &set_signal, TX_WAIT_FOREVER); 	// Wait for ever
        if (0 == gGpsVerifyTestInProgress) {
            break;
        }
    }

    if (gGeofence.timeoutSeconds > 0) {
        stop_app_timer(timer_handle);
    }

    location_remove_geofences(1, &id);
}

static void process_singleshot()
{
    int i;
    uint32_t id;
    ULONG set_signal = 0;

    LOC_LOG_QFLOG("power_level = %u", gSingleshot.power_level);

    gGpsVerifyTestInProgress = 1;
    id = location_get_single_shot(gSingleshot.power_level);

    LOC_LOG_QFLOG("Waiting for Location...");
    tx_event_flags_get(gSignalLocation,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    location_cancel_single_shot(id);
}

static void process_data()
{
    int i;
    uint32_t id;
    ULONG set_signal = 0;
    qapi_TIMER_handle_t timer_handle;

    LOC_LOG_QFLOG("noFixes = %u", gGnssData.noFixes);
    LOC_LOG_QFLOG("timeoutSeconds = %u", gGnssData.timeoutSeconds);

    gGpsVerifyTestInProgress = 1;
    id = location_start_get_gnss_data(0);

    if (gGnssData.timeoutSeconds > 0) {
        timer_handle = start_app_timer(id, gGnssData.timeoutSeconds, &timer_gnssdata_cb);
        if (id > 1000) {
            LOC_LOG_QFLOG("location_start_get_gnss_data() returned suspicious ID (%u), "
                "check logs for failure !", id);
        }
        else {
            LOC_LOG_QFLOG("Tracking started, ID = %u", id);
        }
    }

    for (i = 0; i < gGnssData.noFixes; i++) {
        LOC_LOG_QFLOG("Waiting for Location...");
        tx_event_flags_get(gSignalLocation,
            1, TX_OR_CLEAR,
            &set_signal, TX_WAIT_FOREVER); 	// Wait for ever
        if (0 == gGpsVerifyTestInProgress) {
            break;
        }
    }

    if (gGnssData.timeoutSeconds > 0) {
        stop_app_timer(timer_handle);
    }
    location_stop_get_gnss_data(id);
}

/*---------------------------------------------
Wrappers around QAPI functions
---------------------------------------------*/

/* Init/Deinit */
static int location_init()
{
    qapi_Location_Error_t ret;

    txm_module_object_allocate(&gSignalGeofenceResponse, sizeof(TX_EVENT_FLAGS_GROUP));
    txm_module_object_allocate(&gSignalResponse, sizeof(TX_EVENT_FLAGS_GROUP));
    txm_module_object_allocate(&gSignalLocation, sizeof(TX_EVENT_FLAGS_GROUP));
    txm_module_object_allocate(&gSignalGeofenceBreach, sizeof(TX_EVENT_FLAGS_GROUP));

    tx_event_flags_create(gSignalGeofenceResponse, "gSignalGeofenceResponse");
    tx_event_flags_create(gSignalResponse, "gSignalResponse");
    tx_event_flags_create(gSignalLocation, "gSignalLocation");
    tx_event_flags_create(gSignalGeofenceBreach, "gSignalGeofenceBreach");

    memset(&sLocationCallbacks, 0, sizeof(qapi_Location_Callbacks_t));
    sLocationCallbacks.size = sizeof(qapi_Location_Callbacks_t);

    sLocationCallbacks.capabilitiesCb = location_capabilities_callback;
    sLocationCallbacks.responseCb = location_response_callback;
    sLocationCallbacks.collectiveResponseCb = location_collective_response_callback;
    sLocationCallbacks.trackingCb = location_tracking_callback;
    sLocationCallbacks.batchingCb = location_batching_callback;
    sLocationCallbacks.geofenceBreachCb = location_geofence_breach_callback;
    sLocationCallbacks.singleShotCb = location_single_shot_callback;
    sLocationCallbacks.gnssDataCb = location_gnss_data_callback;

    LOC_LOG_QFLOG("Invoking qapi_Loc_Init()");

    ret = (qapi_Location_Error_t)qapi_Loc_Init(
            (UINT)&clientId,
            (UINT)&sLocationCallbacks);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("LOC INIT returned failure ! (ret %d)", ret);
        return -1;
    }

    LOC_LOG_QFLOG("Calling qapi_Loc_Set_User_Buffer uB=0x%p size=%u", userBuffer, USER_BUFFER_SIZE);
    ret = (qapi_Location_Error_t)qapi_Loc_Set_User_Buffer(
            (qapi_loc_client_id)clientId,
            (uint8_t*)userBuffer,
            (size_t)USER_BUFFER_SIZE);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Set user buffer failed ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success ! (client ID %d)", clientId);

    return 0;
}

static int location_deinit()
{
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("Invoking qapi_Loc_Deinit(), clientId = %d", clientId);
    ret = (qapi_Location_Error_t)qapi_Loc_Deinit(clientId);
    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    tx_event_flags_delete(gSignalGeofenceResponse);
    tx_event_flags_delete(gSignalResponse);
    tx_event_flags_delete(gSignalLocation);
    tx_event_flags_delete(gSignalGeofenceBreach);

    return 0;
}

/* Tracking */
static uint32_t location_start_tracking(uint32_t minInterval, uint32_t minDistance)
{
    uint32_t id = (uint32_t)-1;
    qapi_Location_Options_t locationOptions = { 0 };
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION START TRACKING");
    LOC_LOG_QFLOG("Interval %u, Distance %u", minInterval, minDistance);

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QFLOG("Invoking qapi_Loc_Start_Tracking()");
    ret = (qapi_Location_Error_t)qapi_Loc_Start_Tracking(
        (UINT)clientId, (UINT)&locationOptions, (UINT)&id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QFLOG("Returned success, id %d", id);

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return id;
}

static int location_update_tracking_options(uint32_t id, uint32_t minInterval, uint32_t minDistance)
{
    qapi_Location_Options_t locationOptions;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION UPDATE TRACKING OPTIONS");
    LOC_LOG_QFLOG("id %u, Interval %u, Distance %u", id,
        minInterval, minDistance);

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QFLOG("Invoking qapi_Loc_Update_Tracking_Options()");
    ret = (qapi_Location_Error_t)qapi_Loc_Update_Tracking_Options(
        clientId, id, &locationOptions);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

static int location_stop_tracking(uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION STOP TRACKING");

    LOC_LOG_QFLOG("Invoking qapi_Loc_Stop_Tracking() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Stop_Tracking(clientId, id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

/* Batching */
static uint32_t location_start_batching(
    uint32_t minInterval, uint32_t minDistance)
{
    uint32_t id = (uint32_t)-1;
    qapi_Location_Options_t locationOptions;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION START BATCHING");
    LOC_LOG_QFLOG("Interval %u, Distance %u", minInterval, minDistance);

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QFLOG("Invoking qapi_Loc_Start_Batching");
    ret = (qapi_Location_Error_t)qapi_Loc_Start_Batching(clientId, &locationOptions, &id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QFLOG("Returned success, id %d", id);

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return id;
}

static int location_update_batching_options(uint32_t id, uint32_t minInterval, uint32_t minDistance)
{
    qapi_Location_Options_t locationOptions;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION UPDATE BATCHING OPTIONS");
    LOC_LOG_QFLOG("id %u, Interval %u, Distance %u", id,
        minInterval, minDistance);

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QFLOG("Invoking qapi_Loc_Update_Batching_Options()");
    ret = (qapi_Location_Error_t)qapi_Loc_Update_Batching_Options(clientId, id, &locationOptions);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

static int location_stop_batching(uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION STOP BATCHING");

    LOC_LOG_QFLOG("Invoking qapi_Loc_Stop_Batching() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Stop_Batching(clientId, id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

static int location_get_batched_locations(uint32_t id, size_t count)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION GET BATCHED LOCATIONS");
    LOC_LOG_QFLOG("ID %d, Count %d", id, count);

    LOC_LOG_QFLOG("Invoking qapi_Loc_Get_Batched_Locations()");
    ret = (qapi_Location_Error_t)qapi_Loc_Get_Batched_Locations(clientId, id, count);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER);  // Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

/* Geofence */
static uint32_t* location_add_geofences(
    size_t count, qapi_Geofence_Option_t* options,
    qapi_Geofence_Info_t* datas)
{
    uint32_t* ids = NULL;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION ADD GEOFENCES");

    LOC_LOG_QFLOG("Invoking qapi_Loc_Add_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Add_Geofences(
        clientId, count, options, datas, &ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return NULL;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return ids;
}

static int location_modify_geofences(
    size_t count, uint32_t* ids, qapi_Geofence_Option_t* options)
{
    uint32_t i;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION MODIFY GEOFENCES");

    // print count and ID list
    LOC_LOG_QFLOG("Count %d, ID/Options list: ", count);
    for (i = 0; i < count; ++i) {
        LOC_LOG_QFLOG("id=%u breach=%u respon=%u dwell=%u", ids[i],
            options[i].breachTypeMask, options[i].responsiveness,
            options[i].dwellTime);
    }

    LOC_LOG_QFLOG("Invoking qapi_Loc_Modify_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Modify_Geofences(clientId, count, ids, options);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

static int location_pause_geofences(size_t count, uint32_t* ids)
{
    ULONG set_signal = 0;
    uint32_t i = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION PAUSE GEOFENCES");

    // print count and ID list
    LOC_LOG_QFLOG("Count %d, ID list: ", count);
    for (i = 0; i < count; i++) {
        LOC_LOG_QFLOG("ID: %d", ids[i]);
    }

    LOC_LOG_QFLOG("Invoking qapi_Loc_Pause_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Pause_Geofences(clientId, count, ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

static int location_resume_geofences(size_t count, uint32_t* ids)
{
    ULONG set_signal = 0;
    uint32_t i = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION RESUME GEOFENCES");

    // print count and ID list
    LOC_LOG_QFLOG("Count %d, ID list: ", count);
    for(i = 0; i < count; i++) {
        LOC_LOG_QFLOG("ID: %d", ids[i]);
    }

    LOC_LOG_QFLOG("Invoking qapi_Loc_Resume_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Resume_Geofences(clientId, count, ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback..");
    tx_event_flags_get(gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

static int location_remove_geofences(size_t count, uint32_t* ids)
{
    ULONG set_signal = 0;
    uint32_t i;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION REMOVE GEOFENCES");

    // print count and ID list
    LOC_LOG_QFLOG("Count %d, ID list: ", count);
    for(i = 0; i < count; i++) {
        LOC_LOG_QFLOG("ID: %d", ids[i]);
    }

    LOC_LOG_QFLOG("Invoking qapi_Loc_Remove_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Remove_Geofences(clientId, count, ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

/* Single-shot */
static uint32_t location_get_single_shot(uint32_t powerLevel)
{
    uint32_t id = (uint32_t)-1;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION GET SINGLE SHOT");
    LOC_LOG_QFLOG("Location Power Level %u", powerLevel);

    LOC_LOG_QFLOG("Invoking qapi_Loc_Get_Single_Shot()");
    ret = (qapi_Location_Error_t)qapi_Loc_Get_Single_Shot(
        (UINT)clientId, (UINT)powerLevel, (UINT)&id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QFLOG("Returned success, id %d", id);

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return id;
}

static int location_cancel_single_shot(uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION CANCEL SINGLE SHOT");

    LOC_LOG_QFLOG("Invoking qapi_Loc_Cancel_Single_Shot() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Cancel_Single_Shot(clientId, id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

/* GNSS Data */
static uint32_t location_start_get_gnss_data()
{
    uint32_t id = (uint32_t)-1;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION START GET GNSS DATA");

    LOC_LOG_QFLOG("Invoking qapi_Loc_Start_Get_Gnss_Data()");
    ret = (qapi_Location_Error_t)qapi_Loc_Start_Get_Gnss_Data(
        (UINT)clientId, (UINT)&id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QFLOG("Returned success, id %d", id);

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return id;
}

static int location_stop_get_gnss_data(uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QFLOG("LOCATION STOP GET GNSS DATA");

    LOC_LOG_QFLOG("Invoking qapi_Loc_Stop_Get_Gnss_Data() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Stop_Get_Gnss_Data(clientId, id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QFLOG("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QFLOG("Returned success");

    LOC_LOG_QFLOG("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QFLOG("REQUEST SUCCESS");
    return 0;
}

/* QAPI callback functions */
/* CAPABILITIES CALLBACKs */
static void location_capabilities_callback(
    qapi_Location_Capabilities_Mask_t capabilitiesMask)
{
    LOC_LOG_QFLOG("LOC API CALLBACK : CAPABILITIES (mask 0x%x)", capabilitiesMask);
}

/* RESPONSE CALLBACKs */
static void location_response_callback(
    qapi_Location_Error_t err, uint32_t id) {
    LOC_LOG_QFLOG("LOC API CALLBACK : RESPONSE (err=%u id=%u)", err, id);
    tx_event_flags_set(gSignalResponse, 1, TX_OR);
}

/* COLLECTIVE RESPONSE CALLBACKs */
static void location_collective_response_callback(size_t count,
    qapi_Location_Error_t* err,
    uint32_t* ids)
{
    uint32_t i = 0;
    LOC_LOG_QFLOG("LOC API CALLBACK : COLLECTIVE RESPONSE (count %d)", count);

    for (i = 0; i < count; ++i) {
        LOC_LOG_QFLOG("ids[%d] = %d", i, *(ids + i));
    }
    for (i = 0; i < count; ++i) {
        LOC_LOG_QFLOG("err[%d] = %d", i, *(err + i));
    }
#ifdef QAPI_BUGS_FIXED
#else
    geofence_id = *ids;
#endif
    tx_event_flags_set(gSignalGeofenceResponse, 1, TX_OR);
}

/* TRACKING CALLBACK */
static void location_tracking_callback(qapi_Location_t location)
{
    LOC_LOG_QFLOG("LOC API CALLBACK : TRACKING");
    log_location(location);
    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(gSignalLocation, 1, TX_OR);
    }
}

/* BATCHING CALLBACK */
static void location_batching_callback(
    size_t count, qapi_Location_t* pLocation) {
    uint32_t i = 0;
    qapi_Location_t location;

    LOC_LOG_QFLOG("LOC API CALLBACK : BATCHING (count %d)", count);

    for (i = 0; i < count; ++i) {
        LOC_LOG_QFLOG("location[%d]:", i);
        memcpy(&location, pLocation, sizeof(location));
        log_location(location);
        pLocation++;
    }

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(gSignalLocation, 1, TX_OR);
    }
}

/* GEOFENCE BREACH CALLBACK */
static void location_geofence_breach_callback(
    qapi_Geofence_Breach_Notification_t notification)
{
    uint32_t i = 0;
    uint64_t ts_sec = 0;
    LOC_LOG_QFLOG("LOC API CALLBACK : GEOFENCE BREACH (count %d)", notification.count);

    // Breach type
    if (notification.type == QAPI_GEOFENCE_BREACH_ENTER) {
        LOC_LOG_QFLOG("Breach Type: ENTER");
    }
    else if (notification.type == QAPI_GEOFENCE_BREACH_EXIT) {
        LOC_LOG_QFLOG("Breach Type: EXIT");
    }
    else if (notification.type == QAPI_GEOFENCE_BREACH_DWELL_IN) {
        LOC_LOG_QFLOG("Breach Type: DWELL IN");
    }
    else if (notification.type == QAPI_GEOFENCE_BREACH_DWELL_OUT) {
        LOC_LOG_QFLOG("Breach Type: DWELL OUT");
    }
    else {
        LOC_LOG_QFLOG("Breach type invalid: %d", notification.type);
    }

    // Timestamp and List of IDs received
    ts_sec = notification.location.timestamp / 1000;
    LOC_LOG_QFLOG("List of IDs recvd (count %d):",
        notification.count);
    for (i = 0; i < notification.count; i++) {
        LOC_LOG_QFLOG("ID: %d", notification.ids[i]);
    }

    log_location(notification.location);

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(gSignalGeofenceBreach, 1, TX_OR);
    }
}

/* SINGLE SHOT CALLBACK */
static void location_single_shot_callback(qapi_Location_t location,
    qapi_Location_Error_t err)
{
    LOC_LOG_QFLOG("LOC API CALLBACK : SINGLE_SHOT");

    if (QAPI_LOCATION_ERROR_TIMEOUT == err) {
        LOC_LOG_QFLOG("Timeout occurred, location is invalid");
    }
    else {
        log_location(location);
    }

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(gSignalLocation, 1, TX_OR);
    }
}

/* GNSS DATA CALLBACK */
static void location_gnss_data_callback(qapi_Gnss_Data_t gnssData)
{
    LOC_LOG_QFLOG("LOC API CALLBACK : GNSS DATA");

    LOC_LOG_QFLOG("JAMMER_GPS: %u JAMMER_GLONASS: %u",
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GPS],
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GLONASS]);

    LOC_LOG_QFLOG("JAMMER_BEIDOU: %u JAMMER_GALILEO: %u",
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_BEIDOU],
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GALILEO]);

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(gSignalLocation, 1, TX_OR);
    }
}

/* Timer start/stop functions */
static qapi_TIMER_handle_t start_app_timer(uint32_t id, uint32_t timeoutSeconds, void* sigs_func_ptr)
{
    qapi_TIMER_handle_t timer_handle;
    qapi_TIMER_define_attr_t timer_def_attr;
    qapi_TIMER_set_attr_t timer_set_attr;

    timer_def_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE; // notification type
    timer_def_attr.sigs_func_ptr = sigs_func_ptr; // callback to call when
                                                  // the timer expires
    timer_def_attr.sigs_mask_data = id; // this data will be returned in
                                        // the callback
    timer_def_attr.deferrable = false;  // set to true for nondeferrable timer
                                        // define the timer. Note: This call allocates memory and hence
                                        // qapi_Timer_Undef() should be called whenever the timer usage is done.
    qapi_Timer_Def(&timer_handle, &timer_def_attr);
    timer_set_attr.reload = FALSE; // Do not restart timer after it expires
    timer_set_attr.time = timeoutSeconds * 1000;
    timer_set_attr.unit = QAPI_TIMER_UNIT_MSEC;
    // set or start the timer
    qapi_Timer_Set(timer_handle, &timer_set_attr);

    return timer_handle;
}

static void stop_app_timer(qapi_TIMER_handle_t timer_handle)
{
    // stop a running timer
    qapi_Timer_Stop(timer_handle);
    // Undef the timer. Releases memory allocated in qapi_Timer_Def()
    qapi_Timer_Undef(timer_handle);
}

/* Timer callback functions */
static void timer_init_cb(uint32_t id)
{
    LOC_LOG_QFLOG("Init timer expired! id=%d", id);
    tx_event_flags_set(gSignalInit, 1, TX_OR);
}

static void timer_tracking_cb(uint32_t id)
{
    LOC_LOG_QFLOG("Tracking timer expired! id=%d", id);
    tx_event_flags_set(gSignalLocation, 1, TX_OR);
    gGpsVerifyTestInProgress = 0;
}

static void timer_batching_cb(uint32_t id)
{
    LOC_LOG_QFLOG("Batching timer expired! id=%d", id);
    tx_event_flags_set(gSignalLocation, 1, TX_OR);
    gGpsVerifyTestInProgress = 0;
}

static void timer_geofence_cb(uint32_t id)
{
    LOC_LOG_QFLOG("Geofence timer expired! id=%d", id);
    tx_event_flags_set(gSignalGeofenceBreach, 1, TX_OR);
    gGpsVerifyTestInProgress = 0;
}

static void timer_gnssdata_cb(uint32_t id)
{
    LOC_LOG_QFLOG("Gnssdata timer expired! id=%d", id);
    tx_event_flags_set(gSignalLocation, 1, TX_OR);
    gGpsVerifyTestInProgress = 0;
}

/* Utility functions */
static void log_location(qapi_Location_t location)
{
    uint64_t ts_sec = location.timestamp / 1000;

    LOC_LOG_QFLOG("Time(HH:MM:SS): %02d:%02d:%02d",
        (int)((ts_sec / 3600) % 24), (int)((ts_sec / 60) % 60), (int)(ts_sec % 60));

    if (location.flags & QAPI_LOCATION_HAS_LAT_LONG_BIT) {
        LOC_LOG_QFLOG("LATITUDE: %d.%d",
            (int)location.latitude, (abs((int)(location.latitude * 100000))) % 100000);
        LOC_LOG_QFLOG("LONGITUDE: %d.%d",
            (int)location.longitude, (abs((int)(location.longitude * 100000))) % 100000);
    }
    else {
        LOC_LOG_QFLOG("Latitude and longitude are not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_ALTITUDE_BIT) {
        LOC_LOG_QFLOG("ALTITUDE: %d.%d",
            (int)location.altitude, (abs((int)(location.altitude * 100))) % 100);
        LOC_LOG_QFLOG("ALTITUDE MSL: %d.%d",
            (int)location.altitudeMeanSeaLevel, (abs((int)(location.altitudeMeanSeaLevel * 100))) % 100);
    }
    else {
        LOC_LOG_QFLOG("Altitude is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_SPEED_BIT) {
        LOC_LOG_QFLOG("SPEED: %d.%d",
            (int)location.speed, (abs((int)(location.speed * 100))) % 100);
    }
    else {
        LOC_LOG_QFLOG("Speed is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_BEARING_BIT) {
        LOC_LOG_QFLOG("BEARING: %d.%d",
            (int)location.bearing, (abs((int)(location.bearing * 100))) % 100);
    }
    else {
        LOC_LOG_QFLOG("Bearing is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_ACCURACY_BIT) {
        LOC_LOG_QFLOG("ACCURACY: %d.%d",
            (int)location.accuracy, (abs((int)(location.accuracy * 100))) % 100);
    }
    else {
        LOC_LOG_QFLOG("Accuracy is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_VERTICAL_ACCURACY_BIT) {
        LOC_LOG_QFLOG("VERTICAL ACCURACY: %d.%d",
            (int)location.verticalAccuracy, (abs((int)(location.verticalAccuracy * 100))) % 100);
    }
    else {
        LOC_LOG_QFLOG("Vertical accuracy is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_SPEED_ACCURACY_BIT) {
        LOC_LOG_QFLOG("SPEED ACCURACY: %d.%d",
            (int)location.speedAccuracy, (abs((int)(location.speedAccuracy * 100))) % 100);
    }
    else {
        LOC_LOG_QFLOG("Speed accuracy is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_BEARING_ACCURACY_BIT) {
        LOC_LOG_QFLOG("BEARING ACCURACY: %d.%d",
            (int)location.bearingAccuracy, (abs((int)(location.bearingAccuracy * 100))) % 100);
    }
    else {
        LOC_LOG_QFLOG("Bearing accuracy is not provided");
    }
}

double qapi_loc_util_string_to_double(char* str){

    uint8_t digitValue = 0;
    double value = 0.0;
    char digit = '\0';
    int i = 0, negative = 0, dotSeen = 0;
    double fracMultiplier = 1;
    int strLen = strlen(str);

    if(strLen <= 0){
        return 0.0;
    }

    // traverse all characters in string
    if(str[0] == '-'){
        i++;
        negative = 1;
        if(strLen == 1){
            return 0.0;
        }
    }
    for(; i < strLen; i++){
        digit = str[i];
        if(dotSeen == 0 && digit == '.'){
            dotSeen = 1;
            continue;
        }
        if(dotSeen == 1){
            fracMultiplier *= 0.1;
        }
        // ascii 48 = '0', 57 = '9'
        if(digit < 48 || digit > 57){
            return 0.0;
        }
        digitValue = digit - 48;
        value = value*10 + digitValue;
    }
    value = value * fracMultiplier;
    if(negative == 1)
        value = value * -1;

    return value;
}
