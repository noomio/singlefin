/*===========================================================================
                         TEST_UTILS.C

DESCRIPTION
   File contains the utilities for the Presence sensor demo application 
  
   Copyright (c) 2017 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "test_app_module.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

extern TX_BYTE_POOL *byte_pool_sensor;

#define TLV_TYPE_MASK 0xC0
#define TLV_HEADER_MAX_LENGTH 6

#define TLV_TYPE_UNKNOWN           (uint8_t)0xFF
#define TLV_TYPE_OBJECT            (uint8_t)0x10
#define TLV_TYPE_OBJECT_INSTANCE   (uint8_t)0x00
#define TLV_TYPE_RESOURCE          (uint8_t)0xC0
#define TLV_TYPE_MULTIPLE_RESOURCE (uint8_t)0x80
#define TLV_TYPE_RESOURCE_INSTANCE (uint8_t)0x40

qapi_Net_LWM2M_Data_t* lwm2m_object_add(qapi_Net_LWM2M_Data_t* head,
                                            qapi_Net_LWM2M_Data_t* node)
{
  qapi_Net_LWM2M_Data_t* target = NULL;

  if (NULL == head) return node;

  if(head->object_ID > node->object_ID)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->object_ID < node->object_ID)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}


qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_add(qapi_Net_LWM2M_Instance_Info_t* head,
                                                   qapi_Net_LWM2M_Instance_Info_t* node)
{
  qapi_Net_LWM2M_Instance_Info_t* target = NULL;

  if (NULL == head) return node;

  if(head->instance_ID > node->instance_ID)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->instance_ID < node->instance_ID)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

qapi_Net_LWM2M_Resource_Info_t* lwm2m_resource_add(qapi_Net_LWM2M_Resource_Info_t* head,
                                                   qapi_Net_LWM2M_Resource_Info_t* node)
{
  qapi_Net_LWM2M_Resource_Info_t* target = NULL;

  if (NULL == head) return node;

  if(head->resource_ID > node->resource_ID)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->resource_ID < node->resource_ID)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

qapi_Net_LWM2M_Data_t* lwm2m_object_remove(qapi_Net_LWM2M_Data_t *head,
                                           qapi_Net_LWM2M_Data_t ** nodeP,
                                           uint8_t id)
{
  qapi_Net_LWM2M_Data_t * target;

  if (head == NULL)
  {
    if (nodeP) *nodeP = NULL;
    return NULL;
  }

  if (head->object_ID == id)
  {
    if (nodeP) *nodeP = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->object_ID < id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->object_ID == id)
  {
    if (nodeP) *nodeP = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (nodeP) *nodeP = NULL;
  }

  return head;
}


qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_remove(qapi_Net_LWM2M_Instance_Info_t *head,
                                                      qapi_Net_LWM2M_Instance_Info_t ** nodeP,
                                                      uint8_t id)
{
  qapi_Net_LWM2M_Instance_Info_t * target;

  if (head == NULL)
  {
    if (nodeP) *nodeP = NULL;
    return NULL;
  }

  if (head->instance_ID== id)
  {
    if (nodeP) *nodeP = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->instance_ID< id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->instance_ID== id)
  {
    if (nodeP) *nodeP = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (nodeP) *nodeP = NULL;
  }

  return head;
}

qapi_Net_LWM2M_Data_t* lwm2m_object_find(qapi_Net_LWM2M_Data_t* head,
                                         uint16_t id)
{
  while (NULL != head && head->object_ID < id)
  {
    head = head->next;
  }

  if (NULL != head && head->object_ID == id)
  return head;

  return NULL;
}

qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_find(qapi_Net_LWM2M_Instance_Info_t* head,
                                                    uint16_t id)
{
  while (NULL != head && head->instance_ID < id)
  {
    head = head->next;
  }

  if (NULL != head && head->instance_ID == id)
  return head;

  return NULL;
}

qapi_Net_LWM2M_Resource_Info_t* lwm2m_resource_find(qapi_Net_LWM2M_Resource_Info_t* head,
                                                    uint16_t id)
{
  while (NULL != head && head->resource_ID< id)
  {
    head = head->next;
  }

  if (NULL != head && head->resource_ID == id)
  return head;

  return NULL;
}

qapi_Net_LWM2M_Flat_Data_t *data_malloc(int size)
{
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;

  if (size <= 0)
  {
    return NULL;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&data, size * sizeof(qapi_Net_LWM2M_Flat_Data_t), TX_NO_WAIT);

  if(data != NULL)
  {
    memset(data, 0, size * sizeof(qapi_Net_LWM2M_Flat_Data_t));
  }

  return data;
}

void data_free(int size,
                      qapi_Net_LWM2M_Flat_Data_t *data)
{
  int i = 0;

  LOG_INFO("Size to free: %d", size);
  if(size == 0 || data == NULL)
  {
    return;
  }

  for (i = 0; i < size; i++)
  {
    switch(data[i].type)
    {
      case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
      case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      case QAPI_NET_LWM2M_TYPE_OBJECT:
           data_free(data[i].value.asChildren.count, data[i].value.asChildren.array);
        break;

      case QAPI_NET_LWM2M_TYPE_STRING_E:
      case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
        if (data[i].value.asBuffer.buffer != NULL)
        {
          tx_byte_release(data[i].value.asBuffer.buffer);
        }

      default:
        // do nothing
        break;
    }
  }
  tx_byte_release(data);
}

/**
 * @fn utils_copyValue() 
 * @brief This function is used to copy value input string into output string
 * @param dst destination string
 * @param src source string
 * @param len length to be copied
 * @return  void
 */
