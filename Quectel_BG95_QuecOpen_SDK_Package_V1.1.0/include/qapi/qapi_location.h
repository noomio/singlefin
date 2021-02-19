/* Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
 * @file qapi_location.h
 *
 * @details Functions for GNSS location driver
 *
 */

#ifndef _QAPI_LOCATION_H_
#define _QAPI_LOCATION_H_

#include <stdint.h>
#include <stddef.h>
#include "qapi_txm_base.h"
#include "stringl.h"

/*
!! IMPORTANT NOTE: "LOCATION" VERSION CATEGORY.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define TXM_QAPI_LOCATION_INIT                         TXM_QAPI_LOCATION_BASE
#define TXM_QAPI_LOCATION_DEINIT                       TXM_QAPI_LOCATION_BASE + 1
#define TXM_QAPI_LOCATION_START_TRACKING               TXM_QAPI_LOCATION_BASE + 2
#define TXM_QAPI_LOCATION_STOP_TRACKING                TXM_QAPI_LOCATION_BASE + 3
#define TXM_QAPI_LOCATION_UPDATE_TRACKING_OPTIONS      TXM_QAPI_LOCATION_BASE + 4
#define TXM_QAPI_LOCATION_START_BATCHING               TXM_QAPI_LOCATION_BASE + 5
#define TXM_QAPI_LOCATION_STOP_BATCHING                TXM_QAPI_LOCATION_BASE + 6
#define TXM_QAPI_LOCATION_UPDATE_BATCHING_OPTIONS      TXM_QAPI_LOCATION_BASE + 7
#define TXM_QAPI_LOCATION_GET_BATCHED_LOCATIONS        TXM_QAPI_LOCATION_BASE + 8
#define TXM_QAPI_LOCATION_ADD_GEOFENCES                TXM_QAPI_LOCATION_BASE + 9
#define TXM_QAPI_LOCATION_REMOVE_GEOFENCES             TXM_QAPI_LOCATION_BASE + 10
#define TXM_QAPI_LOCATION_MODIFY_GEOFENCES             TXM_QAPI_LOCATION_BASE + 11
#define TXM_QAPI_LOCATION_PAUSE_GEOFENCES              TXM_QAPI_LOCATION_BASE + 12
#define TXM_QAPI_LOCATION_RESUME_GEOFENCES             TXM_QAPI_LOCATION_BASE + 13
#define TXM_QAPI_LOCATION_GET_SINGLE_SHOT              TXM_QAPI_LOCATION_BASE + 14
#define TXM_QAPI_LOCATION_CANCEL_SINGLE_SHOT           TXM_QAPI_LOCATION_BASE + 15
#define TXM_QAPI_LOCATION_SET_USER_BUFFER              TXM_QAPI_LOCATION_BASE + 16
#define TXM_QAPI_LOCATION_START_GET_GNSS_DATA          TXM_QAPI_LOCATION_BASE + 17
#define TXM_QAPI_LOCATION_STOP_GET_GNSS_DATA           TXM_QAPI_LOCATION_BASE + 18
#define TXM_QAPI_LOCATION_GET_BACKOFF_TIME             TXM_QAPI_LOCATION_BASE + 19
#define TXM_QAPI_LOCATION_GET_BEST_AVAILABLE_POSITION  TXM_QAPI_LOCATION_BASE + 20
#define TXM_QAPI_LOCATION_GET_SINGLE_SHOT_V2           TXM_QAPI_LOCATION_BASE + 21
#define TXM_QAPI_LOCATION_START_MOTION_TRACKING        TXM_QAPI_LOCATION_BASE + 22
#define TXM_QAPI_LOCATION_STOP_MOTION_TRACKING         TXM_QAPI_LOCATION_BASE + 23

#define CUSTOM_CB_QAPI_LOCATION_CAPABILITIES        CUSTOM_CB_QAPI_LOCATION_BASE
#define CUSTOM_CB_QAPI_LOCATION_RESPONSE            CUSTOM_CB_QAPI_LOCATION_BASE + 1
#define CUSTOM_CB_QAPI_LOCATION_COLLECTIVE_RESPONSE CUSTOM_CB_QAPI_LOCATION_BASE + 2
#define CUSTOM_CB_QAPI_LOCATION_TRACKING            CUSTOM_CB_QAPI_LOCATION_BASE + 3
#define CUSTOM_CB_QAPI_LOCATION_BATCHING            CUSTOM_CB_QAPI_LOCATION_BASE + 4
#define CUSTOM_CB_QAPI_LOCATION_GEOFENCE_BREACH     CUSTOM_CB_QAPI_LOCATION_BASE + 5
#define CUSTOM_CB_QAPI_LOCATION_SINGLE_SHOT         CUSTOM_CB_QAPI_LOCATION_BASE + 6
#define CUSTOM_CB_QAPI_LOCATION_GNSS_DATA           CUSTOM_CB_QAPI_LOCATION_BASE + 7
#define CUSTOM_CB_QAPI_LOCATION_META_DATA           CUSTOM_CB_QAPI_LOCATION_BASE + 8
#define CUSTOM_CB_QAPI_LOCATION_GNSS_NMEA           CUSTOM_CB_QAPI_LOCATION_BASE + 9
// Use Location Extended Base for further callbacks
#define CUSTOM_CB_QAPI_LOCATION_MOTION_TRACKING     CUSTOM_CB_QAPI_LOCATION_EXT_BASE

    /** @addtogroup qapi_location
@{ */

 /**
 * GNSS location error codes.
 */
typedef enum {
    QAPI_LOCATION_ERROR_SUCCESS = 0,              /**< Success. */
    QAPI_LOCATION_ERROR_GENERAL_FAILURE,          /**< General failure. */
    QAPI_LOCATION_ERROR_CALLBACK_MISSING,         /**< Callback is missing. */
    QAPI_LOCATION_ERROR_INVALID_PARAMETER,        /**< Invalid parameter. */
    QAPI_LOCATION_ERROR_ID_EXISTS,                /**< ID already exists. */
    QAPI_LOCATION_ERROR_ID_UNKNOWN,               /**< ID is unknown. */
    QAPI_LOCATION_ERROR_ALREADY_STARTED,          /**< Already started. */
    QAPI_LOCATION_ERROR_NOT_INITIALIZED,          /**< Not initialized. */
    QAPI_LOCATION_ERROR_GEOFENCES_AT_MAX,         /**< Maximum number of geofences reached. */
    QAPI_LOCATION_ERROR_NOT_SUPPORTED,            /**< Not supported. */
    QAPI_LOCATION_ERROR_TIMEOUT,                  /**< Timeout when asking single shot. */
    QAPI_LOCATION_ERROR_LOAD_FAILURE,             /**< GNSS engine could not get loaded. */
    QAPI_LOCATION_ERROR_LOCATION_DISABLED,        /**< Location module license is disabled. */
    QAPI_LOCATION_ERROR_BEST_AVAIL_POS_INVALID,   /**< Best available position is invalid. */
} qapi_Location_Error_t;

