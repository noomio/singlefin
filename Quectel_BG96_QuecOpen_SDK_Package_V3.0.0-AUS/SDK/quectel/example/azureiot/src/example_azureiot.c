/******************************************************************************
*@file     example_azureiot.c
*@brief   Detection of network state and notify the main task to start mqtt session with 
*            Azure iot hub.
*  ---------------------------------------------------------------------------
*
*  Copyright (c) 2018 Quectel Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Quectel Technologies, Inc.
*  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================
						   Header file
===========================================================================*/
#if defined(__EXAMPLE_AZUREIOT__)
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
//#include <errno.h>
#include <string.h>
#include <stdarg.h>

//#include "testrunnerswitcher.h"
#include "iothub_client_ll.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xlogging.h"

#include "serializer.h"
#include "iothub_client_ll.h"
#include "iothubtransportmqtt.h"
#include "iothub_client_core_common.h"

#include "qapi_dss.h"
#include "qapi_status.h"
#include "qapi_netservices.h"
#include "qapi_fs_types.h"
#include "qapi_fs.h"
#include "qapi_socket.h"
#include "qapi_dnsc.h"
#include "qapi_uart.h"
#include "qapi_timer.h"
#include "qapi_diag.h"

#include "quectel_utils.h"
#include "quectel_uart_apis.h"
#include "example_azureiot.h"

//<Azure SDK Header file>
#include "iothub.h"
#include "iothub_device_client.h"
#include "iothub_client_options.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "parson.h"
//</Azure SDK Header file>

/*===========================================================================
                             DEFINITION
===========================================================================*/
#define QL_DEF_APN	        "CMNET"
#define DSS_ADDR_INFO_SIZE	5
#define DSS_ADDR_SIZE       16

#define GET_ADDR_INFO_MIN(a, b) ((a) > (b) ? (b) : (a))

#define QUEC_TCP_UART_DBG
#ifdef QUEC_TCP_UART_DBG
#define TCP_UART_DBG(...)	\
{\
	tcp_uart_debug_print(__VA_ARGS__);	\
}
#else
#define TCP_UART_DBG(...)
#endif

#define THREAD_STACK_SIZE    (1024 * 16)
#define THREAD_PRIORITY      (180)
#define BYTE_POOL_SIZE       (1024 * 16)

#define CMD_BUF_SIZE  100

#define DEFAULT_PUB_TIME 5

#define MAX_LOG_MSG_SIZE 128

#define MESSAGERESPONSE(code, message) const char deviceMethodResponse[] = message; \
	*response_size = sizeof(deviceMethodResponse) - 1;                              \
	*response = malloc(*response_size);                                             \
	(void)memcpy(*response, deviceMethodResponse, *response_size);                  \
	result = code;                                                                  \

#define FIRMWARE_UPDATE_STATUS_VALUES \
    DOWNLOADING,                      \
    APPLYING,                         \
    REBOOTING,                        \
    IDLE                              \

/*Enumeration specifying firmware update status */
DEFINE_ENUM(FIRMWARE_UPDATE_STATUS, FIRMWARE_UPDATE_STATUS_VALUES);
DEFINE_ENUM_STRINGS(FIRMWARE_UPDATE_STATUS, FIRMWARE_UPDATE_STATUS_VALUES);


/*===========================================================================
                           Global variable
===========================================================================*/
/* TCPClient dss thread handle */
#ifdef QAPI_TXM_MODULE
	static TX_THREAD *dss_thread_handle; 
#else
	static TX_THREAD _dss_thread_handle;
	static TX_THREAD *ts_thread_handle = &_dss_thread_handle;
#endif

static unsigned char tcp_dss_stack[THREAD_STACK_SIZE];

TX_EVENT_FLAGS_GROUP *tcp_signal_handle;

qapi_DSS_Hndl_t tcp_dss_handle = NULL;	            /* Related to DSS netctrl */

static char apn[QUEC_APN_LEN];					/* APN */
static char apn_username[QUEC_APN_USER_LEN];	/* APN username */
static char apn_passwd[QUEC_APN_PASS_LEN];		/* APN password */

/* @Note: If netctrl library fail to initialize, set this value will be 1,
 * We should not release library when it is 1. 
 */
signed char tcp_netctl_lib_status = DSS_LIB_STAT_INVALID_E;
unsigned char tcp_datacall_status = DSS_EVT_INVALID_E;

TX_BYTE_POOL *byte_pool_tcp;
#define TCP_BYTE_POOL_SIZE		10*8*1024
UCHAR free_memory_tcp[TCP_BYTE_POOL_SIZE];

/* uart rx tx buffer */
static char *uart_rx_buff = NULL;	/*!!! should keep this buffer as 4K Bytes */
static char *uart_tx_buff = NULL;

/* uart config para*/
static QT_UART_CONF_PARA uart_conf =
{
	NULL,
	QT_UART_PORT_02,
	NULL,
	0,
	NULL,
	0,
	115200
};

/* Paste in your device connection string  */
const char *connect_string = "HostName=IOTTHRDXDATA.azure-devices.net;DeviceId=LINUX_SIMU;SharedAccessKey=KLPkYoLdcTiz6W1nNHontbVd5XMpBgKEvINTg5Yze6A=";

IOTHUB_CLIENT_LL_HANDLE device_handle = NULL;
static char msgText[1024];
static size_t g_message_count_send_confirmations = 0;
static const char* initialFirmwareVersion = "1.0.0";

// <datadefinition>
typedef struct MESSAGESCHEMA_TAG
{
	char* name;
	char* format;
	char* fields;
} MessageSchema;

typedef struct TELEMETRYSCHEMA_TAG
{
	char* interval;
	char* messageTemplate;
	MessageSchema messageSchema;
} TelemetrySchema;

typedef struct TELEMETRYPROPERTIES_TAG
{
	TelemetrySchema temperatureSchema;
	TelemetrySchema humiditySchema;
	TelemetrySchema pressureSchema;
} TelemetryProperties;