void copy_value(void * dst,
                const void *src,
                size_t len)
{
  size_t i = 0;

  if (dst == NULL || src == NULL)
  {
    LOG_INFO("Passed NULL Arguments\n");
    return;
  }

  for (i = 0; i < len; i++)
  {
    ((uint8_t *)dst)[i] = ((uint8_t *)src)[len - 1 - i];
  }

}

static size_t int_to_text(int64_t data,
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

static size_t float_to_text(double data,
                            uint8_t *string,
                            size_t length)
{
  size_t int_length = 0;
  size_t dec_length = 0;
  int64_t int_part = 0;
  double dec_part = 0;

  if(string == NULL)
  {
    LOG_INFO(" Passed NULL Arguments\n");
    return 0;
  }

  if(data <= (double)INT64_MIN || data >= (double)INT64_MAX)
  {
    return 0;
  }

  int_part = (int64_t)data;
  dec_part = data - int_part;
  if(dec_part < 0)
  {
    dec_part = 1 - dec_part;
  }
  else
  {
    dec_part = 1 + dec_part;
  }

  if(dec_part <= 1 + FLT_EPSILON)
  {
    dec_part = 0;
  }

  if (int_part == 0 && data < 0)
  {
    // deal with numbers between -1 and 0
    if(length < 4) return 0;
    string[0] = '-';
    string[1] = '0';
    int_length = 2;
  }
  else
  {
    int_length = int_to_text(int_part, string, length);
    if(int_length == 0) return 0;
  }
  dec_length = 0;
  if(dec_part >= FLT_EPSILON)
  {
    int i;
    double floor;

    if(int_length >= length - 1) return 0;

    i = 0;
    floor = FLT_EPSILON;
    do
    {
      dec_part *= 10;
      floor *= 10;
      i++;
    }while(dec_part - (int64_t)dec_part > floor);

    dec_length = int_to_text(dec_part, string + int_length, length - int_length);
    if(dec_length <= 1) return 0;

    // replace the leading 1 with a dot
    string[int_length] = '.';
  }

  return int_length + dec_length;
}

/**
 * @fn utils_int64ToPlainText()
 * @brief This function is used to convert uint64 value into plain text
 *
 * @param data Input float value
 * @param buffer output param after conversion
 * @return 0 on failure
 *         length of buffer after conversion on success
 */
size_t int64_to_text(int64_t data,
                     uint8_t **buffer)
{
#define STR_LENGTH 32
  uint8_t string[STR_LENGTH];
  size_t length = 0;

  if(buffer == NULL ) 
  { 
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }

  length = int_to_text(data, string, STR_LENGTH);
  if(length == 0)
  {
    return 0;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&(*buffer),
                   length, TX_NO_WAIT);
  if(*buffer == NULL)
  {
    return 0;
  }

  memcpy(*buffer, string, length);

  return length;
}

/**
 * @fn utils_float64ToPlainText()
 * @brief This function is used to convert Float 64 value into plain text
 *
 * @param data Input float value
 * @param buffer output param after conversion
 * @return 0 on failure
 *         length of buffer after conversion on success
 */
size_t float64_to_text(double data,
                       uint8_t **buffer)
{
  uint8_t string[STR_LENGTH * 2];
  size_t length = 0;
  if(buffer== NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }
  length = float_to_text(data, string, STR_LENGTH * 2);
  if(length == 0)
  {
    return 0;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&(*buffer),
                   length, TX_NO_WAIT);
  if(*buffer == NULL)
  {
    return 0;
  }

  memcpy(*buffer, string, length);

  return length;
}

/**
 * @fn utils_boolToPlainText()
 * @brief This function is used to convert bool value into plain text
 *
 * @param data Input bool value
 * @param buffer output param after conversion
 * @return 0 on failure
 *         length of buffer after conversion on success
 */
size_t bool_to_text(bool data,
                    uint8_t **buffer)
{
  if(buffer == NULL ) 
  { 
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }
  return int64_to_text((int64_t)(data?1:0), buffer);
}

/**
 * @fn static size_t prv_textSerialize()
 * @brief This function is used to convert data into text format
 *
 * @param dataP     data lwm2m_data_t strucuture pointer
 * @param bufferP   double pointer to hold result
 *
 * @return    number of bytes copied on success
 *        Zero on failure
 */
static size_t serialize_text(qapi_Net_LWM2M_Flat_Data_t *data,
                             uint8_t **buffer)
{
  if(data == NULL || buffer == NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;  
  }

  switch(data->type)
  {
    case QAPI_NET_LWM2M_TYPE_STRING_E:
      tx_byte_allocate(byte_pool_sensor, (VOID **)&(*buffer),
                       data->value.asBuffer.length, TX_NO_WAIT);
      if(*buffer == NULL)
      {
        return 0;
      }
      memcpy(*buffer, data->value.asBuffer.buffer, data->value.asBuffer.length);
      return data->value.asBuffer.length;

    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      return int64_to_text(data->value.asInteger, buffer);

    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      return float64_to_text(data->value.asFloat, buffer);

    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      return bool_to_text(data->value.asBoolean, buffer);

    case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
    case QAPI_NET_LWM2M_TYPE_UNDEFINED:
    default:
      return 0;
  }
}

static int uri_to_string(qapi_Net_LWM2M_Uri_t *uri,
                         uint8_t * buffer,
                         size_t buffer_len,
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

  res = int_to_text(uri->objectId, buffer + head, buffer_len - head);
  if (res <= 0)
  {
    return -1;
  }
  head += res;

  if(head >= buffer_len - 1)
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
    res = int_to_text(uri->instanceId, buffer + head, buffer_len - head);
    if(res <= 0)
    {
      return -1;
    }
    head += res;
    if(head >= buffer_len - 1)
    {
      return -1;
    }
    if(depth)
    {
      *depth = URI_DEPTH_RESOURCE;
    }
    if(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
    {
      buffer[head] = '/';
      head++;
      res = int_to_text(uri->resourceId, buffer + head, buffer_len - head);
      if(res <= 0) return -1;
      head += res;
      if(head >= buffer_len - 1) return -1;
      if(depth) *depth = URI_DEPTH_RESOURCE_INSTANCE;
    }
  }

  buffer[head] = '/';
  head++;

  LOG_INFO("length: %u, buffer: \"%.*s\"", head, head, buffer);

  return head;
}

static int set_buffer(qapi_Net_LWM2M_Flat_Data_t *data,
                     uint8_t *buffer,
                     size_t buffer_len)
{
  if(data == NULL || buffer == NULL || buffer_len == 0 ) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return 0;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&(data->value.asBuffer.buffer), buffer_len, TX_NO_WAIT);
  if (data->value.asBuffer.buffer == NULL)
  {
    data_free(1, data);
    return 0;
  }
  data->value.asBuffer.length = buffer_len;
  memcpy(data->value.asBuffer.buffer, buffer, buffer_len);

  return 1;
}