typedef uint16_t qapi_Location_Flags_Mask_t;
/**
 * Flags to indicate which values are valid in a location.
 */
typedef enum {
    QAPI_LOCATION_HAS_LAT_LONG_BIT =          (1 << 0),   /**< Location has a valid latitude and longitude. */
    QAPI_LOCATION_HAS_ALTITUDE_BIT =          (1 << 1),   /**< Location has a valid altitude. */
    QAPI_LOCATION_HAS_SPEED_BIT =             (1 << 2),   /**< Location has a valid speed. */
    QAPI_LOCATION_HAS_BEARING_BIT =           (1 << 3),   /**< Location has a valid bearing. */
    QAPI_LOCATION_HAS_ACCURACY_BIT =          (1 << 4),   /**< Location has valid accuracy. */
    QAPI_LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1 << 5),   /**< Location has valid vertical accuracy. */
    QAPI_LOCATION_HAS_SPEED_ACCURACY_BIT =    (1 << 6),   /**< Location has valid speed accuracy. */
    QAPI_LOCATION_HAS_BEARING_ACCURACY_BIT =  (1 << 7),   /**< Location has valid bearing accuracy. */
    QAPI_LOCATION_HAS_ALTITUDE_MSL_BIT =      (1 << 8),   /**< Location has valid altitude wrt mean sea level. */
    QAPI_LOCATION_IS_BEST_AVAIL_POS_BIT =     (1 << 9),   /**< Location is the currently best available position. */
} qapi_Location_Flags_t;

typedef uint32_t qapi_Location_Meta_Data_Flags_Mask_t;
/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * Location 1.3.0  &  Engine status field added. @tblendline
 * }
 *
 * Flags to indicate which values are valid in meta data.
 */
typedef enum {
    QAPI_LOCATION_META_DATA_HAS_BACKOFF_TIME                  =  (1 << 0),   /**< Meta Data has backoff time. */
    QAPI_LOCATION_META_DATA_HAS_DISTANCE_TO_NEAREST_GEOFENCE  =  (1 << 1),   /**< Meta Data has distance to nearest geofence. */
    QAPI_LOCATION_META_DATA_HAS_ENGINE_STATUS                 =  (1 << 4),   /**< Meta Data has location engine status. */
} qapi_Location_Meta_Data_Flags_t;

/**
 * @versiontable{2.0,2.45,
 * Location 1.3.0  &  Introduced. @tblendline
 * }
 *
 * Location engine status events.
 */
typedef enum {
    QAPI_LOCATION_ENGINE_STATUS_UNKNOWN = 0,
    QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_SUCCESS,      /**< Data call for assistance data DL successful. */
    QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_FAILED,       /**< Data call for assistance data DL failed. */
    QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS,        /**< XTRA assistance data DL successful. */
    QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_FAILED,         /**< XTRA assistance data DL failed. */
    QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED,      /**< XTRA assistance data DL throttled. */
    QAPI_LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS,         /**< Time info DL successful. */
    QAPI_LOCATION_ENGINE_STATUS_NTP_DL_FAILED,          /**< Time info DL failed. */
    QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS,           /**< Location engine started position tracking. */
    QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED,            /**< Location engine failed to start position tracking. */
    QAPI_LOCATION_ENGINE_STATUS_FIX_FAILED              /**< Location engine failed to derive a position. */
} qapi_Location_Meta_Data_Engine_Status_t;

/**
 * Flags to indicate Geofence breach status.
 */
typedef enum {
    QAPI_GEOFENCE_BREACH_ENTER = 0, /**< Entering Geofence breach. */
    QAPI_GEOFENCE_BREACH_EXIT,      /**< Exiting Geofence breach. */
    QAPI_GEOFENCE_BREACH_DWELL_IN,  /**< Dwelling in a breached Geofence. */
    QAPI_GEOFENCE_BREACH_DWELL_OUT, /**< Dwelling outside of a breached Geofence. */
    QAPI_GEOFENCE_BREACH_UNKNOWN,   /**< Breach is unknown. */
} qapi_Geofence_Breach_t;

typedef uint16_t qapi_Geofence_Breach_Mask_t;
/** Flags to indicate Geofence breach mask bit. */
typedef enum {
    QAPI_GEOFENCE_BREACH_ENTER_BIT =     (1 << 0),   /**< Breach enter bit. */
    QAPI_GEOFENCE_BREACH_EXIT_BIT =      (1 << 1),   /**< Breach exit bit. */
    QAPI_GEOFENCE_BREACH_DWELL_IN_BIT =  (1 << 2),   /**< Breach dwell in bit. */
    QAPI_GEOFENCE_BREACH_DWELL_OUT_BIT = (1 << 3),   /**< Breach dwell out bit. */
} qapi_Geofence_Breach_Mask_Bits_t;

typedef uint32_t qapi_Location_Capabilities_Mask_t;
/** Flags to indicate the capabilities bit. */
typedef enum {
    QAPI_LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT =     (1 << 0),  /**< Capabilities time-based tracking bit. */
    QAPI_LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT =     (1 << 1),  /**< Capabilities time-based batching bit. */
    QAPI_LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT = (1 << 2),  /**< Capabilities distance-based tracking bit. */
    QAPI_LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT = (1 << 3),  /**< Capabilities distance-based batching bit. */
    QAPI_LOCATION_CAPABILITIES_GEOFENCE_BIT =                (1 << 4),  /**< Capabilities Geofence bit. */
    QAPI_LOCATION_CAPABILITIES_GNSS_DATA_BIT =               (1 << 5),  /**< Capabilities Geofence bit. */
} qapi_Location_Capabilities_Mask_Bits_t;

/** Flags to indicate the constellation type. */
typedef enum {
    QAPI_GNSS_SV_TYPE_UNKNOWN = 0, /**< Unknown. */
    QAPI_GNSS_SV_TYPE_GPS,         /**< GPS. */
    QAPI_GNSS_SV_TYPE_SBAS,        /**< SBAS. */
    QAPI_GNSS_SV_TYPE_GLONASS,     /**< GLONASS. */
    QAPI_GNSS_SV_TYPE_QZSS,        /**< QZSS. */
    QAPI_GNSS_SV_TYPE_BEIDOU,      /**< BEIDOU. */
    QAPI_GNSS_SV_TYPE_GALILEO,     /**< GALILEO. */
    QAPI_MAX_NUMBER_OF_CONSTELLATIONS /**< Maximum number of constellations. */
} qapi_Gnss_Sv_t;

/** Flags to indicate allowed power levels for fix computation. */
typedef enum {
    QAPI_LOCATION_POWER_HIGH = 0,           /**< Use all technologies available to calculate location.   */
    QAPI_LOCATION_POWER_LOW,                /**< Use all low power technologies to calculate location.   */
    QAPI_LOCATION_POWER_MED,                /**< Use only low and medium power technologies to calculate location */
} qapi_Location_Power_Level_t;

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Flags to indicate the desired level of accuracy for fix computation.
 */
