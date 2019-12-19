#ifndef __QAPI_QUECTEL_H__
#define __QAPI_QUECTEL_H__

#include "tx_api.h"
#include "qapi_status.h"
#include "qapi_txm_base.h"
#include "qapi_adc_types.h"
#include "qapi_dss.h"

/********************* Quectel OEM error number defines **********************/
typedef enum{
	QAPI_QT_ERR_OK = 0x1000,
	QAPI_QT_ERR_NORMAL_FAIL,
	QAPI_QT_ERR_INVALID_PARAM,
	QAPI_QT_ERR_OPERATE_FAILED,
	QAPI_QT_ERR_NO_MEM,
	QAPI_QT_ERR_PORT_ID,
	QAPI_QT_ERR_STREAM_ID,
	QAPI_QT_ERR_LOC_STOPPED,
	QAPI_QT_ERR_LOC_STARTED,
	QAPI_QT_ERR_GEOFENCE_ID_NOT_EXIST,  //the GEO fence is not exist
	QAPI_QT_ERR_FOTA_INVALID_PKG,
	QAPI_QT_ERR_FOTA_NOT_IN_DOWNLOADING,

	/*  end  */
	QAPI_QT_ERR_END = 0x2000
}QAPI_QT_ERR_NUM;


/********************* Quectel Release Information **********************/
typedef struct{
	char build_time[32];
	char build_date[32];
	char rel_time[32];
	char rel_date[32];
	char sw_version[128];
	char ql_sw_ver[64];
	char ql_sw_ver_sub[16];
}qapi_QT_Rel_Info;

/********************* Quectel OEM defines **********************/

typedef enum
{
	QAPI_FATAL_ERR_RESET = 0,
	QAPI_FATAL_ERR_SAHARA = 1,
	QAPI_FATAL_ERR_MAX
}QAPI_FATAL_ERR_MODE;
/*******gps******/
typedef struct
{
    char nmea[201];
}qt_nmea_sentence;


/* --- ATC Pipe --- */
typedef enum qapi_at_port_status_e
{
	QAPI_PORT_OK = 0,
	QAPI_PORT_ERROR,
	QAPI_PORT_INV_ARG,
	QAPI_PORT_ERROR_SIO,
	QAPI_PORT_OPEN_FAILURE,
	QAPI_PORT_MAX
} qapi_at_port_status_t;

typedef enum qpi_at_port_e
{
	QAPI_AT_PORT_0 = 0,
	QAPI_AT_PORT_1,
	QAPI_AT_PORT_2,	//Reserved
	QAPI_AT_PORT_3,	//Reserved

	QAPI_AT_PORT_MAX
} qapi_at_port_t;

typedef struct qapi_atc_pipe_data_s
{
	char   data[2048];
	int    len;
} qapi_at_pipe_data_t; 

typedef signed short int2;

typedef int2	qapi_at_stream_id_t;
typedef void  (*qapi_at_resp_func_cb_t)(qapi_at_pipe_data_t *data);	//response callback


/********** 	EO FENCE START 	**********/
#define MAX_FENCE_NUM   10  //geo fence number
#define MAX_POINT_NUM   4   //the max number of Polygon's edge
typedef enum
{
    FENCE_BREACH_DISABLE       = 0,    //disable Geo-Fence
    FENCE_BREACH_ENTER         = 1,    //reports when enters the Geo-Fence
    FENCE_BREACH_LEAVE         = 2,    //reports when leaves the Geo-Fence
    FENCE_BREACH_ENTER_LEAVE   = 3,    //reports when enters or leaves the Geo-Fence
    FENCE_BREACH_MAX
}fence_breach_type;

typedef enum
{
    FENCE_SHAPE_C_R         = 0,    //circularity with center and radius
    FENCE_SHAPE_C_P         = 1,    //circularity with center and one point on the circle
    FENCE_SHAPE_TRIANGLE    = 2,    //triangle
    FENCE_SHAPE_QUADRANGLE  = 3,    //quadrangle
    FENCE_SHAPE_MAX
}fence_shap;


typedef enum
{
    FENCE_POSITION_UNKNOWN = 0,  //unknow postion
    FENCE_POSITION_INSIDE = 1,            //point is inside
    FENCE_POSITION_OUTSIDE = 2,            //point is outside
}fence_postion_type;

