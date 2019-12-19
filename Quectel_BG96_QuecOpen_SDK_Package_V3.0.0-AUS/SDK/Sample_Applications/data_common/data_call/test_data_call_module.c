/*===========================================================================
                         TEST_DATA_CALL_MODULE.C

DESCRIPTION
   This file deals with normal data call start and stop procedure.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "test_app_module.h"
#include "test_utils.h"


/* The max length of APN used in DAM */
#define     DAM_APN_SZ           101

/* APN used in DAM data call bring up */
char dam_dss_apn[DAM_APN_SZ] = {"CMNET"};

/* The opaque data service handle. All DSS related 
   functions use this handle as an input parameter. */
qapi_DSS_Hndl_t dam_dss_hndl;

/* IP version used in the data call */
uint32_t dam_dss_ip_version = 0;

/* Local IPv4 Address assigned by NW */
uint32_t local_ipaddr_v4 = 0L;
bool local_ipaddr_v4_valid = false;

/*=============================================================================

FUNCTION DAM_DSS_NET_IS_CONN

DESCRIPTION
  This function is to exercise QAPIs for DSS routine.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_dss_app_routine(qapi_DSS_Hndl_t hndl)
{
  unsigned int addr_cnt = 0;
  unsigned int len = 15;  
  unsigned int mtu = 0;
  char addr_v4[16];
  char addr_v6[130];
  char submsk_v4[16];
  char gtwy_v4[16];
  char gtwy_v6[130];
  int  ip_family = 0;  
  char *data_buf = NULL;
  qapi_DSS_Addr_Info_t *info_ptr = NULL;  
  qapi_DSS_Addr_Info_t *v4_info_ptr = NULL; 
  qapi_DSS_Addr_Info_t *v6_info_ptr = NULL;   
  qapi_DSS_Data_Pkt_Stats_t *data_stats_ptr = NULL;
  qapi_DSS_Call_Tech_Type_t call_tech = QAPI_DSS_CALL_TECH_INVALID_E;
  qapi_DSS_Data_Bearer_Tech_t bearer_tech = QAPI_DSS_DATA_BEARER_TECH_UNKNOWN_E;

  do
  {
    /*----------------------------------------------------------------------------
      qapi_DSS_Get_IP_Addr_Count
      qapi_DSS_Get_IP_Addr
      qapi_DSS_Get_IP_Addr_Per_Family
    ----------------------------------------------------------------------------*/ 

    if(QAPI_OK != qapi_DSS_Get_IP_Addr_Count(hndl, &addr_cnt))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_IP_Addr_Count fail \n");
      break;
    }

    LOG_INFO("DAM_APP:qapi_DSS_Get_IP_Addr_Count:ip_addr_cnt:%d \n",addr_cnt);

    info_ptr = (qapi_DSS_Addr_Info_t *)data_malloc(sizeof(qapi_DSS_Addr_Info_t)*addr_cnt);
    
    if (NULL == info_ptr)
    {
      LOG_ERROR("DAM_APP:Alloc IP address memory fail \n");
      break;
    }
    
    memset(info_ptr,0,(sizeof(qapi_DSS_Addr_Info_t)*addr_cnt));
    
    if(QAPI_OK != qapi_DSS_Get_IP_Addr(hndl, info_ptr, addr_cnt))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_IP_Addr fail \n");
      break;
    }

    /* Save assigned ipv4 address */
    if(info_ptr->iface_addr_s.valid_addr &&  ((QAPI_DSS_IP_VERSION_4 == dam_dss_ip_version) || (QAPI_DSS_IP_VERSION_4_6== dam_dss_ip_version)))
    {
      LOG_INFO("Get valid local IPv4 Addr \n");
      local_ipaddr_v4 = info_ptr->iface_addr_s.addr.v4;
      local_ipaddr_v4_valid = true;
    }

    if ((QAPI_DSS_IP_VERSION_4 == dam_dss_ip_version) || (QAPI_DSS_IP_VERSION_4_6== dam_dss_ip_version))
    {
      v4_info_ptr = info_ptr + ip_family;      
      if(info_ptr->iface_addr_s.valid_addr)
      {
        ip_family++;
        addr_cnt--;
        LOG_INFO("DAM_APP:IP address is %s \n subnet mask address %s \n gateway address %s \n"
                 ,inet_ntop(AF_INET,&v4_info_ptr->iface_addr_s.addr.v4,addr_v4,sizeof(addr_v4))
                 ,inet_ntop(AF_INET,&v4_info_ptr->iface_mask,submsk_v4,sizeof(submsk_v4))
                 ,inet_ntop(AF_INET,&v4_info_ptr->gtwy_addr_s.addr.v4,gtwy_v4,sizeof(gtwy_v4)));
      }
      else
      {
        LOG_ERROR("DAM_APP:IPv4 address is not valid");
      }
    }

    if ((addr_cnt) && ((QAPI_DSS_IP_VERSION_6 == dam_dss_ip_version) || (QAPI_DSS_IP_VERSION_4_6== dam_dss_ip_version)))
    {
      v6_info_ptr = info_ptr + ip_family; 
      if (v6_info_ptr->iface_addr_s.valid_addr)
      {
        LOG_INFO("DAM_APP:V6 address is %s \n V6 prefix length %u \n V6 gateway address %s \n v6 gateway length %u \n"
                  ,inet_ntop(AF_INET6,&v6_info_ptr->iface_addr_s.addr.v6_addr64,addr_v6,sizeof(addr_v6))
                  ,info_ptr->iface_mask
                  ,inet_ntop(AF_INET6,&v6_info_ptr->gtwy_addr_s.addr.v6_addr64,gtwy_v6,sizeof(gtwy_v6))
                  ,info_ptr->gtwy_mask);       
      }
      else
      {
        LOG_ERROR(("IPv6 address is not valid"));
      }      
    }

    memset(info_ptr,0,(sizeof(qapi_DSS_Addr_Info_t)*addr_cnt));

    if(QAPI_OK != qapi_DSS_Get_IP_Addr_Per_Family(hndl, info_ptr, QAPI_DSS_IP_VERSION_4))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_IP_Addr_Per_Family fail \n");
      break;
    }
    
    LOG_INFO("DAM_APP:qapi_DSS_Get_IP_Addr_Per_Family:ipv4_addr:%s \n",inet_ntop(AF_INET,&info_ptr->iface_addr_s.addr.v4,addr_v4,sizeof(addr_v4)));


    /*----------------------------------------------------------------------------
      qapi_DSS_Get_Pkt_Stats
    ----------------------------------------------------------------------------*/ 

    data_stats_ptr= (qapi_DSS_Data_Pkt_Stats_t *)data_malloc(sizeof(qapi_DSS_Data_Pkt_Stats_t));
    
    if (NULL == data_stats_ptr)
    {
      LOG_ERROR("DAM_APP:Alloc data stats buffer fail \n");
      break;
    }
    
    memset(data_stats_ptr,0,(sizeof(qapi_DSS_Data_Pkt_Stats_t)));

    if(QAPI_OK != qapi_DSS_Get_Pkt_Stats(hndl, data_stats_ptr))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_Pkt_Stats fail \n");
      break;
    }

    LOG_INFO("DAM_APP:qapi_DSS_Get_Pkt_Stats:bytes_rx:%d, bytes_tx:%d \n",data_stats_ptr->bytes_rx, data_stats_ptr->bytes_tx);

    /*----------------------------------------------------------------------------
      qapi_DSS_Reset_Pkt_Stats
    ----------------------------------------------------------------------------*/ 

    if(QAPI_OK != qapi_DSS_Reset_Pkt_Stats(hndl))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Reset_Pkt_Stats fail \n");
      break;
    }

    LOG_INFO("DAM_APP:qapi_DSS_Reset_Pkt_Stats success \n");
    
    /*----------------------------------------------------------------------------
      qapi_DSS_Get_Call_Tech
    ----------------------------------------------------------------------------*/ 
    memset(&call_tech,0,sizeof(qapi_DSS_Call_Tech_Type_t));

    if(QAPI_OK != qapi_DSS_Get_Call_Tech(hndl, &call_tech))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_Call_Tech fail \n");
      break;
    }

    LOG_INFO("DAM_APP:qapi_DSS_Get_Call_Tech:call tech:%d \n", call_tech);

    /*----------------------------------------------------------------------------
      qapi_DSS_Get_Current_Data_Bearer_Tech
    ----------------------------------------------------------------------------*/ 

    if(QAPI_OK != qapi_DSS_Get_Current_Data_Bearer_Tech(hndl, &bearer_tech))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_Current_Data_Bearer_Tech fail \n");
      break;
    }
    
    LOG_INFO("DAM_APP:qapi_DSS_Get_Current_Data_Bearer_Tech:bearer tech:%d \n", bearer_tech);

    /*----------------------------------------------------------------------------
      qapi_DSS_Get_Device_Name
    ----------------------------------------------------------------------------*/ 

    data_buf = (char *)data_malloc(sizeof(char)*len);
    
    if (NULL == data_buf)
    {
      LOG_ERROR("DAM_APP:Alloc device name buffer fail \n");
      break;
    }
    
    if(QAPI_OK != qapi_DSS_Get_Device_Name(hndl, data_buf, len))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_Device_Name fail \n");
      break;
    }
    
    LOG_INFO("DAM_APP:qapi_DSS_Get_Device_Name:device name:%s \n", data_buf);

    /*----------------------------------------------------------------------------
      qapi_DSS_Get_Qmi_Port_Name
    ----------------------------------------------------------------------------*/ 
    memset(data_buf,0,sizeof(char)*len);
    
    if(QAPI_OK != qapi_DSS_Get_Qmi_Port_Name(hndl, data_buf, len))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_Qmi_Port_Name fail \n");
      break;
    }
    
    LOG_INFO("DAM_APP:qapi_DSS_Get_Qmi_Port_Name:QMI port name:%s \n", data_buf);

    /*----------------------------------------------------------------------------
      qapi_DSS_Get_Link_Mtu
    ----------------------------------------------------------------------------*/ 

    if(QAPI_OK != qapi_DSS_Get_Link_Mtu(hndl, &mtu))
    {
      LOG_ERROR("DAM_APP:qapi_DSS_Get_Link_Mtu fail \n");
      break;
    }

    LOG_INFO("DAM_APP:qapi_DSS_Get_Link_Mtu:MTU size:%d \n",mtu);
  
  }while(0);

  if(data_buf)
  {
    data_free(data_buf);
    data_buf = NULL;
  }

  if(info_ptr)
  {
    data_free(info_ptr);
    info_ptr = NULL;
  }
  
  if(data_stats_ptr)
  {
    data_free(data_stats_ptr);
    data_stats_ptr = NULL;
  }

}