typedef enum {
    QAPI_LOCATION_ACCURACY_UNKNOWN = 0,     /**< Accuracy is not specified, use default. */
    QAPI_LOCATION_ACCURACY_LOW,             /**< Low Accuracy for location is acceptable. */
    QAPI_LOCATION_ACCURACY_MED,             /**< Medium Accuracy for location is acceptable. */
    QAPI_LOCATION_ACCURACY_HIGH,            /**< Only High Accuracy for location is acceptable. */
} qapi_Location_Accuracy_Level_t;

typedef uint32_t qapi_Location_Motion_Type_Mask_t;
/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Flags to indicate the type of motion.
 */
typedef enum {
    QAPI_LOCATION_MOTION_TYPE_REST =              (1 << 0),   /**< Device at rest. */
    QAPI_LOCATION_MOTION_TYPE_MOVE_ABSOLUTE =     (1 << 1),   /**< Device in motion. */
    QAPI_LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT =  (1 << 2),   /**< Device in significant motion,
                                                                   where significant motion is
                                                                   identified by the device being
                                                                   in motion for a certain amount
                                                                   of time.
                                                                   */
} qapi_Location_Motion_Type_t;

/** Structure for location information. */
typedef struct {
    size_t size;                        /**< Size. Set to the size of qapi_Location_t. */
    qapi_Location_Flags_Mask_t flags;   /**< Bitwise OR of qapi_Location_Flags_t. */
    uint64_t timestamp;                 /**< UTC timestamp for a location fix; milliseconds since Jan. 1, 1970. */
    double latitude;                    /**< Latitude in degrees. */
    double longitude;                   /**< Longitude in degrees. */
    double altitude;                    /**< Altitude in meters above the WGS 84 reference ellipsoid. */
    double altitudeMeanSeaLevel;        /**< Altitude in meters with respect to mean sea level. */
    float speed;                        /**< Speed in meters per second. */
    float bearing;                      /**< Bearing in degrees; range: 0 to 360. */
    float accuracy;                     /**< Accuracy in meters. */
    float verticalAccuracy;             /**< Vertical accuracy in meters. */
    float speedAccuracy;                /**< Speed accuracy in meters/second. */
    float bearingAccuracy;              /**< Bearing accuracy in degrees (0 to 359.999). */
} qapi_Location_t;

/** Structure for GNSS data information. */
typedef struct {
    size_t size;                                            /**< Size. Set to the size of #qapi_Gnss_Data_t. */
    uint32_t jammerInd[QAPI_MAX_NUMBER_OF_CONSTELLATIONS];  /**< Jammer indication. */
} qapi_Gnss_Data_t;

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * Location 1.3.0  &  Added Engine Status field. @tblendline
 * }
 *
 * Structure for Location Meta data information.
 */
typedef struct {
    size_t size;                                  /**< Size. Set to the size of #qapi_Meta_Data_t. */
    qapi_Location_Meta_Data_Flags_Mask_t flags;   /**< Bitwise OR of qapi_Location_Meta_Data_Flags_t. */
    uint32_t backoffTimeMillis;                   /**< Backoff value in milliseconds. */
    uint32_t nearestGeofenceMeters;               /**< Distance to nearest geofence in meters. */
    qapi_Location_Meta_Data_Engine_Status_t engineStatus;  /**< Location Engine status. */
} qapi_Location_Meta_Data_t;

/* Source of the NMEA string, Whether Final position, Intermediate position or SV Info*/
typedef enum {
	QAPI_NMEA_SOURCE_UNKNOWN = 0,
	QAPI_NMEA_SOURCE_POSITION_FINAL,
	QAPI_NMEA_SOURCE_POSITION_INTERMEDIATE,
	QAPI_NMEA_SOURCE_SV_INFO,
	QAPI_NMEA_SOURCE_GNSS_ENGINE,
}qapi_Nmea_Source;

/** Structure for NMEA data */
typedef struct {
    size_t size;          /**< Size. Set to the size of #qapi_Location_Nmea_t. */
    uint64_t timestamp;   /**< Timestamp when this NMEA is generated; milliseconds since Jan. 1, 1970 */
    const char* nmea;     /**< NMEA sentence string */
    size_t length;        /**< Length of the NMEA sentence string */
	qapi_Nmea_Source source; /**< NMEA Source */
} qapi_Gnss_Nmea_t;

/** Structure for location options. */
typedef struct {
    size_t size;          /**< Size. Set to the size of #qapi_Location_Options_t. */
    uint32_t minInterval; /**<  There are three different interpretations of this field, 
                                depending if minDistance is 0 or not:
                                1. Time-based tracking (minDistance = 0). minInterval is the minimum 
                                time interval in milliseconds that must elapse between final position reports.
                                2. Distance-based tracking (minDistance > 0). minInterval is the
                                maximum time period in milliseconds after the minimum distance
                                criteria has been met within which a location update must be provided. 
                                If set to 0, an ideal value will be assumed by the engine.
                                3. Batching. minInterval is the minimum time interval in milliseconds that 
                                must elapse between position reports.
                                */
    uint32_t minDistance; /**< Minimum distance in meters that must be traversed between position reports. 
                               Setting this interval to 0 will be a pure time-based tracking/batching.
                               */
    qapi_Location_Accuracy_Level_t accuracyLevel; /**< Accuracy level required for fix computation. */
} qapi_Location_Options_t;

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Structure for singleshot request options.
 */
typedef struct {
    size_t size;                                    /**< Size. Set to the size of #qapi_Singleshot_Options_t. */
    qapi_Location_Power_Level_t    powerLevel;      /**< Power level required for fix computation. */
    qapi_Location_Accuracy_Level_t accuracyLevel;   /**< Accuracy level required for fix computation. */
    uint32_t                       timeout;         /**< Timeout (in milliseconds) for singleshot fix. */
} qapi_Singleshot_Options_t;

/** Structure for Geofence options. */
typedef struct {
    size_t size;                                /**< Size. Set to the size of #qapi_Geofence_Option_t. */
    qapi_Geofence_Breach_Mask_t breachTypeMask; /**< Bitwise OR of #qapi_Geofence_Breach_Mask_Bits_t bits. */
    uint32_t responsiveness;                    /**< Specifies in milliseconds the user-defined rate of detection for a Geofence
                                                     breach. This may impact the time lag between the actual breach event and
                                                     when it is reported. The gap between the actual breach and the time it 
                                                     is reported depends on the user setting. The power implication is 
                                                     inversely proportional to the responsiveness value set by the user.
                                                     The higher the responsiveness value, the lower the power implications, 
                                                     and vice-versa. */
    uint32_t dwellTime;                         /**< Dwell time is the time in milliseconds a user spends in the Geofence before 
                                                     a dwell event is sent. */
} qapi_Geofence_Option_t;