/**
 * Encode an integer value to a byte representation.
 * Returns the length of the result. For values < 0xff length is 1,
 * for values < 0xffff length is 2 and so on.
 * @param data        Input value
 * @param data_buffer Result in data_buffer is in big endian encoding
 *                    Negative values are represented in two's complement as of
 *                    OMA-TS-LightweightM2M-V1_0-20160308-D, Appendix C
 */
size_t encode_int(int64_t data,
                  uint8_t data_buffer[BIT64_BUFFER_SIZE])
{
  size_t length = 0;

  memset(data_buffer, 0x00, BIT64_BUFFER_SIZE);

  if(data >= INT8_MIN && data <= INT8_MAX)
  {
    length = 1;
    data_buffer[0] = data;
  }
  else if(data >= INT16_MIN && data <= INT16_MAX)
  {
    int16_t value;

    value = data;
    length = 2;
    data_buffer[0] = (value >> 8) & 0xFF;
    data_buffer[1] = value & 0xFF;
  }
  else if(data >= INT32_MIN && data <= INT32_MAX)
  {
    int32_t value;

    value = data;
    length = 4;
    copy_value(data_buffer, &value, length);
  }
  else if (data >= INT64_MIN && data <= INT64_MAX)
  {
    length = 8;
    copy_value(data_buffer, &data, length);
  }

  return length;
}