/*=============================================================================

FUNCTION DAM_DSS_NET_IS_CONN

DESCRIPTION
  This function call the related APIs after the data call is established.
  User can put their routine here if data call up is a precondition. 

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_dss_net_is_conn(qapi_DSS_Hndl_t hndl)
{
  /*----------------------------------------------------------------------------
    DSS QAPI operation
  ----------------------------------------------------------------------------*/
  dam_dss_app_routine(hndl);

  /*----------------------------------------------------------------------------
    DAM APP QAPI operation
  ----------------------------------------------------------------------------*/
  dam_app_routine(hndl);

  return;
}

/*=============================================================================

FUNCTION DAM_DSS_NET_NO_NET

DESCRIPTION
  This function call the related APIs after the data call is torn down.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_dss_net_no_net(qapi_DSS_Hndl_t hndl)
{
  qapi_DSS_CE_Reason_t ce_reason;
  
  LOG_INFO("DAM_APP:Release DSS Handle \n");

  memset(&ce_reason,0,sizeof(qapi_DSS_CE_Reason_t));

  /* Get call end reason */
  if(QAPI_OK == qapi_DSS_Get_Call_End_Reason(hndl, &ce_reason, QAPI_DSS_IP_FAMILY_V4_E))
  {
    LOG_INFO("DAM_APP:Data call released, call end reason:%d \n", ce_reason.reason_type);    
  }

  qapi_DSS_Rel_Data_Srvc_Hndl(dam_dss_hndl);

}

