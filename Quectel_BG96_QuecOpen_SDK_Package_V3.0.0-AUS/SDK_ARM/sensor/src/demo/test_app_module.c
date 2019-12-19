/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file  test_app_module.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the Presence sensor demo application
 *
 **************************************************************************************************************/

#include "test_app_module.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "qapi_socket.h"
#include "qapi_cli.h"
#include "qflog_utils.h"

#define MAX_SEN_INSTANCE 1
#define MAX_URI_LEN 256
#define MAX_APN_LEN 64
#define LWM2M_MAX_ID   ((uint16_t)0xFFFF)
#define PROCESSING_DELAY 2
#define PRV_LINK_BUFFER_SIZE  1024
#define MAX_REG_RETRY  10

#define QAPI_USAGE_ERROR 2
#define     SENSOR_BYTE_POOL_SIZE 30720*8 

UCHAR free_memory_sensor[SENSOR_BYTE_POOL_SIZE];

unsigned long __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

void __attribute__((weak)) __stack_chk_fail (void)
{
    return;
}

UINT memscpy(
          void   *dst,
          UINT   dst_size,
          const  void  *src,
          UINT   src_size
          )
{
  UINT  copy_size = (dst_size <= src_size)? dst_size : src_size;
  memcpy(dst, src, copy_size);
  return copy_size;
}

TX_BYTE_POOL *byte_pool_sensor;

qapi_Net_LWM2M_App_Handler_t test_object_handler_app = NULL;

TX_THREAD *app_thread_handle;

qapi_Net_LWM2M_Event_t sen_obj_event;
bool lwm2m_client_sleep_state = false;

bool dam_app1_created = false;

bool app_registerd = false;

uint16_t digital_counter;

static qapi_TIMER_handle_t obs_notify_Timer;
static qapi_TIMER_handle_t btc_Timer;
static qapi_TIMER_handle_t ctb_Timer;

#define PRV_CONCAT_STR(buf, len, index, str, str_len)    \
{                                                    \
  if ((len)-(index) < (str_len)) return -1;        \
  memscpy((buf)+(index), ((len) - (index)), (str), (str_len));         \
  (index) += (str_len);                            \
}

static int utils_plainTextToInt64(uint8_t * buffer,
                                  int length,
                                  int64_t * dataP)
{
  uint64_t result = 0;
  int sign = 1;
  int i = 0;

  if (0 == length || dataP == NULL || buffer == NULL ) return 0;

  if (buffer[0] == '-')
  {
    sign = -1;
    i = 1;
  }

  while (i < length)
  {
    if ('0' <= buffer[i] && buffer[i] <= '9')
    {
      if (result > (UINT64_MAX / 10)) return 0;
      result *= 10;
      result += buffer[i] - '0';
    }
    else
    {
      return 0;
    }
    i++;
  }

  if (result > INT64_MAX) return 0;

  if (sign == -1)
  {
    *dataP = 0 - result;
  }
  else
  {
    *dataP = result;
  }

  return 1;
}

static int utils_plainTextToFloat64(uint8_t * buffer,
                                    int length,
                                    double * dataP)
{
  double result;
  int sign;
  int i;

  if (0 == length || buffer == NULL || dataP == NULL) return 0;

  if (buffer[0] == '-')
  {
    sign = -1;
    i = 1;
  }
  else
  {
    sign = 1;
    i = 0;
  }

  result = 0;
  while (i < length && buffer[i] != '.')
  {
    if ('0' <= buffer[i] && buffer[i] <= '9')
    {
      if (result > (DBL_MAX / 10)) return 0;
      result *= 10;
      result += (buffer[i] - '0');
    }
    else
    {
      return 0;
    }
    i++;
  }
  if (buffer[i] == '.')
  {
    double dec;

    i++;
    if (i == length) return 0;

    dec = 0.1;
    while (i < length)
    {
      if ('0' <= buffer[i] && buffer[i] <= '9')
      {
        if (result > (DBL_MAX - 1)) return 0;
        result += (buffer[i] - '0') * dec;
        dec /= 10;
      }
      else
      {
        return 0;
      }
      i++;
    }
  }

  *dataP = result * sign;
  return 1;
}

static size_t utils_intToText(int64_t data,
                              uint8_t *string,
                              size_t length)
{
  int index = 0;
  bool minus = false;
  size_t result = 0;
  if(string == NULL)
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }

  if (data < 0)
  {
    minus = true;
    data = 0 - data;
  }
  else
  {
    minus = false;
  }

  index = length - 1;
  do
  {
    string[index] = '0' + data%10;
    data /= 10;
    index --;
  } while(index >= 0 && data > 0);

  if(data > 0)
  return 0;

  if(minus == true)
  {
    if(index == 0)
    return 0;
    string[index] = '-';
  }
  else
  {
    index++;
  }

  result = length - index;

  if (result < length)
  {
    memscpy(string, length, string + index, result);
  }

  return result;
}

static size_t utils_floatToText(double data,
                         uint8_t * string,
                         size_t length)
{
  size_t intLength = 0;
  size_t decLength = 0;
  int64_t intPart = 0;
  double decPart = 0;

  if(string == NULL)
  {
    LOG_INFO(" Passed NULL Arguments\n");
    return 0;
  }

  if (data <= (double)INT64_MIN || data >= (double)INT64_MAX) return 0;

  intPart = (int64_t)data;
  decPart = data - intPart;
  if (decPart < 0)
  {
    decPart = 1 - decPart;
  }
  else
  {
    decPart = 1 + decPart;
  }

  if (decPart <= 1 + FLT_EPSILON)
  {
    decPart = 0;
  }

  if (intPart == 0 && data < 0)
  {
    // deal with numbers between -1 and 0
    if (length < 4) return 0;   // "-0.n"
    string[0] = '-';
    string[1] = '0';
    intLength = 2;
  }
  else
  {
    intLength = utils_intToText(intPart, string, length);
    if (intLength == 0) return 0;
  }
  decLength = 0;
  if (decPart >= FLT_EPSILON)
  {
    int i;
    double noiseFloor;

    if (intLength >= length - 1) return 0;

    i = 0;
    noiseFloor = FLT_EPSILON;
    do
    {
      decPart *= 10;
      noiseFloor *= 10;
      i++;
    } while (decPart - (int64_t)decPart > noiseFloor);

    decLength = utils_intToText(decPart, string + intLength, length - intLength);
    if (decLength <= 1) return 0;

    // replace the leading 1 with a dot
    string[intLength] = '.';
  }

  return intLength + decLength;
}

static int uri_toString(qapi_Net_LWM2M_Uri_t *uri,
                        uint8_t * buffer,
                        size_t bufferLen,
                        qapi_Net_LWM2M_Uri_Depth_t *depth)
{
  size_t head = 0;
  int res = 0;

  if(buffer == NULL)
  {
    LOG_INFO("Passed NULL Arguments \n");
    return -1;
  }
  buffer[0] = '/';

  if(uri == NULL)
  {
    if(depth) 
    {
      *depth = URI_DEPTH_OBJECT;
    }
   return 1;
  }

  head = 1;

  res = utils_intToText(uri->objectId, buffer + head, bufferLen - head);
  if (res <= 0)
  {
    return -1;
  }
  head += res;

  if(head >= bufferLen - 1)
  {
    return -1;
  }
  if(depth) 
  {
    *depth = URI_DEPTH_OBJECT_INSTANCE;
  }

  if(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E)
  {
    buffer[head] = '/';
    head++;
    res = utils_intToText(uri->instanceId, buffer + head, bufferLen - head);
    if(res <= 0)
    {
      return -1;
    }
    head += res;
    if(head >= bufferLen - 1) return -1;
    if(depth) *depth = URI_DEPTH_RESOURCE;
    if(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
    {
      buffer[head] = '/';
      head++;
      res = utils_intToText(uri->resourceId, buffer + head, bufferLen - head);
      if(res <= 0) return -1;
      head += res;
      if(head >= bufferLen - 1) return -1;
      if(depth) *depth = URI_DEPTH_RESOURCE_INSTANCE;
    }
  }

  buffer[head] = '/';
  head++;

  LOG_INFO("length: %u, buffer: \"%.*s\"", head, head, buffer);

  return head;
}

static int setBuffer(qapi_Net_LWM2M_Flat_Data_t *dataP,
                     uint8_t * buffer,
                     size_t bufferLen)
{
  if( dataP == NULL || buffer == NULL || bufferLen == 0 ) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return 0;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&(dataP->value.asBuffer.buffer), bufferLen, TX_NO_WAIT);
  if (dataP->value.asBuffer.buffer == NULL)
  {
    data_free(1, dataP);
    return 0;
  }
  dataP->value.asBuffer.length = bufferLen;
  memcpy(dataP->value.asBuffer.buffer, buffer, bufferLen);

  return 1;
}


static void lwm2m_data_encode_string(const char * string,
                              qapi_Net_LWM2M_Flat_Data_t * dataP)
{
  size_t len = 0;
  int res = 0;

  if(dataP == NULL)
  {
    LOG_INFO("Passed NULL Arguments\n");
    return;
  }

  if (string == NULL)
  {
    len = 0;
  }
  else
  {
    for (len = 0; string[len] != 0; len++);
  }

  if (len == 0)
  {
    dataP->value.asBuffer.length = 0;
    dataP->value.asBuffer.buffer = NULL;
    res = 1;
  }
  else
  {
    res = setBuffer(dataP, (uint8_t *)string, len);
  }

  if (res == 1)
  {
    dataP->type = QAPI_NET_LWM2M_TYPE_STRING_E;
  }
  else
  {
    dataP->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
  }
}

static void lwm2m_data_encode_int(int64_t value,
                           qapi_Net_LWM2M_Flat_Data_t * dataP)
{

  if( dataP == NULL ) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return ;
  }

  dataP->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  dataP->value.asInteger = value;
}

/*static void lwm2m_data_encode_float(double value,
                             qapi_Net_LWM2M_Flat_Data_t * dataP)
{
  LOG_INFO("value: %d", value);
  if( dataP == NULL ) 
  { 
    LOG_INFO("Passed  Argument is NULL");
    return ;
  }

  dataP->type = QAPI_NET_LWM2M_TYPE_FLOAT_E;
  dataP->value.asFloat = value;
}
*/

static void lwm2m_data_encode_bool(bool value,
                            qapi_Net_LWM2M_Flat_Data_t * dataP)
{

  if( dataP == NULL ) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return ;
  }

  dataP->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  dataP->value.asBoolean = value;
}

/**
 * @fn utils_copyValue() 
 * @brief This function is used to copy value input string into output string
 * @param dst destination string
 * @param src source string
 * @param len length to be copied
 * @return  void
 */
static void utils_copyValue(void * dst,
    const void * src,
    size_t len)
{ 
#ifdef LWM2M_LITTLE_ENDIAN
  size_t i = 0;
#endif  
  if (dst == NULL || src == NULL)
  {
    LOG_INFO("LWM2M_LOG:Passed NULL Arguments\n");
    return;
  }
#ifdef LWM2M_BIG_ENDIAN
  LWM2M_MEMCPY(dst, src, len);
#else
#ifdef LWM2M_LITTLE_ENDIAN
  for (i = 0; i < len; i++)
  {
    ((uint8_t *)dst)[i] = ((uint8_t *)src)[len - 1 - i];
  }
#endif
#endif
}

/**
 * @fn utils_opaqueToFloat()
 * @brief This function is used to convert opaque to Float
 * @param buffer input opaque buffer
 * @param buffer_len buffer length
 * @param dataP output float param
 * @return 0 on failure
 *         buffer_len on success
 */
static int utils_opaqueToFloat(const uint8_t * buffer,
    size_t buffer_len,
    double * dataP)
{ 
  if( buffer == NULL || dataP == NULL ) 
  { 
    LOG_INFO("LWM2M_LOG: Passed  Argument is NULL");
    return 0;
  }

  switch (buffer_len)
  {
    case 4:
      {
        float temp;

        utils_copyValue(&temp, buffer, buffer_len);

        *dataP = temp;
      }
      return 4;

    case 8:
      utils_copyValue(dataP, buffer, buffer_len);
      return 8;

    default:
      return 0;
  }

}