/**
 * Encode a float value to a byte representation.
 * @param data        Input value
 * @param data_buffer Result in data_buffer is in big endian encoding
 *                    Negative values are represented in two's complement as of
 *                    OMA-TS-LightweightM2M-V1_0-20160308-D, Appendix C
 */
size_t encode_float(double data,
                    uint8_t data_buffer[BIT64_BUFFER_SIZE])
{
  size_t length = 0;

  memset(data_buffer, 0, BIT64_BUFFER_SIZE);

  if((data < 0.0 - (double)FLT_MAX) || (data >(double)FLT_MAX))
  {
    length = 8;
    copy_value(data_buffer, &data, 8);
  }
  else
  {
    float value;

    length = 4;
    value = (float)data;
    copy_value(data_buffer, &value, 4);
  }

  return length;
}

/**
 * @fn void lwm2m_data_encode_opaque()
 * @brief This function is used to write opaque type of data into
 *        lwm2m_data_t structure
 *
 * @param buffer       buffer data to be written
 * @param length        length of the buffer
 * @param dataP     data lwm2m_data_t strucuture pointer
 *
 * @return    void
 */
void encode_opaque_data(uint8_t * buffer,
                        size_t length,
                        qapi_Net_LWM2M_Flat_Data_t *data)
{
  int res;
  if (buffer == NULL || data == NULL) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return ;
  }

  if (length == 0)
  {
    data->value.asBuffer.length = 0;
    data->value.asBuffer.buffer = NULL;
    res = 1;
  }
  else
  {
    res = set_buffer(data, buffer, length);
  }

  if(res == 1)
  {
    data->type = QAPI_NET_LWM2M_TYPE_OPAQUE_E;
  }
  else
  {
    data->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
  }
}

/**
 * @fn prv_getHeaderType()
 * @brief This function is used to get the TLV header type based on
 *     lwm2m header type
 * @param type lwm2m header type value 
 * @return  If not found _PRV_TLV_TYPE_UNKNOWN type is returned
 */
static uint8_t get_header_type(qapi_Net_LWM2M_Value_Type_t type)
{
  switch (type)
  {
    case QAPI_NET_LWM2M_TYPE_OBJECT:
      return TLV_TYPE_OBJECT;

    case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      return TLV_TYPE_OBJECT_INSTANCE;

    case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
      return TLV_TYPE_MULTIPLE_RESOURCE;

    case QAPI_NET_LWM2M_TYPE_STRING_E:
    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
    case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
      return TLV_TYPE_RESOURCE;

    case QAPI_NET_LWM2M_TYPE_UNDEFINED:
    default:
      return TLV_TYPE_UNKNOWN;
  }
}

/**
 * @fn prv_getDataType()
 * @brief This function is used to get the lwm2m data type based on
 *     tlv data type
 * @param type tlv data type value 
 * @return  If not found LWM2M_TYPE_UNDEFINED type is returned
 */
static qapi_Net_LWM2M_Value_Type_t get_data_type(uint8_t type)
{
  switch(type)
  {
    case TLV_TYPE_OBJECT:
      return QAPI_NET_LWM2M_TYPE_OBJECT;

    case TLV_TYPE_OBJECT_INSTANCE:
      return QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;

    case TLV_TYPE_MULTIPLE_RESOURCE:
      return QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;

    case TLV_TYPE_RESOURCE:
    case TLV_TYPE_RESOURCE_INSTANCE:
      return QAPI_NET_LWM2M_TYPE_OPAQUE_E;

    default:
      return QAPI_NET_LWM2M_TYPE_UNDEFINED;
  }
}

/**
 * @fn prv_getHeaderLength()
 * @brief This function is used to get the actual header length to 
 *     increment the buffer to fetch the data
 * @param id data ID
 * @param datalen data length
 * @return  retuns 2 or more than 2 as per processing
 */