typedef struct CHILLER_TAG
{
	// Reported properties
	char* protocol;
	char* supportedMethods;
	char* type;
	char* firmware;
	FIRMWARE_UPDATE_STATUS firmwareUpdateStatus;
	char* location;
	double latitude;
	double longitude;
	TelemetryProperties telemetry;

	// Manage firmware update process
	char* new_firmware_version;
	char* new_firmware_URI;
} Chiller;
// </datadefinition>

void quec_uart_debug(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...)
{
    char log_buf[ MAX_LOG_MSG_SIZE ]; 										\
    va_list ap;
    va_start( ap, format );

    vsnprintf( log_buf, MAX_LOG_MSG_SIZE -1 , format, ap );

    va_end( ap );
    TCP_UART_DBG("%s\r\n",log_buf);
}

static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
	(void)reason;
	(void)user_context;
    
	// This sample DOES NOT take into consideration network outages.
	if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
	{
		TCP_UART_DBG("The device client is connected to iothub\r\n");
	}
	else
	{
		TCP_UART_DBG("The device client has been disconnected\r\n");
	}
}

/*  Converts the Chiller object into a JSON blob with reported properties ready to be sent across the wire as a twin. */
static char* serializeToJson(Chiller* chiller)
{
	char* result;

	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);

	// Only reported properties:
	(void)json_object_set_string(root_object, "Protocol", chiller->protocol);
	(void)json_object_set_string(root_object, "SupportedMethods", chiller->supportedMethods);
	(void)json_object_set_string(root_object, "Type", chiller->type);
	(void)json_object_set_string(root_object, "Firmware", chiller->firmware);
	(void)json_object_set_string(root_object, "FirmwareUpdateStatus", ENUM_TO_STRING(FIRMWARE_UPDATE_STATUS, chiller->firmwareUpdateStatus));
	//(void)json_object_set_string(root_object, "FirmwareUpdateStatus", ENUM_TO_STRING(IOTHUB_CLIENT_RESULTX, IOTHUB_CLIENT_INVALID_ARGX));
	(void)json_object_set_string(root_object, "Location", chiller->location);
	(void)json_object_set_number(root_object, "Latitude", chiller->latitude);
	(void)json_object_set_number(root_object, "Longitude", chiller->longitude);
	(void)json_object_dotset_string(root_object, "Telemetry.TemperatureSchema.Interval", chiller->telemetry.temperatureSchema.interval);
	(void)json_object_dotset_string(root_object, "Telemetry.TemperatureSchema.MessageTemplate", chiller->telemetry.temperatureSchema.messageTemplate);
	(void)json_object_dotset_string(root_object, "Telemetry.TemperatureSchema.MessageSchema.Name", chiller->telemetry.temperatureSchema.messageSchema.name);
	(void)json_object_dotset_string(root_object, "Telemetry.TemperatureSchema.MessageSchema.Format", chiller->telemetry.temperatureSchema.messageSchema.format);
	(void)json_object_dotset_string(root_object, "Telemetry.TemperatureSchema.MessageSchema.Fields", chiller->telemetry.temperatureSchema.messageSchema.fields);
	(void)json_object_dotset_string(root_object, "Telemetry.HumiditySchema.Interval", chiller->telemetry.humiditySchema.interval);
	(void)json_object_dotset_string(root_object, "Telemetry.HumiditySchema.MessageTemplate", chiller->telemetry.humiditySchema.messageTemplate);
	(void)json_object_dotset_string(root_object, "Telemetry.HumiditySchema.MessageSchema.Name", chiller->telemetry.humiditySchema.messageSchema.name);
	(void)json_object_dotset_string(root_object, "Telemetry.HumiditySchema.MessageSchema.Format", chiller->telemetry.humiditySchema.messageSchema.format);
	(void)json_object_dotset_string(root_object, "Telemetry.HumiditySchema.MessageSchema.Fields", chiller->telemetry.humiditySchema.messageSchema.fields);
	(void)json_object_dotset_string(root_object, "Telemetry.PressureSchema.Interval", chiller->telemetry.pressureSchema.interval);
	(void)json_object_dotset_string(root_object, "Telemetry.PressureSchema.MessageTemplate", chiller->telemetry.pressureSchema.messageTemplate);
	(void)json_object_dotset_string(root_object, "Telemetry.PressureSchema.MessageSchema.Name", chiller->telemetry.pressureSchema.messageSchema.name);
	(void)json_object_dotset_string(root_object, "Telemetry.PressureSchema.MessageSchema.Format", chiller->telemetry.pressureSchema.messageSchema.format);
	(void)json_object_dotset_string(root_object, "Telemetry.PressureSchema.MessageSchema.Fields", chiller->telemetry.pressureSchema.messageSchema.fields);

	result = json_serialize_to_string(root_value);

	json_value_free(root_value);

	return result;
}