static int lwm2m_data_decode_float(qapi_Net_LWM2M_Flat_Data_t * dataP,
                                   double * valueP)
{
  int result = 0;
  if( dataP == NULL || valueP == NULL ) 
  {
    LOG_INFO(" Passed  Argument is NULL");
    return 0; 
  }

  switch (dataP->type)
  {
    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      *valueP = dataP->value.asFloat;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      *valueP = (double)dataP->value.asInteger;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_STRING_E:
      result = utils_plainTextToFloat64(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      break;
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
	  result = utils_opaqueToFloat(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);		
      if (result == (int)dataP->value.asBuffer.length)
      {
        result = 1;
      }
      else
      {
        result = 0;
      }
      break;

    default:
      return 0;
  }

  return result;
}

static int utils_opaqueToInt(const uint8_t * buffer,
    size_t buffer_len,
    int64_t * dataP)
{
  if( buffer == NULL || dataP == NULL ) 
  {
    LOG_INFO("LWM2M_LOG: Passed  Argument is NULL");
    return 0;
  } 

  *dataP = 0;

  switch (buffer_len)
  {
    case 1:
      {
        *dataP = (int8_t)buffer[0];

        break;
      }

    case 2:
      {
        int16_t value;

        utils_copyValue(&value, buffer, buffer_len);

        *dataP = value;
        break;
      }

    case 4:
      {
        int32_t value;

        utils_copyValue(&value, buffer, buffer_len);

        *dataP = value;
        break;
      }

    case 8:
      if(buffer_len <= sizeof(int64_t) )
      {
        utils_copyValue(dataP, buffer, buffer_len);
        return buffer_len;
      }
      else
      {
        return 0;
      }

    default:
      return 0;
  }

  return buffer_len;
}

static int lwm2m_data_decode_int(qapi_Net_LWM2M_Flat_Data_t * dataP,
                                 int64_t * valueP)
{
  int result;
  if ( dataP == NULL || valueP == NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }

  switch (dataP->type)
  {
    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      *valueP = dataP->value.asInteger;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_STRING_E:
      result = utils_plainTextToInt64(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      break;

    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
      result = utils_opaqueToInt(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      if (result == (int)dataP->value.asBuffer.length)
      {
        result = 1;
      }
      else
      {
        result = 0;
      }
      break;

    default:
      return 0;
  }

  return result;
}

/*static int lwm2m_data_decode_bool(qapi_Net_LWM2M_Flat_Data_t * dataP,
                                  bool * valueP)
{
  int result; 
  if ( dataP == NULL || valueP == NULL ) 
  {
    LOG_INFO(" Passed  Argument is NULL");
    return 0;
  }

  switch (dataP->type)
  {
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      *valueP = dataP->value.asBoolean;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_STRING_E:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
      if (dataP->value.asBuffer.length != 1) return 0;

      switch (dataP->value.asBuffer.buffer[0])
      {
        case '0':
          *valueP = false;
          result = 1;
          break;
        case '1':
          *valueP = true;
          result = 1;
          break;
        default:
          result = 0;
          break;
      }
      break;

    default:
      result = 0;
      break;
  }

  return result;
}
*/
int obj_inst_index_presence;
int obj_inst_created_presence[MAX_SEN_INSTANCE] = {0};

qapi_Status_t cmd_handler_cb(qapi_CLI_App_Handler_t handle, 
                             qapi_CLI_Parameter_Data_t *param_data,
                             void *user_data)
{
  sen_obj_cmd_t*        cmd_ptr = NULL;
  qapi_Net_LWM2M_Sen_t *sen_param = NULL;

  tx_byte_allocate(byte_pool_sensor, (VOID **)&sen_param, sizeof(qapi_Net_LWM2M_Sen_t), TX_NO_WAIT);

  if(sen_param == NULL)
  {
    LOG_INFO("Cannot assign memory for buffer");
    return QAPI_ERR_NO_MEMORY;
  }

  memset(sen_param, 0x00, sizeof(qapi_Net_LWM2M_Sen_t));

  sen_param->app_data.cmd_op.handle = handle;
  sen_param->app_data.cmd_op.param_data = param_data;
  sen_param->app_data.cmd_op.user_data = user_data;

  LOG_INFO("Switching from QCLI to application callback context");

  if((cmd_ptr = sen_obj_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return QAPI_ERR_NO_ENTRY;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = SEN_OBJ_CMD_FUNC_EVT;
    cmd_ptr->cmd_data.data               = (void *)sen_param;

    if(app_registerd == false)
    {
      tx_byte_release(sen_param);
      return QAPI_ERROR;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    sen_obj_enqueue_cmd(cmd_ptr);
  }

  sen_obj_set_signal();

  return QAPI_OK;

}

#define SEN_TYPE "PIR"

const qapi_CLI_Command_t lwm2m_sen_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description, Abort_function */
    {auto_create, false, "start", "Start ", "Register the application and create presence sensor instance", NULL},
    {set_input_cnt, false, "set_counter", "set_counter <counter value> [0 - 65535] ", "Set the digital input counter value.", NULL},
};

qapi_Net_LWM2M_Data_t *object_presence_sensor = NULL;
qapi_Net_LWM2M_Observed_t *sen_observed_list = NULL;
qapi_Net_LWM2M_Pending_Observed_t *sen_pending_observed_list = NULL;

qapi_CLI_Command_Group_t lwm2m_sen_cmd_group =
{
    "PRESENCE_SENSOR",              /* Group_String: will display cmd prompt as "PRESENCE_SENSOR> " */
    sizeof(lwm2m_sen_cmd_list)/sizeof(lwm2m_sen_cmd_list[0]),   /* Command_Count */
    lwm2m_sen_cmd_list        /* Command_List */
};

qapi_CLI_App_Handler_t lwm2m_sen_handle;     /* Handle for Presence sensor command Group. */

/********************************************************************************
 * This function is used to register the Presence Sensor Object Command Group with QCLI.
 ********************************************************************************/
void Initialize_Sensor_App(void)
{
  qapi_Status_t result = QAPI_ERROR;
  int i = 0;

  for(i = 0; i < MAX_REG_RETRY; i++)
  {
    /* Attempt to reqister the Command Groups with the qcli framework.*/

    result = qapi_CLI_Register_Command_Group(&lwm2m_sen_handle, NULL, &lwm2m_sen_cmd_group, cmd_handler_cb);
    if(result == QAPI_OK)
    {
      qapi_CLI_Pass_Pool_Ptr(lwm2m_sen_handle, byte_pool_sensor);
      return;
    }
    qapi_Timer_Sleep(1000, QAPI_TIMER_UNIT_MSEC, true);
  }
    return;
}

void notify_signal_cb(uint32 userData)
{
  int id = 0;
  sen_obj_cmd_t* cmd_ptr = NULL;
  qapi_Net_LWM2M_Sen_t *sen_param = NULL;

  tx_byte_allocate(byte_pool_sensor, (VOID **)&sen_param, sizeof(qapi_Net_LWM2M_Sen_t), TX_NO_WAIT);

  if(sen_param == NULL)
  {
    LOG_INFO("Cannot assign memory for sensor object parameter");
    return;
  }

  memset(sen_param, 0x00, sizeof(qapi_Net_LWM2M_Sen_t));

  sen_param->app_data.ul_op.id = id;
  sen_param->app_data.ul_op.msg_type = QAPI_NET_LWM2M_SEN_NOTIFY_E;

  LOG_INFO("Switching from Timer to application callback context");

  if((cmd_ptr = sen_obj_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = SEN_OBJ_APP_ORIGINATED_EVT;
    cmd_ptr->cmd_data.data               = (void *)sen_param;

    if(app_registerd == false)
    {
      //QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
      tx_byte_release(sen_param);
      return;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    sen_obj_enqueue_cmd(cmd_ptr);
  }

  sen_obj_set_signal();

  return;
}

bool check_readable(uint16_t obj_id, uint16_t id)
{
  bool res = false;

  if(obj_id == PRESENCE_SENSOR_OBJECT_ID)
  {
    switch(id)
    {
      case RES_M_DIG_INPUT_STATE:
      case RES_O_DIG_INPUT_COUNTER:
      case RES_O_SENSOR_TYPE:
      case RES_O_BUSY_TO_CLEAR_DELAY:
      case RES_O_CLEAR_TO_BUSY_DELAY:
           res = true;
      break;
      default:
           res = false;
      break;
    }
  }
  return res;
}

bool check_writable(uint16_t obj_id, uint16_t id)
{
  bool res = false;

  if(obj_id == PRESENCE_SENSOR_OBJECT_ID)
  {
    switch(id)
    {
      case RES_O_BUSY_TO_CLEAR_DELAY:
      case RES_O_CLEAR_TO_BUSY_DELAY:
           res = true;
      break;
      default:
           res = false;
      break;
    }
  }
  return res;
}

bool check_executable(uint16_t obj_id, uint16_t id)
{
  bool res = false;

  if(obj_id == PRESENCE_SENSOR_OBJECT_ID)
  {
    switch(id)
    {
      case RES_O_RESET_DIG_INPUT_COUNTER:
           res = true;
      break;
      default:
           res = false;
      break;
    }
  }
  return res;
}


static qapi_Status_t qapi_Net_LWM2M_Free_Resource_Info(qapi_Net_LWM2M_Resource_Info_t *resourceP)
{
  qapi_Net_LWM2M_Resource_Info_t *resource_linkP = NULL;
  if (resourceP == NULL)
  {
    return QAPI_ERROR;
  }
  while (resourceP != NULL)
  {
    resource_linkP =   resourceP->next;
    if((resourceP->type == QAPI_NET_LWM2M_TYPE_STRING_E) && resourceP->value.asBuffer.buffer)
    {
      tx_byte_release(resourceP->value.asBuffer.buffer);
    }
    tx_byte_release(resourceP);
    resourceP = resource_linkP;
  }
   return QAPI_OK;
}

static qapi_Status_t qapi_Net_LWM2M_Free_Single_Instance_Info(qapi_Net_LWM2M_Instance_Info_t *instanceP)
{
  qapi_Net_LWM2M_Instance_Info_t *instance_linkP = NULL;
  if (instanceP == NULL)
  {
    return QAPI_ERROR;
  }
  if(instanceP != NULL)
  {
    instance_linkP = instanceP->next;
    qapi_Net_LWM2M_Free_Resource_Info(instanceP->resource_info);
    tx_byte_release(instanceP);
    instanceP = instance_linkP;
  }
  return QAPI_OK;
}

uint16_t object_check_readable(qapi_Net_LWM2M_Data_t *object,
                              qapi_Net_LWM2M_Uri_t *uri)
{
  uint16_t result = QAPI_NET_LWM2M_IGNORE_E;
  qapi_Net_LWM2M_Data_t *target = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  int size;

  if(object == NULL || uri == NULL ) 
  {
    LOG_INFO(" fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  target = lwm2m_object_find(object, uri->objectId);

  if(target == NULL)
  {
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }
  
  if(!(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E)) 
  {
    return QAPI_NET_LWM2M_205_CONTENT_E;
  }
  
  if(lwm2m_instance_find(target->instance_info, uri->instanceId) == NULL)
  {
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }
  
  if(!(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)) 
  {
    return QAPI_NET_LWM2M_205_CONTENT_E;
  }

  size = 1;
  data = data_malloc(1);

  if(data == NULL)
  {
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  }
  
  data->id = uri->resourceId;

  result = read_data_sen(uri->instanceId, &size, &data, object);

  data_free(1, data);

  if(result == QAPI_OK)
  {
    result = QAPI_NET_LWM2M_205_CONTENT_E;
  }

  return result;
}

uint16_t object_check_numeric(qapi_Net_LWM2M_Data_t *object,
                             qapi_Net_LWM2M_Uri_t *uri)
{
  uint16_t result = QAPI_NET_LWM2M_IGNORE_E;
  qapi_Net_LWM2M_Data_t *target = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  int size;

  if (object == NULL || uri == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  if(!(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)) 
  {
    return QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E;
  }

  target = lwm2m_object_find(object, uri->objectId);

  if (target == NULL)
  {
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }

  size = 1;
  data = data_malloc(1);

  if(data == NULL)
  {
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  }
  
  data->id = uri->resourceId;

  result = read_data_sen(uri->instanceId, &size, &data, object);

  if (result == QAPI_OK)
  {
    switch (data->type)
    {
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
        result = (uint16_t)QAPI_NET_LWM2M_205_CONTENT_E;
        break;
      default:
        result = (uint16_t) QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E;
    }
  }

  data_free(1, data);

  return result;
}

qapi_Status_t sensor_object_app_callback(qapi_Net_LWM2M_App_Handler_t handler, 
                                         qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data,
                                         void                         *user_data)
{
  sen_obj_cmd_t*        cmd_ptr = NULL;
  qapi_Net_LWM2M_Sen_t *sen_param = NULL;

  tx_byte_allocate(byte_pool_sensor, (VOID **)&sen_param, sizeof(qapi_Net_LWM2M_Sen_t), TX_NO_WAIT);

  if(sen_param == NULL)
  {
    LOG_INFO("Cannot assign memory for buffer");
    return QAPI_ERR_NO_MEMORY;
  }

  memset(sen_param, 0x00, sizeof(qapi_Net_LWM2M_Sen_t));

  tx_byte_allocate(byte_pool_sensor, (VOID **)&(sen_param->app_data.dl_op.lwm2m_srv_data), 
                   sizeof(qapi_Net_LWM2M_Server_Data_t), TX_NO_WAIT);

  if(sen_param->app_data.dl_op.lwm2m_srv_data == NULL)
  {
    LOG_INFO("Cannot assign memory for buffer");
    return QAPI_ERR_NO_MEMORY;
  }
  memset(sen_param->app_data.dl_op.lwm2m_srv_data, 0x00, sizeof(qapi_Net_LWM2M_Server_Data_t));
  memcpy(sen_param->app_data.dl_op.lwm2m_srv_data, lwm2m_srv_data, sizeof(qapi_Net_LWM2M_Server_Data_t));
  sen_param->app_data.dl_op.handler = handler;
  if(lwm2m_srv_data->lwm2m_attr != NULL)
  {
    tx_byte_allocate(byte_pool_sensor, (VOID **)&(sen_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr), 
                     sizeof(qapi_Net_LWM2M_Attributes_t), TX_NO_WAIT);

    if(sen_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr == NULL)
    {
      LOG_INFO("Cannot assign memory for buffer");
      return QAPI_ERR_NO_MEMORY;
    }
    memset(sen_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));
    memcpy(sen_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr, lwm2m_srv_data->lwm2m_attr, sizeof(qapi_Net_LWM2M_Attributes_t));
  }
  if(lwm2m_srv_data->payload != NULL && lwm2m_srv_data->payload_len != 0)
  {
    tx_byte_allocate(byte_pool_sensor, (VOID **)&(sen_param->app_data.dl_op.lwm2m_srv_data->payload), 
                     sen_param->app_data.dl_op.lwm2m_srv_data->payload_len, TX_NO_WAIT);

    if(sen_param->app_data.dl_op.lwm2m_srv_data->payload == NULL)
    {
      LOG_INFO("Cannot assign memory for buffer");
      return QAPI_ERR_NO_MEMORY;
    }
    memset(sen_param->app_data.dl_op.lwm2m_srv_data->payload, 0x00, sen_param->app_data.dl_op.lwm2m_srv_data->payload_len);
    memcpy(sen_param->app_data.dl_op.lwm2m_srv_data->payload, lwm2m_srv_data->payload, sen_param->app_data.dl_op.lwm2m_srv_data->payload_len);
  }
  sen_param->app_data.dl_op.data = user_data;

  LOG_INFO("Switching from QCLI to application callback context");

  if((cmd_ptr = sen_obj_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return QAPI_ERR_NO_ENTRY;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = SEN_OBJ_CALL_BACK_EVT;
    cmd_ptr->cmd_data.data               = (void *)sen_param;

    if(app_registerd == false)
    {
      //QCLI_Printf(lwm2m_ext_handle,"Application not registerd ");
      if(sen_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr != NULL)
      tx_byte_release(sen_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr);
      tx_byte_release(sen_param->app_data.dl_op.lwm2m_srv_data);
      tx_byte_release(sen_param);
      return QAPI_ERROR;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    sen_obj_enqueue_cmd(cmd_ptr);
  }

  sen_obj_set_signal();

  return QAPI_OK;

}

qapi_Status_t sen_obj_delete_all(qapi_Net_LWM2M_App_Handler_t handler, 
                                 qapi_Net_LWM2M_Server_Data_t op_data)
{
  int i = 0;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Object_Info_t instance_info = {0};
  qapi_Net_LWM2M_Uri_t uri;

  if(handler == test_object_handler_app)
  {
    object = lwm2m_object_find(object_presence_sensor,PRESENCE_SENSOR_OBJECT_ID);
    if(object != NULL)
    {
      for(i =0; i< MAX_SEN_INSTANCE; i++)
      {
        if(obj_inst_created_presence[i] == 1)
        {
          instance = lwm2m_instance_find(object->instance_info, i);
          if(instance != NULL)
          {
            memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
            uri.flag =  (QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E);
            uri.objectId = PRESENCE_SENSOR_OBJECT_ID;
            uri.instanceId = i;

            observe_delete_sen_handle_req(handler, &uri);
            object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
            if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
            {
              LOG_INFO("Instance id %d cleared ", i);
              object->no_instances --;
              obj_inst_created_presence[i] = 0;
              instance = NULL;

              instance_info.no_object_info = 1;
              tx_byte_allocate(byte_pool_sensor, (VOID **)&instance_info.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
              if(instance_info.id_info == NULL)
              {
                return QAPI_ERR_NO_MEMORY;
              }
              memset(instance_info.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_t));
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
              instance_info.id_info->object_ID = PRESENCE_SENSOR_OBJECT_ID;
              instance_info.id_info->instance_ID = i;

              qapi_Net_LWM2M_Delete_Object_Instance(handler, &instance_info);

              if(instance_info.id_info)
              {
                tx_byte_release(instance_info.id_info);
                instance_info.id_info = NULL;
              }
            }
          }
        }
      }
      LOG_INFO("LWM2M_LOG: Deleted all instances.\n");
      return QAPI_OK;
    }
  }

  return QAPI_ERROR;
}

qapi_Status_t sen_obj_handle_event(qapi_Net_LWM2M_App_Handler_t handler, 
                                   qapi_Net_LWM2M_Server_Data_t op_data)
{
  int i = 0;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t *object = NULL;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  if(op_data.event == QAPI_NET_LWM2M_SLEEP_E)
  {
    lwm2m_client_sleep_state = true;
  }

  if(op_data.event == QAPI_NET_LWM2M_WAKEUP_E)
  {
    lwm2m_client_sleep_state = false;
  }

  if(op_data.event == QAPI_NET_LWM2M_REGISTERTION_COMPELTED_E)
  {
    lwm2m_client_sleep_state = false;
  }

  if(op_data.event == QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E)
  {
    if(handler == test_object_handler_app)
    {
      test_object_handler_app = NULL;
      stop_sen_obs_notify_timer(handler);

      object = lwm2m_object_find(object_presence_sensor, PRESENCE_SENSOR_OBJECT_ID);
      if(object != NULL)
      {
        for(i =0; i< MAX_SEN_INSTANCE; i++)
        {
          if(obj_inst_created_presence[i] == 1)
          {
            instance = lwm2m_instance_find(object->instance_info, i);
            if(instance != NULL)
            {
              object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
              if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
              {
                LOG_INFO("Instance id %d cleared ", i);
                object->no_instances --;
                obj_inst_created_presence[i] = 0;
                instance = NULL;
              }
            }
          }
        }
      }
    }
    LOG_INFO("LWM2M_LOG: Extensible object application de-registered successfully.\n");

  }

  if(op_data.event)
  {
    sen_obj_event = op_data.event;
    return QAPI_OK;
  }

  return QAPI_ERROR;
}

qapi_Status_t sen_object_register(int id)
{
  qapi_Status_t result = QAPI_ERROR;
  void *user_data = NULL;

  if(id == 0)
  {
    if(test_object_handler_app != NULL)
    {
      LOG_INFO("LWM2M_LOG: Presence Sensor object application already registered.\n");
      //QCLI_Printf(lwm2m_ext_handle, "Application 1 already registered.\n");
      return result;
    }
  
    result = qapi_Net_LWM2M_Register_App_Extended(&test_object_handler_app, 
                                                  user_data,
                                                  sensor_object_app_callback);
    if(result != QAPI_OK)
    {
      test_object_handler_app = NULL;
      user_data = NULL;
      return result;
    }

    qapi_Net_LWM2M_Pass_Pool_Ptr(test_object_handler_app, byte_pool_sensor);
  }

  if(result == QAPI_OK)
  {
    //QCLI_Printf(lwm2m_ext_handle, "Application %d registered successfully.\n", id+1);
    LOG_INFO("LWM2M_LOG: Presence sensor object application %d registered successfully.\n", id+1);
  }

  else
  {
    //QCLI_Printf(lwm2m_ext_handle, "Error in application %d registeration.\n", id+1);
    LOG_INFO("LWM2M_LOG: Error in Presence sensor object application %d registration.\n", id+1);
  }

  return result;
}

qapi_Status_t sen_object_deregister(int id)
{
  qapi_Status_t result = QAPI_ERROR;
  int i = 0;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t *object = NULL;

  if(id == 0)
  {
    if(test_object_handler_app == NULL)
    {
      LOG_INFO("LWM2M_LOG: Presence sensor object application not registered.\n");
      //QCLI_Printf(lwm2m_ext_handle, "Application 1 not registered.\n");
      return result;
    }

    result = qapi_Net_LWM2M_DeRegister_App(test_object_handler_app);
  }

  if(result == QAPI_OK)
  {
    if(id == 0)
    {
      test_object_handler_app = NULL;

      object = lwm2m_object_find(object_presence_sensor, PRESENCE_SENSOR_OBJECT_ID);
      if(object != NULL)
      {
        for(i =0; i< MAX_SEN_INSTANCE; i++)
        {
          if(obj_inst_created_presence[i] == 1)
          {
            instance = lwm2m_instance_find(object->instance_info, i);
            if(instance != NULL)
            {
              object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
              if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
              {
                LOG_INFO("Instance id %d cleared ", i);
                object->no_instances --;
                obj_inst_created_presence[i] = 0;
                instance = NULL;
              }
            }
          }
        }
      }
    }

    //QCLI_Printf(lwm2m_ext_handle, "Application %d de-registered. successfully\n", id+1);
    LOG_INFO("LWM2M_LOG: Presence sensor object application de-registered successfully.\n");
  }

  else
  {
    //QCLI_Printf(lwm2m_ext_handle, "Error in Application de-registration\n", id+1);
    LOG_INFO("LWM2M_LOG: Error in Presence sensor object application de-registration.\n");
  }

  return result;
}

/**
 * @fn     create_object()
 *
 * @brief  This fucntion is used to create object for Presence sensor object application.
 *
 * @param  int id      Identifier for different application 
 * @return QAPI_OK     Success case
 *         QAPI_ERROR  Failure case
 */
qapi_Status_t create_object(int id)
{
  qapi_Net_LWM2M_Data_t *object = NULL;

  if(id == 0)
  {
    if(object_presence_sensor == NULL)
    {
      tx_byte_allocate(byte_pool_sensor, (VOID **)&object, sizeof(qapi_Net_LWM2M_Data_t), TX_NO_WAIT);
      if(object == NULL)
      {
        LOG_INFO("Memory allocation failure. \n");
        //QCLI_Printf(lwm2m_ext_handle, "Object not created.\n");
        return QAPI_ERR_NO_MEMORY;
      }
  
      memset(object, 0x00, sizeof(qapi_Net_LWM2M_Data_t));
  
      object->object_ID = PRESENCE_SENSOR_OBJECT_ID;
      object->next = NULL;
      object->no_instances = 0;
      object->instance_info = NULL;
  
      object_presence_sensor = lwm2m_object_add(object_presence_sensor,
                                                object);
  
      if(object_presence_sensor != NULL)
      {
        LOG_INFO("Presence sensor object created. \n");
        //QCLI_Printf(lwm2m_ext_handle, "Object 1001 created.\n");
        return QAPI_OK;
      }
      else
      {
        LOG_INFO("Error in creation of Presence sensor object \n");
        //QCLI_Printf(lwm2m_ext_handle, "Error in creation of object 1001. \n");
        return QAPI_ERROR;
      }
    }
    else
    {
      LOG_INFO("Presence sensor object already created.\n");
      //QCLI_Printf(lwm2m_ext_handle, "Object 1001 & 1002 already created. \n");
      return QAPI_ERR_EXISTS;
    }
  }

  return QAPI_ERROR;
}

qapi_Status_t create_object_instance_app()
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t lwm2m_data;
  qapi_Net_LWM2M_Instance_Info_t lwm2m_inst;
  qapi_Net_LWM2M_Resource_Info_t lwm2m_res;
  uint32_t obj_id;

  memset(&lwm2m_inst, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));
  memset(&lwm2m_res, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  if(test_object_handler_app == NULL)
  {
    LOG_INFO("Application not registerd Please register application ");
    //QCLI_Printf(lwm2m_ext_handle, "Application 1 not registerd Please register application \n");
    return QAPI_ERROR;
  }
  if(object_presence_sensor == NULL)
  {
    LOG_INFO("Object is not valid.");
    //QCLI_Printf(lwm2m_ext_handle, "Object is not valid. \n");
    return QAPI_ERROR;
  }

  obj_id = PRESENCE_SENSOR_OBJECT_ID;

  object = lwm2m_object_find(object_presence_sensor, obj_id);

  if(object == NULL)
  {
    LOG_INFO("Object not found.");
    //QCLI_Printf(lwm2m_ext_handle, "Object not found. \n");
    return QAPI_ERR_NO_ENTRY;
  }

  memset(&lwm2m_data, 0x00, sizeof(qapi_Net_LWM2M_Data_t));

  if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
  {
    instance = obj_create_presence_sensor(&lwm2m_data);
  }

  if(lwm2m_data.object_ID == PRESENCE_SENSOR_OBJECT_ID && instance != NULL)
  {
    lwm2m_data.instance_info = &lwm2m_inst;
    lwm2m_data.instance_info->instance_ID = obj_inst_index_presence;
    lwm2m_data.instance_info->no_resources = instance->no_resources;
    lwm2m_data.instance_info->resource_info = &lwm2m_res;
  }

  if((instance != NULL) && test_object_handler_app != NULL)
  {
    result = qapi_Net_LWM2M_Create_Object_Instance(test_object_handler_app, &lwm2m_data);
  }

  else
  {
    LOG_INFO("Cannot create object instance");
    //QCLI_Printf(lwm2m_ext_handle, "Cannot create object instance error is %d\n", result);
    return QAPI_ERR_NO_MEMORY;
  }

  if(result == QAPI_OK)
  {
    LOG_INFO("Object instance created successfully.\n");
    //QCLI_Printf(lwm2m_ext_handle, "Object instance %d created successfully for object 1001\n", obj_inst_index_generic);
    object->instance_info = lwm2m_instance_add(object->instance_info, instance);
    if(object->instance_info && object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      obj_inst_created_presence[obj_inst_index_presence] = 1;
    }
  }

  else
  {
    qapi_Net_LWM2M_Free_Single_Instance_Info(instance);
    //QCLI_Printf(lwm2m_ext_handle, "Object instance not created.Error is %d\n", result);
    LOG_INFO("Object instance not created.Error is %d\n", result);
  }

  return result;
}

qapi_Net_LWM2M_Instance_Info_t* obj_create_presence_sensor(qapi_Net_LWM2M_Data_t *lwm2m_data)
{
  int i =0;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res3 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res4 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res5 = NULL;

  object = lwm2m_object_find(object_presence_sensor, PRESENCE_SENSOR_OBJECT_ID);

  if(object == NULL)
  {
    LOG_INFO("Object not found.");
    return NULL;
  }

  for(i = 0;i < MAX_SEN_INSTANCE; i++)
  {
    if(obj_inst_created_presence[i] == 0)
    {
      obj_inst_index_presence= i;
      break;
    }
  }

  if(i == MAX_SEN_INSTANCE)
  {
    LOG_INFO("Maximum instances reached for object.");
    return NULL;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&instance, sizeof(qapi_Net_LWM2M_Instance_Info_t), TX_NO_WAIT);

  if(instance == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }

  memset(instance, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));

  instance->instance_ID = obj_inst_index_presence;
  instance->next = NULL;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&instance->resource_info, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(instance->resource_info == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(instance->resource_info, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  instance->resource_info->resource_ID     = RES_M_DIG_INPUT_STATE;
  instance->resource_info->type            = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  instance->resource_info->value.asBoolean = true;
  instance->resource_info->next            = NULL;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&res1, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res1 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res1, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res1->resource_ID             = RES_O_DIG_INPUT_COUNTER;
  res1->type                    = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  res1->value.asInteger         = 5;
  res1->next                    = NULL;
  instance->resource_info->next = res1;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&res2, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res2 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res2, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res2->resource_ID             = RES_O_RESET_DIG_INPUT_COUNTER;
  res2->type                    = QAPI_NET_LWM2M_TYPE_OPAQUE_E;
  res2->next                    = NULL;
  res1->next                    = res2;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&res3, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res3 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res3, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res3->resource_ID             = RES_O_SENSOR_TYPE;
  res3->type                    = QAPI_NET_LWM2M_TYPE_STRING_E;
  res3->value.asBuffer.length   = strlen(SEN_TYPE)+1;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&res3->value.asBuffer.buffer, res3->value.asBuffer.length, TX_NO_WAIT);
  if(res3->value.asBuffer.buffer == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memcpy(res3->value.asBuffer.buffer, SEN_TYPE, res3->value.asBuffer.length);
  res3->next                    = NULL;
  res2->next                    = res3;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&res4, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res4 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res4, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res4->resource_ID               = RES_O_BUSY_TO_CLEAR_DELAY;
  res4->type                      = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  res4->value.asInteger           = 3000;
  res4->next                      = NULL;
  res3->next                      = res4;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&res5, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res5 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res5, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res5->resource_ID               = RES_O_CLEAR_TO_BUSY_DELAY;
  res5->type                      = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  res5->value.asInteger           = 3000;
  res5->next                      = NULL;
  res4->next                      = res5;

  object->no_instances++;

  lwm2m_data->object_ID = PRESENCE_SENSOR_OBJECT_ID;
  lwm2m_data->no_instances = 1;

  return instance;
}

qapi_Status_t create_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t lwm2m_data;
  qapi_Net_LWM2M_Instance_Info_t lwm2m_inst;
  qapi_Net_LWM2M_Resource_Info_t lwm2m_res;
  uint32_t obj_id;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t app_data;

  obj_id = op_data.obj_info.obj_id;

  memset(&lwm2m_inst, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));
  memset(&lwm2m_res, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));
  memset(&lwm2m_data, 0x00, sizeof(qapi_Net_LWM2M_Data_t));

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  if(handler == test_object_handler_app)
  {
    object = lwm2m_object_find(object_presence_sensor, obj_id);
    if(object == NULL)
    {
      LOG_INFO("Object not found.");
      return QAPI_ERR_NO_ENTRY;
    }

    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      instance = obj_create_presence_sensor(&lwm2m_data);
    }

    if(lwm2m_data.object_ID == PRESENCE_SENSOR_OBJECT_ID && instance != NULL)
    {
      lwm2m_data.instance_info = &lwm2m_inst;
      lwm2m_data.instance_info->instance_ID = obj_inst_index_presence;
      lwm2m_data.instance_info->no_resources = instance->no_resources;
      lwm2m_data.instance_info->resource_info = &lwm2m_res;
    }
  }

  else
  {
    LOG_INFO("Object not found.");
    return QAPI_ERR_NO_ENTRY;
  }

  if(instance != NULL)
  {
    LOG_INFO("Object instance created successfully.\n");
  }
  else
  {
    LOG_INFO("Cannot create object instance");
    return QAPI_ERR_NO_MEMORY;
  }

  object->instance_info = lwm2m_instance_add(object->instance_info, instance);
  if(object->instance_info)
  {
    memset(&app_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));
    app_data.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;

    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      obj_inst_created_presence[obj_inst_index_presence] = 1;
      app_data.obj_info.obj_inst_id = obj_inst_index_presence;
    }

    app_data.msg_type = QAPI_NET_LWM2M_CREATE_RESPONSE_MSG_E;
    app_data.obj_info.obj_id = obj_id;
    app_data.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
    app_data.status_code = QAPI_NET_LWM2M_201_CREATED_E;
    app_data.conf_msg = 0;
    app_data.msg_id_len = op_data.msg_id_len;
    memcpy(app_data.msg_id, op_data.msg_id, op_data.msg_id_len);

    qapi_Net_LWM2M_Send_Message(handler, &app_data);
  }

  if((instance != NULL))
  {
    result = qapi_Net_LWM2M_Create_Object_Instance(handler, &lwm2m_data);
  }
  else
  {
    LOG_INFO("Cannot create object instance");
    return QAPI_ERR_NO_MEMORY;
  }

  return result;
}

qapi_Status_t delete_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t         *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Object_Info_t instance_info;
  qapi_Net_LWM2M_Uri_t uri;
  
  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info = op_data.obj_info;
  memset(&instance_info, 0x00, sizeof(qapi_Net_LWM2M_Object_Info_t));

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
      {
        LOG_INFO("Deletion on resource level not allowed \n ");
        send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
        return QAPI_ERROR;
      }
      else
      {
        if(handler == test_object_handler_app)
        {
          object = lwm2m_object_find(object_presence_sensor, obj_info.obj_id);
        }

        if(object != NULL)
        {
          instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
          if(instance != NULL)
          {
            instance_info.no_object_info = 1;
            tx_byte_allocate(byte_pool_sensor, (VOID **)&instance_info.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
            if(instance_info.id_info == NULL)
            {
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              return QAPI_ERR_NO_MEMORY;
            }
            memset(instance_info.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_t));
            instance_info.id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
            instance_info.id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
            instance_info.id_info->object_ID = obj_info.obj_id;
            instance_info.id_info->instance_ID = obj_info.obj_inst_id;

            uri.flag =  obj_info.obj_mask;
            uri.objectId = obj_info.obj_id;
            uri.instanceId = obj_info.obj_inst_id;
            uri.resourceId = obj_info.res_id;
            uri.resourceInstId = obj_info.res_inst_id;

            observe_delete_sen_handle_req(handler, &uri);

            object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, obj_info.obj_inst_id);
            if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
            {
              LOG_INFO("Instance id %d cleared ", obj_info.obj_inst_id);
              object->no_instances --;
              if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
              {
                obj_inst_created_presence[obj_info.obj_inst_id] = 0;
              }
              send_response_message(handler, op_data, QAPI_NET_LWM2M_202_DELETED_E);
              result = QAPI_OK;
            }
            else
            {
              result = QAPI_ERROR;
            }

            if(result == QAPI_OK)
            {
              result = qapi_Net_LWM2M_Delete_Object_Instance(handler, &instance_info);
            }

            if(instance_info.id_info)
            {
              tx_byte_release(instance_info.id_info);
              instance_info.id_info = NULL;
            }
            return result;
          }
          else
          {
            LOG_INFO("Instance id %d not valid", obj_info.obj_inst_id);
          }
        }
        else
        {
          LOG_INFO("Object id %d not found", obj_info.obj_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          return QAPI_ERR_NO_ENTRY;
        }
      }
    }
    else
    {
     if(sen_obj_delete_all(handler,op_data) == QAPI_OK)
      {
        send_response_message(handler, op_data, QAPI_NET_LWM2M_202_DELETED_E);
        return QAPI_OK;
      }
    }
  }
  LOG_INFO("Instance id not deleted");
  send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
  return result;
}

void payload_set_data_presence(qapi_Net_LWM2M_Flat_Data_t* data,
                               qapi_Net_LWM2M_Instance_Info_t* instance)
{
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  if( data == NULL || instance == NULL) 
  {
    LOG_INFO("LWM2M_LOG: fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  } 

  switch (data->id)
  {
    case RES_M_DIG_INPUT_STATE:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_bool(resource->value.asBoolean, data);
      break;

    case RES_O_DIG_INPUT_COUNTER:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      break;

    case RES_O_SENSOR_TYPE:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_string((const char *)resource->value.asBuffer.buffer, data);
      break;

    case RES_O_BUSY_TO_CLEAR_DELAY:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      break;

    case RES_O_CLEAR_TO_BUSY_DELAY:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      break;
  }
}

qapi_Status_t sen_object_read(qapi_Net_LWM2M_App_Handler_t handler,
                              qapi_Net_LWM2M_Uri_t uri, qapi_Net_LWM2M_Content_Type_t format, 
                              uint8_t **buffer, uint32_t *length)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  int i = 0;
  int size = 0;
  qapi_Net_LWM2M_Content_Type_t cont_type;

  if(handler == NULL || buffer == NULL || length == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info.obj_mask = (qapi_Net_LWM2M_ID_t)uri.flag;
  obj_info.obj_id= uri.objectId;
  obj_info.obj_inst_id= uri.instanceId;
  obj_info.res_id= uri.resourceId;
  obj_info.res_inst_id= uri.resourceInstId;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == test_object_handler_app)
    {
      if(lwm2m_object_find(object_presence_sensor, obj_info.obj_id))
      {
        if(check_readable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Read on resource %d not allowed.", obj_info.res_id);
          return QAPI_ERROR;
        }
      }
    }
  }

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == test_object_handler_app)
    {
      object = lwm2m_object_find(object_presence_sensor, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              goto end;
            }
            size = 1;
            data = data_malloc(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          result = read_data_sen(instance->instance_ID, &size, &data, object);
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = data_malloc(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = read_data_sen(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                 &((data)[i].value.asChildren.array), object);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    goto end;
  }

  if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    cont_type = QAPI_NET_LWM2M_TEXT_PLAIN;
  }
  else
  {
    cont_type = QAPI_NET_LWM2M_M2M_TLV;
  }

  if(result == QAPI_OK)
  {
    *length = lwm2m_data_encode_payload(&uri,
                                       size,
                                       data,
                                       &cont_type,
                                       buffer);
     if(*length == 0)
     {
       if (format != QAPI_NET_LWM2M_TEXT_PLAIN || size != 1
           || (data)->type != QAPI_NET_LWM2M_TYPE_STRING_E || (data)->value.asBuffer.length != 0)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         goto end;
       }
     }
  }

  if(data)
  {
    data_free(size, data);
  }

end:
  return result;
}