/** Structure for Geofence information. */
typedef struct {
    size_t size;      /**< Size. Set to the size of #qapi_Geofence_Info_t. */
    double latitude;  /**< Latitude of the center of the Geofence in degrees. */
    double longitude; /**< Longitude of the center of the Geofence in degrees. */
    double radius;    /**< Radius of the Geofence in meters. */
} qapi_Geofence_Info_t;

/** Structure for Geofence breach notification. */
typedef struct {
    size_t size;                    /**< Size. Set to the size of #qapi_Geofence_Breach_Notification_t. */
    size_t count;                   /**< Number of IDs in the array. */
    uint32_t* ids;                  /**< Array of IDs that have been breached. */
    qapi_Location_t location;       /**< Location associated with a breach. */
    qapi_Geofence_Breach_t type;    /**< Type of breach. */
    uint64_t timestamp;             /**< Timestamp of the breach; milliseconds since Jan. 1, 1970. */
} qapi_Geofence_Breach_Notification_t;

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Structure for location motion reporting options.
 */
typedef struct {
    size_t size;                                       /**< Size. Set to the size of
                                                            #qapi_Location_Motion_Options_t. */
    qapi_Location_Motion_Type_Mask_t motionTypeMask;   /**< Types of motion for which reporting
                                                            is desired. */
} qapi_Location_Motion_Options_t;

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Structure for location motion information.
 */
typedef struct {
    size_t size;                                       /**< Size. Set to the size of
                                                            #qapi_Location_Motion_Info_t. */
    qapi_Location_Motion_Type_t motionType;            /**< Type of motion detected. */
    uint64_t timestamp;                                /**< Timestamp of the motion event;
                                                            milliseconds since Jan. 1, 1970. */
} qapi_Location_Motion_Info_t;

/* GNSS Location Callbacks */

/**
*  Provides the capabilities of the system.
         It is called once after qapi_Loc_Init() is called.

  @param[in] capabilitiesMask Bitwise OR of #qapi_Location_Capabilities_Mask_Bits_t.

  @return None.
*/
typedef void(*qapi_Capabilities_Callback)(
    qapi_Location_Capabilities_Mask_t capabilitiesMask
);

/**
*  Response callback, which is used by all tracking, batching 
         and Single Shot APIs.
         It is called for every tracking, batching and single shot API.

  @param[in] err  #qapi_Location_Error_t associated with the request.
                  If this is not QAPI_LOCATION_ERROR_SUCCESS then id is not valid.
  @param[in] id   ID to be associated with the request.

  @return None.
*/
typedef void(*qapi_Response_Callback)(
    qapi_Location_Error_t err,
    uint32_t id
);

/**
*  Collective response callback is used by Geofence APIs.
         It is called for every Geofence API call.

  @param[in] count  Number of locations in arrays.
  @param[in] err    Array of #qapi_Location_Error_t associated with the request.
  @param[in] ids    Array of IDs to be associated with the request.

  @return None.
*/
typedef void(*qapi_Collective_Response_Callback)(
    size_t count,
    qapi_Location_Error_t* err,
    uint32_t* ids
);

/**
*  Tracking callback used for the qapi_Loc_Start_Tracking() API.
         This is an optional function and can be NULL.
         It is called when delivering a location in a tracking session.

  @param[in] location  Structure containing information related to the 
                       tracked location.

  @return None.
*/
typedef void(*qapi_Tracking_Callback)(
    qapi_Location_t location
);

/**
*  Batching callback used for the qapi_Loc_Start_Batching() API.
         This is an optional function and can be NULL.
         It is called when delivering a location in a batching session.

  @param[in] count     Number of locations in an array.
  @param[in] location  Array of location structures containing information
                       related to the batched locations.

  @return None.
*/
typedef void(*qapi_Batching_Callback)(
    size_t count,
    qapi_Location_t* location
);

/**
*  Geofence breach callback used for the qapi_Loc_Add_Geofences() API.
         This is an optional function and can be NULL.
         It is called when any number of geofences have a state change.

  @param[in] geofenceBreachNotification     Breach notification information.

  @return None.
*/
typedef void(*qapi_Geofence_Breach_Callback)(
    qapi_Geofence_Breach_Notification_t geofenceBreachNotification
);

/**
*  Single shot callback used for the qapi_Loc_Get_Single_Shot() API.
        This is an optional function and can be NULL.
        It is called when delivering a location in a single shot session
        broadcasted to all clients, no matter if a session has started by client.

   @param[in] location  Structure containing information related to the
                        tracked location.
   @param[in] err       #qapi_Location_Error_t associated with the request.
                        This could be QAPI_LOCATION_ERROR_SUCCESS (location 
                        is valid) or QAPI_LOCATION_ERROR_TIMEOUT (a timeout
                        occurred, location is not valid).

   @return None.
*/
typedef void(*qapi_Single_Shot_Callback)(
    qapi_Location_t location,
    qapi_Location_Error_t err
);

/**
*  GNSS data callback used for the qapi_Loc_Start_Get_Gnss_Data() API.
         This is an optional function and can be NULL.
         It is called when delivering a GNSS Data structure. The GNSS data
         structure contains useful information (e.g., jammer indication). 
         This callback will be called every second.

  @param[in] gnssData  Structure containing information related to the 
                       requested GNSS Data.

  @return None.
*/
typedef void(*qapi_Gnss_Data_Callback)(
    qapi_Gnss_Data_t gnssData
);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Meta data callback used for below scenarios.
         - qapi_Loc_Get_Backoff_Value() API
         This is an optional function and can be NULL.
         It is called when delivering some location meta data.

  @param[in] metaData  Structure containing meta data related to ongoing
                       location sessions.

  @return None.
*/
typedef void(*qapi_Location_Meta_Data_Callback)(
    qapi_Location_Meta_Data_t metaData
);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * NMEA callback used for reporting NMEA statements.
         This is an optional function and can be NULL.
         It is called when delivering NMEA report from Modem.

  @param[in] nmeaData  Structure containing the NMEA sentence

  @return None.
*/
typedef void(*qapi_Gnss_Nmea_Callback)(
    qapi_Gnss_Nmea_t gnssNmea
);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Tracking callback used for the qapi_Loc_Start_Motion_Tracking() API.
         This is an optional function and can be NULL.
         It is called when delivering motion info in a motion tracking session.

  @param[in] motionInfo  Structure containing information about the
                         detected motion.

  @return None.
*/
typedef void(*qapi_Motion_Tracking_Callback)(
    qapi_Location_Motion_Info_t motionInfo
);