static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	(void)userContextCallback;
	g_message_count_send_confirmations++;
	TCP_UART_DBG("Confirmation callback received for message %zu with result %s\r\n", g_message_count_send_confirmations, ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

static void reported_state_callback(int status_code, void* userContextCallback)
{
	(void)userContextCallback;
	TCP_UART_DBG("Device Twin reported properties update completed with result: %d\r\n", status_code);
}

static void sendChillerReportedProperties(Chiller* chiller)
{
	if (device_handle != NULL)
	{
		char* reportedProperties = serializeToJson(chiller);
		(void)IoTHubDeviceClient_SendReportedState(device_handle, (const unsigned char*)reportedProperties, strlen(reportedProperties), reported_state_callback, NULL);
		free(reportedProperties);
	}
}

// <firmwareupdate>
/*
 This is a thread allocated to process a long-running device method call.
 It uses device twin reported properties to communicate status values
 to the Remote Monitoring solution accelerator.
*/
static int do_firmware_update(void *param)
{
	Chiller *chiller = (Chiller *)param;
	TCP_UART_DBG("Running simulated firmware update: URI: %s, Version: %s\r\n", chiller->new_firmware_URI, chiller->new_firmware_version);

	TCP_UART_DBG("Simulating download phase...\r\n");
	chiller->firmwareUpdateStatus = DOWNLOADING;
	sendChillerReportedProperties(chiller);

	ThreadAPI_Sleep(5000);

	TCP_UART_DBG("Simulating apply phase...\r\n");
	chiller->firmwareUpdateStatus = APPLYING;
	sendChillerReportedProperties(chiller);

	ThreadAPI_Sleep(5000);

	TCP_UART_DBG("Simulating reboot phase...\r\n");
	chiller->firmwareUpdateStatus = REBOOTING;
	sendChillerReportedProperties(chiller);

	ThreadAPI_Sleep(5000);

	size_t size = strlen(chiller->new_firmware_version) + 1;
	(void)memcpy(chiller->firmware, chiller->new_firmware_version, size);

	chiller->firmwareUpdateStatus = IDLE;
	sendChillerReportedProperties(chiller);

	return 0;
}
// </firmwareupdate>

void getFirmwareUpdateValues(Chiller* chiller, const unsigned char* payload)
{
	free(chiller->new_firmware_version);
	free(chiller->new_firmware_URI);
	chiller->new_firmware_URI = NULL;
	chiller->new_firmware_version = NULL;

	JSON_Value* root_value = json_parse_string((char *)payload);
	JSON_Object* root_object = json_value_get_object(root_value);

	JSON_Value* newFirmwareVersion = json_object_get_value(root_object, "Firmware");

	if (newFirmwareVersion != NULL)
	{
		const char* data = json_value_get_string(newFirmwareVersion);
		if (data != NULL)
		{
			size_t size = strlen(data) + 1;
			chiller->new_firmware_version = malloc(size);
			(void)memcpy(chiller->new_firmware_version, data, size);
		}
	}

	JSON_Value* newFirmwareURI = json_object_get_value(root_object, "FirmwareUri");

	if (newFirmwareURI != NULL)
	{
		const char* data = json_value_get_string(newFirmwareURI);
		if (data != NULL)
		{
			size_t size = strlen(data) + 1;
			chiller->new_firmware_URI = malloc(size);
			(void)memcpy(chiller->new_firmware_URI, data, size);
		}
	}

	// Free resources
	json_value_free(root_value);

}

// <devicemethodcallback>
static int device_method_callback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback)
{
	Chiller *chiller = (Chiller *)userContextCallback;

	int result;

	TCP_UART_DBG("Direct method name:    %s\r\n", method_name);

	TCP_UART_DBG("Direct method payload: %.*s\r\n", (int)size, (const char*)payload);

	if (strcmp("Reboot", method_name) == 0)
	{
		MESSAGERESPONSE(201, "{ \"Response\": \"Rebooting\" }")
	}
	else if (strcmp("EmergencyValveRelease", method_name) == 0)
	{
		MESSAGERESPONSE(201, "{ \"Response\": \"Releasing emergency valve\" }")
	}
	else if (strcmp("IncreasePressure", method_name) == 0)
	{
		MESSAGERESPONSE(201, "{ \"Response\": \"Increasing pressure\" }")
	}
	else if (strcmp("FirmwareUpdate", method_name) == 0)
	{
		if (chiller->firmwareUpdateStatus != IDLE)
		{
			TCP_UART_DBG("Attempt to invoke firmware update out of order\r\n");
			MESSAGERESPONSE(400, "{ \"Response\": \"Attempting to initiate a firmware update out of order\" }")
		}
		else
		{
			getFirmwareUpdateValues(chiller, payload);

			if (chiller->new_firmware_version != NULL && chiller->new_firmware_URI != NULL)
			{
				// Create a thread for the long-running firmware update process.
				THREAD_HANDLE thread_apply;
				THREADAPI_RESULT t_result = ThreadAPI_Create(&thread_apply, do_firmware_update, chiller);
				if (t_result == THREADAPI_OK)
				{
					TCP_UART_DBG("Starting firmware update thread\r\n");
					MESSAGERESPONSE(201, "{ \"Response\": \"Starting firmware update thread\" }")
				}
				else
				{
					TCP_UART_DBG("Failed to start firmware update thread\r\n");
					MESSAGERESPONSE(500, "{ \"Response\": \"Failed to start firmware update thread\" }")
				}
			}
			else
			{
				TCP_UART_DBG("Invalid method payload\r\n");
				MESSAGERESPONSE(400, "{ \"Response\": \"Invalid payload\" }")
			}
		}
	}
	else
	{
		// All other entries are ignored.
		TCP_UART_DBG("Method not recognized\r\n");
		MESSAGERESPONSE(400, "{ \"Response\": \"Method not recognized\" }")
	}

	return result;
}
// </devicemethodcallback>

// <sendmessage>
static void send_message(IOTHUB_DEVICE_CLIENT_HANDLE handle, char* message, char* schema)
{
	IOTHUB_MESSAGE_HANDLE message_handle = IoTHubMessage_CreateFromString(message);

	// Set system properties
	(void)IoTHubMessage_SetMessageId(message_handle, "MSG_ID");
	(void)IoTHubMessage_SetCorrelationId(message_handle, "CORE_ID");
	(void)IoTHubMessage_SetContentTypeSystemProperty(message_handle, "application%2fjson");
	(void)IoTHubMessage_SetContentEncodingSystemProperty(message_handle, "utf-8");

	// Set application properties
	MAP_HANDLE propMap = IoTHubMessage_Properties(message_handle);
	(void)Map_AddOrUpdate(propMap, "$$MessageSchema", schema);
	(void)Map_AddOrUpdate(propMap, "$$ContentType", "JSON");

	time_t now = time(&now);
	struct tm* timeinfo;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) /* Suppress warning about possible unsafe function in Visual Studio */