qapi_Status_t read_data_sen(uint16_t instanceid, int *size, qapi_Net_LWM2M_Flat_Data_t **data,
                            qapi_Net_LWM2M_Data_t *object)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  int i = 0;

  uint16_t reslist_ob[] = {
                             RES_M_DIG_INPUT_STATE,
                             RES_O_DIG_INPUT_COUNTER,
                             RES_O_SENSOR_TYPE,
                             RES_O_BUSY_TO_CLEAR_DELAY,
                             RES_O_CLEAR_TO_BUSY_DELAY,
                           };

  int nbres = 0;
  int temp;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(*size == 0)
  {
    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      nbres = sizeof(reslist_ob)/sizeof(uint16_t);
    }
    temp = nbres;
     
    *data = data_malloc(nbres);
    if(*data == NULL)
    {
      return QAPI_ERR_NO_MEMORY;
    }
    *size = nbres;

    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      for(i = 0; i<temp; i++)
      {
        (*data)[i].id = reslist_ob[i];
      }
    }
  }

  i = 0;
  do
  {
    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    payload_set_data_presence((*data)+i, target);
    i++;
  }while(i< *size);

  return QAPI_OK;
}

qapi_Status_t read_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *read_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  uint8_t *payload = NULL;
  uint32_t payload_length = 0;
  int i = 0;
  int size = 0;
  qapi_Net_LWM2M_Content_Type_t cont_type;
  qapi_Net_LWM2M_Uri_t uri;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  obj_info = op_data.obj_info;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == test_object_handler_app)
    {
      if(lwm2m_object_find(object_presence_sensor, obj_info.obj_id))
      {
        if(check_readable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Read on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&read_data, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(read_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(read_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == test_object_handler_app)
    {
      object = lwm2m_object_find(object_presence_sensor, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              goto end;
            }
            size = 1;
            data = data_malloc(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          result = read_data_sen(instance->instance_ID, &size, &data, object);
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = data_malloc(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = read_data_sen(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                 &((data)[i].value.asChildren.array), object);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

  if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    cont_type = QAPI_NET_LWM2M_TEXT_PLAIN;
  }
  else
  {
    cont_type = QAPI_NET_LWM2M_M2M_TLV;
  }

  uri.flag = obj_info.obj_mask;
  uri.objectId = obj_info.obj_id;
  uri.instanceId = obj_info.obj_inst_id;
  uri.resourceId = obj_info.res_id;
  uri.resourceInstId = obj_info.res_inst_id;

  if(result == QAPI_OK)
  {
    payload_length = lwm2m_data_encode_payload(&uri,
                                               size,
                                               data,
                                               &cont_type,
                                               &payload);
     if (payload_length == 0)
     {
       if (op_data.content_type != QAPI_NET_LWM2M_TEXT_PLAIN || size != 1
           || (data)->type != QAPI_NET_LWM2M_TYPE_STRING_E || (data)->value.asBuffer.length != 0)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
         goto end;
       }
     }
  }

  if(data)
  {
    data_free(size, data);
  }

  if (payload_length == 0)
  {
    LOG_INFO("Payload Length is zero");
    result = QAPI_ERROR;
    goto end;
  }
  else
  {
    read_data->payload_len = payload_length;
    tx_byte_allocate(byte_pool_sensor, (VOID **)&(read_data->payload), 
                     payload_length, TX_NO_WAIT);

    if(read_data->payload == NULL)
    {
      LOG_INFO("Error in memory allocation");
      send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
      result = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    if (payload == NULL)
    {
      LOG_INFO("Payload is NULL");
      result = QAPI_ERROR;
      goto end;
    }
    else
    {
      memcpy(read_data->payload, payload, payload_length);
      read_data->msg_type    = QAPI_NET_LWM2M_RESPONSE_MSG_E;
      read_data->obj_info    = obj_info;
      read_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
      read_data->conf_msg    = 0;
      read_data->msg_id_len  = op_data.msg_id_len;
      memcpy(read_data->msg_id, op_data.msg_id, op_data.msg_id_len);
      read_data->content_type = cont_type;
    
      result = qapi_Net_LWM2M_Send_Message(handler, read_data);
    }
  }

end:
  if(read_data)
  {
    if(read_data->payload)
    {
      tx_byte_release(read_data->payload);
      read_data->payload = NULL;
    }
    tx_byte_release(read_data);
    read_data = NULL;
  }
  return result;
}

qapi_Status_t write_data_sen(uint16_t instanceid, int size, qapi_Net_LWM2M_Flat_Data_t *data,
                            qapi_Net_LWM2M_Data_t *object)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  int i = 0;
  int64_t int_val = 0;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Status_t result = QAPI_ERROR;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
  {
    i = 0;
    do
    {
      switch(data[i].id)
      {
        case RES_M_DIG_INPUT_STATE:
        case RES_O_DIG_INPUT_COUNTER:
        case RES_O_RESET_DIG_INPUT_COUNTER:
        case RES_O_SENSOR_TYPE:
            result = QAPI_ERR_NOT_SUPPORTED;
            break;
        case RES_O_BUSY_TO_CLEAR_DELAY:
        case RES_O_CLEAR_TO_BUSY_DELAY:
            result = QAPI_OK;
            break;
        default:
            result = QAPI_ERROR;
            break;
      }
      i++;
    }while(i < size && result == QAPI_OK);
  }

  if(result == QAPI_OK)
  {
    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      i = 0;
      do
      {
        switch(data[i].id)
        {
          case RES_O_BUSY_TO_CLEAR_DELAY:
          {
            if(lwm2m_data_decode_int(data+i,&(int_val)) == 1)
            {
              resource = lwm2m_resource_find(target->resource_info, data[i].id);
              if(resource != NULL)
              {
                resource->value.asInteger = int_val;
                result = QAPI_OK;
              }
              else
              {
                result = QAPI_ERR_NO_ENTRY;
              }
            }
            else
            {
              result = QAPI_ERR_INVALID_PARAM;
            }
          }
          break;
        
          case RES_O_CLEAR_TO_BUSY_DELAY:
          {
            if(lwm2m_data_decode_int(data+i,&(int_val)) == 1)
            {
              resource = lwm2m_resource_find(target->resource_info, data[i].id);
              if(resource != NULL)
              {
                resource->value.asInteger = int_val;
                result = QAPI_OK;
              }
              else
              {
                result = QAPI_ERR_NO_ENTRY;
              }
            }
            else
            {
              result = QAPI_ERR_INVALID_PARAM;
            }
          }
          break;
    
          default:
          if(size > 1)
          result = QAPI_OK;
          else
          result = QAPI_ERR_NO_ENTRY;
        }
        i++;
      }while(i < size && result == QAPI_OK);
    }
  }
  return result;
}

qapi_Status_t write_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                            qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *write_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  size_t data_len;
  qapi_Net_LWM2M_Uri_t uri;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }
  
  obj_info = op_data.obj_info;

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
  uri.flag = obj_info.obj_mask;
  uri.objectId = obj_info.obj_id;
  uri.instanceId = obj_info.obj_inst_id;
  uri.resourceId = obj_info.res_id;
  uri.resourceInstId = obj_info.res_inst_id;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == test_object_handler_app)
    {
      if(lwm2m_object_find(object_presence_sensor, obj_info.obj_id))
      {
        if(check_writable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Write on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&write_data, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(write_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(write_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E ))
  {
    if(handler == test_object_handler_app)
    {
      object = lwm2m_object_find(object_presence_sensor, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance != NULL)
        {
          data_len = lwm2m_data_decode_payload(&uri,
                                               op_data.payload,
                                               op_data.payload_len,
                                               op_data.content_type,
                                               &data);
          if(data_len == 0)
          {
            LOG_INFO("Error in payload decoding.");
            result = QAPI_ERR_BAD_PAYLOAD;
            send_response_message(handler, op_data, QAPI_NET_LWM2M_406_NOT_ACCEPTABLE_E);
            goto end;
          }
        }
        else
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
      }
      else
      {
        LOG_INFO("Object level write not allowed.");
        result = QAPI_ERR_NOT_SUPPORTED;
        send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
        goto end;
      }
    }
    else
    {
      LOG_INFO("Object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

  result = write_data_sen(instance->instance_ID, data_len, data, object);

  if(data)
  {
    //lwm2m_data_free(data_len, data);
  }

  if(result == QAPI_OK)
  {
    write_data->msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
    write_data->obj_info = op_data.obj_info;
    write_data->status_code = QAPI_NET_LWM2M_204_CHANGED_E;
    write_data->conf_msg = 0;
    write_data->msg_id_len = op_data.msg_id_len;
    memcpy(write_data->msg_id, op_data.msg_id, op_data.msg_id_len);

    result = qapi_Net_LWM2M_Send_Message(handler, write_data);
  }

  else if(result == QAPI_ERR_NO_ENTRY)
  {
    LOG_INFO("Not valid request");
    result = QAPI_ERR_NO_ENTRY;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
    goto end;
  }

  else if(result == QAPI_ERR_INVALID_PARAM)
  {
    LOG_INFO("Error in proccesing request");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    goto end;
  }
  
  else if(result == QAPI_ERR_NOT_SUPPORTED)
  {
    LOG_INFO("Write not allowed on readable resources");
    result = QAPI_ERR_NOT_SUPPORTED;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
    goto end;
  }

  else if(result == QAPI_ERROR)
  {
    LOG_INFO("Resource not present");
    result = QAPI_ERROR;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

end:
  if(write_data)
  {
    tx_byte_release(write_data);
    write_data = NULL;
  }

  return result;
}

qapi_Status_t exec_data_sen(uint16_t instanceid, qapi_Net_LWM2M_Data_t *object)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  qapi_Net_LWM2M_Uri_t uri;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
  {
    resource = lwm2m_resource_find(target->resource_info, RES_M_DIG_INPUT_STATE);
    resource1 = lwm2m_resource_find(target->resource_info, RES_O_DIG_INPUT_COUNTER);
    if(resource1 != NULL && resource != NULL)
    {
      memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
      uri.flag = (QAPI_NET_LWM2M_OBJECT_ID_E| QAPI_NET_LWM2M_INSTANCE_ID_E| QAPI_NET_LWM2M_RESOURCE_ID_E);
      uri.objectId = PRESENCE_SENSOR_OBJECT_ID;
      uri.instanceId = 0;
      uri.resourceId = RES_M_DIG_INPUT_STATE;

      resource1->value.asInteger = 0;
      resource->value.asBoolean = false;

      sen_obj_resource_value_changed(test_object_handler_app, &uri);
    }
    else
    {
      return QAPI_ERR_INVALID_PARAM;
    }
  }

  return QAPI_OK;
}

qapi_Status_t exec_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *exec_data = NULL;

  if(handler == NULL )
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  
  obj_info = op_data.obj_info;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == test_object_handler_app)
    {
      if(lwm2m_object_find(object_presence_sensor, obj_info.obj_id))
      {
        if(check_executable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Execute on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
  }

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == test_object_handler_app)
    {
      object = lwm2m_object_find(object_presence_sensor, obj_info.obj_id);
    }

    if(object == NULL)
    {
      LOG_INFO("Object id not found.");
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      return QAPI_ERR_NO_ENTRY;
    }
    else
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E ))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          return QAPI_ERR_NO_ENTRY;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              return QAPI_ERR_NO_ENTRY;
            }
            else
            {
              result = exec_data_sen(instance->instance_ID, object);

              if(result != QAPI_OK)
              {
                LOG_INFO("Execute Operation unsuccessful.");
                send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
                return QAPI_ERR_NO_ENTRY;
              }

              tx_byte_allocate(byte_pool_sensor, (VOID **)&exec_data, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
              if(exec_data == NULL)
              {
                LOG_INFO("Error in memory allocation");
                send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
                return QAPI_ERR_NO_MEMORY;
              }
              memset(exec_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

              exec_data->msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
              exec_data->obj_info = op_data.obj_info;
              exec_data->status_code = QAPI_NET_LWM2M_204_CHANGED_E;
              exec_data->conf_msg = 0;
              exec_data->msg_id_len = op_data.msg_id_len;
              memcpy(exec_data->msg_id, op_data.msg_id, op_data.msg_id_len);

              result = qapi_Net_LWM2M_Send_Message(handler, exec_data);
  
               if(exec_data)
               {
                 tx_byte_release(exec_data);
                 exec_data = NULL;
               }
            }
          }
          else
          {
            LOG_INFO("Execute on instance level not allowed.");
            send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
            return QAPI_ERR_INVALID_PARAM;
          }
        }
      }
      else
      {
        LOG_INFO("Execute on object level not allowed.");
        send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
        return QAPI_ERR_INVALID_PARAM;
      }
    }
  }

  return result;
}

