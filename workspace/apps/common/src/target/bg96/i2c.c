#include "txm_module.h"
#include "qapi_i2c_master.h"
#include "i2c.h"
#include "stdio.h"

typedef struct {
    const i2c_num_t num;
    const uint32_t instance;
    void *handle;
    uint32_t transferred1;
	uint32_t transferred2;
	uint32_t frequency;
	volatile uint32_t lock;
}i2c_map_tbl_t;

i2c_map_tbl_t i2c_map_tbl[I2C_MAX_NO] = {
    {  	I2C1,  QAPI_I2CM_INSTANCE_004_E,      	NULL,	0,	0, I2C_DEFAULT_FREQ, 0},
    {  	I2C2,  QAPI_I2CM_INSTANCE_005_E,      	NULL,	0,	0, I2C_DEFAULT_FREQ, 0}
};

static const struct i2c_list_entry i2c_module_consts[] = {
    { "I2C1",       I2C1 },
    { "I2C2",       I2C2 },
    { NULL, 0 }
};


volatile int num = -1;
static void invoke_i2c_callback_1(const uint32 status, void *param){
	// param not passed correctly...
	int i2c_num = *(int*)param;
	num = i2c_num;
	uint32_t expected;
	//if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){
		while(__atomic_compare_exchange_n(
					&i2c_map_tbl[I2C1].lock,
					&expected,
					0,
					false,
					__ATOMIC_SEQ_CST,
					__ATOMIC_SEQ_CST) != 1);
	//}
}


static void invoke_i2c_callback_2(const uint32 status, void *param){
	// param not passed correctly...
	//int i2c_num = *(int*)param;
	uint32_t expected;
	//if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){
		while(__atomic_compare_exchange_n(
					&i2c_map_tbl[I2C2].lock,
					&expected,
					0,
					false,
					__ATOMIC_SEQ_CST,
					__ATOMIC_SEQ_CST) != 1);
	//}
}


const int param = 1;
int i2c_transfer(i2c_num_t i2c_num, uint16_t addr, uint8_t *tx_data, size_t tx_data_size, 
	uint8_t *rx_data, size_t rx_data_size, uint32_t delay_us){


	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){

	    qapi_I2CM_Config_t config;
	   	qapi_I2CM_Descriptor_t desc[2];

	   	// Configure the bus speed and slave address
		config.bus_Frequency_KHz = i2c_map_tbl[i2c_num].frequency; 
		config.slave_Address     = addr;
		config.SMBUS_Mode        = 0;
		config.slave_Max_Clock_Stretch_Us = 100000;
		config.core_Configuration1 = 0;
		config.core_Configuration2 = 0;

		uint8_t to_send = 0;

		if(tx_data != NULL){
			desc[0].buffer      = tx_data;
			desc[0].length      = tx_data_size;
			desc[0].transferred = (uint32_t)&i2c_map_tbl[i2c_num].transferred1;
			desc[0].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE | QAPI_I2C_FLAG_STOP;
			to_send++;
 		}

		if(rx_data != NULL){
			desc[1].buffer      = rx_data;
			desc[1].length      = rx_data_size;
			desc[1].transferred = (uint32_t)&i2c_map_tbl[i2c_num].transferred2;  
			desc[1].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_READ  | QAPI_I2C_FLAG_STOP;
			to_send++;
		}
		
		if(i2c_num == I2C1){
			if(__atomic_load_n(&i2c_map_tbl[i2c_num].lock,__ATOMIC_SEQ_CST) == 0){
				if(qapi_I2CM_Transfer(i2c_map_tbl[i2c_num].handle, &config, &desc[0], to_send, 
					invoke_i2c_callback_1, &param, delay_us) == QAPI_OK){
					uint32_t expected;
					while(__atomic_compare_exchange_n(
						&i2c_map_tbl[i2c_num].lock,
						&expected,
						1,
						false,
						__ATOMIC_SEQ_CST,
						__ATOMIC_SEQ_CST) != 1)
						;

						printf("num=%d\r\n",num);
						num = -1;
					return 0;
				}

			}else
				return 2;

		}else if(i2c_num == I2C2){
			if(__atomic_load_n(&i2c_map_tbl[i2c_num].lock,__ATOMIC_SEQ_CST) == 0){
				if(qapi_I2CM_Transfer(i2c_map_tbl[i2c_num].handle, &config, &desc[0], to_send, 
					invoke_i2c_callback_2, &i2c_map_tbl[i2c_num].num, delay_us) == QAPI_OK){
					uint32_t expected;
					while(__atomic_compare_exchange_n(
						&i2c_map_tbl[i2c_num].lock,
						&expected,
						1,
						false,
						__ATOMIC_SEQ_CST,
						__ATOMIC_SEQ_CST) != 1);
					return 0;
				}

			}else
				return 2;
		}

	}

	return 1;
}



