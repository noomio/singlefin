/*
*
* @author :  Nikolas Karakotas
* @date   :  16/01/2021
*
*/
#include <stdio.h> 
#include "txm_module.h"
#include "qapi_fs_types.h"
#include "qapi_fs.h"
#include "qapi_spi_master.h"
#include "spi.h"


typedef struct {
    const spi_num_t 	num;
    const uint32_t 		instance;
    void*				handle;
    spi_cs_mode_t		cs_mode;
    spi_cs_polarity_t 	cs_polarity;
	spi_endian_t		endian;
	spi_mode_t 			mode;
	spi_bits_per_word_t	bpw;
	uint32_t 			frequency;
	uint8_t				cs_delay;
	uint8_t				inter_word_delay;
	uint8_t				slaves;					// max 7
	bool 				loopback;
	volatile uint32_t   status;
	volatile uint32_t 	lock;
	volatile uint32_t 	signal;
	bool				configured;
}spi_map_tbl_t;

spi_map_tbl_t spi_map_tbl[SPI_MAX_NO] = {
    {  	SPI1,  QAPI_SPIM_INSTANCE_6_E,	NULL,	SPI_CS_KEEP_ASSERTED,	SPI_CS_ACTIVE_LOW, SPI_BYTE_ORDER_LITTLE_ENDIAN,	SPI_MODE_0,	SPI_BPW_8,	SPI_DEFAULT_FREQ, 0, 0, 0, false, 0, 0, 0, 0},
    {  	SPI2,  QAPI_SPIM_INSTANCE_5_E,  NULL,	SPI_CS_KEEP_ASSERTED,	SPI_CS_ACTIVE_LOW, SPI_BYTE_ORDER_LITTLE_ENDIAN,	SPI_MODE_0, SPI_BPW_8,	SPI_DEFAULT_FREQ, 0, 0, 0, false, 0, 0, 0, 0}
};

static const struct spi_list_entry spi_module_consts[] = {
    { "SPI1",       SPI1 },
    { "SPI2",       SPI2 },
    { NULL, 0 }
};


void spi_cb_func(uint32 status, void *cb_para){

	int spi_num = *((int*)cb_para);
	uint32_t expected;

	if(spi_num >= SPI1 && spi_num <=SPI2 ){

		spi_map_tbl[spi_num].status = status; // no need for atomic
	    
	    if (QAPI_SPI_COMPLETE == status){
	    	while(__atomic_compare_exchange_n(
						&spi_map_tbl[spi_num].signal,
						&expected,
						1,
						false,
						__ATOMIC_SEQ_CST,
						__ATOMIC_SEQ_CST) != 1);
	    	

	    }else if (QAPI_SPI_QUEUED == status || QAPI_SPI_IN_PROGRESS == status){

	    }else{
	    	while(__atomic_compare_exchange_n(
						&spi_map_tbl[spi_num].signal,
						&expected,
						1,
						false,
						__ATOMIC_SEQ_CST,
						__ATOMIC_SEQ_CST) != 1);
	    }
	}

}


int spi_power_down(spi_num_t spi_num){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		return qapi_SPIM_Power_Off(spi_map_tbl[spi_num].handle);
	}

	return 1;
}

int spi_power_up(spi_num_t spi_num){

	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		return qapi_SPIM_Power_On(spi_map_tbl[spi_num].handle);
	}

	return 1;
}

int spi_config(spi_num_t spi_num){

	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		if(qapi_SPIM_Open(spi_map_tbl[spi_num].instance, &spi_map_tbl[spi_num].handle) == QAPI_OK 
			&& !spi_map_tbl[spi_num].lock){
			if(qapi_SPIM_Power_On(spi_map_tbl[spi_num].handle) == QAPI_OK){
				spi_map_tbl[spi_num].configured = true;
				return 0;
			}
		}
	}

	return 1;

}

int spi_deconfig(spi_num_t spi_num){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		if(qapi_SPIM_Close(spi_map_tbl[spi_num].handle) == QAPI_OK){
			if(qapi_SPIM_Power_Off(spi_map_tbl[spi_num].handle) == QAPI_OK){
				spi_map_tbl[spi_num].configured = false;
				return 0;
			}
		}
	}

	return 1;
}

int spi_set_frequency(spi_num_t spi_num, uint32_t frequency){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].frequency = frequency;
		return 0;
	}

	return 1;
}

int spi_set_endianess(spi_num_t spi_num, spi_endian_t endian){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].endian = endian;
		return 0;
	}

	return 1;
}

int spi_set_bit_per_word(spi_num_t spi_num, spi_bits_per_word_t bpw){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].bpw = bpw;
		return 0;
	}

	return 1;
}

int spi_set_mode(spi_num_t spi_num, spi_mode_t mode){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].mode = mode;
		return 0;
	}

	return 1;
}

int spi_set_inter_word_delay(spi_num_t spi_num, uint8_t delay){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].inter_word_delay = delay;
		return 0;
	}

	return 1;
}

int spi_set_loopback(spi_num_t spi_num, bool enable){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].loopback = enable;
		return 0;
	}

	return 1;
}

int spi_set_chip_select_mode(spi_num_t spi_num, spi_cs_mode_t mode){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].cs_mode = mode;
		return 0;
	}

	return 1;
}

