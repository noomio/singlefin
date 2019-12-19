/*===========================================================================
                         TEST_DNS_MODULE.C

DESCRIPTION
   This file deals with DNS related procedure.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "test_app_module.h"
#include "test_utils.h"
#include "qapi_dnsc.h"


/* Primary DNS Server of CU */
#define TEST_PRI_IPV4_DNS_SERVER "112.65.184.255" 
/* Secondary DNS Server of CU */
#define TEST_SEC_IPV4_DNS_SERVER "211.95.1.97" 
/* domain name */
#define TEST_DOMAIN_NAME "www.baidu.com"
/* name of dedicated iface */
#define IFACE_NAME "rmnet_data0"


#define IP_ADDR_SIZE 48

/*=============================================================================

FUNCTION DAM_DNS_INIT_MEM

DESCRIPTION
  This function is to allocate variable memory for DNS routine.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/

int dam_dns_init_mem(qapi_Net_DNS_Server_List_t **dns_svr_list, CHAR **ip_str, struct ip46addr **resolved_ip_addr)
{
  int ret = QAPI_DSS_ERROR;
  LOG_INFO("DAM_APP:dam_dns_init_mem start \n");
  
  if(*dns_svr_list)
  {
    data_free(*dns_svr_list);
    *dns_svr_list = NULL;
  }

  if(*ip_str)
  {
    data_free(*ip_str);
    *ip_str = NULL;
  }
   
  if(*resolved_ip_addr)
  {
    data_free(*resolved_ip_addr);
    *resolved_ip_addr = NULL;
  }

  do
  {
    /* Allocate DNS srv list */
    *dns_svr_list = (qapi_Net_DNS_Server_List_t *)data_malloc(sizeof(qapi_Net_DNS_Server_List_t));
    if(*dns_svr_list == NULL)
    {
      LOG_ERROR("DAM_APP:Allocate dns_svr_list fail \n");
      ret = QAPI_DSS_ERROR;
      break;
    }

    /* Allocate ip_str */
    *ip_str = (CHAR *)data_malloc(sizeof(CHAR)*IP_ADDR_SIZE);
    if(*ip_str == NULL)
    {
      LOG_ERROR("DAM_APP:Allocate ip_str fail \n");
      ret = QAPI_DSS_ERROR;
      break;
    }

    /* Allocate resolved_ip_addr */
    *resolved_ip_addr = (struct ip46addr *)data_malloc(sizeof(struct ip46addr));
    if(*resolved_ip_addr == NULL)
    {
      LOG_ERROR("DAM_APP:Allocate resolved_ip_addr fail \n");
      ret = QAPI_DSS_ERROR;
      break;
    }

    ret = QAPI_DSS_SUCCESS;
  } while(0);

  LOG_INFO("DAM_APP:dns_dam_init_mem exit with ret %d \n", ret);
  return ret;

}