int i2c_start(i2c_num_t i2c_num){

	qapi_I2CM_Config_t config;
	qapi_I2CM_Descriptor_t desc[1];

	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){

	   	// Configure the bus speed and slave address
		config.bus_Frequency_KHz = i2c_map_tbl[i2c_num].frequency; 
		config.slave_Address     = 0;
		config.SMBUS_Mode        = 0;
		config.slave_Max_Clock_Stretch_Us = 100000;
		config.core_Configuration1 = 0;
		config.core_Configuration2 = 0;

		desc[0].buffer      = NULL;
		desc[0].length      = 0;
		desc[0].transferred = 0; // (uint32)&transferred1;
		desc[0].flags       = QAPI_I2C_FLAG_START;

		return qapi_I2CM_Transfer(i2c_map_tbl[i2c_num].handle, &config, &desc[0], 1, NULL, &i2c_map_tbl[i2c_num].num, 0);

	}

	return 1;
}

int i2c_stop(i2c_num_t i2c_num){

    qapi_I2CM_Config_t config;
   	qapi_I2CM_Descriptor_t desc[2];

	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){

	   	// Configure the bus speed and slave address
		config.bus_Frequency_KHz = i2c_map_tbl[i2c_num].frequency; 
		config.slave_Address     = 0;
		config.SMBUS_Mode        = 0;
		config.slave_Max_Clock_Stretch_Us = 100000;
		config.core_Configuration1 = 0;
		config.core_Configuration2 = 0;

		desc[0].buffer      = NULL;
		desc[0].length      = 0;
		desc[0].transferred = 0; // (uint32)&transferred1;
		desc[0].flags       = QAPI_I2C_FLAG_STOP;

		return qapi_I2CM_Transfer(i2c_map_tbl[i2c_num].handle, &config, &desc[0], 1, NULL, &i2c_map_tbl[i2c_num].num, 0);
	
	}

	return 1;
}


int i2c_config(i2c_num_t i2c_num){

	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){
		if(qapi_I2CM_Open(i2c_map_tbl[i2c_num].instance, &i2c_map_tbl[i2c_num].handle) == QAPI_OK 
			&& !i2c_map_tbl[i2c_num].lock){
	   		return qapi_I2CM_Power_On(i2c_map_tbl[i2c_num].handle);
	    }
	}

	return 1;
}

int i2c_deconfig(i2c_num_t i2c_num){
	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){
		if(qapi_I2CM_Close (i2c_map_tbl[i2c_num].handle) == QAPI_OK)
			return qapi_I2CM_Power_Off(i2c_map_tbl[i2c_num].handle);
	}

	return 1;
}

int i2c_set_frequency(i2c_num_t i2c_num, uint32_t frequency){
	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO){
		i2c_map_tbl[i2c_num].frequency = frequency;
		return 0;
	}
	return 1;
}

int i2c_power_down(i2c_num_t i2c_num){
	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO)
		return qapi_I2CM_Power_Off(i2c_map_tbl[i2c_num].handle);

	return 1;
}

int i2c_power_up(i2c_num_t i2c_num){
	if(i2c_num >= 0 && i2c_num < I2C_MAX_NO)
		return qapi_I2CM_Power_On(i2c_map_tbl[i2c_num].handle);
	
	return 1;
}