static int get_header_length(uint16_t id,
                             size_t data_len)
{
  int length;

  length = 2;

  if (id > 0xFF)
  {
    length += 1;
  }

  if (data_len > 0xFFFF)
  {
    length += 3;
  }
  else if (data_len > 0xFF)
  {
    length += 2;
  }
  else if (data_len > 7)
  {
    length += 1;
  }

  return length;
}

/**
 * @fn prv_createHeader()
 * @brief This function is used to create header 
 * @param header 
 * @param isInstance specifies the resource instance
 * @param type lwm2m data type, eg int, float etc...
 * @param id specifies object instance or resource value
 * @param data_len length of the data to be populated
 * @return returns header length populated
 */
static int create_header(uint8_t *header,
                         bool is_instance,
                         qapi_Net_LWM2M_Value_Type_t type,
                         uint16_t id,
                         size_t data_len)
{
  int header_len = 0;
  int offset = 0;
  uint8_t hdr_type = 0;
  if(header == NULL) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }
  header_len = get_header_length(id, data_len);
  if(is_instance == true)
  {
    hdr_type = TLV_TYPE_RESOURCE_INSTANCE;
  }
  else
  {
    hdr_type = get_header_type(type);
  }

  header[0] = 0;
  header[0] |= (uint8_t)hdr_type & TLV_TYPE_MASK;

  if (id > 0xFF)
  {
    header[0] |= 0x20;
    header[1] = (id >> 8) & 0XFF;
    header[2] = id & 0XFF;
    offset = 3;
  }
  else
  {
    header[1] = id;
    offset = 2;
  }
  if (data_len <= 7)
  {
    header[0] += data_len;
  }
  else if (data_len <= 0xFF)
  {
    header[0] |= 0x08;
    header[offset] = data_len;
  }
  else if (data_len <= 0xFFFF)
  {
    header[0] |= 0x10;
    header[offset] = (data_len >> 8) & 0XFF;
    header[offset + 1] = data_len & 0XFF;
  }
  else if (data_len <= 0xFFFFFF)
  {
    header[0] |= 0x18;
    header[offset] = (data_len >> 16) & 0XFF;
    header[offset + 1] = (data_len >> 8) & 0XFF;
    header[offset + 2] = data_len & 0XFF;
  }

  return header_len;
}

/**
 * @fn lwm2m_decode_TLV()
 * @brief This function is used to decode the recieved TLV data
 * @param buffer input TLV buffer
 * @param buffer_len buffer length
 * @param oType output type value
 * @param oID output object ID
 * @param oDataIndex output number of instances
 * @param oDatalen output length of the data 
 * @return 0 on failure
 *     valid length decoded on success
 */

int tlv_decode(uint8_t *buffer,
               size_t buffer_len,
               qapi_Net_LWM2M_Value_Type_t *type,
               uint16_t *id,
               size_t *data_index,
               size_t *data_len)
{

  if(buffer == NULL || type == NULL || id == NULL 
      || data_index == NULL || data_len == NULL ) 
  {
    LOG_INFO("LWM2M_LOG: Passed  Argument is NULL");
    return 0;
  }

  if(buffer_len < 2)
  {
    return 0;
  }

  *data_index = 2;

  *type = get_data_type(buffer[0] & TLV_TYPE_MASK);

  // ID is 16 bits long
  if((buffer[0] & 0x20) == 0x20)
  {
    if(buffer_len < 3)
    {
      return 0;
    }
    *data_index += 1;
    *id = (buffer[1]<<8) + buffer[2];
  }
  // ID is 8 bits long
  else
  {
    *id = buffer[1];
  }

  switch(buffer[0] & 0x18)
  {
    case 0x00:                            // no length field
      *data_len = buffer[0]&0x07;
      break;
    case 0x08:                            // length field is 8 bits long
      if(buffer_len < *data_index + 1)
      {
        return 0;
      }
      *data_len = buffer[*data_index];
      *data_index += 1;
      break;
    case 0x10:                           // length field is 16 bits long
      if(buffer_len < *data_index + 2)
      {
        return 0;
      }
      *data_len = (buffer[*data_index]<<8) + buffer[*data_index+1];
      *data_index += 2;
      break;
    case 0x18:                           // length field is 24 bits long
      if(buffer_len < *data_index + 3)
      {
        return 0;
      }
      *data_len = (buffer[*data_index]<<16) + (buffer[*data_index+1]<<8) + buffer[*data_index+2];
      *data_index += 3;
      break;
    default:
      // can't happen
      return 0;
  }

  if (*data_index + *data_len > buffer_len)
  {
    return 0;
  }

  return *data_index + *data_len;
}