#endif
	timeinfo = gmtime(&now);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	char timebuff[50];
	//strftime(timebuff, 50, "%Y-%m-%dT%H:%M:%SZ", timeinfo);
    sprintf_s(timebuff, sizeof(timebuff), "%d-%d-%dT%d:%d:%dZ",
                                            timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,
                                            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    TCP_UART_DBG("QT# time is %s\r\n", timebuff);
	(void)Map_AddOrUpdate(propMap, "$$CreationTimeUtc", timebuff);
	IoTHubDeviceClient_SendEventAsync(handle, message_handle, send_confirm_callback, NULL);
	IoTHubMessage_Destroy(message_handle);
}

void start_azureiot_session()
{
    int ret = 0;
	double minTemperature = 50.0;
	double minPressure = 55.0;
	double minHumidity = 30.0;
	double temperature = 0;
	double pressure = 0;
	double humidity = 0;

	srand((unsigned int)time(NULL));
    xlogging_set_log_function(&quec_uart_debug);

    // Used to initialize sdk subsystem
	IoTHub_Init();

    if ((device_handle = IoTHubDeviceClient_CreateFromConnectionString(connect_string, MQTT_Protocol)) == NULL)
    {
        TCP_UART_DBG("QT# device_handle is NULL\r\n");
    }
    else
    {
		// Setting connection status callback to get indication of connection to iothub
		(void)IoTHubDeviceClient_SetConnectionStatusCallback(device_handle, connection_status_callback, NULL);
        ThreadAPI_Sleep(5000);
        
		Chiller chiller;
		memset(&chiller, 0, sizeof(Chiller));
		chiller.protocol = "MQTT";
		chiller.supportedMethods = "Reboot,FirmwareUpdate,EmergencyValveRelease,IncreasePressure";
		chiller.type = "Chiller";
		size_t size = strlen(initialFirmwareVersion) + 1;
		chiller.firmware = malloc(size);
		memcpy(chiller.firmware, initialFirmwareVersion, size);
		chiller.firmwareUpdateStatus = IDLE;
		chiller.location = "Building 44";
		chiller.latitude = 47.638928;
		chiller.longitude = -122.13476;
		chiller.telemetry.temperatureSchema.interval = "00:00:05";
		chiller.telemetry.temperatureSchema.messageTemplate = "{\"temperature\":${temperature},\"temperature_unit\":\"${temperature_unit}\"}";
		chiller.telemetry.temperatureSchema.messageSchema.name = "chiller-temperature;v1";
		chiller.telemetry.temperatureSchema.messageSchema.format = "JSON";
		chiller.telemetry.temperatureSchema.messageSchema.fields = "{\"temperature\":\"Double\",\"temperature_unit\":\"Text\"}";
		chiller.telemetry.humiditySchema.interval = "00:00:05";
		chiller.telemetry.humiditySchema.messageTemplate = "{\"humidity\":${humidity},\"humidity_unit\":\"${humidity_unit}\"}";
		chiller.telemetry.humiditySchema.messageSchema.name = "chiller-humidity;v1";
		chiller.telemetry.humiditySchema.messageSchema.format = "JSON";
		chiller.telemetry.humiditySchema.messageSchema.fields = "{\"humidity\":\"Double\",\"humidity_unit\":\"Text\"}";
		chiller.telemetry.pressureSchema.interval = "00:00:05";
		chiller.telemetry.pressureSchema.messageTemplate = "{\"pressure\":${pressure},\"pressure_unit\":\"${pressure_unit}\"}";
		chiller.telemetry.pressureSchema.messageSchema.name = "chiller-pressure;v1";
		chiller.telemetry.pressureSchema.messageSchema.format = "JSON";
		chiller.telemetry.pressureSchema.messageSchema.fields = "{\"pressure\":\"Double\",\"pressure_unit\":\"Text\"}";

		sendChillerReportedProperties(&chiller);

		(void)IoTHubDeviceClient_SetDeviceMethodCallback(device_handle, device_method_callback, &chiller);

		while (1)
		{
			temperature = minTemperature + ((rand() % 10) + 5);
			pressure = minPressure + ((rand() % 10) + 5);
			humidity = minHumidity + ((rand() % 20) + 5);

			if (chiller.firmwareUpdateStatus == IDLE)
			{
				(void)sprintf_s(msgText, sizeof(msgText), "{\"temperature\":%d.%d,\"temperature_unit\":\"F\"}", 
				                                                ((int)temperature), ((int)(temperature*100))%100);
				TCP_UART_DBG("Sending %s\r\n", msgText);
                send_message(device_handle, msgText, chiller.telemetry.temperatureSchema.messageSchema.name);


				(void)sprintf_s(msgText, sizeof(msgText), "{\"pressure\":%d.%d,\"pressure_unit\":\"psig\"}", 
                				                                ((int)pressure), ((int)(pressure*100))%100);
				TCP_UART_DBG("Sending %s\r\n", msgText);
				send_message(device_handle, msgText, chiller.telemetry.pressureSchema.messageSchema.name);


				(void)sprintf_s(msgText, sizeof(msgText), "{\"humidity\":%d.%d,\"humidity_unit\":\"%%\"}", 
                				                                ((int)humidity), ((int)(humidity*100))%100);
				TCP_UART_DBG("Sending %s\r\n", msgText);
                send_message(device_handle, msgText, chiller.telemetry.humiditySchema.messageSchema.name);
            }

			ThreadAPI_Sleep(100000);
		}

		TCP_UART_DBG("\r\nShutting down\r\n");

		// Clean up the iothub sdk handle and free resources
		IoTHubDeviceClient_Destroy(device_handle);
		free(chiller.firmware);
		free(chiller.new_firmware_URI);
		free(chiller.new_firmware_version);
	}
}