typedef struct
{
    double Latitude;    //be radius when shape is circle.
    double Longitude;
}st_point;

typedef struct
{   
    unsigned char id;
    unsigned char mode;
    unsigned char shape;
    st_point point[10];
}st_fence;


typedef enum {
    LOCATION_HAS_ALTITUDE_MASK =          (1 << 0),   /**< Location has a valid altitude. */
    LOCATION_HAS_SPEED_MASK =             (1 << 1),   /**< Location has a valid speed. */
    LOCATION_HAS_HEADING_MASK =           (1 << 2),   /**< Location has a valid bearing. */
    LOCATION_HAS_DOP_MASK =               (1 << 3),   /**< Location has a valid PDOP HDOP and VDOP. */
} location_mask;

typedef struct {

  uint16_t year;
  uint8_t month;
  uint8_t day_of_week;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t millisecond;
}utc_calendar_time_type; //utc time
typedef struct {
    location_mask flags; // bitwise OR of location_mask to mark which params are valid
    utc_calendar_time_type calendar_time;   // UTC time for location fix
    double latitude;         // in degrees,  range [-90.0, 90]
    double longitude;        // in degrees,  range [-180, 180]
    double altitude;         // in meters, altitude with regard to mean sea level
    float speed;             // in meters per second
    float heading;           // in degrees; range [0, 360]
    float PDOP;
    float HDOP;
    float VDOP;
} report_location;

typedef struct{
    unsigned char id;  //range [0, 9]
    unsigned char mode; // fence_breach_type
    report_location report_position;
}geo_fence_notification;

/********** 	GEO FENCE END 	**********/

/********** 	GNSS START 	**********/
typedef uint64_t qtLocEventRegMaskT;
#define QT_LOC_EVENT_MASK_POSITION_REPORT 		((qtLocEventRegMaskT)0x00000001ull)
#define QT_LOC_EVENT_MASK_GNSS_SV_INFO 			((qtLocEventRegMaskT)0x00000002ull)
#define QT_LOC_EVENT_MASK_NMEA 					((qtLocEventRegMaskT)0x00000004ull)

typedef void (*qapi_QT_Loc_CB_t)(qt_nmea_sentence *nmea_data);	//response callback
/********** 	GNSS END 	**********/

/********** 	Fota START 	**********/
/* 
*error_id:[out]
*0         Download successful
*701       HTTP(S) unknown error
*others    unknown error
*/
typedef void(*qapi_Fota_Http_dl_CB_t)(int error_id);

/* 
*error_id:[out]
*0         upgrade success 
*504       firmware upgrade failed
*505       upgrade package not exist   
*506       upgrade package check failed
*others    unknown error
*/
typedef void (*qapi_QT_Fota_Response_CB_t)(short int error_id);	//response callback

/* 
*phase: [out]
*0  upgrading period
*1  restoring period
*Note: The fota upgrading start with upgrading period. When upgrading progress reach to 100%, 
*      it will become to restoring period. When restoring progress reach to 100%, It means that upgrading is successful. 
*      Then the module should be restarted.
*      
*percent:[out]
*upgrade progress, range:[1-100]
*/
typedef void (*qapi_QT_Fota_Upgrade_Progress_CB_t)(unsigned char phase, unsigned char percent);	//response callback

/********** 	Fota END 	**********/