int spi_set_chip_select_polarity(spi_num_t spi_num, spi_cs_polarity_t polarity){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].cs_polarity = polarity;
		return 0;
	}

	return 1;
}

int spi_set_chip_select_delay(spi_num_t spi_num, uint8_t delay){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].cs_delay = delay;
		return 0;
	}

	return 1;
}

int spi_set_slaves_num(spi_num_t spi_num, uint8_t num){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){
		spi_map_tbl[spi_num].slaves = num;
		return 0;
	}

	return 1;
}

int spi_send(spi_num_t spi_num, uint8_t *tx_buf, size_t len){
	return spi_send_receive(spi_num, tx_buf, NULL, len);
}

int spi_send_receive(spi_num_t spi_num, uint8_t *tx_buf, uint8_t *rx_buf, size_t len){
	if((spi_num >= 0 && spi_num < SPI_MAX_NO) && spi_map_tbl[spi_num].configured){
		
		qapi_SPIM_Config_t config;
		config.SPIM_Mode = (qapi_SPIM_Shift_Mode_t)spi_map_tbl[spi_num].mode;
		config.SPIM_CS_Polarity = (qapi_SPIM_CS_Polarity_t)spi_map_tbl[spi_num].cs_polarity;
		config.SPIM_endianness = (qapi_SPIM_Byte_Order_t)spi_map_tbl[spi_num].endian;
		config.SPIM_Bits_Per_Word = spi_map_tbl[spi_num].bpw;
		config.SPIM_Slave_Index = spi_map_tbl[spi_num].slaves;
		config.Clk_Freq_Hz = spi_map_tbl[spi_num].frequency;
		config.CS_Clk_Delay_Cycles = spi_map_tbl[spi_num].cs_delay;
		config.Inter_Word_Delay_Cycles = spi_map_tbl[spi_num].inter_word_delay;
		config.SPIM_CS_Mode = (qapi_SPIM_CS_Mode_t)spi_map_tbl[spi_num].cs_mode;
		config.loopback_Mode = (qbool_t)spi_map_tbl[spi_num].loopback;



		qapi_SPIM_Descriptor_t desc;
		desc.tx_buf = tx_buf;
		desc.rx_buf = rx_buf;
		desc.len = len;
	


		//check for lock
		if(__atomic_load_n(&spi_map_tbl[spi_num].lock,__ATOMIC_SEQ_CST) == 0){
			// lock
			spi_map_tbl[spi_num].status = -1;
			uint32_t expected;
			while(__atomic_compare_exchange_n(
				&spi_map_tbl[spi_num].lock,
				&expected,
				1,
				false,
				__ATOMIC_SEQ_CST,
				__ATOMIC_SEQ_CST) != 1);

			if(qapi_SPIM_Full_Duplex(spi_map_tbl[spi_num].handle, &config, &desc, 1, spi_cb_func, &spi_map_tbl[spi_num].num, false) == QAPI_OK){
				//wait for signal
				do{
					tx_thread_sleep(5);
				}while(__atomic_load_n(&spi_map_tbl[spi_num].signal,__ATOMIC_SEQ_CST) == 0);
				//free signal
				while(__atomic_compare_exchange_n(
					&spi_map_tbl[spi_num].signal,
					&expected,
					0,
					false,
					__ATOMIC_SEQ_CST,
					__ATOMIC_SEQ_CST) != 1);

			}

			//free lock
			while(__atomic_compare_exchange_n(
				&spi_map_tbl[spi_num].lock,
				&expected,
				0,
				false,
				__ATOMIC_SEQ_CST,
				__ATOMIC_SEQ_CST) != 1);

			int status = (spi_map_tbl[spi_num].status == QAPI_SPI_COMPLETE) ?  0 : spi_map_tbl[spi_num].status;
			return status;

		}


	}

	return -1;
}

void spi_config_dump(spi_num_t spi_num){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO){

		printf("%s:\r\n"
			"Mode: %u\r\n"
			"CS Polarity: %u\r\n"
			"CS Delay: %u\r\n"
			"CS Mode: %u\r\n"
			"Endian: %u\r\n"
			"Bits per Word: %u\r\n"
			"Slaves: %u\r\n"
			"Frequency Hz: %u\r\n"
			"Inter Word Delay: %u\r\n"
			"Loopback: %u\r\n",
			spi_module_consts[spi_num].key,
			spi_map_tbl[spi_num].mode,
			spi_map_tbl[spi_num].cs_polarity,
			spi_map_tbl[spi_num].cs_delay,
			spi_map_tbl[spi_num].cs_mode,
			spi_map_tbl[spi_num].endian,
			spi_map_tbl[spi_num].bpw,
			spi_map_tbl[spi_num].slaves,
			spi_map_tbl[spi_num].frequency,			
			spi_map_tbl[spi_num].inter_word_delay,			
			spi_map_tbl[spi_num].loopback);
	}else{
		puts("NULL\r\n");
	}
}

const char *spi_get_name(spi_num_t spi_num){
	if(spi_num >= 0 && spi_num < SPI_MAX_NO)
		return spi_module_consts[spi_num].key;
	else
		return NULL;
}