/** Location callbacks requirements. */
typedef struct {
    size_t size;
    /**< Size. Set to the size of qapi_Location_Callbacks_t. */
    qapi_Capabilities_Callback         capabilitiesCb;
    /**< Capabilities callback is mandatory. */
    qapi_Response_Callback             responseCb;
    /**< Response callback is mandatory. */
    qapi_Collective_Response_Callback  collectiveResponseCb;
    /**< Collective response callback is mandatory. */
    qapi_Tracking_Callback             trackingCb;
    /**< Tracking callback is optional. */
    qapi_Batching_Callback             batchingCb;
    /**< Batching callback is optional. */
    qapi_Geofence_Breach_Callback      geofenceBreachCb;
    /**< Geofence breach callback is optional. */
    qapi_Single_Shot_Callback          singleShotCb;
    /**< Single shot callback is optional. */
    qapi_Gnss_Data_Callback            gnssDataCb;
    /**< GNSS data callback is optional. @newpagetable */
    qapi_Location_Meta_Data_Callback   metaDataCb;
    /**< Meta data callback is optional. */
    qapi_Gnss_Nmea_Callback            gnssNmeaCb;
    /**< Motion tracking callback is optional. */
    qapi_Motion_Tracking_Callback      motionTrackingCb;
} qapi_Location_Callbacks_t;

/** Location client identifier. */
typedef uint32_t qapi_loc_client_id;

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS
static __inline void qapi_location_cb_uspace_dispatcher(UINT cb_id,
    void *app_cb,
    UINT cb_param1,
    UINT cb_param2,
    UINT cb_param3,
    UINT cb_param4,
    UINT cb_param5,
    UINT cb_param6,
    UINT cb_param7,
    UINT cb_param8)
{
    void(*pfn_app_cb1) (qapi_Location_Capabilities_Mask_t);
    void(*pfn_app_cb2) (qapi_Location_Error_t, uint32_t);
    void(*pfn_app_cb3) (size_t, qapi_Location_Error_t*, uint32_t*);
    void(*pfn_app_cb4) (qapi_Location_t);
    void(*pfn_app_cb5) (size_t, qapi_Location_t*);
    void(*pfn_app_cb6) (qapi_Geofence_Breach_Notification_t);
    void(*pfn_app_cb7) (qapi_Location_t, qapi_Location_Error_t);
    void(*pfn_app_cb8) (qapi_Gnss_Data_t);
    void(*pfn_app_cb9) (qapi_Location_Meta_Data_t);
    void(*pfn_app_cb10) (qapi_Gnss_Nmea_t);
    void(*pfn_app_cb11) (qapi_Location_Motion_Info_t);

    qapi_Location_t location;
    qapi_Geofence_Breach_Notification_t notification;
    qapi_Gnss_Data_t gnssData;
    qapi_Location_Meta_Data_t metaData;
    qapi_Gnss_Nmea_t gnssNmea;
    qapi_Location_Motion_Info_t motionInfo;

    switch (cb_id) {
    case CUSTOM_CB_QAPI_LOCATION_CAPABILITIES:
        pfn_app_cb1 = (void(*)(qapi_Location_Capabilities_Mask_t))app_cb;
        (pfn_app_cb1)((qapi_Location_Capabilities_Mask_t)cb_param1);
        break;

    case CUSTOM_CB_QAPI_LOCATION_RESPONSE:
        pfn_app_cb2 = (void(*)(qapi_Location_Error_t, uint32_t))app_cb;
        (pfn_app_cb2)((qapi_Location_Error_t)cb_param1,
            (uint32_t)cb_param2);
        break;

    case CUSTOM_CB_QAPI_LOCATION_COLLECTIVE_RESPONSE:
        pfn_app_cb3 = (void(*)(size_t, qapi_Location_Error_t*, uint32_t*))app_cb;
        (pfn_app_cb3)((size_t)cb_param1,
            (qapi_Location_Error_t*)cb_param2,
            (uint32_t*)cb_param3);
        break;

    case CUSTOM_CB_QAPI_LOCATION_TRACKING:
        pfn_app_cb4 = (void(*)(qapi_Location_t))app_cb;
        memcpy(&location, (qapi_Location_t*)cb_param1, sizeof(qapi_Location_t));
        (pfn_app_cb4)(location);
        break;

    case CUSTOM_CB_QAPI_LOCATION_BATCHING:
        pfn_app_cb5 = (void(*)(size_t, qapi_Location_t*))app_cb;
        (pfn_app_cb5)((size_t)cb_param1,
            (qapi_Location_t*)cb_param2);
        break;

    case CUSTOM_CB_QAPI_LOCATION_GEOFENCE_BREACH:
        pfn_app_cb6 = (void(*)(qapi_Geofence_Breach_Notification_t))app_cb;
        memcpy(&notification, (qapi_Geofence_Breach_Notification_t*)cb_param1,
                sizeof(qapi_Geofence_Breach_Notification_t));
        (pfn_app_cb6)(notification);
        break;

    case CUSTOM_CB_QAPI_LOCATION_SINGLE_SHOT:
        pfn_app_cb7 = (void(*)(qapi_Location_t, qapi_Location_Error_t))app_cb;
        memcpy(&location, (qapi_Location_t*)cb_param1, sizeof(qapi_Location_t));
        (pfn_app_cb7)(location, (qapi_Location_Error_t)cb_param2);
        break;
    
    case CUSTOM_CB_QAPI_LOCATION_GNSS_DATA:
        pfn_app_cb8 = (void(*)(qapi_Gnss_Data_t))app_cb;
        gnssData = *((qapi_Gnss_Data_t*)cb_param1);
        (pfn_app_cb8)(gnssData);
        break;

    case CUSTOM_CB_QAPI_LOCATION_META_DATA:
        pfn_app_cb9 = (void(*)(qapi_Location_Meta_Data_t))app_cb;
        metaData = *((qapi_Location_Meta_Data_t*)cb_param1);
        (pfn_app_cb9)(metaData);
        break;

    case CUSTOM_CB_QAPI_LOCATION_GNSS_NMEA:
        pfn_app_cb10 = (void(*)(qapi_Gnss_Nmea_t))app_cb;
        gnssNmea = *((qapi_Gnss_Nmea_t*)cb_param1);
        (pfn_app_cb10)(gnssNmea);
        break;

    case CUSTOM_CB_QAPI_LOCATION_MOTION_TRACKING:
        pfn_app_cb11 = (void(*)(qapi_Location_Motion_Info_t))app_cb;
        motionInfo = *((qapi_Location_Motion_Info_t*)cb_param1);
        (pfn_app_cb11)(motionInfo);
        break;

    default:
        break;
    }
}