/********************* Quectel OEM ID defines **********************/
/* @Note: Each new OEM ID needs to be defined in this enum */
typedef enum
{
	TXM_OEM_API_QT_BASE = TXM_QC_OEM_API_BASE,	/**<-Quectel TXM API IDs Start with 0x20000->*/
	TXM_OEM_API_QT_REL_INFO,
	TXM_OEM_API_QT_RESET_DEVICE,

	/** ATC PIPE TXM ID */
	TXM_OEM_API_AT_PORT_OPEN,
	TXM_OEM_API_AT_SEND,
	TXM_OEM_API_AT_SEND_HEXBYTE,
	TXM_OEM_API_AT_SEND_GENERAL,
	TXM_OEM_API_AT_PORT_CLOSE,

	/** ADC TXM ID */
	TXM_OEM_API_QT_ADC_READ,

	/** GNSS TXM ID */
	TXM_OEM_API_QT_LOC_START,
	TXM_OEM_API_QT_LOC_STOP,

    /** FOTA TXM ID */
    TXM_OEM_API_QT_FOTA_REMOVE_PACKAGE,
    TXM_OEM_API_QT_FOTA_HTTP_DL_START,
    TXM_OEM_API_QT_FOTA_UPDATE_START,

	/** RAI TXM ID*/
	TXM_OEM_API_QT_RAI,

	TXM_OEM_API_QT_SAHARA_GET,
	TXM_OEM_API_QT_SAHARA_SET,

	/** PSM CORE CONFIG TXM ID*/
	TXM_OEM_API_QT_PSM_SRV_CFG_SET,
	TXM_OEM_API_QT_PSM_SRV_CFG_GET,

	/** SHUTDOWN TXM ID*/
	TXM_OEM_API_QT_SHUTDOWN_DEVICE,

	TXM_OEM_API_QT_REG_WDOG,
	TXM_OEM_API_QT_KICK_WDOG,
	TXM_OEM_API_QT_STOP_WDOG,
	TXM_OEM_API_QT_USBMODE_SET,

	/* USB EVENT */
	TXM_OEM_API_QT_GET_USB_EVENT,
	TXM_OEM_API_QT_PWRKEY_EVENT_SET,
	TXM_OEM_API_QT_PWRKEY_EVENT_GET,

	/* GET RANDOM DATE */
	TXM_OEM_API_QT_PRNG_DATA_GET,

    /** FOTA EXTENDED TXM ID */
    TXM_OEM_API_QT_FOTA_GET_PACKAGE_INFO,
    TXM_OEM_API_QT_FOTA_HTTP_DL_CANCEL,

	TXM_OEM_API_QT_END = TXM_QC_OEM_API_END		/**<-Quectel TXM API IDs End with 0x3000->*/
} TXM_OEM_API_ID_DEFINES_E;

#define LOC_CUSTOM_NMEA_CB             CUSTOM_CB_QAPI_QUECTEL_API_BASE
#define ATC_PIPE_PORT0_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 1
#define ATC_PIPE_PORT1_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 2
#define ATC_PIPE_PORT2_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 3
#define ATC_PIPE_PORT3_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 4

/********************* Quectel OEM QAPI functions **********************/
#ifdef  QAPI_TXM_MODULE  


static __inline void qapi_loc_cb_uspace_dispatcher(UINT cb_id,
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
  //void (*pfn_app_cb1) (boolean , char* , uint8_t*, uint8_t,uint32_t);
  void (*pfn_app_cb1)(char *);
  if(cb_id == LOC_CUSTOM_NMEA_CB)
  {
    pfn_app_cb1 = (void (*)(char * ))app_cb;
   (pfn_app_cb1)((char*)cb_param1);
  }	
}

static __inline void  atc_pipe_cb_uspace_dispatcher(UINT cb_id,
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
    void (*pfn_app_cb1)(qapi_at_pipe_data_t *);
    if(cb_id == ATC_PIPE_PORT0_CB || cb_id == ATC_PIPE_PORT1_CB || cb_id == ATC_PIPE_PORT2_CB || cb_id == ATC_PIPE_PORT3_CB)
    {
        pfn_app_cb1 = (void (*)(qapi_at_pipe_data_t * ))app_cb;
        (pfn_app_cb1)((qapi_at_pipe_data_t*)cb_param1);
    }	
}


                                                             
#define qapi_QT_Welcome(a)			((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_BASE,(ULONG)a, (ULONG)0, (ULONG)0))
#define qapi_QT_Get_Rel_Info(a)		((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_REL_INFO,(ULONG)a, (ULONG)0, (ULONG)0))
#define qapi_QT_Reset_Device(a)		((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_RESET_DEVICE,(ULONG)a, (ULONG)0, (ULONG)0))

#define qapi_QT_Apps_AT_Port_Open(a, b, c, d)		((UINT)(_txm_module_system_call5)(TXM_OEM_API_AT_PORT_OPEN,(ULONG)a, (ULONG)b, (ULONG)c, (ULONG)d,(ULONG)atc_pipe_cb_uspace_dispatcher))
#define qapi_QT_Apps_Send_AT(a, b)				((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_SEND,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Apps_Send_AT_HexByte(a, b)		((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_SEND_HEXBYTE,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Apps_Send_AT_General(a, b, c)	((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_SEND_GENERAL,(ULONG)a, (ULONG)b, (ULONG)c))
#define qapi_QT_Apps_AT_Port_Close(a)			((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_PORT_CLOSE,(ULONG)a, (ULONG)0, (ULONG)0))