/*=============================================================================

FUNCTION DAM_DSS_CB

DESCRIPTION
  This function/call back is invoked after the data call startup.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void dam_dss_cb
(
  qapi_DSS_Hndl_t         hndl,          /* Handle for which this event is associated */
  void                   *user_data,     /* Application-provided user data */
  qapi_DSS_Net_Evt_t      evt,           /* Event identifier */
  qapi_DSS_Evt_Payload_t *payload_ptr    /* Associated event information */
)
{
  switch (evt)
  {
    case QAPI_DSS_EVT_NET_IS_CONN_E:
      LOG_INFO("DAM_APP:QAPI_DSS_EVT_NET_IS_CONN_E rcved in data call \n");
      tx_event_flags_set(dam_obj_param.dam_obj_signal, DAM_SIG_DATA_CALL_CONN, TX_OR);
      break;

    case QAPI_DSS_EVT_NET_NO_NET_E:
      LOG_INFO("DAM_APP:QAPI_DSS_EVT_NET_NO_NET_E rcved in data call \n");
      tx_event_flags_set(dam_obj_param.dam_obj_signal, DAM_SIG_DATA_CALL_NO_NET, TX_OR);
      break;

    default:      
      LOG_INFO("DAM_APP:Unexpected event %d rcved in data call \n", evt);
      break;
  }
  
}