#define qapi_Loc_Init(a, b)                            ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_INIT                           , (ULONG) a, (ULONG) b, (ULONG) qapi_location_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Deinit(a)                             ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_DEINIT                         , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Start_Tracking(a, b, c)               ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_START_TRACKING                 , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Stop_Tracking(a, b)                   ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_STOP_TRACKING                  , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Update_Tracking_Options(a, b, c)      ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_UPDATE_TRACKING_OPTIONS        , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Start_Batching(a, b, c)               ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_START_BATCHING                 , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Stop_Batching(a, b)                   ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_STOP_BATCHING                  , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Update_Batching_Options(a, b, c)      ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_UPDATE_BATCHING_OPTIONS        , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Get_Batched_Locations(a, b, c)        ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_GET_BATCHED_LOCATIONS          , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Add_Geofences(a, b, c, d, e)          ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_ADD_GEOFENCES                  , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Remove_Geofences(a, b, c)             ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_REMOVE_GEOFENCES               , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Modify_Geofences(a, b, c, d)          ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_MODIFY_GEOFENCES               , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Pause_Geofences(a, b, c)              ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_PAUSE_GEOFENCES                , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Resume_Geofences(a, b, c)             ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_RESUME_GEOFENCES               , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Get_Single_Shot(a, b, c)              ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_GET_SINGLE_SHOT                , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Cancel_Single_Shot(a, b)              ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_CANCEL_SINGLE_SHOT             , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Set_User_Buffer(a, b, c)              ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_SET_USER_BUFFER                , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Start_Get_Gnss_Data(a, b)             ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_START_GET_GNSS_DATA            , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Stop_Get_Gnss_Data(a, b)              ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_STOP_GET_GNSS_DATA             , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Get_Backoff_Time(a, b)                ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_GET_BACKOFF_TIME               , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Get_Best_Available_Position(a, b)     ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_GET_BEST_AVAILABLE_POSITION    , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Get_Single_Shot_V2(a, b, c)           ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_GET_SINGLE_SHOT_V2             , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Start_Motion_Tracking(a, b, c)        ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_START_MOTION_TRACKING          , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Loc_Stop_Motion_Tracking(a, b)            ((UINT) (_txm_module_system_call12)(TXM_QAPI_LOCATION_STOP_MOTION_TRACKING           , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#else // QAPI_TXM_MODULE

UINT qapi_location_handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

/**
*  Initializes a location session and registers the callbacks.

  @param[out] pClientId  Pointer to client ID type, where the unique identifier
                         for this location client is returned.
  @param[in] pCallbacks  Pointer to the structure with the callback
                         functions to be registered.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_CALLBACK_MISSING -- One of the mandatory
            callback functions is missing. \n
  QAPI_LOCATION_ERROR_GENERAL_FAILURE -- There is an internal error. \n
  QAPI_LOCATION_ERROR_ALREADY_STARTED -- A location session has
            already been initialized.
*/
qapi_Location_Error_t qapi_Loc_Init(
        qapi_loc_client_id* pClientId,
        const qapi_Location_Callbacks_t* pCallbacks);

/**
*  De-initializes a location session.

  @param[in] clientId  Client identifier for the location client to be
                       deinitialized.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Deinit(
        qapi_loc_client_id clientId);

/**
*  Sets the user buffer to be used for sending back callback data.

  @param[in] clientId   Client ID for which user buffer is to be set
  @param[in] pUserBuffer User memory buffer to hold information passed
                         in callbacks. Note that since buffer is shared by all
                         the callbacks this has to be consumed at the user side
                         before it can be used by another callback to avoid any
                         potential race condition.
  @param[in] bufferSize  Size of user memory buffer to hold information passed
                         in callbacks. This size should be large enough to
                         accomodate the largest data size passed in a callback.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_GENERAL_FAILURE -- There is an internal error. \n
*/
qapi_Location_Error_t qapi_Loc_Set_User_Buffer(
        qapi_loc_client_id clientId,
        uint8_t* pUserBuffer,
        size_t bufferSize);