#define qapi_QT_ADC_Read(a,b)		((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_ADC_READ,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_RAI_Set(a,b)						((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_RAI,(ULONG)a, (ULONG)b, (ULONG)0))

/********** 	GNSS START 	**********/
#define qapi_QT_Loc_Start(a,b,c)		((UINT)(_txm_module_system_call4)(TXM_OEM_API_QT_LOC_START,(ULONG)a, (ULONG)b,(ULONG)c, (ULONG)qapi_loc_cb_uspace_dispatcher))
#define qapi_QT_Loc_Stop()			((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_LOC_STOP,(ULONG)0, (ULONG)0, (ULONG)0))
/********** 	GNSS END 	**********/

/********** 	FOTA START 	**********/
#define qapi_QT_Remove_Fota_Package()              ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_FOTA_REMOVE_PACKAGE,(ULONG)0, (ULONG)0, (ULONG)0)) 
#define qapi_QT_Fota_Http_Download_Start(a,b)      ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_FOTA_HTTP_DL_START,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Fota_Update_Start(a,b)		       ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_FOTA_UPDATE_START,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Fota_Get_Package_Info(a,b)         ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_FOTA_GET_PACKAGE_INFO,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Fota_Http_Download_Cancel()        ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_FOTA_HTTP_DL_CANCEL,(ULONG)0, (ULONG)0, (ULONG)0))
/********** 	FOTA END 	**********/

#define qapi_QT_Sahara_Mode_Get(a)		((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_SAHARA_GET,(ULONG)a, (ULONG)0, (ULONG)0))
#define qapi_QT_Sahara_Mode_Set(a)		((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_SAHARA_SET,(ULONG)a, (ULONG)0, (ULONG)0))


#define qapi_QT_PSM_SrvCfg_Set(a)					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_PSM_SRV_CFG_SET,(ULONG)a, (ULONG)0, (ULONG)0))
#define qapi_QT_PSM_SrvCfg_Get(a)					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_PSM_SRV_CFG_GET,(ULONG)a, (ULONG)0, (ULONG)0))

#define qapi_QT_Shutdown_Device()					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_SHUTDOWN_DEVICE,(ULONG)0, (ULONG)0, (ULONG)0))
#define qapi_QT_Register_Wdog(a,b)					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_REG_WDOG,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Kick_Wdog(a)					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_KICK_WDOG,(ULONG)a, (ULONG)0, (ULONG)0))
#define qapi_QT_Stop_Wdog(a)					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_STOP_WDOG,(ULONG)a, (ULONG)0, (ULONG)0))
#define qapi_QT_Usbmode_Set(a)					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_USBMODE_SET,(ULONG)a, (ULONG)0, (ULONG)0))

#define qapi_QT_Get_USB_Event(a)					((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_GET_USB_EVENT,(ULONG)a, (ULONG)0, (ULONG)0))
#define qapi_QT_PWRKEY_Switch_Set(a)             ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_PWRKEY_EVENT_SET,(ULONG)a, (ULONG)0, (ULONG)0))
#define	qapi_QT_PWRKEY_State_Get(a)               ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_PWRKEY_EVENT_GET,(ULONG)a, (ULONG)0, (ULONG)0))
#define	qapi_QT_Random_Data_Get(a,b)               ((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_QT_PRNG_DATA_GET,(ULONG)a, (ULONG)b, (ULONG)0))
#else

/*
@func
  qapi_QT_Welcome
@brief
  An example for QAPI public.
*/
qapi_Status_t qapi_QT_Welcome(char *welcome);

/*
@func
  qapi_QT_Get_Rel_Info
@brief
  Get apps release information. 
*/
qapi_Status_t qapi_QT_Get_Rel_Info(qapi_QT_Rel_Info *rel_info);

/*
@func
  qapi_QT_Reset_Device
@brief
  reset module. 
*/
qapi_Status_t qapi_QT_Reset_Device(uint16_t mode);

/*
@func
  qapi_QT_Shutdown_Device
@brief
  Shutdown module. 
*/
qapi_Status_t qapi_QT_Shutdown_Device(void);