qapi_Status_t discover_data_sen(uint16_t instanceid, int *size, qapi_Net_LWM2M_Flat_Data_t **data,
                                qapi_Net_LWM2M_Data_t *object)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  int i = 0;
  uint16_t reslist_ob[] = {
                             RES_M_DIG_INPUT_STATE,
                             RES_O_DIG_INPUT_COUNTER,
                             RES_O_RESET_DIG_INPUT_COUNTER,
                             RES_O_SENSOR_TYPE,
                             RES_O_BUSY_TO_CLEAR_DELAY,
                             RES_O_CLEAR_TO_BUSY_DELAY,
                           };

  int nbres = 0;
  int temp;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(*size == 0)
  {
    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      nbres = sizeof(reslist_ob)/sizeof(uint16_t);
    }

    temp = nbres;
     
    *data = data_malloc(nbres);
    if(*data == NULL)
    {
      return QAPI_ERR_NO_MEMORY;
    }
    *size = nbres;

    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      for(i = 0; i<temp; i++)
      {
        (*data)[i].id = reslist_ob[i];
      }
    }
  }

  else
  {
    if(object->object_ID == PRESENCE_SENSOR_OBJECT_ID)
    {
      for(i =0; i < *size ; i++)
      {
        switch((*data)[i].id)
        {
          case RES_M_DIG_INPUT_STATE:
          case RES_O_DIG_INPUT_COUNTER:
          case RES_O_RESET_DIG_INPUT_COUNTER:
          case RES_O_SENSOR_TYPE:
          case RES_O_BUSY_TO_CLEAR_DELAY:
          case RES_O_CLEAR_TO_BUSY_DELAY:
               break;
          default:
              return QAPI_ERROR;
        }
      }
    }
  }

  return QAPI_OK;
}