/**
 * @fn tlv_parse()
 * @brief This function is used to parse the tlv values received
 * @param buffer input buffer to be parsed
 * @param bufferLen buffer length 
 * @param dataP output buffer after parsing is completed
 * @return 0 on failure
 *     number of elements parsed on success
 */
int tlv_parser(uint8_t *buffer,
               size_t buffer_len,
               qapi_Net_LWM2M_Flat_Data_t **data)
{
  qapi_Net_LWM2M_Value_Type_t type;
  uint16_t id = 0;
  size_t data_index = 0;
  size_t data_len = 0;
  int index = 0;
  int result = 0;
  int size = 0;

  if(data == NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }

  *data = NULL;

  while((result = tlv_decode((uint8_t*)buffer + index, buffer_len - index, &type,
                              &id,&data_index, &data_len)) != 0)
  {
    qapi_Net_LWM2M_Flat_Data_t *tlv_new;

    tlv_new = data_malloc(size + 1);
    if(tlv_new == NULL)
    {
      data_free(size, *data);
      return 0;
    }
    if(size >= 1 && *data != NULL)
    {
      memscpy(tlv_new, (size + 1) * sizeof(qapi_Net_LWM2M_Flat_Data_t),
              *data, size * sizeof(qapi_Net_LWM2M_Flat_Data_t));
      tx_byte_release(*data);
    }
    *data = tlv_new;

    (*data)[size].type = type;
    (*data)[size].id = id;
    if(type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE || type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)
    {
      (*data)[size].value.asChildren.count = tlv_parser(buffer + index + data_index,
                                                        data_len,
                                                        &((*data)[size].value.asChildren.array));
      if ((*data)[size].value.asChildren.count == 0)
      {
        data_free(size + 1, *data);
        return 0;
      }
    }
    else
    {
      encode_opaque_data(buffer + index + data_index, data_len, (*data) + size);
    }
    size++;
    index += result;
  }

  return size;
}

/**
 * @fn prv_getLength()
 * @brief This function is used to get the length of the each item in the
 *        tlv items
 * @param size number of items in the decoded tlv data
 * @param dataP decoded data
 * @return  -1 on failure
 *      length of the data on success
 */
static size_t get_item_length(int size,
                              qapi_Net_LWM2M_Flat_Data_t *data)
{
  int length = 0;
  int i = 0;

  length = 0;
  if(data == NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }
  for (i = 0 ; i < size && length != -1 ; i++)
  {
    switch (data[i].type)
    {
      case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
        {
          int sub_length;

          sub_length = get_item_length(data[i].value.asChildren.count, data[i].value.asChildren.array);
          if(sub_length == -1)
          {
            length = -1;
          }
          else
          {
            length += get_header_length(data[i].id, sub_length) + sub_length;
          }
        }
        break;

      case QAPI_NET_LWM2M_TYPE_STRING_E:
      case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
        length += get_header_length(data[i].id, data[i].value.asBuffer.length) + data[i].value.asBuffer.length;
        break;

      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
        {
          size_t data_len;
          uint8_t unused_buffer[BIT64_BUFFER_SIZE];

          data_len = encode_int(data[i].value.asInteger, unused_buffer);
          length += get_header_length(data[i].id, data_len) + data_len;
        }
        break;

      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
        {
          size_t data_len;

          if ((data[i].value.asFloat < 0.0 - (double)FLT_MAX)
              || (data[i].value.asFloat >(double)FLT_MAX))
          {
            data_len = 8;
          }
          else
          {
            data_len = 4;
          }

          length += get_header_length(data[i].id, data_len) + data_len;
        }
        break;

      case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
        // Booleans are always encoded on one byte
        length += get_header_length(data[i].id, 1) + 1;
        break;

      case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
        // Object Link are always encoded on four bytes
        length += get_header_length(data[i].id, 4) + 4;
        break;

      default:
        length = -1;
        break;
    }
  }

  if (length < 0)
  {
    return 0;
  }
  else
  {
    return (size_t)length;
  }
}