/*
@func
  qapi_QT_Apps_AT_Port_Open
  [input] data:  allocate user space for the carried parameter of cb.
@brief
  Open Apps AT command port and register a callback. 
*/
qapi_Status_t qapi_QT_Apps_AT_Port_Open(qapi_at_port_t port_id, qapi_at_stream_id_t *stream_id, qapi_at_resp_func_cb_t cb, qapi_at_pipe_data_t *data);

/*
@func
  qapi_QT_Apps_Send_AT
@brief
  Send AT command in specifial stream port id. 
*/
qapi_Status_t qapi_QT_Apps_Send_AT(qapi_at_stream_id_t stream_id, const char *command_name);

/*
@func
  qapi_QT_Apps_Send_AT_HexByte
@brief
  Send string in specifial stream port id with Hex format.
*/
qapi_Status_t qapi_QT_Apps_Send_AT_HexByte(qapi_at_stream_id_t stream_id, const char *hex_str);

/*
@func
  qapi_QT_Apps_Send_AT_General
@brief
  Send typeless data in secifial stream port id. Caller can tranmit any type of data through stream port. And
  response will be notified in callback which registed in qapi_QT_Apps_AT_Port_Open(). 
*/
qapi_Status_t qapi_QT_Apps_Send_AT_General(qapi_at_stream_id_t stream_id, void *trans_data, uint32 trans_len);

/*
@func
  qapi_QT_Apps_AT_Port_Close
@brief
  Close AT command port with stream port id. 
*/
void qapi_QT_Apps_AT_Port_Close(qapi_at_stream_id_t stream_id);

/*
@func
  qapi_QT_ADC_Read
@brief
  Read the value of ADC.
*/
qapi_Status_t qapi_QT_ADC_Read(const char *pChn_Name,qapi_ADC_Read_Result_t *result);

/*
@func
  qapi_QT_Loc_Start
  [input]nmea    allocate user space for the carried parameter of cb.
@brief
  Start GNSS Location.
*/
qapi_Status_t qapi_QT_Loc_Start(qtLocEventRegMaskT evt_mask, qapi_QT_Loc_CB_t cb, qt_nmea_sentence* nmea);

/*
@func
  qapi_QT_Loc_Stop
@brief
  Stop GNSS Location.
*/
qapi_Status_t qapi_QT_Loc_Stop(void);

/*
@func
  qapi_QT_Remove_Fota_Package
@brief
  Remove fota package.
*/
qapi_Status_t qapi_QT_Remove_Fota_Package(void);

/*
@func
  qapi_QT_Fota_Http_Download_Start
@param[in]
url  fota package path. for example: HTTP://220.180.239.212:8005/BG96_112A_119.zip. The url must be started with "http://" or "https://", not case sensitive
response_cb  response callback, This callback will come when download failed or success.
@brief
  Start Fota package download.
*/
qapi_Status_t qapi_QT_Fota_Http_Download_Start(char* url, qapi_Fota_Http_dl_CB_t response_cb);

/*
@func
  qapi_QT_Fota_Update_Start
@param [in]
  response_cb  response callback,This callback will come when fota upgrade failed or success.
  upgrade_progress_cb   upgrade progress callback, This callback will come every one percent.
@brief
  Start Fota Update.
  
  Note:
    Before use this interface, the delta firmware should be uploded to the efs.
    The storage path for delta firmware package is /fota/update.zip. Both the path and the package name cannot be modified. 
    Customers can use the QEFS Explorer tool provided by Quectel to upload the upgrade package
*/
qapi_Status_t qapi_QT_Fota_Update_Start(qapi_QT_Fota_Response_CB_t response_cb, qapi_QT_Fota_Upgrade_Progress_CB_t upgrade_progress_cb);

/*
@func
  qapi_QT_Fota_Get_Package_Info
@brief
  Get the DFOTA Package Information.
@param[out]
  dwl_size  Pointer, size of the downloaded file.
  total_size  Pointer, total size of the package file on the HTTP server.
*/
qapi_Status_t qapi_QT_Fota_Get_Package_Info(uint32 *dwl_size, uint32 *total_size);

/*
@func
  qapi_QT_Fota_Http_Download_Cancel
@brief
  Cancel the current download process.
*/
qapi_Status_t qapi_QT_Fota_Http_Download_Cancel(void);

