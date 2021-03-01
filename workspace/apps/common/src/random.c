/*
*
* @author :  Nikolas Karakotas
* @date   :  01/02/2021
*
*/


#include <stdio.h>
#include <qapi.h>
#include <txm_module.h>
#include <qapi_quectel.h>


void randombytes(uint8_t *buf, uint64_t size ){

	qapi_QT_Random_Data_Get(size,buf);

}