void tcp_uart_dbg_init()
{
  	if (TX_SUCCESS != tx_byte_allocate(byte_pool_tcp, (VOID *)&uart_rx_buff, 4*1024, TX_NO_WAIT))
  	{
  		IOT_DEBUG("tx_byte_allocate [uart_rx_buff] failed!");
    	return;
  	}

  	if (TX_SUCCESS != tx_byte_allocate(byte_pool_tcp, (VOID *)&uart_tx_buff, 4*1024, TX_NO_WAIT))
  	{
  		IOT_DEBUG("tx_byte_allocate [uart_tx_buff] failed!");
    	return;
  	}

    uart_conf.rx_buff = uart_rx_buff;
	uart_conf.tx_buff = uart_tx_buff;
	uart_conf.tx_len = 4096;
	uart_conf.rx_len = 4096;

	/* debug uart init 			*/
	uart_init(&uart_conf);
	/* start uart receive */
	uart_recv(&uart_conf);
}
void tcp_uart_debug_print(const char* fmt, ...) 
{
	va_list arg_list;
    char dbg_buffer[128] = {0};
    
	va_start(arg_list, fmt);
    vsnprintf((char *)(dbg_buffer), sizeof(dbg_buffer), (char *)fmt, arg_list);
    va_end(arg_list);

    strcat(dbg_buffer,"\r\n");
    qapi_UART_Transmit(uart_conf.hdlr, dbg_buffer, strlen(dbg_buffer), NULL);
    qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);   //50
}

/*
@func
	dss_net_event_cb
@brief
	Initializes the DSS netctrl library for the specified operating mode.
*/
static void tcp_net_event_cb
( 
	qapi_DSS_Hndl_t 		hndl,
	void 				   *user_data,
	qapi_DSS_Net_Evt_t 		evt,
	qapi_DSS_Evt_Payload_t *payload_ptr 
)
{
	qapi_Status_t status = QAPI_OK;
	
	TCP_UART_DBG("Data test event callback, event: %d\n", evt);

	switch (evt)
	{
		case QAPI_DSS_EVT_NET_IS_CONN_E:
		{
			TCP_UART_DBG("Data Call Connected.\n");
			tcp_show_sysinfo();
			/* Signal main task */
  			tx_event_flags_set(tcp_signal_handle, DSS_SIG_EVT_CONN_E, TX_OR);
			tcp_datacall_status = DSS_EVT_NET_IS_CONN_E;
			
			break;
		}
		case QAPI_DSS_EVT_NET_NO_NET_E:
		{
			TCP_UART_DBG("Data Call Disconnected.\n");
			
			if (DSS_EVT_NET_IS_CONN_E == tcp_datacall_status)
			{
				/* Release Data service handle and netctrl library */
				if (tcp_dss_handle)
				{
					status = qapi_DSS_Rel_Data_Srvc_Hndl(tcp_dss_handle);
					if (QAPI_OK == status)
					{
						TCP_UART_DBG("Release data service handle success\n");
						tx_event_flags_set(tcp_signal_handle, DSS_SIG_EVT_EXIT_E, TX_OR);
					}
				}
				
				if (DSS_LIB_STAT_SUCCESS_E == tcp_netctl_lib_status)
				{
					qapi_DSS_Release(QAPI_DSS_MODE_GENERAL);
				}
			}
			else
			{
				/* DSS status maybe QAPI_DSS_EVT_NET_NO_NET_E before data call establishment */
				tx_event_flags_set(tcp_signal_handle, DSS_SIG_EVT_NO_CONN_E, TX_OR);
			}

			break;
		}
		default:
		{
			TCP_UART_DBG("Data Call status is invalid.\n");
			
			/* Signal main task */
  			tx_event_flags_set(tcp_signal_handle, DSS_SIG_EVT_INV_E, TX_OR);
			tcp_datacall_status = DSS_EVT_INVALID_E;
			break;
		}
	}
}