/*=============================================================================

FUNCTION DAM_DSS_SET_PARA

DESCRIPTION
  This function set call parameters to bring up data call.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
qapi_Status_t dam_dss_set_para(qapi_DSS_Hndl_t hndl)
{
  qapi_DSS_Call_Param_Value_t call_params;
  qapi_Status_t ret;

  LOG_INFO("DAM_APP:dam_dss_start_datacall \n");
  
  memset(&call_params, 0, sizeof(call_params));

  do
  {
    call_params.buf_val = NULL;
    call_params.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;
    if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param(hndl, QAPI_DSS_CALL_INFO_TECH_PREF_E, &call_params))
    {
      ret = QAPI_DSS_ERROR;
      LOG_INFO("DAM_APP:DSS Set Data Call Tech fail \n");
      break;
    }
    
    call_params.buf_val = dam_dss_apn;
    call_params.num_val = strlen(dam_dss_apn); 
    if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param(hndl, QAPI_DSS_CALL_INFO_APN_NAME_E, &call_params))
    {
      ret = QAPI_DSS_ERROR;
      LOG_INFO("DAM_APP:DSS Set Data Call APN fail \n");
      break;
    }

    dam_dss_ip_version = QAPI_DSS_IP_VERSION_4;
    call_params.buf_val = NULL;
    call_params.num_val = dam_dss_ip_version;
    if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param(hndl, QAPI_DSS_CALL_INFO_IP_VERSION_E, &call_params))
    {
      ret = QAPI_DSS_ERROR;      
      LOG_INFO("DAM_APP:DSS Set Data Call IP fail \n");
      break;
    }
    
    call_params.buf_val = NULL;
    call_params.num_val = 1;
    if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param(hndl, QAPI_DSS_CALL_INFO_UMTS_PROFILE_IDX_E, &call_params))
    {
      ret = QAPI_DSS_ERROR;      
      LOG_INFO("DAM_APP:DSS Set Data Call Profile fail \n");
      break;
    }

    ret = QAPI_DSS_SUCCESS;
  }while(0);

  return ret;
}

/*=============================================================================

FUNCTION DAM_DSS_START_DATACALL

DESCRIPTION
  This function will start data call with IP settings.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_dss_start_datacall(void)
{
  uint32_t                    received_sigs = 0;

  LOG_INFO("DAM_APP:dam_dss_start_datacall \n");

  do
  {
    /*----------------------------------------------------------------------------
      Initialize DSS and get service handle
    ----------------------------------------------------------------------------*/ 
    
    if (QAPI_DSS_SUCCESS != qapi_DSS_Init(QAPI_DSS_MODE_GENERAL))
    {
      LOG_INFO("DAM_APP:DSS Init fail \n");
    }

    if (QAPI_DSS_SUCCESS != qapi_DSS_Get_Data_Srvc_Hndl(dam_dss_cb, NULL, &dam_dss_hndl))
    {
      LOG_INFO("DAM_APP:DSS Get Srvc Handle fail \n");
      break;
    }


    /*----------------------------------------------------------------------------
      Set data call parameters
    ----------------------------------------------------------------------------*/
    if (QAPI_DSS_SUCCESS != dam_dss_set_para(dam_dss_hndl))
    {
      LOG_INFO("DAM_APP:DSS Set Para fail \n");
      break;
    }

    /*----------------------------------------------------------------------------
      Start data call
    ----------------------------------------------------------------------------*/ 

    if (QAPI_DSS_SUCCESS != qapi_DSS_Start_Data_Call(dam_dss_hndl))
    {
      LOG_INFO("DAM_APP:DSS Start Data Call fail \n");
      break;
    }
    
  } while (0);

  return;
}


