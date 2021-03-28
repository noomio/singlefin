/*
*
* @author :  Nikolas Karakotas
* @date   :  01/02/2021
*
*/


#include "fin_internal.h"


void hwrandombytes(uint8_t *buf, uint64_t size ){

	qapi_QT_Random_Data_Get(size,buf);

}