/*
@func
  qapi_QT_RAI_Set
@brief
  Set the Release Assistant Information indicator.
@param[in]
  hndl  Pointer to data service handle
  rai_type  Type of Release Assistant Information indicator
  	     0  No information available
  	     1  No further uplink or downlink data transmission
  	     2  Only a single downlink data transmission and no further uplink data 
*/
qapi_Status_t qapi_QT_RAI_Set(qapi_DSS_Hndl_t hndl, uint8_t rai_type);

/*
@func
  qapi_QT_Sahara_Mode_Get
@brief
  Get the NV Item value of Sahara mode setting.
*/
qapi_Status_t qapi_QT_Sahara_Mode_Get(QAPI_FATAL_ERR_MODE *mode);

/*
@func
  qapi_QT_Sahara_Mode_Set
@brief
  Enable or disable module enter sahara dump mode when module crash.
  It will take effect after the module restarted.
*/
qapi_Status_t qapi_QT_Sahara_Mode_Set(QAPI_FATAL_ERR_MODE mode);

/*
@func
  qapi_QT_PSM_SrvCfg_Set
@brief
  Set PSM core server configuration
*/
qapi_Status_t qapi_QT_PSM_SrvCfg_Set(int gpio_enable);

/*
@func
  qapi_QT_PSM_SrvCfg_Get
@brief
  Get PSM core server configuration
*/
qapi_Status_t qapi_QT_PSM_SrvCfg_Get(int *gpio_enable);

/*
@func
  qapi_QT_Register_dog
@brief
  Dynamic method for tasks to register for dog services
@param [in]
  time  starvation detection threshhold in milliseconds 
@param [out]
  dog_id  for feed watchdog function use
*/
qapi_Status_t qapi_QT_Register_Wdog( uint32 time,uint32 *dog_id);

/*
@func
  qapi_QT_kick_watchdog
@brief
  Report to the Watchdog task, so that it can tell that the monitored
  task is still functioning properly. 
@param [in]  
  id  register Wdog function return dog_id
*/
qapi_Status_t qapi_QT_Kick_Wdog(uint32 id);

/*
@func
  qapi_QT_stop_watchdog
@brief
  Set the sotftware dog_enable on/off
@param [in]
  mode enable or disable watchdog
       1  disable all watchdog
       0  able all watchdog    
*/
qapi_Status_t qapi_QT_Stop_Wdog(boolean mode);

/*
@func
  qapi_QT_Usbmode_Set
@brief
  Set the sw usb on/off
@param [in]
  mode enable or disable the usb
       1  disable the usb
       0  able the usb   
*/
qapi_Status_t qapi_QT_Usbmode_Set(boolean mode);

/*
@func
  qapi_QT_Get_USB_Event
@brief
  Get USB events : 
    0 : DEVICE CONNECT
    1 : DEVICE DISCONNECT
    2 : DEVICE SUSPEND
    3 : DEVICE RESUME
    4 : DEVICE RESUME COMPLETED
    5 : DEVICE REMOTE WAKEUP
    6 : DEVICE CONFIGURED
    7 : DEVICE UNCONFIGURED
    8 : DEVICE RESET
    9 : DEVICE SPEED CHANGE
@param[in]
  usb_evt : pointer, store the USB Event.
*/
qapi_Status_t qapi_QT_Get_USB_Event(uint32 *usb_evt);

/*
@func
  qapi_QT_PWRKEY_Switch_Set
@brief
  Set the sw PWRKEY enable/disable
@param [in]
  val enable or disable the PWRKEY
       1  disable the PWRKEY When module is turn on
       0  enable the PWRKEY
*/
qapi_Status_t qapi_QT_PWRKEY_Switch_Set(uint32 mode);

/*
@func
  qapi_QT_PWRKEY_State_Get
@brief
  Get PWRKEY state

*/
qapi_Status_t qapi_QT_PWRKEY_State_Get(uint32 *val);

/*
@func
  qapi_QT_Random_Data_Get
@brief
  Gets the hardware random number
@param [in]
  prng_size  get the length of random Number.Parameters range of the support:1,2,4...512 
@param [out]
  prng_data  Stores the generated random Number,the size of the data returned depends on prng_size
*/
qapi_Status_t qapi_QT_Random_Data_Get(uint16 prng_size, uint8* prng_data);
#endif

#endif /*__QAPI_QUECTEL_H__*/