/**
*  Starts a tracking session, which returns a 
         session ID that will be used by the other tracking APIs and in 
         the response callback to match the command with a response. Locations are 
         reported on the tracking callback passed in qapi_Loc_Init() periodically
         according to the location options.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successfully started.
                QAPI_LOCATION_ERROR_ALREADY_STARTED if a qapi_Loc_Start_Tracking session is already in progress.
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no trackingCb was passed in qapi_Loc_Init().
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if pOptions parameter is invalid.

  @param[in] clientId     Client identifier for the location client.
  @param[in] pOptions     Pointer to a structure containing the options: 
                          - minInterval -- There are two different interpretations of this field, 
                                depending if minDistance is 0 or not:
                                1. Time-based tracking (minDistance = 0). minInterval is the minimum 
                                time interval in milliseconds that must elapse between final position reports.
                                2. Distance-based tracking (minDistance > 0). minInterval is the
                                maximum time period in milliseconds after the minimum distance
                                criteria has been met within which a location update must be provided. 
                                If set to 0, an ideal value will be assumed by the engine.
                          - minDistance -- Minimum distance in meters that must be traversed between position 
                                reports. Setting this interval to 0 will be a pure time-based tracking.
  @param[out] pSessionId   Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Start_Tracking(
        qapi_loc_client_id clientId,
        const qapi_Location_Options_t* pOptions,
        uint32_t* pSessionId);

/**
*  Stops a tracking session associated with the id 
         parameter
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if successful.
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a tracking session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the session to be stopped.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Stop_Tracking(
        qapi_loc_client_id clientId,
        uint32_t sessionId);

/**
*  Changes the location options of a 
         tracking session associated with the id parameter.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if successful.
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if pOptions parameter is invalid.
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a tracking session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the session to be changed.
  @param[in] pOptions   Pointer to a structure containing the options:
                          - minInterval -- There are two different interpretations of this field, 
                                depending if minDistance is 0 or not:
                                1. Time-based tracking (minDistance = 0). minInterval is the minimum 
                                time interval in milliseconds that must elapse between final position reports.
                                2. Distance-based tracking (minDistance > 0). minInterval is the
                                maximum time period in milliseconds after the minimum distance
                                criteria has been met within which a location update must be provided. 
                                If set to 0, an ideal value will be assumed by the engine.
                          - minDistance -- Minimum distance in meters that must be traversed between position 
                                reports. Setting this interval to 0 will be a pure time-based tracking.@tablebulletend

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Update_Tracking_Options(
        qapi_loc_client_id clientId,
        uint32_t sessionId,
        const qapi_Location_Options_t* pOptions);

/**
*  Starts a batching session, which returns a 
         session ID that will be used by the other batching APIs and in 
         the response callback to match the command with a response.
         
         Locations are 
         reported on the batching callback passed in qapi_Loc_Init() periodically
         according to the location options. A batching session starts tracking on
         the low power processor and delivers them in batches by the 
         batching callback when the batch is full or when qapi_Loc_Get_Batched_Locations() 
         is called. This allows for the processor that calls this API to sleep 
         when the low power processor can batch locations in the background and 
         wake up the processor calling the API only when the batch is full, 
         thus saving power.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successfully started.
                QAPI_LOCATION_ERROR_ALREADY_STARTED if a qapi_Loc_Start_Batching session is already in progress.
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no batchingCb was passed in qapi_Loc_Init().
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if pOptions parameter is invalid.
                QAPI_LOCATION_ERROR_NOT_SUPPORTED if batching is not supported.

  @param[in] clientId    Client identifier for the location client.
  @param[in] pOptions    Pointer to a structure containing the options:
                          - minInterval -- minInterval is the minimum time interval in milliseconds that 
                                must elapse between position reports.
                          - minDistance -- Minimum distance in meters that must be traversed between 
                                position reports.@tablebulletend
  @param[out] pSessionId  Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Start_Batching(
        qapi_loc_client_id clientId,
        const qapi_Location_Options_t* pOptions,
        uint32_t* pSessionId);

/**
*  Stops a batching session associated with the id
         parameter.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if successful.
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a batching session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the session to be stopped.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Stop_Batching(
        qapi_loc_client_id clientId,
        uint32_t sessionId);

/**
*  Changes the location options of a 
         batching session associated with the id parameter.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if successful.
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if pOptions parameter is invalid.
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a batching session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the session to be changed.
  @param[in] pOptions   Pointer to a structure containing the options:
                          - minInterval -- minInterval is the minimum time interval in milliseconds that 
                                must elapse between position reports.
                          - minDistance -- Minimum distance in meters that must be traversed between 
                                position reports.@tablebulletend

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Update_Batching_Options(
        qapi_loc_client_id clientId,
        uint32_t sessionId,
        const qapi_Location_Options_t* pOptions);

/**
*  Gets a number of locations that are 
         currently stored or batched on the low power processor, delivered by 
         the batching callback passed to qapi_Loc_Init(). Locations are then 
         deleted from the batch stored on the low power processor.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if successful, will be followed by batchingCallback call.
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no batchingCb was passed in qapi_Loc_Init().
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a batching session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the session for which the number of locations is requested.
  @param[in] count      Number of requested locations. The client can set this to 
                        MAXINT to get all the batched locations. If set to 0 no location
                        will be present in the callback function.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Get_Batched_Locations(
        qapi_loc_client_id clientId,
        uint32_t sessionId,
        size_t count);

/**
*  Adds a specified number of Geofences and returns an 
         array of Geofence IDs that will be used by the other Geofence APIs, 
         as well as in the collective response callback to match the command with 
         a response. The Geofence breach callback delivers the status of each
         Geofence according to the Geofence options for each.
           collectiveResponseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successful
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no geofenceBreachCb
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if any parameters are invalid
                QAPI_LOCATION_ERROR_NOT_SUPPORTED if geofence is not supported

  @param[in] clientId  Client identifier for the location client.
  @param[in] count     Number of Geofences to be added.
  @param[in] pOptions  Array of structures containing the options:
                       - breachTypeMask -- Bitwise OR of GeofenceBreachTypeMask bits
                       - responsiveness in milliseconds
                       - dwellTime in seconds @vertspace{-14}
  @param[in] pInfo     Array of structures containing the data:
                       - Latitude of the center of the Geofence in degrees 
                       - Longitude of the center of the Geofence in degrees
                       - Radius of the Geofence in meters @vertspace{-14}
  @param[out] pIdArray  Array of IDs of Geofences to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Add_Geofences(
        qapi_loc_client_id clientId,
        size_t count,
        const qapi_Geofence_Option_t* pOptions,
        const qapi_Geofence_Info_t* pInfo,
        uint32_t** pIdArray);

/**
*  Removes a specified number of Geofences.
           collectiveResponseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successful
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a geofence session.

  @param[in] clientId  Client identifier for the location client.
  @param[in] count     Number of Geofences to be removed.
  @param[in] pIDs      Array of IDs of the Geofences to be removed.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Remove_Geofences(
        qapi_loc_client_id clientId,
        size_t count,
        const uint32_t* pIDs);

/**
*  Modifies a specified number of Geofences.
           collectiveResponseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successful
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a geofence session.
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if any parameters are invalid

  @param[in] clientId  Client identifier for the location client.
  @param[in] count     Number of Geofences to be modified.
  @param[in] pIDs      Array of IDs of the Geofences to be modified.
  @param[in] options  Array of structures containing the options:
                       - breachTypeMask -- Bitwise OR of GeofenceBreachTypeMask bits
                       - responsiveness in milliseconds
                       - dwellTime in seconds @tablebulletend

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Modify_Geofences(
        qapi_loc_client_id clientId,
        size_t count,
        const uint32_t* pIDs,
        const qapi_Geofence_Option_t* options);

/**
*  Pauses a specified number of Geofences, which is 
         similar to qapi_Loc_Remove_Geofences() except that they can be resumed 
         at any time.
           collectiveResponseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successful
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a geofence session.

  @param[in] clientId  Client identifier for the location client.
  @param[in] count     Number of Geofences to be paused.
  @param[in] pIDs      Array of IDs of the Geofences to be paused.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Pause_Geofences(
        qapi_loc_client_id clientId,
        size_t count,
        const uint32_t* pIDs);

/**
*  Resumes a specified number of Geofences that are 
         paused.
           collectiveResponseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successful
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a geofence session.

  @param[in] clientId  Client identifier for the location client.
  @param[in] count     Number of Geofences to be resumed.
  @param[in] pIDs      Array of IDs of the Geofences to be resumed.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Resume_Geofences(
        qapi_loc_client_id clientId,
        size_t count,
        const uint32_t* pIDs);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Deprecated. @tblendline
 * }
 * Attempts a single location fix. It returns a session ID that 
         will be used by qapi_Loc_Cancel_Single_Shot API and in 
         the response callback to match the command with a response.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successfully started.
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no singleShotCb passed in qapi_Loc_Init().
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if anyparameter is invalid.
         If responseCb reports LOCATION_ERROR_SUCESS, then the following is what
         can happen to end the single shot session:
         1) A location will be reported on the singleShotCb.
         2) QAPI_LOCATION_ERROR_TIMEOUT will be reported on the singleShotCb.
         3) The single shot session is canceled using the qapi_Loc_Cancel_Single_Shot API
         In either of these cases, the session is considered complete and the session id will
         no longer be valid.

  @param[in]  clientId    Client identifier for the location client.
  @param[in]  powerLevel  Indicates what available technologies to use to compute the location.
  @param[out] pSessionId  Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.

  @deprecated Use API qapi_Loc_Get_Single_Shot_V2() instead.
*/
qapi_Location_Error_t qapi_Loc_Get_Single_Shot(
    qapi_loc_client_id clientId,
    qapi_Location_Power_Level_t powerLevel,
    uint32_t* pSessionId);