qapi_Status_t disc_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *disc_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  uint8_t *payload = NULL;
  uint32_t payload_length = 0;
  int i = 0;
  int size = 0;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info = op_data.obj_info;

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == test_object_handler_app)
    {
      object = lwm2m_object_find(object_presence_sensor, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if(object->instance_info == NULL)
      {
        LOG_INFO("Instance list is empty.");
        result = QAPI_ERR_NO_ENTRY;
        send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
        goto end;
      }

      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              goto end;
            }
            size = 1;
            data = data_malloc(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          result = discover_data_sen(instance->instance_ID, &size, &data, object);
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = data_malloc(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = discover_data_sen(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                     &((data)[i].value.asChildren.array), object);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

  if(result == QAPI_OK)
  {
    int len = 0;
    qapi_Net_LWM2M_Uri_t uri;
    memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
    uri.flag = obj_info.obj_mask;
    uri.objectId = obj_info.obj_id;
    uri.instanceId = obj_info.obj_inst_id;
    uri.resourceId = obj_info.res_id;
    uri.resourceInstId = obj_info.res_inst_id;
    
    len = sen_obj_discover_serialize(handler, op_data, &uri, size, data, &payload);
  
    if(len <= 0)
    {
      send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
      result = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    else
    {
      payload_length = len;
    }
  }

  if(data)
  {
    data_free(size, data);
  }

  if(payload == NULL || payload_length == 0)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&disc_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(disc_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(disc_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  disc_data->payload_len = payload_length;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&(disc_data->payload), 
                   payload_length, TX_NO_WAIT);
  if(disc_data->payload == NULL)
  {
    LOG_INFO("Error in memory allocation");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memcpy(disc_data->payload, payload, payload_length);
  disc_data->msg_type    = QAPI_NET_LWM2M_RESPONSE_MSG_E;
  disc_data->obj_info    = obj_info;
  disc_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
  disc_data->conf_msg    = 0;
  disc_data->msg_id_len  = op_data.msg_id_len;
  memcpy(disc_data->msg_id, op_data.msg_id, op_data.msg_id_len);
  disc_data->content_type = QAPI_NET_LWM2M_APPLICATION_LINK_FORMAT;

  result = qapi_Net_LWM2M_Send_Message(handler, disc_data);

end:
  if(disc_data)
  {
    if(disc_data->payload)
    {
      tx_byte_release(disc_data->payload);
      disc_data->payload = NULL;
    }
    tx_byte_release(disc_data);
    disc_data = NULL;
  }
  return result;
}

void start_sen_obs_notify_timer()
{
  qapi_Status_t result = QAPI_ERROR;
  if(sen_observed_list == NULL)
  {
    qapi_TIMER_define_attr_t time_attr;

    time_attr.deferrable = FALSE;
    time_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE;
    time_attr.sigs_func_ptr = &notify_signal_cb;
    time_attr.sigs_mask_data = NULL;

    result = qapi_Timer_Def(&obs_notify_Timer, &time_attr);
    if(result == QAPI_OK)
    {
      LOG_INFO("Timer defined successfully.\n");
    }
    else
    {
      LOG_INFO("Timer defined failed.\n");
    }
  }
}

void stop_sen_obs_notify_timer(qapi_Net_LWM2M_App_Handler_t handler)
{
  if(handler == test_object_handler_app)
  {
    qapi_Timer_Stop(obs_notify_Timer);
    qapi_Timer_Undef(obs_notify_Timer);
  }
}

void btc_signal_cb(uint32 userData)
{
  int id = 0;
  sen_obj_cmd_t* cmd_ptr = NULL;
  qapi_Net_LWM2M_Sen_t *sen_param = NULL;

  tx_byte_allocate(byte_pool_sensor, (VOID **)&sen_param, sizeof(qapi_Net_LWM2M_Sen_t), TX_NO_WAIT);

  if(sen_param == NULL)
  {
    LOG_INFO("Cannot assign memory for sensor object parameter");
    return;
  }

  memset(sen_param, 0x00, sizeof(qapi_Net_LWM2M_Sen_t));

  sen_param->app_data.ul_op.id = id;
  sen_param->app_data.ul_op.msg_type = QAPI_NET_LWM2M_SEN_CLEAR_E;

  LOG_INFO("Switching from Timer to application callback context");

  if((cmd_ptr = sen_obj_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = SEN_OBJ_APP_ORIGINATED_EVT;
    cmd_ptr->cmd_data.data               = (void *)sen_param;

    if(app_registerd == false)
    {
      //QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
      tx_byte_release(sen_param);
      return;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    sen_obj_enqueue_cmd(cmd_ptr);
  }

  sen_obj_set_signal();

  return;
}

void ctb_signal_cb(uint32 userData)
{
  int id = 0;
  sen_obj_cmd_t* cmd_ptr = NULL;
  qapi_Net_LWM2M_Sen_t *sen_param = NULL;

  tx_byte_allocate(byte_pool_sensor, (VOID **)&sen_param, sizeof(qapi_Net_LWM2M_Sen_t), TX_NO_WAIT);

  if(sen_param == NULL)
  {
    LOG_INFO("Cannot assign memory for sensor object parameter");
    return;
  }

  memset(sen_param, 0x00, sizeof(qapi_Net_LWM2M_Sen_t));

  sen_param->app_data.ul_op.id = id;
  sen_param->app_data.ul_op.msg_type = QAPI_NET_LWM2M_SEN_BUSY_E;

  LOG_INFO("Switching from Timer to application callback context");

  if((cmd_ptr = sen_obj_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = SEN_OBJ_APP_ORIGINATED_EVT;
    cmd_ptr->cmd_data.data               = (void *)sen_param;

    if(app_registerd == false)
    {
      //QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
      tx_byte_release(sen_param);
      return;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    sen_obj_enqueue_cmd(cmd_ptr);
  }

  sen_obj_set_signal();

  return;
}

void start_busy_to_clear_timer()
{
  qapi_Status_t result = QAPI_ERROR;

  qapi_TIMER_define_attr_t time_attr;
  time_attr.deferrable = FALSE;
  time_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE;
  time_attr.sigs_func_ptr = &btc_signal_cb;
  time_attr.sigs_mask_data = NULL;

  result = qapi_Timer_Def(&btc_Timer, &time_attr);
  if(result == QAPI_OK)
  {
    LOG_INFO("Timer defined successfully.\n");
  }
  else
  {
    LOG_INFO("Timer defined failed.\n");
  }
}

void start_clear_to_busy_timer()
{
  qapi_Status_t result = QAPI_ERROR;

  qapi_TIMER_define_attr_t time_attr;
  time_attr.deferrable = FALSE;
  time_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE;
  time_attr.sigs_func_ptr = &ctb_signal_cb;
  time_attr.sigs_mask_data = NULL;

  result = qapi_Timer_Def(&ctb_Timer, &time_attr);
  if(result == QAPI_OK)
  {
    LOG_INFO("Timer defined successfully.\n");
  }
  else
  {
    LOG_INFO("Timer defined failed.\n");
  }
}

void stop_busy_to_clear_timer()
{
  //qapi_Timer_Stop(obs_notify_Timer);
  qapi_Timer_Undef(btc_Timer);
}

void stop_clear_to_busy_timer()
{
  //qapi_Timer_Stop(obs_notify_Timer);
  qapi_Timer_Undef(ctb_Timer);
}

static qapi_Net_LWM2M_Observed_t * lwm2m_find_observed(qapi_Net_LWM2M_App_Handler_t handler,
                                                       qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *target = NULL;

  if(uri == NULL || handler == NULL)
  {
    return NULL;
  }

  if(handler == test_object_handler_app)
  {
    target = sen_observed_list;
  }

  while(target != NULL && (target->uri.objectId != uri->objectId
        || target->uri.flag != uri->flag
        || ((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && target->uri.instanceId != uri->instanceId)
        || ((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) && target->uri.resourceId != uri->resourceId)))
  {
    target = target->next;
  }

  return target;
}

static void lwm2m_unlink_observed(qapi_Net_LWM2M_App_Handler_t handler,
                                  qapi_Net_LWM2M_Observed_t *observed)
{
  if(handler == NULL || observed == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if(handler == test_object_handler_app && sen_observed_list == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if(handler == test_object_handler_app)
  {
    if (sen_observed_list == observed)
    {
      sen_observed_list = sen_observed_list->next;
    }
    else
    {
      qapi_Net_LWM2M_Observed_t *list = NULL;
  
      list = sen_observed_list;
      while (list->next != NULL && list->next != observed)
      {
        list = list->next;
      }
  
      if(list->next != NULL)
      {
        list->next = list->next->next;
      }
    }
  }

  return;
}

static qapi_Net_LWM2M_Observe_info_t *lwm2m_find_observe_info(qapi_Net_LWM2M_Observed_t *observed,
                                                              uint8_t *msg_id,
                                                              uint8_t msg_id_len)
{
  qapi_Net_LWM2M_Observe_info_t *target = NULL;

  if(observed == NULL || msg_id == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return NULL;
  }
  
  target = observed->observe_info;
  while(target != NULL && (memcmp((target->msg_id + target->msg_id_len - 2), (msg_id +msg_id_len -2), 2) != 0))
  {
    target = target->next;
  }

  return target;
}

static qapi_Net_LWM2M_Observe_info_t *lwm2m_get_observe_info(qapi_Net_LWM2M_App_Handler_t handler,
                                                             qapi_Net_LWM2M_Uri_t *uri,
                                                             uint8_t *msg_id,
                                                             uint8_t msg_id_len)
{
  qapi_Net_LWM2M_Observed_t *observed = NULL;
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  bool alloc = false;

  if(uri == NULL || msg_id == NULL || handler == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return NULL;
  }

  if(handler == test_object_handler_app)
  {
    if(sen_observed_list == NULL)
    {
      start_sen_obs_notify_timer();
    }
  }

  observed = lwm2m_find_observed(handler, uri);

  if(observed == NULL)
  {
    tx_byte_allocate(byte_pool_sensor, (VOID **)&observed, 
                   sizeof(qapi_Net_LWM2M_Observed_t), TX_NO_WAIT);
    if(observed == NULL)
    {
      return NULL;
    }
    alloc = true;
    memset(observed, 0x00, sizeof(qapi_Net_LWM2M_Observed_t));
    memcpy(&(observed->uri), uri, sizeof(qapi_Net_LWM2M_Uri_t));
    if(handler == test_object_handler_app)
    {
      observed->next = sen_observed_list;
      sen_observed_list = observed;
    }
  }

  observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
  if(observe_info == NULL)
  {
    tx_byte_allocate(byte_pool_sensor, (VOID **)&observe_info, 
                   sizeof(qapi_Net_LWM2M_Observe_info_t), TX_NO_WAIT);
    if(observe_info == NULL)
    {
      if(alloc == true)
      {
        tx_byte_release(observed);
        observed = NULL;
      }
      return NULL;
    }
    memset(observe_info, 0x00, sizeof(qapi_Net_LWM2M_Observe_info_t));
    observe_info->active = false;
    observe_info->msg_id_len = msg_id_len;
    memcpy(observe_info->msg_id, msg_id, msg_id_len);
    observe_info->next = observed->observe_info;
    observed->observe_info = observe_info;
  }

  return observe_info;
}

qapi_Net_LWM2M_Observe_info_t* sen_findInheritedWatcher(qapi_Net_LWM2M_App_Handler_t handler,
                                                        qapi_Net_LWM2M_Uri_t *uri,
                                                        uint8_t *msg_id,
                                                        uint8_t msg_id_len,
                                                        uint8_t flag)
{
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  qapi_Net_LWM2M_Observed_t *observed = NULL;

  if(uri == NULL || msg_id == NULL || handler == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return NULL;
  }

  if((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    uri->flag &= ~QAPI_NET_LWM2M_RESOURCE_ID_E;
    observed = lwm2m_find_observed(handler, uri);
    if(observed == NULL)
    {
      uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
      observed = lwm2m_find_observed(handler, uri);
      if(observed == NULL)
      {
        //Do nothing
      }
      else
      {
        observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
      }
      uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
    }
    else
    {
      observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
      if(observe_info != NULL && observe_info->attributes!= NULL)
      {
        if((observe_info->attributes->set_attr_mask & flag) == 0)
        {
          uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
          observed = lwm2m_find_observed(handler, uri);
          if(observed == NULL)
          {
            observe_info = NULL;
            LOG_INFO("Observed list not found for URI \n");
          }
          else
          {
            observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
          }
          uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
        }
      }
      // If inherited watcher has no attributes set, move to object level
      else if(observe_info != NULL && observe_info->attributes == NULL)
      {
        uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
        observed = lwm2m_find_observed(handler, uri);
        if(observed == NULL)
        {
          observe_info = NULL;
          LOG_INFO("Observed list not found for URI \n");
        }
        else
        {
          observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
        }
        uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      }
    }
    uri->flag |= QAPI_NET_LWM2M_RESOURCE_ID_E;
  }
  else if((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E))
  {
    uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
    observed = lwm2m_find_observed(handler, uri);
    if(observed == NULL)
    {
      //Do nothing
    }
    else
    {
      observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
    }
    uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
  }

  return observe_info;
}

qapi_Status_t observe_sen_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                     qapi_Net_LWM2M_Data_t *object,
                                     qapi_Net_LWM2M_Server_Data_t op_data,
                                     int size, qapi_Net_LWM2M_Flat_Data_t *data)
{
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  qapi_Net_LWM2M_Uri_t uri;
  uint8_t msg_id[QAPI_MAX_LWM2M_MSG_ID_LENGTH];
  uint8_t msg_id_len;
  qapi_time_get_t curr_time;
  qapi_Status_t result = QAPI_ERROR;

  if(handler == NULL || object == NULL || data == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_ERROR;
  }

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  uri.flag = op_data.obj_info.obj_mask;
  uri.objectId = op_data.obj_info.obj_id;
  uri.instanceId = op_data.obj_info.obj_inst_id;
  uri.resourceId = op_data.obj_info.res_id;
  uri.resourceInstId = op_data.obj_info.res_inst_id;

  msg_id_len = op_data.msg_id_len;
  memcpy(msg_id, op_data.msg_id, op_data.msg_id_len);

  observe_info = lwm2m_get_observe_info(handler, &uri, msg_id, msg_id_len);

  if(observe_info == NULL)
  {
    LOG_INFO("Observe info is NULL\n");
    return QAPI_ERROR;
  }

  if(handler == test_object_handler_app)
  {
    qapi_TIMER_set_attr_t time_attr;
    time_attr.unit = QAPI_TIMER_UNIT_SEC;
    time_attr.reload = TRUE;
    time_attr.time = 1;
    result = qapi_Timer_Set(obs_notify_Timer, &time_attr);
    if(result == QAPI_OK)
    {
      LOG_INFO("Timer set successfully.\n");
    }
    else
    {
      LOG_INFO("Timer set failed.\n");
    }
  }

  observe_info->active = true;
  qapi_time_get(QAPI_TIME_SECS, &curr_time);
  memset(observe_info->msg_id, 0, QAPI_MAX_LWM2M_MSG_ID_LENGTH);
  memcpy(observe_info->msg_id, msg_id, msg_id_len);    
  observe_info->msg_id_len = msg_id_len;

  observe_info->last_time = curr_time.time_secs;

  if(uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    switch(data->type)
    {
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
           if (1 != lwm2m_data_decode_int(data, &(observe_info->lastvalue.asInteger))) 
           return QAPI_ERROR;
           break;
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
            if (1 != lwm2m_data_decode_float(data, &(observe_info->lastvalue.asFloat))) 
            return QAPI_ERROR;
            break;
      default:
            break;
    }
  }

  return QAPI_OK;
}

qapi_Status_t observe_cancel_sen_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                            qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Observed_t *obs = NULL;
  qapi_Net_LWM2M_Observed_t *observed = NULL;
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  qapi_Net_LWM2M_Uri_t uri;

  if(handler == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_ERROR;
  }

  if(handler == test_object_handler_app)
  {
    obs = sen_observed_list;
  }

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  // Handling cancel observe with OBSERVE flag set to 1
  if(LWM2M_MAX_ID == op_data.notification_id && 
    ((op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) != 0))
  {
    uri.flag = op_data.obj_info.obj_mask;
    uri.objectId = op_data.obj_info.obj_id;
    uri.instanceId = op_data.obj_info.obj_inst_id;
    uri.resourceId = op_data.obj_info.res_id;
    uri.resourceInstId = op_data.obj_info.res_inst_id;

    observed = lwm2m_find_observed(handler, &uri);
    if(observed != NULL)
    {
      observe_info = observed->observe_info;
      while (observe_info != NULL && (strncmp((char *)(observe_info->msg_id + observe_info->msg_id_len - 2),
                                     (char *)(op_data.msg_id + op_data.msg_id_len -2), 2) != 0))
      {
        observe_info = observe_info->next;
      }
      if(observe_info)
      {
        observe_info->active = false;
        return QAPI_OK;
      }
    }
  }

  // Handling cancel observe came through RESET message
  else if((op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) == 0)
  {
    for(observed = obs; observed != NULL; observed = observed->next)
    {
      qapi_Net_LWM2M_Observe_info_t * parent = NULL;
      parent = observed->observe_info;
      while (parent != NULL && (parent->not_id != op_data.notification_id ))
      {
        parent = parent->next;
      }
      if (parent != NULL)
      {
        observe_info = parent;
        observe_info->active = false;
        return QAPI_OK;
      }
    }
  }

  return result;
}

void observe_delete_sen_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                   qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *obs = NULL;
  qapi_Net_LWM2M_Observed_t *observed = NULL;

  if(handler == NULL || uri == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if(handler == test_object_handler_app)
  {
    obs = sen_observed_list;
  }

  observed = obs;

  while(observed != NULL)
  {
    if(observed->uri.objectId == uri->objectId
       &&(((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) == 0)
       || observed->uri.instanceId == uri->instanceId))
    {
      qapi_Net_LWM2M_Observed_t *next;
      qapi_Net_LWM2M_Observe_info_t *target;

      next = observed->next;

      for(target = observed->observe_info ; target != NULL; target = target->next)
      {
        if(target->attributes != NULL)
        {
          tx_byte_release(target->attributes);
          target->attributes= NULL;
        }
      }
      while(observed->observe_info != NULL)
      {
        qapi_Net_LWM2M_Observe_info_t *next;

        next = observed->observe_info->next;
        tx_byte_release(observed->observe_info);
        observed->observe_info = next;
      }

      lwm2m_unlink_observed(handler, observed);
      LOG_INFO("LWM2M_LOG: Removed observedP  %p from list\n",observed);
      tx_byte_release(observed);
      observed = NULL;

      observed = next;
    }
    else
    {
      observed = observed->next;
    }
  }
}

qapi_Status_t observe_cancel_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  result = observe_cancel_sen_handle_req(handler, op_data);

  return result;
}

qapi_Status_t observe_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                              qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *obs_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  uint8_t *payload = NULL;
  uint32_t payload_length = 0;
  int i = 0;
  int size = 0;
  qapi_Net_LWM2M_Content_Type_t cont_type;
  qapi_Net_LWM2M_Uri_t uri;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
  obj_info = op_data.obj_info;

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == test_object_handler_app)
    {
      object = lwm2m_object_find(object_presence_sensor, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              goto end;
            }
            size = 1;
            data = data_malloc(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          result = read_data_sen(instance->instance_ID, &size, &data, object);
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = data_malloc(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = read_data_sen(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                 &((data)[i].value.asChildren.array), object);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

  if(result == QAPI_OK)
  {
    result = observe_sen_handle_req(handler, object, op_data, size, data);
  }

  if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    cont_type = QAPI_NET_LWM2M_TEXT_PLAIN;
  }
  else
  {
    cont_type = QAPI_NET_LWM2M_M2M_TLV;
  }

  uri.flag = obj_info.obj_mask;
  uri.objectId = obj_info.obj_id;
  uri.instanceId = obj_info.obj_inst_id;
  uri.resourceId = obj_info.res_id;
  uri.resourceInstId = obj_info.res_inst_id;

  if(result == QAPI_OK)
  {
    payload_length = lwm2m_data_encode_payload(&uri,
                                               size,
                                               data,
                                               &cont_type,
                                               &payload);
     if (result != QAPI_OK || payload_length == 0)
     {
       if (op_data.content_type != QAPI_NET_LWM2M_TEXT_PLAIN || size != 1
           || (data)->type != QAPI_NET_LWM2M_TYPE_STRING_E || (data)->value.asBuffer.length != 0)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
         goto end;
       }
     }
  }

  if(data)
  {
    data_free(size, data);
  }
  if(payload == NULL || payload_length == 0)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
    return QAPI_ERR_BAD_PAYLOAD;
  }
  tx_byte_allocate(byte_pool_sensor, (VOID **)&obs_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(obs_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(obs_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  obs_data->payload_len = payload_length;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&(obs_data->payload), 
                   payload_length, TX_NO_WAIT);
  if(obs_data->payload == NULL)
  {
    LOG_INFO("Error in memory allocation");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memcpy(obs_data->payload, payload, payload_length);
  obs_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
  obs_data->obj_info    = obj_info;
  obs_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
  obs_data->conf_msg    = 0;
  obs_data->msg_id_len  = op_data.msg_id_len;
  memcpy(obs_data->msg_id, op_data.msg_id, op_data.msg_id_len);
  obs_data->content_type = cont_type;
  obs_data->observation_seq_num = 0;
  
  result = qapi_Net_LWM2M_Send_Message(handler, obs_data);

  if(obs_data)
  {
    if(obs_data->payload)
    {
      tx_byte_release(obs_data->payload);
      obs_data->payload = NULL;
    }
    tx_byte_release(obs_data);
    obs_data = NULL;
  }

end:
  return result;

}

qapi_Status_t sen_object_write_attr(qapi_Net_LWM2M_App_Handler_t handler, 
                                    qapi_Net_LWM2M_Server_Data_t op_data)
{
  uint8_t result = QAPI_NET_LWM2M_IGNORE_E;

  if(handler == NULL )
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  result = sen_object_set_observe_param(handler, op_data);

  if(result == QAPI_NET_LWM2M_204_CHANGED_E)
  {
    return QAPI_OK;
  }
  else
  {
    return QAPI_ERROR;
  }

}

uint8_t sen_object_set_observe_param(qapi_Net_LWM2M_App_Handler_t handler,
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  uint8_t result = QAPI_NET_LWM2M_IGNORE_E;
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  qapi_Net_LWM2M_Uri_t uri;
  uint8_t msg_id[QAPI_MAX_LWM2M_MSG_ID_LENGTH];
  uint8_t msg_id_len;
  qapi_Net_LWM2M_Attributes_t attributes;

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
  memset(&attributes, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));

  if(op_data.lwm2m_attr == NULL) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  memcpy(&attributes, op_data.lwm2m_attr, sizeof(qapi_Net_LWM2M_Attributes_t));
  uri.flag = op_data.obj_info.obj_mask;
  uri.objectId = op_data.obj_info.obj_id;
  uri.instanceId = op_data.obj_info.obj_inst_id;
  uri.resourceId = op_data.obj_info.res_id;
  uri.resourceInstId = op_data.obj_info.res_inst_id;

  msg_id_len = op_data.msg_id_len;
  memcpy(msg_id, op_data.msg_id, msg_id_len);

  if(!(uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && (uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  if(handler == test_object_handler_app)
  {
    result = object_check_readable(object_presence_sensor, &uri);
  }

  if (result != QAPI_NET_LWM2M_205_CONTENT_E)
  {
    send_response_message(handler, op_data, (qapi_Net_LWM2M_Response_Code_t)result);
    return (uint8_t)result;
  }

  if((attributes.set_attr_mask & LWM2M_ATTR_FLAG_NUMERIC) != 0)
  {
    if(handler == test_object_handler_app)
    {
      result = object_check_numeric(object_presence_sensor, &uri);
    }

    if (result != QAPI_NET_LWM2M_205_CONTENT_E)
    {
      send_response_message(handler, op_data, (qapi_Net_LWM2M_Response_Code_t)result);
      return(uint8_t) result;
    }
  }

  observe_info = lwm2m_get_observe_info(handler, &uri, msg_id, msg_id_len);
  
  if(observe_info == NULL)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return (uint8_t) QAPI_NET_LWM2M_500_INTERNAL_SERVER_E; 
  }
  
  // Check rule lt value + 2*step� value < gt value
  if ((((attributes.set_attr_mask | (observe_info->attributes?observe_info->attributes->set_attr_mask:0))
          & ~(attributes.clr_attr_mask)) & LWM2M_ATTR_FLAG_NUMERIC) == LWM2M_ATTR_FLAG_NUMERIC)
  {
    float gt = 0.0;
    float lt = 0.0;
    float stp = 0.0;

    if ((attributes.set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E) != 0)
    {
      gt = attributes.greaterThan;
    }
    else if (observe_info->attributes != NULL)
    {
      gt = observe_info->attributes->greaterThan;
    }
    else
    {
      //Do nothing
    }

    if ((attributes.set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E) != 0)
    {
      lt = attributes.lessThan;
    }
    else if (observe_info->attributes != NULL)
    {
      lt = observe_info->attributes->lessThan;
    }
    else
    {
      //Do nothing
    }

     if ((attributes.set_attr_mask & QAPI_NET_LWM2M_STEP_E) != 0)
    {
      stp = attributes.step;
    }
    else if (observe_info->attributes != NULL)
    {
      stp = observe_info->attributes->step;
    }
    else
    {
      //Do nothing
    }

    if (lt + (2 * stp) >= gt) 
    {
      send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
      return (uint8_t)QAPI_NET_LWM2M_400_BAD_REQUEST_E;
    }
  }

  if(observe_info->attributes == NULL)
  {
    if(attributes.set_attr_mask != 0)
    {
      tx_byte_allocate(byte_pool_sensor, (VOID **)&(observe_info->attributes), 
                       sizeof(qapi_Net_LWM2M_Attributes_t), TX_NO_WAIT);

      if(observe_info->attributes == NULL)
      {
        send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
        return (uint8_t)QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
      }
      memset(observe_info->attributes, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));
      memcpy(observe_info->attributes, &attributes, sizeof(qapi_Net_LWM2M_Attributes_t));
    }
  }
  else
  {
    observe_info->attributes->set_attr_mask &= ~(attributes.clr_attr_mask);
    observe_info->attributes->set_attr_mask |= attributes.set_attr_mask;
    
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
    {
      observe_info->attributes->minPeriod = attributes.minPeriod;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
    {
      observe_info->attributes->maxPeriod = attributes.maxPeriod;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E)
    {
      observe_info->attributes->lessThan = attributes.lessThan;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E)
    {
      observe_info->attributes->greaterThan = attributes.greaterThan;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_STEP_E)
    {
      observe_info->attributes->step = attributes.step;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
    {
      observe_info->attributes->minPeriod = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
    {
      observe_info->attributes->maxPeriod = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E)
    {
      observe_info->attributes->lessThan = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E)
    {
      observe_info->attributes->greaterThan = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_STEP_E)
    {
      observe_info->attributes->step = 0;
    }
  }

  send_response_message(handler, op_data, QAPI_NET_LWM2M_204_CHANGED_E);
  return QAPI_NET_LWM2M_204_CHANGED_E;
}

qapi_Net_LWM2M_Observed_t* observe_find_by_uri(qapi_Net_LWM2M_App_Handler_t handler,
                                               qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *target = NULL;

  if(handler == NULL || uri == NULL) 
  { 
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__); 
    return NULL; 
  }

  if(handler == test_object_handler_app && sen_observed_list != NULL)
  {
    target = sen_observed_list;
  }

  while(target != NULL)
  {
    if(target->uri.objectId == uri->objectId)
    {
      if((!(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && !(target->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E))
          ||((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && (target->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E) 
          && (uri->instanceId == target->uri.instanceId)))
      {
        if((!(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) && !(target->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E))
          ||((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) && (target->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E) 
          && (uri->resourceId == target->uri.resourceId)))
        {
          LOG_INFO("Found one with %s observers.", target->observe_info ? "" : " no");
          return target;
        }
      }
    }
    target = target->next;
  }

  LOG_INFO("Found nothing");
  return NULL;
}

void sen_obj_resource_value_changed(qapi_Net_LWM2M_App_Handler_t handler,
                                    qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t * target = NULL;

  if(handler == NULL || uri == NULL) 
  { 
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__); 
    return;
  }

  if(handler == test_object_handler_app && sen_observed_list != NULL)
  {
    target = sen_observed_list;
  }

  while(target != NULL)
  {
    if (target->uri.objectId == uri->objectId)
    {
      if(((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) != 0)
          && (target->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E) != 0
          && uri->instanceId == target->uri.instanceId)
      {
        if(((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)!= 0)
          && (target->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E) != 0
          && uri->resourceId == target->uri.resourceId)
        {
          qapi_Net_LWM2M_Observe_info_t *observe_info;

          LOG_INFO("Found an observation");

          for (observe_info = target->observe_info; observe_info != NULL ; observe_info = observe_info->next)
          {
            if (observe_info->active == true)
            {
              LOG_INFO("Tagging a watcher");
              observe_info->update = true;
            }
          }
        }
      }
    }
    target = target->next;
  }
}

qapi_Status_t sen_observer_add_notify_to_pending_list(qapi_Net_LWM2M_App_Handler_t handler,
                                                      qapi_Net_LWM2M_Observe_info_t *observe_info,
                                                      qapi_Net_LWM2M_Uri_t uri,
                                                      qapi_Net_LWM2M_Content_Type_t format,
                                                      uint8_t *buffer, size_t length)
{
  qapi_Net_LWM2M_Pending_Observed_t *pending_node = NULL;
  qapi_Net_LWM2M_Pending_Observed_t *notify = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *notify_data = NULL;
  qapi_Status_t result = QAPI_ERROR;

  LOG_INFO("Entering ext_observer_add_notify_to_pending_list");
  if(observe_info == NULL || buffer == NULL || handler == NULL)
  {
    LOG_INFO("NULL parameter check failed\n");
    return QAPI_ERROR;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&notify_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(notify_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return QAPI_ERR_NO_MEMORY;
  }
  memset(notify_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  tx_byte_allocate(byte_pool_sensor, (VOID **)&notify, 
                   sizeof(qapi_Net_LWM2M_Pending_Observed_t), TX_NO_WAIT);
  if(notify == NULL)
  {
    LOG_INFO("LWM2M_LOG: Malloc failed\n");
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memset(notify, 0x00, sizeof(qapi_Net_LWM2M_Pending_Observed_t));

  notify->observe_info = observe_info;

  notify_data->payload_len = length;
  tx_byte_allocate(byte_pool_sensor, (VOID **)&(notify_data->payload), 
                   length, TX_NO_WAIT);
  if(notify_data->payload == NULL)
  {
    LOG_INFO("Error in memory allocation");
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memcpy(notify_data->payload, buffer, length);
  notify_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
  notify_data->obj_info.obj_mask    = (qapi_Net_LWM2M_ID_t)uri.flag;
  notify_data->obj_info.obj_id      = uri.objectId;
  notify_data->obj_info.obj_inst_id = uri.instanceId;
  notify_data->obj_info.res_id      = uri.resourceId;
  notify_data->obj_info.res_inst_id = uri.resourceInstId;
  notify_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
  notify_data->conf_msg    = 0;
  notify_data->msg_id_len  = observe_info->msg_id_len;
  memcpy(notify_data->msg_id, observe_info->msg_id, observe_info->msg_id_len);
  notify_data->content_type = format;
  notify_data->observation_seq_num = ++observe_info->seq_num;

  notify->message = (void *)notify_data;

  if(handler == test_object_handler_app)
  {
    if(sen_pending_observed_list == NULL)
    { 
      notify->next = sen_pending_observed_list;
      sen_pending_observed_list = notify;
    } 
    else 
    {
      for(pending_node = sen_pending_observed_list; (pending_node != NULL && pending_node->next != NULL) ; pending_node = pending_node->next);
      pending_node->next = notify;
    }
  }

end:
  return result;
}

void sen_object_notify(int id,
                       time_t current_time,
                       time_t *timeout)
{
  qapi_Net_LWM2M_Observed_t *observed = NULL;
  bool notify_store = false;
  qapi_Net_LWM2M_Content_Type_t cont_type;
  qapi_Status_t result = QAPI_OK;
  qapi_Net_LWM2M_Observed_t *observed_list = NULL;
  qapi_Net_LWM2M_App_Handler_t handler = NULL;

  if(id == 0)
  {
    handler = test_object_handler_app;
  }

  if(handler == NULL || timeout == NULL)
  {
    return;
  }

  if(handler == test_object_handler_app && sen_observed_list == NULL)
  {
    stop_sen_obs_notify_timer(handler);
    return;
  }

  if(handler == test_object_handler_app)
  {
    observed_list = sen_observed_list;
  }

  //Send Pending Notifications
  if(!lwm2m_client_sleep_state && 
    (((handler == test_object_handler_app) && (sen_pending_observed_list != NULL))))
  {
    qapi_Net_LWM2M_Pending_Observed_t *node = NULL;
    qapi_Net_LWM2M_App_Ex_Obj_Data_t *notify_data = NULL;

    if(handler == test_object_handler_app)
    {
      node = sen_pending_observed_list;
    }

    while(node && (node->observe_info))
    {  
      tx_byte_allocate(byte_pool_sensor, (VOID **)&notify_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
      if(notify_data == NULL)
      {
        LOG_INFO("Memory allocation failure.\n");
        return;
      }
      memset(notify_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

      LOG_INFO("LWM2M_LOG: Sending pending notify \n");

      if(handler == test_object_handler_app)
      {
        sen_pending_observed_list = node->next;
      }

      if(!node->observe_info || !node->message)
      {
        if(node->message) 
        {
          if(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload)
          {
            tx_byte_release(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload);
                            ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload = NULL;
          }
          tx_byte_release(node->message);
          node->message = NULL;
        }
        tx_byte_release(node);
        node = NULL;
        if(handler == test_object_handler_app)
        {
          node = sen_pending_observed_list;
        }
        continue;
      }

      node->observe_info->last_time = current_time;

      notify_data->payload_len = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload_len;
      tx_byte_allocate(byte_pool_sensor, (VOID **)&(notify_data->payload), 
                       notify_data->payload_len, TX_NO_WAIT);
      if(notify_data->payload == NULL)
      {
        LOG_INFO("Error in memory allocation");
        result = QAPI_ERR_NO_MEMORY;
        goto end;
      }
      memcpy(notify_data->payload, ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload, notify_data->payload_len);
      notify_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
      notify_data->obj_info.obj_mask    = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.obj_mask;
      notify_data->obj_info.obj_id      = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.obj_id;
      notify_data->obj_info.obj_inst_id = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.obj_inst_id;
      notify_data->obj_info.res_id      = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.res_id;
      notify_data->obj_info.res_inst_id = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.res_inst_id;
      notify_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
      notify_data->conf_msg    = 0;
      notify_data->msg_id_len  = node->observe_info->msg_id_len;
      memcpy(notify_data->msg_id, node->observe_info->msg_id,  node->observe_info->msg_id_len);
      notify_data->content_type = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->content_type; 
      notify_data->observation_seq_num = ++node->observe_info->seq_num;

      result = qapi_Net_LWM2M_Send_Message(handler, notify_data);
      node->observe_info->not_id = notify_data->notification_id;
      node->observe_info->update = false;
end:
      if(notify_data)
      {
        if(notify_data->payload)
        {
          tx_byte_release(notify_data->payload);
          notify_data->payload = NULL;
        }
        tx_byte_release(notify_data);
        notify_data = NULL;
      }
      if(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload)
      {
        tx_byte_release(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload);
                       (((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload) = NULL;
      }
      if((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)
      {
        tx_byte_release(node->message);
        node->message = NULL;
      }
      if(node)
      {
        tx_byte_release(node);
        node = NULL;
      }
      if(handler == test_object_handler_app)
      {
        node = sen_pending_observed_list;
      }
    }
  }

  for(observed = observed_list; observed != NULL; observed = observed->next)
  {
    qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
    uint8_t * buffer = NULL;
    size_t length = 0;
    qapi_Net_LWM2M_Flat_Data_t * data = NULL;
    int size = 0;
    double float_value = 0;
    int64_t integer_value = 0;
    bool store_value = false;
    qapi_Net_LWM2M_App_Ex_Obj_Data_t *notify_data = NULL;
    int result = 0;
    time_t min_period = 0, max_period = 0;
    qapi_Net_LWM2M_Data_t *object = NULL;
    qapi_Net_LWM2M_Observe_info_t *target = NULL;
    qapi_Net_LWM2M_Uri_t uri;

    memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

    if(observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
    {
      if(handler == test_object_handler_app)
      {
        object = lwm2m_object_find(object_presence_sensor, observed->uri.objectId);
      }

      if(object == NULL)
      {
        return;
      }
      // Read resource to value to check value change
      size = 1;
      data = data_malloc(size);
      if(data == NULL)
      {
        return;
      }
      (data)->id = observed->uri.resourceId;

      result = read_data_sen(observed->uri.instanceId, &size, &data, object);
      if(result != QAPI_OK)
      {
        if(data != NULL)
        {
          data_free(size, data);
        }
        continue;
      }
      if(data == NULL)
      continue;

      switch (data->type)
      {
        case QAPI_NET_LWM2M_TYPE_INTEGER_E:
          if (lwm2m_data_decode_int(data, &integer_value) != 1) continue;
          store_value = true;
          break;
        case QAPI_NET_LWM2M_TYPE_FLOAT_E:
          if (lwm2m_data_decode_float(data, &float_value) != 1) continue;
          store_value = true;
          break;
        default:
          break;
      }
    }

    for(observe_info = observed->observe_info; observe_info != NULL; observe_info = observe_info->next)
    {
      uint16_t ssid = 0;
      if(observe_info->msg_id_len >= 2)
      {
        QAPI_LWM2M_SERVER_ID_INFO(observe_info->msg_id, observe_info->msg_id_len, ssid);
      }
      qapi_Net_LWM2M_Default_Attribute_Info(handler, (uint32_t)ssid, &min_period, &max_period);

      if (observe_info->active == true)
      {
        bool notify = false; //Flag that decides if notify needs to be sent

        // check if the resource value has changed and set watcher->update flag to true to trigger notify
        if(store_value)
        {
          LOG_INFO("store value is true\n");
          switch (data->type)
          {
            case QAPI_NET_LWM2M_TYPE_INTEGER_E:
              if(integer_value != observe_info->lastvalue.asInteger)
              {
                LOG_INFO("Integer value changed\n");
                observe_info->update = true;
              }
              else
              {
                LOG_INFO("Integer value did not change\n");
              }
              break;

            case QAPI_NET_LWM2M_TYPE_FLOAT_E:
              if(float_value != observe_info->lastvalue.asFloat)
              {
                LOG_INFO("Float value changed\n");
                observe_info->update = true;
              }
              else
              {
                LOG_INFO("Float value did not change\n");
              }
              break;
          }
        } //End of value change check

        // Get Pmin and Pmax period and if pmin > pmax, notify on Pmax period
        memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
        memcpy(&uri, &observed->uri, sizeof(qapi_Net_LWM2M_Uri_t));

        // Get the Maximum period 
        if(observe_info->attributes == NULL || (observe_info->attributes != NULL 
           && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) == 0))
        {
          target = sen_findInheritedWatcher(handler, &uri, observe_info->msg_id, observe_info->msg_id_len, QAPI_NET_LWM2M_MIN_PERIOD_E);
          if(target != NULL)
          {
            if(target->attributes != NULL && (target->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) != 0)
            {
              max_period = target->attributes->maxPeriod;
            }
          }
          else
          {
            if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) != 0)
            {
              max_period = observe_info->attributes->maxPeriod;
            }
          }
        }
        else
        {
          if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) != 0)
          {
            max_period = observe_info->attributes->maxPeriod;
          }
        }

        // Get the Minimum period 
        if(observe_info->attributes == NULL || (observe_info->attributes != NULL 
           && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) == 0))
        {
          target = sen_findInheritedWatcher(handler, &uri, observe_info->msg_id, observe_info->msg_id_len, QAPI_NET_LWM2M_MIN_PERIOD_E);
          if(target != NULL)
          {
            if(target->attributes != NULL && (target->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
            {
              min_period = target->attributes->minPeriod;
            }
          }
          else
          {
            if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
            {
              min_period = observe_info->attributes->minPeriod;
            }
          }
        }
        else
        {
          if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
          {
            min_period = observe_info->attributes->minPeriod;
          }
        }

        if(min_period <= max_period)
        {
        // If notify needs to be sent due to resource value change
        if (observe_info->update == true) 
        {
          // value changed, should we notify the server ?
          // If no parameters are set, wait for pMin time
          if (observe_info->attributes == NULL || observe_info->attributes->set_attr_mask == 0)
          {
            // no conditions
            notify = true;
            LOG_INFO("Notify with no conditions");
          }

          // If lt, gt and st parameters are set
          if (notify == false && observe_info->attributes != NULL
              && (observe_info->attributes->set_attr_mask & LWM2M_ATTR_FLAG_NUMERIC) != 0
              && data != NULL)
          {
            // check for less than parameter
            if ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E) != 0)
            {
              LOG_INFO("Checking lower threshold");
              // Did we cross the lower treshold ?
              switch (data->type)
              {
                case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                  if ((integer_value < observe_info->attributes->lessThan
                        && observe_info->lastvalue.asInteger > observe_info->attributes->lessThan)
                      || (integer_value > observe_info->attributes->lessThan
                        && observe_info->lastvalue.asInteger < observe_info->attributes->lessThan))
                  {
                    LOG_INFO("Notify on lower threshold crossing");
                    notify = true;
                  }
                  break;
                case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                  if ((float_value < observe_info->attributes->lessThan
                        && observe_info->lastvalue.asFloat > observe_info->attributes->lessThan)
                      || (float_value > observe_info->attributes->lessThan
                        && observe_info->lastvalue.asFloat < observe_info->attributes->lessThan))
                  {
                    LOG_INFO("Notify on lower threshold crossing");
                    notify = true;
                  }
                  break;
                default:
                  break;
              }
            }
            // check for greater than parameter
            if ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E) != 0)
            {
              LOG_INFO("Checking upper threshold");
              // Did we cross the upper treshold ?
              switch (data->type)
              {
                case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                  if ((integer_value < observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asInteger > observe_info->attributes->greaterThan)
                      || (integer_value > observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asInteger < observe_info->attributes->greaterThan))
                  {
                    LOG_INFO("Notify on upper threshold crossing");
                    notify = true;
                  }
                  break;
                case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                  if ((float_value < observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asFloat > observe_info->attributes->greaterThan)
                      || (float_value > observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asFloat < observe_info->attributes->greaterThan))
                  {
                    LOG_INFO("Notify on upper threshold crossing");
                    notify = true;
                  }
                  break;
                default:
                  break;
              }
            }
            // check for step parameter
            if ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_STEP_E) != 0)
            {
              LOG_INFO("Checking step");

              switch (data->type)
              {
                case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                  {
                    int64_t diff;

                    diff = integer_value - observe_info->lastvalue.asInteger;
                    if ((diff < 0 && (0 - diff) >= observe_info->attributes->step)
                        || (diff >= 0 && diff >= observe_info->attributes->step))
                    {
                      LOG_INFO("Notify on step condition");
                      notify = true;
                    }
                  }
                  break;
                case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                  {
                    double diff;

                    diff = float_value - observe_info->lastvalue.asFloat;
                    if ((diff < 0 && (0 - diff) >= observe_info->attributes->step)
                        || (diff >= 0 && diff >= observe_info->attributes->step))
                    {
                      LOG_INFO("Notify on step condition");
                      notify = true;
                    }
                  }
                  break;
                default:
                  break;
              }
            }
          }

          // If pmin or pmax parameters are set and vaue attribute is not set
          if (notify == false && observe_info->attributes != NULL
              && ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
              || (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)))
          {
            if((observe_info->attributes->set_attr_mask & LWM2M_ATTR_FLAG_NUMERIC)== 0)
            {
              notify = true;
            }
          }

          // Get the Minimum period 
          if(observe_info->attributes == NULL || (observe_info->attributes != NULL 
              && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) == 0))
          {
            target = sen_findInheritedWatcher(handler, &uri, observe_info->msg_id, observe_info->msg_id_len,
                                                QAPI_NET_LWM2M_MIN_PERIOD_E);
            if(target != NULL)
            {
              if(target->attributes != NULL && (target->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
              {
                min_period = target->attributes->minPeriod;
              }
            }
            else
            {
              if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
              {
                min_period = observe_info->attributes->minPeriod;
              }
            }
          }
          else
          {
            if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
            {
              min_period = observe_info->attributes->minPeriod;
            }
          }

          if(notify == true)
          {
            if (observe_info->last_time + min_period > current_time)
            {
              notify = false;
            }
            else
            {
              LOG_INFO("Notify on minimal period");
              notify = true;
            }
          }
        } // watcher->update = true
        }
        // Is the Maximum Period reached ?
        if(notify == false)
        {
          if(observe_info->last_time + max_period <= current_time)
          {
            LOG_INFO("Notify on maximal period");
            notify = true;
          }
        }
        if((observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E))
        {
          cont_type = QAPI_NET_LWM2M_TEXT_PLAIN;
        }
        else
        {
          cont_type = QAPI_NET_LWM2M_M2M_TLV;
        }

        if (notify == true)
        {
          tx_byte_allocate(byte_pool_sensor, (VOID **)&notify_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
          if(notify_data == NULL)
          {
           LOG_INFO("Memory allocation failure.\n");
           return;
          }
          memset(notify_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));
          if (buffer == NULL)
          {
            if (data != NULL)
            {
              length = lwm2m_data_encode_payload(&observed->uri,
                                                 size,
                                                 data,
                                                 &cont_type,
                                                 &buffer);

              if(length == 0)
              break;
            }
            else
            {
              if(sen_object_read(handler, observed->uri, cont_type, &buffer, &length) != QAPI_OK)
              {
                buffer = NULL;
                break;
              }
            }

            notify_data->payload_len = length;
            tx_byte_allocate(byte_pool_sensor, (VOID **)&(notify_data->payload), 
                             length, TX_NO_WAIT);
            if(notify_data->payload == NULL)
            {
              LOG_INFO("Error in memory allocation");
              result = QAPI_ERR_NO_MEMORY;
              tx_byte_release(notify_data);
              notify_data = NULL;
              return;
            }
            memcpy(notify_data->payload, buffer, length);
            notify_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
            notify_data->obj_info.obj_mask    = (qapi_Net_LWM2M_ID_t)observed->uri.flag;
            notify_data->obj_info.obj_id      = observed->uri.objectId;
            notify_data->obj_info.obj_inst_id = observed->uri.instanceId;
            notify_data->obj_info.res_id      = observed->uri.resourceId;
            notify_data->obj_info.res_inst_id = observed->uri.resourceInstId;
            notify_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
            notify_data->conf_msg    = 0;
            notify_data->msg_id_len  = observed->observe_info->msg_id_len;
            memcpy(notify_data->msg_id, observed->observe_info->msg_id,  observed->observe_info->msg_id_len);
            notify_data->content_type = cont_type; 
            notify_data->observation_seq_num = ++observed->observe_info->seq_num;

          }


          // check if the device is in sleep state
          if(lwm2m_client_sleep_state)
          {
              // If the observation request is for resource level update the last value.
              if(store_value)
              {
                LOG_INFO("store value is true\n");
                switch (data->type)
                {
                  case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                    if(integer_value != observe_info->lastvalue.asInteger)
                    {
                      LOG_INFO("Integer value changed\n");
                      observe_info->lastvalue.asInteger = integer_value;
                    }
                    else
                    {
                      LOG_INFO("Integer value did not change\n");
                    }
                    break;
      
                  case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                    if(float_value != observe_info->lastvalue.asFloat)
                    {
                      LOG_INFO("Float value changed\n");
                      observe_info->lastvalue.asFloat = float_value;
                    }
                    else
                    {
                      LOG_INFO("Float value did not change\n");
                    }
                    break;
                }
              }
              //Add into notify pending list
              LOG_INFO("Adding notifications into pending list for sleep state\n");
              sen_observer_add_notify_to_pending_list(handler, observe_info, observed->uri, cont_type, buffer, length);
              qapi_Net_LWM2M_Wakeup(handler ,observe_info->msg_id,observe_info->msg_id_len);

              observe_info->last_time = current_time; //To avoid observe_step from being called immediately

              if(notify_data)
              {
                if(notify_data->payload)
                {
                  tx_byte_release(notify_data->payload);
                  notify_data->payload = NULL;
                }
                tx_byte_release(notify_data);
                notify_data = NULL;
              }
              notify_store = true;
          } 
          else
          {
            // Send notifications

              observe_info->last_time = current_time;

              result = qapi_Net_LWM2M_Send_Message(handler, notify_data);
              observed->observe_info->not_id = notify_data->notification_id;

              observe_info->update = false;

              if ((notify == true) && (store_value == true))
              {
                switch (data->type)
                {
                  case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                    observe_info->lastvalue.asInteger = integer_value;
                    break;
                  case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                    observe_info->lastvalue.asFloat = float_value;
                    break;
                  default:
                    break;
                }
              }
          }
          if(notify_data)
          {
            if(notify_data->payload)
            {
              tx_byte_release(notify_data->payload);
              notify_data->payload = NULL;
            }
            tx_byte_release(notify_data);
            notify_data = NULL;
          }
          if(buffer != NULL && !lwm2m_client_sleep_state && !notify_store) 
          {
            buffer = NULL;
          }
        } // notify == true

      }
    }
    if(data != NULL)
    {
      data_free(size, data);
    }
  }
  LOG_INFO("result is %d",result);
}

void send_response_message(qapi_Net_LWM2M_App_Handler_t handler,
                           qapi_Net_LWM2M_Server_Data_t op_data,
                           qapi_Net_LWM2M_Response_Code_t status)
{
  qapi_Net_LWM2M_App_Ex_Obj_Data_t app_data;

  memset(&app_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  app_data.msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
  app_data.obj_info = op_data.obj_info;
  app_data.status_code = status;
  app_data.conf_msg = 0;
  app_data.msg_id_len = op_data.msg_id_len;
  memcpy(app_data.msg_id, op_data.msg_id, op_data.msg_id_len);

  qapi_Net_LWM2M_Send_Message(handler, &app_data);
}

qapi_Status_t sensor_auto_create()
{
  qapi_Status_t result = QAPI_ERROR;

  if(test_object_handler_app == NULL)
  {
    result = sen_object_register(0);
    if(result != QAPI_OK)
    {
      LOG_INFO("Presence sensor object not registered.");
      goto end;
    }
    else
    {
      QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Presence sensor application successfully registered ");
    }
  }

  result = create_object(0);
  if(result != QAPI_OK && result != QAPI_ERR_EXISTS)
  {
    LOG_INFO("Object creation failed.");
    goto end;
  }
  else if(result == QAPI_OK)
  {
    QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Presence sensor object created ");
  }
  else if(result == QAPI_ERR_EXISTS)
  {
    QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Presence sensor object already created ");
  }

  result = create_object_instance_app();
  if(result != QAPI_OK)
  {
    LOG_INFO("Object instance creation failed.");
    QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Presence sensor object instance creation failed  with error %d", result);
    goto end;
  }
  else
  {
    QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Presence sensor object instance successfully created ");
  }

  //QCLI_Printf(lwm2m_sen_handle ," Presence sensor created.");
  start_busy_to_clear_timer();
  start_clear_to_busy_timer();

end:
  return result;
}

qapi_Status_t update_state_clear()
{
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  qapi_Net_LWM2M_Uri_t uri;

  object = lwm2m_object_find(object_presence_sensor, PRESENCE_SENSOR_OBJECT_ID);

  if(object == NULL)
  {
    LOG_INFO("Object not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  instance = lwm2m_instance_find(object->instance_info, 0);

  if(instance == NULL)
  {
    LOG_INFO("Instance id not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  resource = lwm2m_resource_find(instance->resource_info , RES_O_DIG_INPUT_COUNTER);

  if(resource == NULL)
  {
    LOG_INFO("Resource not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  resource1 = lwm2m_resource_find(instance->resource_info , RES_M_DIG_INPUT_STATE);

  if(resource && resource1)
  {
    memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
    uri.flag = (QAPI_NET_LWM2M_OBJECT_ID_E| QAPI_NET_LWM2M_INSTANCE_ID_E| QAPI_NET_LWM2M_RESOURCE_ID_E);
    uri.objectId = PRESENCE_SENSOR_OBJECT_ID;
    uri.instanceId = 0;
    uri.resourceId = RES_M_DIG_INPUT_STATE;

    resource1->value.asBoolean = false;
    resource->value.asInteger = 0;

    sen_obj_resource_value_changed(test_object_handler_app, &uri);

    qapi_Timer_Stop(btc_Timer);
  }

  return QAPI_OK;
}
qapi_Status_t update_state_busy()
{
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  qapi_Net_LWM2M_Uri_t uri;

  object = lwm2m_object_find(object_presence_sensor, PRESENCE_SENSOR_OBJECT_ID);

  if(object == NULL)
  {
    LOG_INFO("Object not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  instance = lwm2m_instance_find(object->instance_info, 0);

  if(instance == NULL)
  {
    LOG_INFO("Instance id not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  resource = lwm2m_resource_find(instance->resource_info , RES_O_DIG_INPUT_COUNTER);

  if(resource == NULL)
  {
    LOG_INFO("Resource not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  resource1 = lwm2m_resource_find(instance->resource_info , RES_M_DIG_INPUT_STATE);

  if(resource && resource1)
  {
    memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
    uri.flag = (QAPI_NET_LWM2M_OBJECT_ID_E| QAPI_NET_LWM2M_INSTANCE_ID_E| QAPI_NET_LWM2M_RESOURCE_ID_E);
    uri.objectId = PRESENCE_SENSOR_OBJECT_ID;
    uri.instanceId = 0;
    uri.resourceId = RES_M_DIG_INPUT_STATE;

    resource1->value.asBoolean = true;
    resource->value.asInteger = digital_counter;
    
    sen_obj_resource_value_changed(test_object_handler_app, &uri);
    qapi_Timer_Stop(ctb_Timer);
  }

  return QAPI_OK;
}

qapi_Status_t auto_create(uint32_t Parameter_Count,
                          qapi_CLI_Parameter_t *Parameter_List)
{
  int id = 0;
  sen_obj_cmd_t* cmd_ptr = NULL;

  qapi_Net_LWM2M_Sen_t *sen_param = NULL;

  tx_byte_allocate(byte_pool_sensor, (VOID **)&sen_param, sizeof(qapi_Net_LWM2M_Sen_t), TX_NO_WAIT);

  if(sen_param == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
    return QAPI_ERR_NO_MEMORY;
  }

  memset(sen_param, 0x00, sizeof(qapi_Net_LWM2M_Sen_t));

  if(id == 0)
  {
    sen_param->app_data.ul_op.id = id;
    sen_param->app_data.ul_op.msg_type = QAPI_NET_LWM2M_SEN_AUTO_E;
  }
  else
  {
    LOG_INFO("Not valid application id.");
    tx_byte_release(sen_param);
    return QAPI_ERR_INVALID_PARAM;
  }

  LOG_INFO("Switching from QCLI to application callback context");

  if(id == 0)
  {
    if((cmd_ptr = sen_obj_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
      return QAPI_ERR_NO_MEMORY;
    }
    else
    {
      cmd_ptr->cmd_hdr.cmd_id              = SEN_OBJ_APP_ORIGINATED_EVT;
      cmd_ptr->cmd_data.data               = (void *)sen_param;
  
      if(app_registerd == false)
      {
        //QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
        tx_byte_release(sen_param);
        return QAPI_ERR_NOT_INITIALIZED;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
      sen_obj_enqueue_cmd(cmd_ptr);
    }
  
    sen_obj_set_signal();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }

  return QAPI_OK;
}

qapi_Status_t set_dig_cnt(uint16_t count)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2 = NULL;
  qapi_TIMER_set_attr_t time_attr;

  object = lwm2m_object_find(object_presence_sensor, PRESENCE_SENSOR_OBJECT_ID);

  if(object == NULL)
  {
    LOG_INFO("Object not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  instance = lwm2m_instance_find(object->instance_info, 0);

  if(instance == NULL)
  {
    LOG_INFO("Instance id not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  resource = lwm2m_resource_find(instance->resource_info , RES_O_DIG_INPUT_COUNTER);

  if(resource == NULL)
  {
    LOG_INFO("Resource not present.");
    return QAPI_ERR_NO_ENTRY;
  }

  resource1 = lwm2m_resource_find(instance->resource_info , RES_O_BUSY_TO_CLEAR_DELAY);
  resource2 = lwm2m_resource_find(instance->resource_info , RES_O_CLEAR_TO_BUSY_DELAY);

  if(count == 0 && resource1)
  {
    memset(&time_attr, 0, sizeof(qapi_TIMER_set_attr_t));
    time_attr.unit = QAPI_TIMER_UNIT_MSEC;
    time_attr.reload = FALSE;
    time_attr.time = resource1->value.asInteger;
    result = qapi_Timer_Set(btc_Timer, &time_attr);
    if(result == QAPI_OK)
    {
      LOG_INFO("Timer set successfully.\n");
    }
    else
    {
      LOG_INFO("Timer set failed.\n");
    }

    QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Digital input counter value set to %lu ",count);

    return QAPI_OK;
  }

  else if(count > 0 && resource2)
  {
    if(resource->value.asInteger == 0)
    {
      digital_counter = count;
      memset(&time_attr, 0, sizeof(qapi_TIMER_set_attr_t));
      time_attr.unit = QAPI_TIMER_UNIT_MSEC;
      time_attr.reload = FALSE;
      time_attr.time = resource2->value.asInteger;
      result = qapi_Timer_Set(ctb_Timer, &time_attr);
      if(result == QAPI_OK)
      {
        LOG_INFO("Timer set successfully.\n");
      }
      else
      {
        LOG_INFO("Timer set failed.\n");
      }
      QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Digital input counter value set to %lu ",count);
      return QAPI_OK;
    }
    else if(resource->value.asInteger > 0)
    {
      resource->value.asInteger = count;
      QFLOG_MSG(MSG_SSID_DFLT,MSG_MASK_2, "Digital input counter value set to %lu ",count);
      return QAPI_OK;
    }
  }

  return result;
}

qapi_Status_t set_input_cnt(uint32_t Parameter_Count,
                            qapi_CLI_Parameter_t *Parameter_List)
{
  int32_t cnt = -1;
  sen_obj_cmd_t* cmd_ptr = NULL;

  qapi_Net_LWM2M_Sen_t *sen_param = NULL;

  if(Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QAPI_USAGE_ERROR;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    cnt = Parameter_List[0].Integer_Value;
  }
  else
  {
    return QAPI_ERROR;
  }

  if((cnt < 0) || (cnt > 65535))
  {
    LOG_INFO("Input counter not in range.");
    return QAPI_ERROR;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&sen_param, sizeof(qapi_Net_LWM2M_Sen_t), TX_NO_WAIT);

  if(sen_param == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
    return QAPI_ERR_NO_MEMORY;
  }

  memset(sen_param, 0x00, sizeof(qapi_Net_LWM2M_Sen_t));

  sen_param->app_data.ul_op.id = cnt;
  sen_param->app_data.ul_op.msg_type = QAPI_NET_LWM2M_SEN_SET_COUNT_E;

  LOG_INFO("Switching from QCLI to application callback context");

  if((cmd_ptr = sen_obj_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return QAPI_ERR_NO_MEMORY;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = SEN_OBJ_APP_ORIGINATED_EVT;
    cmd_ptr->cmd_data.data               = (void *)sen_param;

    if(app_registerd == false)
    {
      //QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
      tx_byte_release(sen_param);
      return QAPI_ERR_NOT_INITIALIZED;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    sen_obj_enqueue_cmd(cmd_ptr);
  }

  sen_obj_set_signal();

  return QAPI_OK;
}

/**
   @brief This function is the main entry point for the application.
*/
int sensor_dam_app_start(void)
{
  int ret;
  CHAR *app_thread_stack_pointer = NULL;

  txm_module_object_allocate(&byte_pool_sensor, sizeof(TX_BYTE_POOL));
  tx_byte_pool_create(byte_pool_sensor, "Sensor application pool", free_memory_sensor, SENSOR_BYTE_POOL_SIZE);

  ret = txm_module_object_allocate(&app_thread_handle, sizeof(TX_THREAD));
  if(ret != TX_SUCCESS)
  {
    //QCLI_Printf(lwm2m_ext_handle, "Failed to allocate handle for thread.\n", ret);
    return ret;
  }
  /* Allocate the stack for presence sensor object application */

  tx_byte_allocate(byte_pool_sensor, (VOID **) &app_thread_stack_pointer, 8192, TX_NO_WAIT);

  /* Create the sensor application thread.  */
  ret = tx_thread_create(app_thread_handle, "sensor_obj", test_obj_task_entry,
                         0, app_thread_stack_pointer, 8192,
                         150, 150, TX_NO_TIME_SLICE, TX_AUTO_START);

  if(ret != TX_SUCCESS)
  {
    if(app_thread_handle)
    {
      tx_byte_release(app_thread_handle);
    }
    //QCLI_Printf(lwm2m_ext_handle, "Failed to create thread for command (%d).\n", ret);
  }
  app_registerd = true;
  return ret;
}

qapi_Net_LWM2M_Attributes_t *lwm2m_find_attributes(qapi_Net_LWM2M_App_Handler_t handler,
                                                   qapi_Net_LWM2M_Server_Data_t op_data,
                                                   qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *observed;
  qapi_Net_LWM2M_Observe_info_t *observe_info;
  qapi_Net_LWM2M_Attributes_t *param = NULL;

  if(uri == NULL || handler == NULL)
  {
    return NULL;
  }

  observed = observe_find_by_uri(handler, uri);

  if(observed == NULL || observed->observe_info == NULL)
  {
    return NULL;
  }

  for(observe_info = observed->observe_info; observe_info != NULL; observe_info = observe_info->next)
  {
    if(memcmp((observe_info->msg_id + observe_info->msg_id_len - 2), (op_data.msg_id + op_data.msg_id_len -2), 2) == 0)
    {
      param = observe_info->attributes;
    }
  }

  return param;
}

int sen_serialize_attributes(qapi_Net_LWM2M_App_Handler_t handler,
                             qapi_Net_LWM2M_Server_Data_t op_data,
                             qapi_Net_LWM2M_Uri_t *uri,
                             qapi_Net_LWM2M_Attributes_t *object_param,
                             uint8_t *buffer,
                             size_t uri_len,
                             size_t buffer_len)
{
  int head;
  int res;
  qapi_Net_LWM2M_Attributes_t *param = NULL;

  head = 0;
  if(handler == NULL || uri == NULL || buffer == NULL) 
  {
    LOG_INFO(" Passed  Argument is NULL");
    return -1;
  }

  param = lwm2m_find_attributes(handler, op_data, uri);
  if(param == NULL)
  {
    param = object_param;
  }

  if(param != NULL)
  {
    head = uri_len;

    if(param->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
    {
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ATTR_SEPARATOR, LWM2M_LINK_ATTR_SEPARATOR_SIZE);
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_ATTR_MIN_PERIOD_STR, LWM2M_ATTR_MIN_PERIOD_LEN);

      res = utils_intToText(param->minPeriod, buffer + head, buffer_len - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
    }
    else if((object_param) && (object_param->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E))
    {
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ATTR_SEPARATOR, LWM2M_LINK_ATTR_SEPARATOR_SIZE);
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_ATTR_MIN_PERIOD_STR, LWM2M_ATTR_MIN_PERIOD_LEN);

      res = utils_intToText(object_param->minPeriod, buffer + head, buffer_len - head);
      if(res <= 0) return -1;
      head += res;
    }

    if(param->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
    {
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ATTR_SEPARATOR, LWM2M_LINK_ATTR_SEPARATOR_SIZE);
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_ATTR_MAX_PERIOD_STR, LWM2M_ATTR_MAX_PERIOD_LEN);

      res = utils_intToText(param->maxPeriod, buffer + head, buffer_len - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
    }
    else if((object_param != NULL) && (object_param->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E))
    {
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ATTR_SEPARATOR, LWM2M_LINK_ATTR_SEPARATOR_SIZE);
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_ATTR_MAX_PERIOD_STR, LWM2M_ATTR_MAX_PERIOD_LEN);

      res = utils_intToText(object_param->maxPeriod, buffer + head, buffer_len - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
    }

    if(param->set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E)
    {
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ATTR_SEPARATOR, LWM2M_LINK_ATTR_SEPARATOR_SIZE);
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_ATTR_GREATER_THAN_STR, LWM2M_ATTR_GREATER_THAN_LEN);

      res = utils_floatToText(param->greaterThan, buffer + head, buffer_len - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
    }
    if(param->set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E)
    {
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ATTR_SEPARATOR, LWM2M_LINK_ATTR_SEPARATOR_SIZE);
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_ATTR_LESS_THAN_STR, LWM2M_ATTR_LESS_THAN_LEN);

      res = utils_floatToText(param->lessThan, buffer + head, buffer_len - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
    }
    if(param->set_attr_mask & QAPI_NET_LWM2M_STEP_E)
    {
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ATTR_SEPARATOR, LWM2M_LINK_ATTR_SEPARATOR_SIZE);
      PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_ATTR_STEP_STR, LWM2M_ATTR_STEP_LEN);

      res = utils_floatToText(param->step, buffer + head, buffer_len - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
    }
    PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ITEM_ATTR_END, LWM2M_LINK_ITEM_ATTR_END_SIZE);
  }

  if(head > 0)
  {
    head -= uri_len+1;
  }
  return head;
}

int sen_serialize_link_data(qapi_Net_LWM2M_App_Handler_t handler,
                            qapi_Net_LWM2M_Server_Data_t op_data,
                            qapi_Net_LWM2M_Flat_Data_t *tlv,
                            qapi_Net_LWM2M_Attributes_t *object_param,
                            qapi_Net_LWM2M_Uri_t *parent_uri,
                            uint8_t *parent_uri_str,
                            size_t parent_uri_len,
                            uint8_t *buffer,
                            size_t buffer_len)
{
  int head = 0;
  int res;
  qapi_Net_LWM2M_Uri_t uri;

  if(handler == NULL || tlv == NULL || parent_uri == NULL 
     || parent_uri_str == NULL || buffer == NULL ) 
  {
    LOG_INFO(" Passed  Argument is NULL");
    return -1;
  }

  switch (tlv->type)
  {
    case QAPI_NET_LWM2M_TYPE_UNDEFINED:
    case QAPI_NET_LWM2M_TYPE_STRING_E:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
    case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
    case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
      if(buffer_len < LWM2M_LINK_ITEM_START_SIZE)
  {
        return -1;
      }
      memscpy(buffer + head, buffer_len, LWM2M_LINK_ITEM_START, LWM2M_LINK_ITEM_START_SIZE);
      head = LWM2M_LINK_ITEM_START_SIZE;

      if(parent_uri_len > 0)
      {
        if (buffer_len - head < parent_uri_len)
        {
          return -1;
        }
        memscpy(buffer + head, buffer_len - head, parent_uri_str, parent_uri_len);
        head += parent_uri_len;
      }

      if(buffer_len - head < LWM2M_LINK_URI_SEPARATOR_SIZE)
      {
        return -1;
      }
      memscpy(buffer + head, buffer_len - head, LWM2M_LINK_URI_SEPARATOR, LWM2M_LINK_URI_SEPARATOR_SIZE);
      head += LWM2M_LINK_URI_SEPARATOR_SIZE;

      res = utils_intToText(tlv->id, buffer + head, buffer_len - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;

      if(tlv->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)
      {
        if(buffer_len - head < LWM2M_LINK_ITEM_DIM_START_SIZE)
        {
          return -1;
        }
        memscpy(buffer + head, buffer_len - head, LWM2M_LINK_ITEM_DIM_START, LWM2M_LINK_ITEM_DIM_START_SIZE);
        head += LWM2M_LINK_ITEM_DIM_START_SIZE;

        res = utils_intToText(tlv->value.asChildren.count, buffer + head, buffer_len - head);
        if(res <= 0)
        {
          return -1;
        }
        head += res;

        if(buffer_len - head < LWM2M_LINK_ITEM_ATTR_END_SIZE)
        {
          return -1;
        }
        memscpy(buffer + head, buffer_len - head, LWM2M_LINK_ITEM_ATTR_END, LWM2M_LINK_ITEM_ATTR_END_SIZE);
        head += LWM2M_LINK_ITEM_ATTR_END_SIZE;
      }
      else
      {
        if(buffer_len - head < LWM2M_LINK_ITEM_END_SIZE)
        {
          return -1;
        }
        memscpy(buffer + head, buffer_len - head, LWM2M_LINK_ITEM_END, LWM2M_LINK_ITEM_END_SIZE);
        head += LWM2M_LINK_ITEM_END_SIZE;
      }

      memscpy(&uri, sizeof(qapi_Net_LWM2M_Uri_t), parent_uri, sizeof(qapi_Net_LWM2M_Uri_t));
      uri.resourceId = tlv->id;
      uri.flag |= QAPI_NET_LWM2M_RESOURCE_ID_E;
      res = sen_serialize_attributes(handler, op_data, &uri, object_param, buffer, head - 1, buffer_len);
      if(res < 0)
      {
        return -1;
      }
      if(res > 0)
      {
        head += res;
      }
      break;

    case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      {
        uint8_t uri_str[LWM2M_URI_MAX_STRING_LEN];
        size_t uri_len;
        size_t index;

        if(parent_uri_len > 0)
        {
          if(LWM2M_URI_MAX_STRING_LEN < parent_uri_len)
          {
            return -1;
          }
          memscpy(uri_str, LWM2M_URI_MAX_STRING_LEN, parent_uri_str, parent_uri_len);
          uri_len = parent_uri_len;
        }
        else
        {
          uri_len = 0;
        }

        if(LWM2M_URI_MAX_STRING_LEN - uri_len < LWM2M_LINK_URI_SEPARATOR_SIZE)
        {
          return -1;
        }
        if(uri_len >= LWM2M_URI_MAX_STRING_LEN)
        {
          return -1;
        }
        memscpy(uri_str + uri_len, LWM2M_URI_MAX_STRING_LEN - uri_len, 
                LWM2M_LINK_URI_SEPARATOR, LWM2M_LINK_URI_SEPARATOR_SIZE);
        uri_len += LWM2M_LINK_URI_SEPARATOR_SIZE;

        res = utils_intToText(tlv->id, uri_str + uri_len, LWM2M_URI_MAX_STRING_LEN - uri_len);
        if(res <= 0)
        {
          return -1;
        }
        uri_len += res;

        memscpy(&uri, sizeof(qapi_Net_LWM2M_Uri_t), parent_uri, sizeof(qapi_Net_LWM2M_Uri_t));
        uri.instanceId = tlv->id;
        uri.flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;

        head = 0;
        PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ITEM_START, LWM2M_LINK_ITEM_START_SIZE);
        PRV_CONCAT_STR(buffer, buffer_len, head, uri_str, uri_len);
        PRV_CONCAT_STR(buffer, buffer_len, head, LWM2M_LINK_ITEM_END, LWM2M_LINK_ITEM_END_SIZE);

        res = sen_serialize_attributes(handler, op_data, &uri, NULL, buffer, head - 1, buffer_len);
        if(res < 0)
        {
          return -1;
        }
        if(res == 0)
        {
          head = 0;
        }
        else
        {
          head += res;
        }
        for (index = 0; index < tlv->value.asChildren.count; index++)
        {
          res = sen_serialize_link_data(handler, op_data, tlv->value.asChildren.array + index, object_param, &uri, uri_str, uri_len, buffer + head, buffer_len - head);
          if(res < 0)
          {
            return -1;
          }
          head += res;
        }
      }
      break;

    case QAPI_NET_LWM2M_TYPE_OBJECT:
    default:
      return -1;
  }

  return head;
}

int sen_obj_discover_serialize(qapi_Net_LWM2M_App_Handler_t handler,
                               qapi_Net_LWM2M_Server_Data_t op_data,
                               qapi_Net_LWM2M_Uri_t *uri,
                               int size,
                               qapi_Net_LWM2M_Flat_Data_t *data,
                               uint8_t **buffer)
{
  uint8_t buffer_link[PRV_LINK_BUFFER_SIZE];
  uint8_t base_uri_str[LWM2M_URI_MAX_STRING_LEN];
  int base_uri_len = 0;
  int index = 0;
  size_t head = 0;
  int res = 0;
  qapi_Net_LWM2M_Uri_t parent_uri;
  qapi_Net_LWM2M_Attributes_t *param;
  qapi_Net_LWM2M_Attributes_t merged_param;

  if(handler == NULL || uri == NULL || data == NULL || buffer == NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return -1;
  }
  LOG_INFO("size: %d", size);

  memset(&parent_uri, 0, sizeof(qapi_Net_LWM2M_Uri_t));
  parent_uri.objectId = uri->objectId;
  parent_uri.flag = QAPI_NET_LWM2M_OBJECT_ID_E;

  if(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    qapi_Net_LWM2M_Uri_t temp_uri;
    qapi_Net_LWM2M_Attributes_t *obj_param;
    qapi_Net_LWM2M_Attributes_t *inst_param;

    memset(&temp_uri, 0, sizeof(qapi_Net_LWM2M_Uri_t));
    temp_uri.objectId = uri->objectId;
    temp_uri.flag = QAPI_NET_LWM2M_OBJECT_ID_E;

    // get object level attributes
    obj_param = lwm2m_find_attributes(handler, op_data, &temp_uri);

    // get object instance level attributes
    temp_uri.instanceId = uri->instanceId;
    temp_uri.flag = QAPI_NET_LWM2M_INSTANCE_ID_E;
    inst_param = lwm2m_find_attributes(handler, op_data, &temp_uri);

    if(obj_param != NULL)
    {
      if(inst_param != NULL)
      {
        memset(&merged_param, 0, sizeof(qapi_Net_LWM2M_Attributes_t));
        merged_param.set_attr_mask = obj_param->set_attr_mask;
        merged_param.set_attr_mask |= inst_param->set_attr_mask;
        if(merged_param.set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
        {
          if(inst_param->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
          {
            merged_param.minPeriod = inst_param->minPeriod;
          }
          else
          {
            merged_param.minPeriod = obj_param->minPeriod;
          }
        }
        if(merged_param.set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
        {
          if(inst_param->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
          {
            merged_param.maxPeriod = inst_param->maxPeriod;
          }
          else
          {
            merged_param.maxPeriod = obj_param->maxPeriod;
          }
        }
        param = &merged_param;
      }
      else
      {
        param = obj_param;
      }
    }
    else
    {
      param = inst_param;
    }
    uri->flag &= ~QAPI_NET_LWM2M_RESOURCE_ID_E;
  }
  else
  {
    param = NULL;

    if(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E)
    {
      PRV_CONCAT_STR(buffer_link, PRV_LINK_BUFFER_SIZE, head, LWM2M_LINK_ITEM_START, LWM2M_LINK_ITEM_START_SIZE);
      PRV_CONCAT_STR(buffer_link, PRV_LINK_BUFFER_SIZE, head, LWM2M_LINK_URI_SEPARATOR, LWM2M_LINK_URI_SEPARATOR_SIZE);

      res = utils_intToText(uri->objectId, buffer_link + head, PRV_LINK_BUFFER_SIZE - head);

      if(res <= 0)
      {
        return -1;
      }
      head += res;
      PRV_CONCAT_STR(buffer_link, PRV_LINK_BUFFER_SIZE, head, LWM2M_LINK_URI_SEPARATOR, LWM2M_LINK_URI_SEPARATOR_SIZE);

      res = utils_intToText(uri->instanceId, buffer_link + head, PRV_LINK_BUFFER_SIZE - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
      PRV_CONCAT_STR(buffer_link, PRV_LINK_BUFFER_SIZE, head, LWM2M_LINK_ITEM_END, LWM2M_LINK_ITEM_END_SIZE);
      parent_uri.instanceId = uri->instanceId;
      parent_uri.flag = QAPI_NET_LWM2M_INSTANCE_ID_E;

      res = sen_serialize_attributes(handler, op_data, &parent_uri, NULL, buffer_link, head - 1, PRV_LINK_BUFFER_SIZE);
      if(res < 0)
      {
        return -1;
      }
      head += res;
    }
    else
    {
      PRV_CONCAT_STR(buffer_link, PRV_LINK_BUFFER_SIZE, head, LWM2M_LINK_ITEM_START, LWM2M_LINK_ITEM_START_SIZE);
      PRV_CONCAT_STR(buffer_link, PRV_LINK_BUFFER_SIZE, head, LWM2M_LINK_URI_SEPARATOR, LWM2M_LINK_URI_SEPARATOR_SIZE);

      res = utils_intToText(uri->objectId, buffer_link + head, PRV_LINK_BUFFER_SIZE - head);
      if(res <= 0)
      {
        return -1;
      }
      head += res;
      PRV_CONCAT_STR(buffer_link, PRV_LINK_BUFFER_SIZE, head, LWM2M_LINK_ITEM_END, LWM2M_LINK_ITEM_END_SIZE);

      res = sen_serialize_attributes(handler, op_data, &parent_uri, NULL, buffer_link, head - 1, PRV_LINK_BUFFER_SIZE);
      if(res < 0)
      {
        return -1;
      }
      head += res;
    }
  }

  base_uri_len = uri_toString(uri, base_uri_str, LWM2M_URI_MAX_STRING_LEN, NULL);
  if(base_uri_len < 0)
  {
    return -1;
  }
  base_uri_len -= 1;

  for(index = 0; index < size && head < PRV_LINK_BUFFER_SIZE; index++)
  {
    res = sen_serialize_link_data(handler, op_data, data + index, param, uri, base_uri_str, base_uri_len, buffer_link + head, PRV_LINK_BUFFER_SIZE - head);
    if(res < 0) return -1;
    head += res;
  }

  if (head > 0)
  {
    head -= 1;
    tx_byte_allocate(byte_pool_sensor, (VOID **)&(*buffer), 
                     head, TX_NO_WAIT);
    if(*buffer == NULL) return 0;
    memcpy(*buffer, buffer_link, head);
  }

  return (int)head;
}