/**
 * @fn tlv_serialize()
 * @brief This function is used to form the buffer to be sent into 
 *     n/w in tlv format
 * @param isResourceInstance indicates if its a resource instance that
 *     is being serialized
 * @param size number of entitites to be serialized
 * @param dataP lwm2m data to be serialized
 * @param bufferP output buffer
 */
size_t tlv_encode(bool is_resource_instance, 
                  int size,
                  qapi_Net_LWM2M_Flat_Data_t *data,
                  uint8_t ** buffer)
{
  size_t length = 0;
  int index = 0;
  int i = 0;
  if(data == NULL || buffer == NULL) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }

  *buffer = NULL;
  length = get_item_length(size, data);
  if (length <= 0)
  {
    return length;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&(*buffer),
                   length, TX_NO_WAIT);
  if(*buffer == NULL)
  {
    return 0;
  }

  index = 0;
  for (i = 0 ; i < size && length != 0 ; i++)
  {
    int header_len;
    bool is_instance;

    is_instance = is_resource_instance;
    switch(data[i].type)
    {
      case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
        is_instance = true;

      case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      {
        uint8_t *tmp_buffer;
        size_t tmp_length;

        tmp_length = tlv_encode(is_instance, data[i].value.asChildren.count, data[i].value.asChildren.array, &tmp_buffer);
        if(tmp_length == 0)
        {
          length = 0;
        }
        else
        {
          header_len = create_header(*buffer + index, false, data[i].type, data[i].id, tmp_length);
          index += header_len;
          memscpy(*buffer + index, length - index, tmp_buffer, tmp_length);
          index += tmp_length;
          tx_byte_release(tmp_buffer);
        }
      }
      break;

      case QAPI_NET_LWM2M_TYPE_STRING_E:
      case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
        header_len = create_header(*buffer + index, is_instance, data[i].type, data[i].id, data[i].value.asBuffer.length);
        {
          index += header_len;
          memscpy(*buffer + index, length - index, data[i].value.asBuffer.buffer, data[i].value.asBuffer.length);
          index += data[i].value.asBuffer.length;
        }
        break;

      case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
        // Object Link is a four-bytes integer
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      {
        size_t data_len;
        uint8_t data_buffer[BIT64_BUFFER_SIZE];

        data_len = encode_int(data[i].value.asInteger, data_buffer);
        header_len = create_header(*buffer + index, is_instance, data[i].type, data[i].id, data_len);
        {
          index += header_len;
          memscpy(*buffer + index, length - index, data_buffer, data_len);
          index += data_len;
        }
      }
      break;

      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      {
        size_t data_len;
        uint8_t data_buffer[BIT64_BUFFER_SIZE];

        data_len = encode_float(data[i].value.asFloat, data_buffer);
        header_len = create_header(*buffer + index, is_instance, data[i].type, data[i].id, data_len);
        {
          index += header_len;
          memscpy(*buffer + index, length - index, data_buffer, data_len);
          index += data_len;
        }
      }
      break;

      case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      header_len = create_header(*buffer + index, is_instance, data[i].type, data[i].id, 1);
      {
        index += header_len;
        (*buffer)[index] = data[i].value.asBoolean ? 1 : 0;
        index += 1;
      }
      break;

      default:
        length = 0;
        break;
    }
  }

  if (length == 0)
  {
    tx_byte_release(*buffer);
    *buffer = NULL;
  }

  return length;
}


/**
 * @fn void int lwm2m_data_parse()
 * @brief This function is used to write lwm2m_data_t structure type of
 *        data into lwm2m_data_t structure
 *
 * @param uriP handle to the resource uri
 * @param buffer handle to chanracter buffer
 * @param format media type of the data
 * @param dataP handle to lwm2m data type
 *
 * @return    0 on error
 *            parsed count on success
 */
int lwm2m_data_decode_payload(qapi_Net_LWM2M_Uri_t * uri,
                              uint8_t *buffer,
                              size_t buffer_len,
                              qapi_Net_LWM2M_Content_Type_t format,
                              qapi_Net_LWM2M_Flat_Data_t **data)
{
  if(uri == NULL || buffer == NULL || data == NULL ) 
  { 
    LOG_INFO(" Passed Argument is NULL");
    return 0;
  }

  switch(format)
  {
    case QAPI_NET_LWM2M_TEXT_PLAIN:
      if((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) == 0)
      {
        return 0;
      }
      *data = data_malloc(1);
      if(*data == NULL)
      {
        return 0;
      }
      (*data)->id = uri->resourceId;
      (*data)->type = QAPI_NET_LWM2M_TYPE_STRING_E;
      return set_buffer(*data, buffer, buffer_len);

    case QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM: // Opaque
      if((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) == 0)
      {
        return 0;
      }
      *data = data_malloc(1);
      if(*data == NULL)
      {
        return 0;
      }
      (*data)->id = uri->resourceId;
      (*data)->type = QAPI_NET_LWM2M_TYPE_OPAQUE_E;
      return set_buffer(*data, buffer, buffer_len);

    case QAPI_NET_LWM2M_M2M_TLV:
      return tlv_parser(buffer, buffer_len, data);

    default:
      return 0;
  }
}