/**
*  Cancels a single shot session.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if successful.
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a single shot session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the single shot session to be cancelled.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Cancel_Single_Shot(
    qapi_loc_client_id clientId,
    uint32_t sessionId);

/**
*  Starts a Get GNSS data session, which returns a 
         session ID that will be used by the qapi_Loc_Stop_Get_Gnss_Data() API and in 
         the response callback to match the command with a response. GNSS data is 
         reported on the GNSS data callback passed in qapi_Loc_Init() periodically
         (every second until qapi_Loc_Stop_Get_Gnss_Data() is called). 
		 
           responseCb returns: \n
                QAPI_LOCATION_ERROR_SUCCESS if session was successfully started. \n
                QAPI_LOCATION_ERROR_ALREADY_STARTED if a qapi_Loc_Start_Get_Gnss_Data() session is already in progress.	\n
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no gnssDataCb was passed in qapi_Loc_Init().

  @param[in] clientId     Client identifier for the location client.
  @param[out] pSessionId  Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Start_Get_Gnss_Data(
        qapi_loc_client_id clientId,
        uint32_t* pSessionId);

/**
*  Stops a Get GNSS data session associated with the ID 
         parameter.
		 
           responseCb returns: \n
                QAPI_LOCATION_ERROR_SUCCESS if successful. \n
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a Get GNSS data session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the session to be stopped.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No location session has
            been initialized.
*/
qapi_Location_Error_t qapi_Loc_Stop_Get_Gnss_Data(
        qapi_loc_client_id clientId,
        uint32_t sessionId);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Requests the GNSS engine to provide a backoff time value.
         This backoff value indicates when the client can expect
         the next location callback based on the ongoing sessions.
         The backoff value is provided via qapi_Location_Meta_Data_Callback
         if registered by the client.
         It returns a session ID that will be sent in response callback
         to match the command with a response.

  @param[in]  clientId    Client identifier for the location client.
  @param[out] pSessionId  Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_ID_UNKNOWN -- Invalid client id provided. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No user space buffer is set. \n
  QAPI_LOCATION_ERROR_CALLBACK_MISSING -- Metadata callback is not set. \n
  QAPI_LOCATION_ERROR_GENERAL_FAILURE -- Internal failure while processing
                                         the request.
*/
qapi_Location_Error_t qapi_Loc_Get_Backoff_Time(
        qapi_loc_client_id clientId,
        uint32_t* pSessionId);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Fetches the best available position with the GNSS Engine.
         It returns a session ID that will be sent in response callback
         to match the command with a response.
           responseCb returns:
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no singleShotCb passed in qapi_Loc_Init().
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if any parameter is invalid.
                QAPI_LOCATION_ERROR_SUCCESS if request was successfully placed to GNSS Engine.
         If responseCb reports LOCATION_ERROR_SUCESS, then the following is what
         can happen:
         1) A location will be reported on the singleShotCb.
            The location object would have QAPI_LOCATION_IS_BEST_AVAIL_POS_BIT
            set in the flags field to indicate that this position is the best
            available position and not a response to the singleshot request.
            This location can have a large accuracy value, which must be
            checked by the client if it suffices it's purpose.

  @param[in]  clientId    Client identifier for the location client.
  @param[out] pSessionId  Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_ID_UNKNOWN -- Invalid client id provided. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No user space buffer is set. \n
  QAPI_LOCATION_ERROR_CALLBACK_MISSING -- Singleshot callback is not set. \n
  QAPI_LOCATION_ERROR_GENERAL_FAILURE -- Internal failure while processing
                                         the request.
*/
qapi_Location_Error_t qapi_Loc_Get_Best_Available_Position(
        qapi_loc_client_id clientId,
        uint32_t* pSessionId);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Attempts a single location fix. It returns a session ID that
         will be used by qapi_Loc_Cancel_Single_Shot API and in
         the response callback to match the command with a response.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successfully started.
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no singleShotCb passed in qapi_Loc_Init().
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if anyparameter is invalid.
         If responseCb reports LOCATION_ERROR_SUCESS, then there are below 3 possible ends
         to this singleshot request:
         1) A location will be reported on the singleShotCb.
         2) QAPI_LOCATION_ERROR_TIMEOUT will be reported on the singleShotCb.
         3) The single shot session is canceled using the qapi_Loc_Cancel_Single_Shot API
         In all cases, the session is considered complete and the session id will
         no longer be valid.

  @param[in]  clientId    Client identifier for the location client.
  @param[in]  pOptions    Pointer to a structure containing the options:
                          - powerLevel  Indicates what level of power consumption is acceptable
                                        while computing the fix.
                          - accuracyLevel Indicates what level of accuracy is desired.
  @param[out] pSessionId  Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_ID_UNKNOWN -- Invalid client id provided. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No user space buffer is set. \n
  QAPI_LOCATION_ERROR_CALLBACK_MISSING -- Singleshot callback is not set. \n
  QAPI_LOCATION_ERROR_GENERAL_FAILURE -- Internal failure while processing
                                         the request.
*/
qapi_Location_Error_t qapi_Loc_Get_Single_Shot_V2(
    qapi_loc_client_id clientId,
    const qapi_Singleshot_Options_t* pOptions,
    uint32_t* pSessionId);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Starts a motion tracking session, which returns a
         session ID that will be used by the other tracking APIs and in
         the response callback to match the command with a response.
         Motion Info is reported on the motion tracking callback passed
         in qapi_Loc_Init() when a motion is detected matching the motion
         options specified in this API.
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if session was successfully started.
                QAPI_LOCATION_ERROR_CALLBACK_MISSING if no motionTrackingCb was passed in
                                                     qapi_Loc_Init().
                QAPI_LOCATION_ERROR_INVALID_PARAMETER if pOptions parameter is invalid.

  @param[in]  clientId     Client identifier for the location client.
  @param[in]  pOptions     Pointer to a structure containing the motion detection options.
  @param[out] pSessionId   Pointer to the session ID to be returned.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_ID_UNKNOWN -- Invalid client id provided. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No user space buffer is set. \n
  QAPI_LOCATION_ERROR_CALLBACK_MISSING -- Motion tracking callback is not set. \n
  QAPI_LOCATION_ERROR_GENERAL_FAILURE -- Internal failure while processing
                                         the request.
*/
qapi_Location_Error_t qapi_Loc_Start_Motion_Tracking(
        qapi_loc_client_id clientId,
        const qapi_Location_Motion_Options_t* pOptions,
        uint32_t* pSessionId);

/**
 * @versiontable{2.0,2.45,
 * Location 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Stops a motion tracking session associated with the id
         parameter
           responseCb returns:
                QAPI_LOCATION_ERROR_SUCCESS if successful.
                QAPI_LOCATION_ERROR_ID_UNKNOWN if clientId is not associated with a tracking session.

  @param[in] clientId   Client identifier for the location client.
  @param[in] sessionId  ID of the session to be stopped.

  @return
  QAPI_LOCATION_ERROR_SUCCESS -- The operation was successful. \n
  QAPI_LOCATION_ERROR_ID_UNKNOWN -- Invalid client id or session id provided. \n
  QAPI_LOCATION_ERROR_NOT_INITIALIZED -- No user space buffer is set. \n
  QAPI_LOCATION_ERROR_GENERAL_FAILURE -- Internal failure while processing
                                         the request.
*/
qapi_Location_Error_t qapi_Loc_Stop_Motion_Tracking(
        qapi_loc_client_id clientId,
        uint32_t sessionId);

#endif // QAPI_TXM_MODULE

/** @} */ /* end_addtogroup qapi_location */

#ifdef __cplusplus
}
#endif

#endif /* _QAPI_LOCATION_H_ */
