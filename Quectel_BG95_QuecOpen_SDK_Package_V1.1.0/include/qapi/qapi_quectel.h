#ifndef __QAPI_QUECTEL_H__
#define __QAPI_QUECTEL_H__

#include "tx_api.h"
#include "qapi_status.h"
#include "qapi_txm_base.h"
#include "qapi_adc_types.h"
#include "qapi/qapi_dss.h"
#include "qapi_fs_types.h"
#include "qapi_i2c_master.h"


/********************* Quectel OEM error number Definitions **********************/
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
	QAPI_QT_ERR_GEOFENCE_ID_NOT_EXIST,
	QAPI_QT_ERR_FOTA_INVALID_PKG,
	QAPI_QT_ERR_FOTA_NOT_IN_DOWNLOADING,

	QAPI_QT_ERR_API_NOT_SUPPORT = 0x1FFE,
    QAPI_QT_ERR_IVNALID_QT_API = 0x1FFF,
	/*  end  */
	QAPI_QT_ERR_END = 0x2000
}QAPI_QT_ERR_NUM;


/********************* Quectel OEM Definitions **********************/
typedef enum
{
	QAPI_FATAL_ERR_RESET = 0,
	QAPI_FATAL_ERR_SAHARA = 1,
	QAPI_FATAL_ERR_MAX
}QAPI_FATAL_ERR_MODE;

/******************* ATC Pipe Definitions******************/
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
/*
@func
  qapi_at_resp_func_cb_t
@brief
  A callback funtion to register when open internal ATC pipe.
@param[out]
  data	- Pointer, to store the data retured from modem side, like AT command executed result or raw data.
*/
typedef void  (*qapi_at_resp_func_cb_t)(qapi_at_pipe_data_t *data);	//response callback