/*=============================================================================

FUNCTION DAM_DNS_APP_ROUTINE

DESCRIPTION
  This function is to exercise QAPIs for DNS routine.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_dns_app_routine(void)
{
  int ret = QAPI_DSS_ERROR;
  ULONG dns_server_id;
  qapi_Net_DNS_Server_List_t *dns_svr_list = NULL; /* List of DNS Servers */
  CHAR *ip_str = NULL; /* Resolved IP Address in string format */
  struct ip46addr *resolved_ip_addr = NULL; /* Resolved IP Addr */
  
  LOG_INFO("DAM_APP:dam_dns_app_routine start \n");

  do
  {
    /* Memory allocation. */
    ret = dam_dns_init_mem(&dns_svr_list, &ip_str, &resolved_ip_addr);
    if(QAPI_DSS_SUCCESS != ret)
    {
      LOG_ERROR("DAM_APP:Allocate mem fail \n");
      break;
    }
    
    if(!qapi_Net_DNSc_Is_Started())
    {
      /* If DNS is not started, start DNS srv. */
      LOG_INFO("DAM_APP:Do qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E) \n");
      ret = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
      if(QAPI_DSS_SUCCESS != ret)
      {
        LOG_ERROR("DAM_APP:qapi_Net_DNSc_Command(start) fail \n");
        break;
      }
    }

    /* Add primary IPv4 DNS Server to dedicated iface(rmnet_data1) */
    LOG_INFO("DAM_APP:Do qapi_Net_DNSc_Add_Server_on_iface \n");
    ret = qapi_Net_DNSc_Add_Server_on_iface(TEST_PRI_IPV4_DNS_SERVER, QAPI_NET_DNS_V4_PRIMARY_SERVER_ID, IFACE_NAME);
    if(QAPI_DSS_SUCCESS != ret)
    {
      LOG_ERROR("DAM_APP:qapi_Net_DNSc_Add_Server_on_iface(primary IPV4 DNS) fail \n");
      break;
    }

    /* Add secondary IPv4 DNS Server to dedicated iface(rmnet_data1) */
    LOG_INFO("DAM_APP:Do qapi_Net_DNSc_Add_Server_on_iface \n");
    ret = qapi_Net_DNSc_Add_Server_on_iface(TEST_SEC_IPV4_DNS_SERVER, QAPI_NET_DNS_V4_SECONDARY_SERVER_ID, IFACE_NAME);
    if(QAPI_DSS_SUCCESS != ret)
    {
      LOG_ERROR("DAM_APP:qapi_Net_DNSc_Add_Server_on_iface(secondary IPV4 DNS) fail \n");
      break;
    }

    LOG_INFO("DAM_APP:Add IPv4 DNS Servers successfully \n");

    /* Get and print out all of the configured DNS Servers. */
    LOG_INFO("DAM_APP:Do qapi_Net_DNSc_Get_Server_List \n");
    ret = qapi_Net_DNSc_Get_Server_List(dns_svr_list, 0);
    if(QAPI_DSS_SUCCESS == ret)
    {
      for (int i = 0; i < MAX_DNS_SVR_NUM; ++i)
      {
        if (dns_svr_list->svr[i].type == AF_UNSPEC)
        {
          LOG_INFO("DAM_APP:Item[%d]: No DNS server is configured \n", i);
        }
        else
        {
          if(inet_ntop(dns_svr_list->svr[i].type, &(dns_svr_list->svr[i].a), ip_str, sizeof(CHAR)*IP_ADDR_SIZE))
          {
            /* Print IP Address of configured DNS server */
            LOG_INFO("DAM_APP:Item[%d]: DNS server %s \n", i, ip_str);
          }
          else
          {
            LOG_INFO("DAM_APP:Item[%d]: NULL DNS server \n", i);
          }
        }
        memset(ip_str, 0, sizeof(CHAR)*IP_ADDR_SIZE);
      }
    }

    /* Resolve TEST_DOMAIN_NAME for its IPv4 address */
    LOG_INFO("DAM_APP:Do qapi_Net_DNSc_Reshost_on_iface \n");
    resolved_ip_addr->type = AF_INET;
    ret = qapi_Net_DNSc_Reshost_on_iface(TEST_DOMAIN_NAME, resolved_ip_addr, IFACE_NAME);
    if(QAPI_DSS_SUCCESS == ret)
    {
      LOG_INFO("DAM_APP:DNS done for %s \n", TEST_DOMAIN_NAME);
      if(inet_ntop(resolved_ip_addr->type, &(resolved_ip_addr->a), ip_str, sizeof(CHAR)*IP_ADDR_SIZE))
      {
        /* Print resolved IPv4 address */
        LOG_INFO("DAM_APP:%s --> %s \n", TEST_DOMAIN_NAME, ip_str);
      }
      else
      {
        LOG_ERROR("DAM_APP:%s --> NULL \n", TEST_DOMAIN_NAME);
      }
    }
    else
    {
      LOG_ERROR("DAM_APP:Fail to resolve %s for IPv4 \n", TEST_DOMAIN_NAME);       
    }

    /* Del Primary IPv4 DNS Server */
    LOG_INFO("DAM_APP:Do qapi_Net_DNSc_Del_Server_on_iface \n");
    dns_server_id = QAPI_NET_DNS_V4_PRIMARY_SERVER_ID;
    ret = qapi_Net_DNSc_Del_Server_on_iface(dns_server_id, IFACE_NAME);
    if(QAPI_DSS_SUCCESS == ret)
    {
      LOG_INFO("DAM_APP:Del Primary IPv4 DNS Server successfully \n");
    }
    else
    {
      LOG_ERROR("DAM_APP:Del Primary IPv4 DNS Server fail \n");
    }

    /* Stop DNS */
    ret = qapi_Net_DNSc_Command(QAPI_NET_DNS_STOP_E);
    if(QAPI_DSS_SUCCESS != ret)
    {
      LOG_ERROR("DAM_APP:qapi_Net_DNSc_Command(stop) fail \n");
      break;
    }
  } while(0);

  if(resolved_ip_addr)
  {
    data_free(resolved_ip_addr);
    resolved_ip_addr = NULL;
  }

  if(dns_svr_list)
  {
    data_free(dns_svr_list);
    dns_svr_list = NULL;
  }

  if(ip_str)
  {
    data_free(ip_str);
    ip_str = NULL;
  }    

  LOG_INFO("DAM_APP:dam_dns_app_routine exit with ret %d \n", ret);
  return;
}

/*=============================================================================

FUNCTION DAM_APP_ROUTINE

DESCRIPTION
  This function is common wrapper to run module routine after data setup.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_app_routine(qapi_DSS_Hndl_t hndl)
{
  dam_dns_app_routine();
  return;
}

/*=============================================================================

FUNCTION DAM_APP_SIGNAL_WAIT

DESCRIPTION
  This function is to check and process signals for the sample app modules.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_app_signal_wait(uint32_t received_sigs)
{
  /* check and handle the signals that app interests */
  return;
}