void tcp_show_sysinfo(void)
{
	int i   = 0;
	int j 	= 0;
	unsigned int len = 0;
	uint8 buff[DSS_ADDR_SIZE] = {0}; 
	qapi_Status_t status;
	qapi_DSS_Addr_Info_t info_ptr[DSS_ADDR_INFO_SIZE];

	status = qapi_DSS_Get_IP_Addr_Count(tcp_dss_handle, &len);
	if (QAPI_ERROR == status)
	{
		TCP_UART_DBG("Get IP address count error\n");
		return;
	}
		
	status = qapi_DSS_Get_IP_Addr(tcp_dss_handle, info_ptr, len);
	if (QAPI_ERROR == status)
	{
		TCP_UART_DBG("Get IP address error\n");
		return;
	}
	
	j = GET_ADDR_INFO_MIN(len, DSS_ADDR_INFO_SIZE);
	
	for (i = 0; i < j; i++)
	{
		TCP_UART_DBG("<--- static IP address information --->\n");
		tcp_inet_ntoa(info_ptr[i].iface_addr_s, buff, DSS_ADDR_SIZE);
		TCP_UART_DBG("static IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		tcp_inet_ntoa(info_ptr[i].gtwy_addr_s, buff, DSS_ADDR_SIZE);
		TCP_UART_DBG("Gateway IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		tcp_inet_ntoa(info_ptr[i].dnsp_addr_s, buff, DSS_ADDR_SIZE);
		TCP_UART_DBG("Primary DNS IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		tcp_inet_ntoa(info_ptr[i].dnss_addr_s, buff, DSS_ADDR_SIZE);
		TCP_UART_DBG("Second DNS IP: %s\n", buff);
	}

	TCP_UART_DBG("<--- End of system info --->\n");
}

/*
@func
	tcp_set_data_param
@brief
	Set the Parameter for Data Call, such as APN and network tech.
*/
static int tcp_set_data_param(void)
{
    qapi_DSS_Call_Param_Value_t param_info;
	
	/* Initial Data Call Parameter */
	memset(apn, 0, sizeof(apn));
	memset(apn_username, 0, sizeof(apn_username));
	memset(apn_passwd, 0, sizeof(apn_passwd));
	strlcpy(apn, QL_DEF_APN, QAPI_DSS_CALL_INFO_APN_MAX_LEN);

    if (NULL != tcp_dss_handle)
    {
        /* set data call param */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;	//Automatic mode(or DSS_RADIO_TECH_LTE)
        TCP_UART_DBG("Setting tech to Automatic\n");
        qapi_DSS_Set_Data_Call_Param(tcp_dss_handle, QAPI_DSS_CALL_INFO_TECH_PREF_E, &param_info);

		/* set apn */
        param_info.buf_val = apn;
        param_info.num_val = strlen(apn);
        TCP_UART_DBG("Setting APN - %s\n", apn);
        qapi_DSS_Set_Data_Call_Param(tcp_dss_handle, QAPI_DSS_CALL_INFO_APN_NAME_E, &param_info);
#ifdef QUEC_CUSTOM_APN
		/* set apn username */
		param_info.buf_val = apn_username;
        param_info.num_val = strlen(apn_username);
        TCP_UART_DBG("Setting APN USER - %s\n", apn_username);
        qapi_DSS_Set_Data_Call_Param(tcp_dss_handle, QAPI_DSS_CALL_INFO_USERNAME_E, &param_info);

		/* set apn password */
		param_info.buf_val = apn_passwd;
        param_info.num_val = strlen(apn_passwd);
        TCP_UART_DBG("Setting APN PASSWORD - %s\n", apn_passwd);
        qapi_DSS_Set_Data_Call_Param(tcp_dss_handle, QAPI_DSS_CALL_INFO_PASSWORD_E, &param_info);
#endif
		/* set IP version(IPv4 or IPv6) */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_IP_VERSION_4;
        TCP_UART_DBG("Setting family to IPv%d\n", param_info.num_val);
        qapi_DSS_Set_Data_Call_Param(tcp_dss_handle, QAPI_DSS_CALL_INFO_IP_VERSION_E, &param_info);
    }
    else
    {
        TCP_UART_DBG("Dss handler is NULL!!!\n");
		return -1;
    }
	
    return 0;
}

/*
@func
	tcp_inet_ntoa
@brief
	utility interface to translate ip from "int" to x.x.x.x format.
*/
int32 tcp_inet_ntoa
(
	const qapi_DSS_Addr_t    inaddr, /* IPv4 address to be converted         */
	uint8                   *buf,    /* Buffer to hold the converted address */
	int32                    buflen  /* Length of buffer                     */
)
{
	uint8 *paddr  = (uint8 *)&inaddr.addr.v4;
	int32  rc = 0;

	if ((NULL == buf) || (0 >= buflen))
	{
		rc = -1;
	}
	else
	{
		if (-1 == snprintf((char*)buf, (unsigned int)buflen, "%d.%d.%d.%d",
							paddr[0],
							paddr[1],
							paddr[2],
							paddr[3]))
		{
			rc = -1;
		}
	}

	return rc;
} /* tcp_inet_ntoa() */

/*
@func
	tcp_netctrl_init
@brief
	Initializes the DSS netctrl library for the specified operating mode.
*/
static int tcp_netctrl_init(void)
{
	int ret_val = 0;
	qapi_Status_t status = QAPI_OK;

	TCP_UART_DBG("Initializes the DSS netctrl library\n");

	/* Initializes the DSS netctrl library */
	if (QAPI_OK == qapi_DSS_Init(QAPI_DSS_MODE_GENERAL))
	{
		tcp_netctl_lib_status = DSS_LIB_STAT_SUCCESS_E;
		TCP_UART_DBG("qapi_DSS_Init success\n");
	}
	else
	{
		/* @Note: netctrl library has been initialized */
		tcp_netctl_lib_status = DSS_LIB_STAT_FAIL_E;
		TCP_UART_DBG("DSS netctrl library has been initialized.\n");
	}
	
	/* Registering callback tcp_dss_handleR */
	do
	{
		TCP_UART_DBG("Registering Callback tcp_dss_handle\n");
		
		/* Obtain data service handle */
		status = qapi_DSS_Get_Data_Srvc_Hndl(tcp_net_event_cb, NULL, &tcp_dss_handle);
		TCP_UART_DBG("tcp_dss_handle %d, status %d\n", tcp_dss_handle, status);
		
		if (NULL != tcp_dss_handle)
		{
			TCP_UART_DBG("Registed tcp_dss_handler success\n");
			break;
		}

		/* Obtain data service handle failure, try again after 10ms */
		qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
	} while(1);

	return ret_val;
}

/*
@func
	tcp_netctrl_start
@brief
	Start the DSS netctrl library, and startup data call.
*/
int tcp_netctrl_start(void)
{
	int rc = 0;
	qapi_Status_t status = QAPI_OK;
		
	rc = tcp_netctrl_init();
	if (0 == rc)
	{
		/* Get valid DSS handler and set the data call parameter */
		tcp_set_data_param();
	}
	else
	{
		TCP_UART_DBG("quectel_dss_init fail.\n");
		return -1;
	}

	TCP_UART_DBG("qapi_DSS_Start_Data_Call start!!!.\n");
	status = qapi_DSS_Start_Data_Call(tcp_dss_handle);
	if (QAPI_OK == status)
	{
		TCP_UART_DBG("Start Data service success.\n");
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
@func
	tcp_netctrl_release
@brief
	Cleans up the DSS netctrl library and close data service.
*/
int tcp_netctrl_stop(void)
{
	qapi_Status_t stat = QAPI_OK;
	
	if (tcp_dss_handle)
	{
		stat = qapi_DSS_Stop_Data_Call(tcp_dss_handle);
		if (QAPI_OK == stat)
		{
			TCP_UART_DBG("Stop data call success\n");
		}
	}
	
	return 0;
}	

/*
@func
	dss_inet_ntoa
@brief
	utility interface to translate ip from "int" to x.x.x.x format.
*/
int32 dss_inet_ntoa
(
	const qapi_DSS_Addr_t    inaddr, /* IPv4 address to be converted         */
	uint8                   *buf,    /* Buffer to hold the converted address */
	int32                    buflen  /* Length of buffer                     */
)
{
	uint8 *paddr  = (uint8 *)&inaddr.addr.v4;
	int32  rc = 0;

	if ((NULL == buf) || (0 >= buflen))
	{
		rc = -1;
	}
	else
	{
		if (-1 == snprintf((char*)buf, (unsigned int)buflen, "%d.%d.%d.%d",
							paddr[0],
							paddr[1],
							paddr[2],
							paddr[3]))
		{
			rc = -1;
		}
	}

	return rc;
} /* tcp_inet_ntoa() */

void quec_config_dns_server()
{
	int e = 0;
	int i, j = 0;

	unsigned int len = 0;
	qapi_Status_t status;
	char iface[15] = {0};
	char first_dns[DSS_ADDR_SIZE] = {0};
	char second_dns[DSS_ADDR_SIZE] = {0};
	qapi_DSS_Addr_Info_t info_ptr[DSS_ADDR_INFO_SIZE];

	/* Get DNS server address */
	status = qapi_DSS_Get_IP_Addr_Count(tcp_dss_handle, &len);
	if (QAPI_ERROR == status)
	{
		TCP_UART_DBG("Get IP address count error\n");
		return;
	}
		
	status = qapi_DSS_Get_IP_Addr(tcp_dss_handle, info_ptr, len);
	if (QAPI_ERROR == status)
	{
		TCP_UART_DBG("Get IP address error\n");
		return;
	}
	
	j = GET_ADDR_INFO_MIN(len, DSS_ADDR_INFO_SIZE);
	
	for (i = 0; i < j; i++)
	{
		memset(first_dns, 0, sizeof(first_dns));
		dss_inet_ntoa(info_ptr[i].dnsp_addr_s, (uint8 *)first_dns, DSS_ADDR_SIZE);
		TCP_UART_DBG("Primary DNS IP: %s\n", first_dns);

		memset(second_dns, 0, sizeof(second_dns));
		dss_inet_ntoa(info_ptr[i].dnss_addr_s, (uint8 *)second_dns, DSS_ADDR_SIZE);
		TCP_UART_DBG("Second DNS IP: %s\n", second_dns);
	}

	/* Start DNS service */
	e = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
	TCP_UART_DBG("Start DNS service, ret: %d", e);

	/* Get current active iface */
	memset(iface, 0, sizeof(iface));
	qapi_DSS_Get_Device_Name(tcp_dss_handle, iface, 15);
	TCP_UART_DBG("device_name: %s\n", iface);
#if 0
	memset(iface, 0, sizeof(iface));
	qapi_DSS_Get_Qmi_Port_Name(dss_handle, iface, 15);
	DNS_UART_DBG("qmi_port_name: %s\n", iface);
#endif

	/* Add dns server into corresponding interface */
	qapi_Net_DNSc_Add_Server_on_iface(first_dns, QAPI_NET_DNS_V4_PRIMARY_SERVER_ID, iface);
	qapi_Net_DNSc_Add_Server_on_iface(second_dns, QAPI_NET_DNS_V4_SECONDARY_SERVER_ID, iface);

}

int qt_read_EFS_file(const char *name, void **buffer_ptr, size_t *buffer_size)
{
	int bytes_read;
	int efs_fd = -1;
	struct qapi_FS_Stat_Type_s stat;
	uint8 *file_buf = NULL;
	stat.st_size = 0;

	if ((!name) || (!buffer_ptr) ||(!buffer_size))
	{
		TCP_UART_DBG("Reading SSL from EFS file failed!\n");
		return QAPI_ERROR;
	}

	if (qapi_FS_Open(name, QAPI_FS_O_RDONLY_E, &efs_fd) < 0)
	{
		TCP_UART_DBG("Opening EFS EFS file %s failed\n", name);
		return QAPI_ERROR;
	}

	if (qapi_FS_Stat_With_Handle(efs_fd, &stat) < 0) 
	{
		TCP_UART_DBG("Reading EFS file %s failed\n", name);
		return QAPI_ERROR;
	}

	TCP_UART_DBG("Reading EFS file size %d \n", stat.st_size);
	
	tx_byte_allocate(byte_pool_tcp, (VOID **) &file_buf, stat.st_size, TX_NO_WAIT);
	if (file_buf  == NULL) 
	{
		TCP_UART_DBG("SSL_cert_store: QAPI_ERR_NO_MEMORY \n");
		return QAPI_ERR_NO_MEMORY;
	}

	qapi_FS_Read(efs_fd, file_buf, stat.st_size, &bytes_read);
	if ((bytes_read < 0) || (bytes_read != stat.st_size)) 
	{
		tx_byte_release(file_buf);
		TCP_UART_DBG("SSL_cert_store: Reading EFS file error\n");
		return QAPI_ERROR;
	}

	*buffer_ptr = file_buf;
	*buffer_size = stat.st_size;

	qapi_FS_Close(efs_fd);

	return QAPI_OK;
}

int quec_convert_ssl_certs()
{
    uint8_t *cert_Buf = NULL; 
    uint32_t cert_Size = 0;
    uint8_t *key_Buf = NULL;
    uint32_t key_Size = 0;
    int result;
    qapi_Net_SSL_Cert_Info_t cert_info;
    qapi_NET_SSL_CA_Info_t calist_info[QAPI_NET_SSL_MAX_CA_LIST];

    /* Read the CA certificate information */
    result = qt_read_EFS_file(AZURE_CA_CRT, (void **)&calist_info[0].ca_Buf, (size_t *)&calist_info[0].ca_Size);
    TCP_UART_DBG("Read %s, result %d\r\n", AZURE_CA_CRT, result);

    if (QAPI_OK != result) 
    {
         TCP_UART_DBG("ERROR: Reading ca information from EFS failed!! \r\n");
         result = QAPI_ERROR;
    }

    cert_info.info.ca_List.ca_Info[0] = &calist_info[0];
    cert_info.info.ca_List.ca_Cnt = 1;
    cert_info.cert_Type = QAPI_NET_SSL_CA_LIST_E;

    result = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, AZURE_CALIST_BIN);
    TCP_UART_DBG("%s qapi_Net_SSL_Cert_Convert_And_Store: %d\r\n", AZURE_CALIST_BIN, result);

    /* Read the client certificate information */
    result = qt_read_EFS_file((char *)AZURE_CLI_CRT, (void **)&cert_Buf, (size_t *)&cert_Size);
    TCP_UART_DBG("Read %s, result %d\r\n", AZURE_CLI_CRT, result);
    if (QAPI_OK != result) 
    {
         TCP_UART_DBG("ERROR: Reading client certificate from EFS failed!! \r\n");
         result = QAPI_ERROR;
    }

    /* Read the client key information */
    result = qt_read_EFS_file((char *)AZURE_CLI_KEY, (void **)&key_Buf, (size_t *)&key_Size);
    TCP_UART_DBG("Read %s, result %d\r\n", AZURE_CLI_KEY, result);
    if (QAPI_OK != result) 
    {
         TCP_UART_DBG("ERROR: Reading key information from EFS failed!! \r\n");
         result = QAPI_ERROR;
    }

    /* Update the client certificate information */
    cert_info.cert_Type = QAPI_NET_SSL_CERTIFICATE_E; 
    cert_info.info.cert.cert_Buf = cert_Buf;
    cert_info.info.cert.cert_Size = cert_Size;
    cert_info.info.cert.key_Buf = key_Buf;
    cert_info.info.cert.key_Size = key_Size;

    /* Convert and store the certificate */ 
    result = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, AZURE_CERT_BIN);
    TCP_UART_DBG("%s qapi_Net_SSL_Cert_Convert_And_Store: %d\r\n", AZURE_CERT_BIN, result);
    return result;
}

/*
@func
	quec_dataservice_entry
@brief
	The entry of data service task.
*/
void quec_dataservice_thread(ULONG param)
{
	ULONG dss_event = 0;
	
	/* Start data call */
	tcp_netctrl_start();

	while (1)
	{
		/* Wait disconnect signal */
		tx_event_flags_get(tcp_signal_handle, DSS_SIG_EVT_DIS_E, TX_OR, &dss_event, TX_WAIT_FOREVER);
		if (dss_event & DSS_SIG_EVT_DIS_E)
		{
			/* Stop data call and release resource */
			tcp_netctrl_stop();
			TCP_UART_DBG("Data service task exit.\n");
			break;
		}
	}

	TCP_UART_DBG("Data Service Thread Exit!\n");
	return;
}

int quectel_task_entry(void)
{
    int   ret = 0;
	ULONG dss_event = 0;
	int32 sig_mask;
    
    qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_SEC, true);
    IOT_DEBUG("QT# quectel_task_entry ...");

    /* Create a byte memory pool. */
    txm_module_object_allocate(&byte_pool_tcp, sizeof(TX_BYTE_POOL));
    tx_byte_pool_create(byte_pool_tcp, "tcp application pool", free_memory_tcp, TCP_BYTE_POOL_SIZE);

	/* Initial uart for debug */
	tcp_uart_dbg_init();
    TCP_UART_DBG("AzureIOT Task Start...\n");

    /* Create event signal handle and clear signals */
    txm_module_object_allocate(&tcp_signal_handle, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(tcp_signal_handle, "dss_signal_event");
	tx_event_flags_set(tcp_signal_handle, 0x0, TX_AND);

	if (TX_SUCCESS != txm_module_object_allocate((VOID *)&dss_thread_handle, sizeof(TX_THREAD))) 
	{
		return -1;
	}
	ret = tx_thread_create(dss_thread_handle, "AzureIOT DSS Thread", quec_dataservice_thread, NULL,
							tcp_dss_stack, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
	if (ret != TX_SUCCESS)
	{
		IOT_INFO("Thread creation failed\n");
	}

	sig_mask = DSS_SIG_EVT_INV_E | DSS_SIG_EVT_NO_CONN_E | DSS_SIG_EVT_CONN_E | DSS_SIG_EVT_EXIT_E;
    while(1)
    {
		/* TCPClient signal process */
		tx_event_flags_get(tcp_signal_handle, sig_mask, TX_OR, &dss_event, TX_WAIT_FOREVER);
		TCP_UART_DBG("SIGNAL EVENT IS [%d]\n", dss_event);		

		if (dss_event & DSS_SIG_EVT_INV_E)
		{
			TCP_UART_DBG("DSS_SIG_EVT_INV_E Signal\n");
			tx_event_flags_set(tcp_signal_handle, ~DSS_SIG_EVT_INV_E, TX_AND);
		}
		else if (dss_event & DSS_SIG_EVT_NO_CONN_E)
		{
			TCP_UART_DBG("DSS_SIG_EVT_NO_CONN_E Signal\n");
			tx_event_flags_set(tcp_signal_handle, ~DSS_SIG_EVT_NO_CONN_E, TX_AND);
		}
		else if (dss_event & DSS_SIG_EVT_CONN_E)
		{
			TCP_UART_DBG("DSS_SIG_EVT_CONN_E Signal\n");

            quec_convert_ssl_certs();
            quec_config_dns_server();

			start_azureiot_session();
            tx_event_flags_set(tcp_signal_handle, ~DSS_SIG_EVT_CONN_E, TX_AND);
		}
		else if (dss_event & DSS_SIG_EVT_EXIT_E)
		{
			TCP_UART_DBG("DSS_SIG_EVT_EXIT_E Signal\n");
			tx_event_flags_set(tcp_signal_handle, ~DSS_SIG_EVT_EXIT_E, TX_AND);
			tx_event_flags_delete(tcp_signal_handle);
			break;
		}
		else
		{
			TCP_UART_DBG("Unkonw Signal\n");
		}

		/* Clear all signals and wait next notification */
		tx_event_flags_set(tcp_signal_handle, 0x0, TX_AND);	//@Fixme:maybe not need
    }
}
#endif  //__EXAMPLE_AZUREIOT__