/**
 * @fn size_t lwm2m_data_serialize()
 * @brief This function is used to copy the value from lwm2m_data_t 
 *        strucuture into buffer
 *
 * @param uriP       lwm2m_uri_t pointer
 * @param size       size
 * @param dataP      data lwm2m_data_t strucuture pointer
 * @param formatP    format of the data
 * @param bufferP    buffer, double pointer to the data copied
 *
 * @return    number of bytes copied on success
 *            0 on failure
 */
size_t lwm2m_data_encode_payload(qapi_Net_LWM2M_Uri_t *uri,
                                 int size,
                                 qapi_Net_LWM2M_Flat_Data_t *data,
                                 qapi_Net_LWM2M_Content_Type_t *format,
                                 uint8_t **buffer)
{
  if(uri == NULL || data == NULL || format == NULL || buffer == NULL ) 
  {
    LOG_INFO(" Passed  Argument is NULL");
    return 0;
  }

  // Check format type
  if (*format == QAPI_NET_LWM2M_TEXT_PLAIN || *format == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM)
  {
    if (size != 1 || ((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) == 0)
        || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
        || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
        || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)
    {
      *format = QAPI_NET_LWM2M_M2M_TLV;
    }
  }

  if((*format == QAPI_NET_LWM2M_TEXT_PLAIN) && (data->type == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
  {
    *format = QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM;
  }

  switch (*format)
  {
    case QAPI_NET_LWM2M_TEXT_PLAIN:
      return serialize_text(data, buffer);

    case QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM:
    {
      if((data->type == QAPI_NET_LWM2M_TYPE_INTEGER_E) 
         ||(data->type == QAPI_NET_LWM2M_TYPE_FLOAT_E) 
         || (data->type == QAPI_NET_LWM2M_TYPE_BOOLEAN_E))
      {
        return 0;
      }
      else
      {
        tx_byte_allocate(byte_pool_sensor, (VOID **)&(*buffer),
                         data->value.asBuffer.length, TX_NO_WAIT);
        if(*buffer == NULL)
        {
          return 0;
        }
        memcpy(*buffer, data->value.asBuffer.buffer, data->value.asBuffer.length);
      }
    }
    return data->value.asBuffer.length;

    case QAPI_NET_LWM2M_M2M_TLV:
      {
        uint8_t base_uri_str[LWM2M_URI_MAX_STRING_LEN];
        int uri_len = 0;
        qapi_Net_LWM2M_Uri_Depth_t root_level;
        bool is_resource_instance;

        uri_len = uri_to_string(uri, base_uri_str, LWM2M_URI_MAX_STRING_LEN, &root_level);
        if(uri_len <= 0) return 0;

        if (root_level == URI_DEPTH_RESOURCE_INSTANCE)
        {
          is_resource_instance = true;
        }
        else
        {
          is_resource_instance = false;
        }
        return tlv_encode(is_resource_instance, size, data, buffer);
      }

    default:
      return 0;
  }
}

/*=========================================================================
  FUNCTION:  sen_format_log_msg

===========================================================================*/
/*!
    @brief
    Format debug message for logging.

    @return
    None
*/
/*=========================================================================*/
void sen_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
)
{
  va_list ap;

  /*-----------------------------------------------------------------------*/

  /* validate input param */
  if( NULL == buf_ptr || buf_size <= 0)
  {
    LOG_INFO("sen_format_log_msg: Bad Param buf_ptr:[%p], buf_size:%d",buf_ptr, buf_size);
    return;
  }

  /*-----------------------------------------------------------------------*/

  va_start( ap, fmt );

  vsnprintf( buf_ptr, (size_t)buf_size, fmt, ap );

  va_end( ap );

} /* sen_format_log_msg */