/************************** SMS Definitions *******************************/
typedef enum {
  WMS_MESSAGE_TAG_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WMS_TAG_TYPE_MT_READ_V01 = 0x00, 
  WMS_TAG_TYPE_MT_NOT_READ_V01 = 0x01, 
  WMS_TAG_TYPE_MO_SENT_V01 = 0x02, 
  WMS_TAG_TYPE_MO_NOT_SENT_V01 = 0x03, 
  WMS_MESSAGE_TAG_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wms_message_tag_type_enum_v01;

typedef wms_message_tag_type_enum_v01 qapi_QT_SMS_Status_e;

typedef struct{
  uint32_t data_len;
  uint8_t data[255];
}qapi_QT_SMS_Message_Info_t;


typedef struct{
//time_t time;
qapi_QT_SMS_Status_e status;
qapi_QT_SMS_Message_Info_t sms_info;
}qapi_QT_SMS_Message_Rcvd_t;

typedef struct {
char        address[20];
char        message[256];
} qapi_QT_SMS_Message_Content_t;

/************************** NETWORK Definitions *******************************/
#define QT_DS_PROFILE_MAX_APN_STRING_LEN (101)
#define QT_DS_PROFILE_MAX_USERNAME_LEN (128)
#define QT_DS_PROFILE_MAX_PASSWORD_LEN (128)

typedef enum {
	QT_NW_CFUN_MIN_FUNC = 0,  
	QT_NW_CFUN_FUNN_FUNC = 1,
	QT_NW_CFUN_SHUT_DOWN = 2,
	QT_NW_CFUN_RESET = 3,
	QT_NW_CFUN_FTM = 4,

	QT_NW_CFUN_MAX
} qapi_QT_NW_CFUN_MODE_e;

typedef enum {
	QT_NW_PREF_GSM = 0,
	QT_NW_PREF_CATM = 1,
	QT_NW_PREF_GSM_CATM = 2,
	QT_NW_PREF_CATNB = 3,
	QT_NW_PREF_GSM_CATNB = 4,
	QT_NW_PREF_CATM_CATNB = 5,
	QT_NW_PREF_GSM_CATM_CATNB = 6,
	QT_NW_PREF_RAT_MAX
}qapi_QT_NW_RAT_PREF_e;

typedef enum {
	QT_NW_PREF_SCAN_CATM_CATNB_GSM = 0,
	QT_NW_PREF_SCAN_CATM_GSM_CATNB = 1,
	QT_NW_PREF_SCAN_CATNB_CATM_GSM = 2,
	QT_NW_PREF_SCAN_CATNB_GSM_CATM = 3,
	QT_NW_PREF_SCAN_GSM_CATM_CATNB = 4,
	QT_NW_PREF_SCAN_GSM_CATNB_CATM = 5,
	QT_NW_PREF_RAT_SCAN_ORDER_MAX
}qapi_QT_NW_RAT_SCAN_ORDER_e;

typedef enum {
	QT_NW_PREF_CS_ONLY = 0,
	QT_NW_PREF_PS_ONLY = 1,
	QT_NW_PREF_CS_PS = 2,
	QT_NW_PREF_SRV_DOMAIN_MAX
} qapi_QT_NW_SRV_DOMAIN_PREF_e;

typedef enum {
	QT_NW_GSM_BAND_EGSM = 0,
	QT_NW_GSM_BAND_PGSM = 1,
	QT_NW_GSM_BAND_PCS_1900 = 2,
	QT_NW_GSM_BAND_DCS_1800 = 3,
	QT_NW_GSM_BAND_CELL_850 = 4,
	QT_NW_GSM_BAND_MAX
}qapi_QT_GSM_BAND_e;

typedef enum
{
	QT_NW_DS_PROFILE_PDP_IPV4 = 1,
	QT_NW_DS_PROFILE_PDP_IPV6 = 2,
	QT_NW_DS_PROFILE_PDP_IPV4V6 = 3,
	QT_NW_DS_PROFILE_PDP_MAX
}qapi_QT_NW_DS_PROFILE_PDP_TYPE_e;

typedef enum {
	QT_NW_DS_PROFILE_AUTH_PAP = 0,
	QT_NW_DS_PROFILE_AUTH_CHAP = 1,
	QT_NW_DS_PROFILE_AUTH_PAP_CHAP = 2,
	QT_NW_DS_PROFILE_AUTH_TYPE_MAX
}qapi_QT_NW_DS_PROFILE_AUTH_TYPE_e;

typedef struct {
	unsigned char gsm_band;
	unsigned long long catm_band_low;
	unsigned long long nb_band_low;
}qapi_QT_NW_Band_Params_t;

typedef enum {
	QT_NW_EMTC = 0,
	QT_NW_NB_IOT = 1,
}qapi_QT_NW_RAT_e;

typedef struct {
	unsigned short year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char time_zone;
}qapi_QT_Real_Time_Cfg_Params_t;

typedef struct {
	unsigned short arfcn;
	unsigned short mcc;
	unsigned short mnc;
	unsigned short lac;
	unsigned long cell_id;
	qapi_QT_GSM_BAND_e band;
	unsigned char bsic;
	unsigned char rxlev;
	unsigned short drx;
	unsigned long c1;
	unsigned long c2;
}qapi_QT_NW_GSM_Meas_Info_t;

typedef struct {
	unsigned long earfcn;
	unsigned short mcc;
	unsigned short mnc;
	unsigned short tac;
	unsigned long cell_id;
	unsigned char freq_band;
	unsigned short pci;
	unsigned short rsrp;
	unsigned short rsrq;
	unsigned short rssi;
	unsigned short sinr;
}qapi_QT_NW_LTE_Meas_Info_t;

typedef struct {
	qapi_QT_NW_DS_PROFILE_PDP_TYPE_e pdp_type;
	qapi_QT_NW_DS_PROFILE_AUTH_TYPE_e auth_type;
	unsigned char apn[QT_DS_PROFILE_MAX_APN_STRING_LEN + 1];
	unsigned char user_name[QT_DS_PROFILE_MAX_USERNAME_LEN + 1];
	unsigned char pass_word[QT_DS_PROFILE_MAX_PASSWORD_LEN + 1];
}qapi_QT_NW_DS_Profile_PDP_Context_t;

typedef struct {
	unsigned char req_psm_enable;
	unsigned long req_active_timer_value;
	unsigned long req_periodic_tau_timer_value;
}qapi_QT_NW_Req_PSM_Cfg_t;

typedef struct {
	unsigned char alloc_psm_enabled;
	unsigned long alloc_active_timer_value;
	unsigned long alloc_periodic_tau_timer_value;
}qapi_QT_NW_Alloc_PSM_Cfg_t;

typedef struct {
	unsigned char req_edrx_enable;
	qapi_QT_NW_RAT_e rat_mode;
	unsigned char req_ptw_cycle;
	unsigned char req_edrx_cycle;
}qapi_QT_NW_Req_eDRX_Cfg_t;

typedef struct {
	unsigned char alloc_edrx_enable;
	unsigned char alloc_ptw_cycle;
	unsigned char alloc_edrx_cycle;
}qapi_QT_NW_Alloc_eDRX_Cfg_t;

/*********************** IIC Interface Definitions*************************/

#define QT_QAPI_I2CM_PORT_01		QAPI_I2CM_INSTANCE_001_E    //IIC1
#define QT_QAPI_SPIM_PORT_01        QAPI_SPIM_INSTANCE_2_E      //SPI1
#define QT_QAPI_SPIM_PORT_02        QAPI_SPIM_INSTANCE_4_E      //SPI2


/********************* Quectel OEM ID Definitions **********************/
/* @Note: Each new OEM ID needs to be defined in this enum */
typedef enum
{
#include "qapi_quectel_id_enum.h"
} TXM_OEM_API_ID_DEFINES_E;


#define ATC_PIPE_PORT0_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 3
#define ATC_PIPE_PORT1_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 4
#define ATC_PIPE_PORT2_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 5
#define ATC_PIPE_PORT3_CB              CUSTOM_CB_QAPI_QUECTEL_API_BASE + 6

/********************* Quectel OEM QAPI Functions **********************/
#ifdef  QAPI_TXM_MODULE  
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
#define qapi_QT_Reset_Device()  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_RESET_DEVICE, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Shutdown_Device()  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_SHUTDOWN_DEVICE, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Sahara_Mode_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_SAHARA_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Sahara_Mode_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_SAHARA_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_USB_Sio_Open()  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_NMEA_SIO_OPEN, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_USB_Sio_Close()  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_NMEA_SIO_CLOSE, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_USB_Sio_Transmit(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_NMEA_SIO_SEND, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_MP_FW_Ver_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_MP_FW_VER_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_AP_FW_Ver_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_AP_FW_VER_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_IMEI_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_IMEI_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_MP_Core_Info_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_MP_CORE_INFO_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_AP_Core_Info_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_AP_CORE_INFO_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Manufacturer_Info_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_MANU_INFO_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Random_Data_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_PRNG_DATA_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Get_USB_Event(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SYS_GET_USB_EVENT, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Phone_Func_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_PHONE_FUNC_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Phone_Func_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_PHONE_FUNC_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Real_Time_Clock_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_REAL_TIME_CLOCK_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_Real_Time_Clock_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_REAL_TIME_CLOCK_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Band_Pref_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_BAND_PREF_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Band_Pref_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_BAND_PREF_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Rat_Pref_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_RAT_PREF_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Rat_Pref_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_RAT_PREF_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Rat_Scan_Pre_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_RAT_SCAN_PREF_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Rat_Scan_Pre_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_RAT_SCAN_PREF_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Srv_Domain_Pref_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_SRV_DOMAIN_PREF_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Srv_Domain_Pref_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_SRV_DOMAIN_PREF_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_PDP_Cfg_Set(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_PDP_CFG_SET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_PDP_Cfg_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_PDP_CFG_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_GSM_Meas_Info_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_GSM_MEAS_INFO_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_LTE_Meas_Info_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_LTE_MEAS_INFO_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_PSM_Cfg_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_PSM_CFG_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_PSM_Cfg_Get(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_PSM_CFG_GET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_eDRX_Cfg_Set(a)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_EDRX_CFG_SET, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_eDRX_Cfg_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_EDRX_CFG_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Extend_Band_Pref_Set(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_EXTEND_BAND_PREF_SET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_NW_Extend_Band_Pref_Get(a,b)  (UINT)_txm_module_system_call6(TXM_OEM_API_QT_NW_EXTEND_BAND_PREF_GET, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SIM_RDY_Check(a)    (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SIM_RDY_Check, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SIM_IMSI_Get(a)     (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SIM_IMSI_Get, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SIM_CCID_Get(a)     (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SIM_CCID_Get, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SIM_MSISDN_Get(a)   (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SIM_MSISDN_Get, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SMS_Message_Send(a)     (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SMS_SEND, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SMS_Message_Read(a,b)   (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SMS_READ, (ULONG)a, (ULONG)b, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SMS_Message_Delete(a)   (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SMS_DELETE, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
#define qapi_QT_SMS_Get_Service_Ready_Status(a) (UINT)_txm_module_system_call6(TXM_OEM_API_QT_SMS_READY, (ULONG)a, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0, (ULONG)0)
/**---------------- ATC Pipe QAPIs Begin --------------**/
#define qapi_QT_Apps_AT_Port_Open(a, b, c, d)	((UINT)(_txm_module_system_call5)(TXM_OEM_API_AT_PORT_OPEN,(ULONG)a, (ULONG)b, (ULONG)c, (ULONG)d,(ULONG)atc_pipe_cb_uspace_dispatcher))
#define qapi_QT_Apps_Send_AT(a, b)				((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_SEND,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Apps_Send_AT_HexByte(a, b)		((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_SEND_HEXBYTE,(ULONG)a, (ULONG)b, (ULONG)0))
#define qapi_QT_Apps_Send_AT_General(a, b, c)	((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_SEND_GENERAL,(ULONG)a, (ULONG)b, (ULONG)c))
#define qapi_QT_Apps_AT_Port_Close(a)			((UINT)(_txm_module_kernel_call_dispatcher)(TXM_OEM_API_AT_PORT_CLOSE,(ULONG)a, (ULONG)0, (ULONG)0))
/**---------------- ATC Pipe QAPIs End --------------**/
#else
/*
@func
  qapi_QT_Reset_Device
@brief
  reset module. 
@param
  null.
*/
qapi_Status_t qapi_QT_Reset_Device(void);


/*
@func
  qapi_QT_Shutdown_Device
@brief
  Shutdown module.
@param
  null.
*/
qapi_Status_t qapi_QT_Shutdown_Device(void);


/*
@func
  qapi_QT_Sahara_Mode_Get
@brief
  Get the NV Item value of Sahara mode setting.
@param[out]
  mode - Configure options.
    QAPI_FATAL_ERR_RESET -- Reset mode.
    QAPI_FATAL_ERR_SAHARA -- Sahara DUMP mode.
*/
qapi_Status_t qapi_QT_Sahara_Mode_Get(QAPI_FATAL_ERR_MODE* mode);


/*
@func
  qapi_QT_Sahara_Mode_Set
@brief
  Enable or disable module enter sahara dump mode when module crash.
  It will take effect after the module restarted.
@param[in]
  mode - Configure options.
    QAPI_FATAL_ERR_RESET -- Reset mode.
    QAPI_FATAL_ERR_SAHARA -- Sahara DUMP mode.
*/
qapi_Status_t qapi_QT_Sahara_Mode_Set(QAPI_FATAL_ERR_MODE mode);


/*
@func
  qapi_QT_USB_Sio_Open
@brief
  Open NMEA USB port for output user application log.
*/
qapi_Status_t qapi_QT_USB_Sio_Open(void);


/*
@func
  qapi_QT_USB_Sio_Close
@brief
  Close NMEA USB port.
*/
qapi_Status_t qapi_QT_USB_Sio_Close(void);


/*
@func
  qapi_QT_USB_Sio_Transmit
@brief
  Output user application log.
@param [in]
  log - Pointer, point to the buffer which store the user application log.
*/
qapi_Status_t qapi_QT_USB_Sio_Transmit(char* log);


/*
@func
  qapi_QT_MP_FW_Ver_Get
@brief
  Get module MP firmware version number.
@param [in]
  version - pointer, to store the MP firmware number.
  len - pointer, the length of version string.
*/
qapi_Status_t qapi_QT_MP_FW_Ver_Get(char* version, uint16* len);


/*
@func
  qapi_QT_AP_FW_Ver_Get
@brief
  Get module AP firmware version number.
@param [in]
  version - pointer, to store the AP firmware number.
  len - pointer, the length of version string.
*/
qapi_Status_t qapi_QT_AP_FW_Ver_Get(char* version, uint16* len);


/*
@func
  qapi_QT_IMEI_Get
@brief
  Get module IMEI number.
@param [in]
  imei - pointer, to store the IMEI number.
  len - pointer, the length of imei string.
*/
qapi_Status_t qapi_QT_IMEI_Get(char* imei, uint16* len);


/*
@func
  qapi_QT_MP_Core_Info_Get
@brief
  Get module MP Core information.
@param [in]
  info - pointer, to store the MP Core information.
  len - pointer, the length of info string.
*/
qapi_Status_t qapi_QT_MP_Core_Info_Get(char* info, uint16* len);


/*
@func
  qapi_QT_AP_Core_Info_Get
@brief
  Get module AP Core information.
@param [in]
  info - pointer, to store the AP Core information.
  len - pointer, the length of info string.
*/
qapi_Status_t qapi_QT_AP_Core_Info_Get(char* info, uint16* len);


/*
@func
  qapi_QT_Manufacturer_Info_Get
@brief
  Get module AP Core information.
@param [in]
  info - pointer, to store the Manufacturer Core information. Here will rerutn "Quectel".
  len - pointer, the length of info string.
*/
qapi_Status_t qapi_QT_Manufacturer_Info_Get(char* info, uint16* len);


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
qapi_Status_t qapi_QT_Get_USB_Event(uint32* usb_evt);
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

#endif /* QAPI_TXM_MODULE */

#endif /*__QAPI_QUECTEL_H__*/
