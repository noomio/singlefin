#ifndef __QAPI_SPI_MASTER_H__
#define __QAPI_SPI_MASTER_H__
/*=================================================================================
 *
 *     SERIAL PERIPHERAL INTERFACE MASTER
 *
 *===============================================================================*/
 /** @file qapi_spi_master.h
 *
 *
 * @addtogroup qapi_spi_master
 * @{
 *
 * @details The serial peripheral interface (SPI) is a full duplex communication
 * bus to interface peripherals in several communication modes as configured by
 * the client software. The SPI driver API provides a high-level interface to
 * expose the capabilities of the SPI master.
 *
 * Typical usage:
 * - qapi_SPIM_Open() -- Get a handle to an SPI instance.
 * - qapi_SPIM_Power_On() -- Turn on all resources required for a successful SPI transaction.
 * - qapi_SPIM_Full_Duplex() -- Generic transfer API to perform a transfer on the SPI bus.
 * - qapi_SPIM_Power_Off() -- Turn off all resources set by qapi_SPIM_Power_On().
 * - qapi_SPIM_Close() -- Destroy all objects created by the SPI handle.
 * 
 * A note about SPI power: \n
 * Calling qapi_SPIM_Open() and leaving it open does not drain any power. 
 * If the client is expecting to do several back-to-back SPI transfers, the recommended
 * approach is to call Power_On, perform all transfers, then call Power_Off. Calling 
 * Power_On/Power_Off for every transfer will affect throughput and increase the bus idle period.
 *
 * SPI transfers: \n
 * SPI transfers use BAM (DMA mode), so we expect buffers passed by the client to
 * be uncached RAM addresses. There is no address or length alignment requirement.
 *
 * SPI modes: \n
 * The SPI master supports all four SPI modes, and this can be changed per transfer.
 * See #qapi_SPIM_Config_t for configuration specification details.
 * The driver supports parallel transfers on different SPI instances.
 * 
 * A note about SPI modes: \n
 * Always check the meaning of SPI modes in your 
 * SPI slave specifications. Some manufacturers use different mode meanings.
 * - SPI Mode 0: CPOL = 0, and CPHA = 0
 * - SPI Mode 1: CPOL = 0, and CPHA = 1
 * - SPI Mode 2: CPOL = 1, and CPHA = 0
 * - SPI Mode 3: CPOL = 1, and CPHA = 1
 *
 * @}
 @code
   
 // 
 // The code sample below demonstrates the use of this interface.
 //

qapi_Status_t spi_test(void)
{
	uint8_t *rx_Buffer;
	int i;
	spi_mode_select();
	spi_mode_screen:
	tmp1=0;
	while(flag == 1) {
		status = tx_event_flags_get(event_flags_0, TEST_RECEIVE_EVENT_MASK, TX_OR, &set_signal, TX_WAIT_FOREVER );
		if ((status != TX_SUCCESS) )
			Test_CONSOLE_WRITE_STRING_LITERAL("Failed tx_event_flags_get_in_I2c_Test");
		status = tx_event_flags_set(event_flags_0, ~TEST_RECEIVE_EVENT_MASK, TX_AND );
		if ((status != TX_SUCCESS) )
			Test_CONSOLE_WRITE_STRING_LITERAL("Failed tx_event_flags_set_in_I2c_Test");
	}
	
    //spiIp.SPIM_Mode = QAPI_SPIM_MODE_0_E;
	SPI_config.SPIM_CS_Polarity = QAPI_SPIM_CS_ACTIVE_LOW_E;
	SPI_config.SPIM_endianness = SPI_BIG_ENDIAN;
	SPI_config.SPIM_Bits_Per_Word = 8;
	SPI_config.SPIM_Slave_Index = 0;
	SPI_config.Clk_Freq_Hz = 10000000;
	SPI_config.CS_Clk_Delay_Cycles = 0;
	SPI_config.Inter_Word_Delay_Cycles = 0;
	SPI_config.SPIM_CS_Mode = QAPI_SPIM_CS_KEEP_ASSERTED_E;
	SPI_config.loopback_Mode = 1;
	//des.len = 32;
	//des.tx_buf = writeBuf;
	//des.rx_buf = readBuf;
	
	tx_byte_pool_create(byte_spi,"i2c_tx_buf", free_memory1,9120);
	tx_byte_allocate(byte_spi , (void **)&rx_Buffer, 8 , TX_NO_WAIT);
	
	spi_status = txm_module_object_allocate(&spi_event_flags_0, sizeof(TX_EVENT_FLAGS_GROUP));
	spi_status = tx_event_flags_create(spi_event_flags_0, "test_sig");
	
	spi_status =  qapi_SPIM_Open(QAPI_SPIM_INSTANCE_1_E, &spi_Handle);
	spi_status =  qapi_SPIM_Power_On(spi_Handle);
	
	spi_desc[0].tx_buf = spi_tx_buf;
	spi_desc[0].rx_buf = spi_rx_buf;
	spi_desc[0].len = 4;
	
	spi_status = qapi_SPIM_Full_Duplex(spi_Handle, &SPI_config, &spi_desc[0], 1, (qapi_SPIM_Callback_Fn_t)spi_client_callback, (void *)0x12,NULL);
	
	for(i=0; i<4; i++){
		if(spi_tx_buf[i]!=spi_rx_buf[i]){
			Test_CONSOLE_WRITE_STRING_LITERAL(Test_OUTPUT_END_OF_LINE_STRING);
			Test_CONSOLE_WRITE_STRING_LITERAL("  SPI Test Completed  ");
			return -1;
		}	
	}
		
	return spi_status;
}

 @endcode
 @}
*/

/*==================================================================================
       Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
                     All Rights Reserved.
       Confidential and Proprietary - Qualcomm Technologies, Inc.
==================================================================================*/ 

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/5.0/buses/api/spi/qapi_spi_master.h#4 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------------- 
01/24/2018   vg     Added Doxygen comments and markup.
09/11/2017   vg     Added spi DAM changes

07/02/2017   rc     Added changes for Doxygen generation. 
04/24/2017   leo    (Tech Comm) Edited/added Doxygen comments and markup.
04/14/2017   ska    Fixed compilation issues for user space
02/19/2017   ska    Fixed compilation issues and added QAPI specific error codes
12/14/2016   rc     Initial version
==================================================================================*/

/*==================================================================================

                               INCLUDE FILES

==================================================================================*/

/*==================================================================================

                                   MACROS

==================================================================================*/


#include "qapi_status.h"
#include "qapi_spi_handler.h"

#define TXM_QAPI_BUSES_SPI_BASE      (TXM_QAPI_BUSES_BASE + 30)
#define TXM_QAPI_BUSES_SPI_CB_BASE   (CUSTOM_CB_QAPI_BUSES_BASE + 10)

#define TXM_QAPI_SPI_OPEN        TXM_QAPI_BUSES_SPI_BASE
#define TXM_QAPI_SPI_FULL_DUPLEX TXM_QAPI_BUSES_SPI_BASE + 1
#define TXM_QAPI_SPI_POWER_ON    TXM_QAPI_BUSES_SPI_BASE + 2
#define TXM_QAPI_SPI_POWER_OFF   TXM_QAPI_BUSES_SPI_BASE + 3
#define TXM_QAPI_SPI_CLOSE       TXM_QAPI_BUSES_SPI_BASE + 4
#define TXM_QAPI_SPI_CB_NOTIFY   TXM_QAPI_BUSES_SPI_CB_BASE

/*
 * Error codes returned by the SPI Master API.
 */
#define QAPI_SPIM_ERROR                         __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 1)
#define QAPI_SPIM_ERROR_INVALID_PARAM           __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 2)
#define QAPI_SPIM_ERROR_MEM_ALLOC               __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 3)
#define QAPI_SPIM_ERROR_TRANSFER_CONFIG_FAIL    __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 4)
#define QAPI_SPIM_ERROR_TRANSFER_FAILURE        __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 5)
#define QAPI_SPIM_ERROR_OPEN_FAILURE            __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 6)
#define QAPI_SPIM_ERROR_CLOSE_FAILURE   	    __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 7)
#define QAPI_SPIM_ERROR_POWER_ON_FAILURE        __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 8)
#define QAPI_SPIM_ERROR_POWER_OFF_FAILURE       __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 9)
#define QAPI_SPIM_ERROR_INVALID_BUFFER		    __QAPI_ERROR(QAPI_MOD_BSP_SPI_MASTER, 10)

//QuectelModifyFlag
/*this is used in spi transfer callback function*/
typedef enum SpiTransferStatus SpiTransferStatus;
enum SpiTransferStatus
{
   QAPI_SPI_QUEUED = 1,/**< The transaction is queued, will be processed by the core in order in which it arrived.*/
   QAPI_SPI_COMPLETE,/**< The transaction is complete.*/
   QAPI_SPI_IN_PROGRESS,/**< The core is currently processing the transfer, this only returned 
					 for DalSpi_GetTransferStatus API.*/
   QAPI_SPI_FAILURE,/**< There was an error while the SPI core processed the transfer.*/
   QAPI_SPI_BUSY,/**< If queueIfBusy flag is set as false, then if the core is currently processing a transaction, 
			     this value is returned, and the transfer is not  queued. The client has to retry the call.*/
   QAPI_SPI_FULL,/**< The internal queueing buffer is full, and the transaction cannot be queued. 
			  The client has to retry the call.*/
   QAPI_SPI_INVALID_ID,/**< An incorrect transfer id was provided while requesting for the status of a transfer.*/
   QAPI_SPI_INVALID_PARAMETER/**< Spi transfer parameter provided by the Client  was invalid.*/
};



/** @addtogroup qapi_spi_master
@{ */

/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/
/**
* SPI callback function type.
*
* This type is used by the client to register its callback notification function. The 
* callback_Ctxt is the context object that will be passed untouched by the SPI Master
* driver to help the client identify which transfer completion instance is being signaled.
*/
typedef void (*qapi_SPIM_Callback_Fn_t) (uint32_t status, void *callback_Ctxt);

/**
* SPI instance enumeration.
*
* This enumeration lists the possible SPI instance indicating which HW SPI master 
* is to be used for the current SPI transaction.
*/

typedef enum
{
    QAPI_SPIM_INSTANCE_1_E = 1, /**< SPIM instance 1. */
    QAPI_SPIM_INSTANCE_2_E,     /**< SPIM instance 2. */
    QAPI_SPIM_INSTANCE_3_E,     /**< SPIM instance 3. */
    QAPI_SPIM_INSTANCE_4_E,     /**< SPIM instance 4. */
    QAPI_SPIM_INSTANCE_5_E,     /**< SPIM instance 5. */
    QAPI_SPIM_INSTANCE_6_E,     /**< SPIM instance 6. */
    QAPI_SPIM_INSTANCE_7_E,     /**< SPIM instance 7. */
    QAPI_SPIM_INSTANCE_8_E,     /**< SPIM instance 8. */
    QAPI_SPIM_INSTANCE_9_E,     /**< SPIM instance 9. */
    QAPI_SPIM_INSTANCE_10_E,    /**< SPIM instance 10. */
    QAPI_SPIM_INSTANCE_11_E,    /**< SPIM instance 11. */
    QAPI_SPIM_INSTANCE_12_E,    /**< SPIM instance 12. */
    QAPI_SPIM_INSTANCE_13_E,    /**< SPIM instance 13. */
    QAPI_SPIM_INSTANCE_14_E,    /**< SPIM instance 14. */
    QAPI_SPIM_INSTANCE_15_E,    /**< SPIM instance 15. */
    QAPI_SPIM_INSTANCE_16_E,    /**< SPIM instance 16. */
    QAPI_SPIM_INSTANCE_17_E,    /**< SPIM instance 17. */
    QAPI_SPIM_INSTANCE_18_E,    /**< SPIM instance 18. */
    QAPI_SPIM_INSTANCE_19_E,    /**< SPIM instance 19. */
    QAPI_SPIM_INSTANCE_20_E,    /**< SPIM instance 20. */
    QAPI_SPIM_INSTANCE_21_E,    /**< SPIM instance 21. */
    QAPI_SPIM_INSTANCE_22_E,    /**< SPIM instance 22. */
    QAPI_SPIM_INSTANCE_23_E,    /**< SPIM instance 23. */
    QAPI_SPIM_INSTANCE_24_E,    /**< SPIM instance 24. */
    QAPI_SPIM_INSTANCE_MAX_E,
} qapi_SPIM_Instance_t;

/**
* SPI phase type.
*
* This type defines the clock phase that the client can set in the 
* SPI configuration.
*/
typedef enum 
{
   QAPI_SPIM_MODE_0_E, /**< CPOL = 0, CPHA = 0. */
   QAPI_SPIM_MODE_1_E, /**< CPOL = 0, CPHA = 1. */
   QAPI_SPIM_MODE_2_E,  /**< CPOL = 1, CPHA = 0. */
   QAPI_SPIM_MODE_3_E,  /**< CPOL = 1, CPHA = 1. */
   QAPI_SPIM_MODE_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_Shift_Mode_t;
 
/** SPI chip select ppolarity type.
*/
typedef enum 
{
   QAPI_SPIM_CS_ACTIVE_LOW_E,   /**< During Idle state, the CS line is held low. */
   QAPI_SPIM_CS_ACTIVE_HIGH_E,  /**< During Idle state, the CS line is held high. */
   QAPI_SPIM_CS_ACTIVE_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_CS_Polarity_t;

/*Note - Endian ness - it is unclear  whether we can 
 * support this in all (FIFO/GSI) modes ! */

/** Order in which bytes from Tx/Rx buffer words are put on the bus.
 */
typedef enum
{
    SPI_NATIVE = 0,         /**< Native. */
    SPI_LITTLE_ENDIAN = 0,  /**< Little Endian. */
    SPI_BIG_ENDIAN          /**< Big Endian (network). */
}qapi_SPIM_Byte_Order_t;

/**
* SPI chip select assertion type.
*
* This type defines how the chip select line is configured between N word cycles.
*/
typedef enum 
{
   QAPI_SPIM_CS_DEASSERT_E,/**< CS is deasserted after transferring data for N clock cycles. */
   QAPI_SPIM_CS_KEEP_ASSERTED_E,/**< CS is asserted as long as the core is in the Run state. */
   QAPI_SPIM_CS_MODE_INVALID_E = 0x7FFFFFFF
}qapi_SPIM_CS_Mode_t;

/**
* SPI master configuration.
*
* The SPI master configuration is the collection of settings specified for each SPI
* transfer call to select the various possible SPI transfer parameters.
*/
typedef struct
{
   /* Add note to timing diagram here.*/
   qapi_SPIM_Shift_Mode_t SPIM_Mode;
   /**< SPIM mode type to be used for the SPI core. */
   qapi_SPIM_CS_Polarity_t SPIM_CS_Polarity;
   /**< CS polarity type to be used for the SPI core. */
   qapi_SPIM_Byte_Order_t SPIM_endianness;
   /** Endian-ness type used for the SPI transfer. */
   uint8_t SPIM_Bits_Per_Word;
   /**< SPI bits per word; any value from 3 to 31. */
   uint8_t SPIM_Slave_Index;
   /**< Slave index, beginning at 0 if mulitple SPI devices are connected to the same master. 
   At most 7 slaves are allowed. If an invalid number (7 or higher) is set, the CS 
   signal will not be used. */
   uint32_t Clk_Freq_Hz;
   /**< Host sets the SPI clock frequency closest to the requested frequency. */
   uint32_t CS_Clk_Delay_Cycles; //QuectelModifyFlag
   /**< Number of clock cycles to wait after asserting CS before starting transfer. */
   uint32_t Inter_Word_Delay_Cycles; //QuectelModifyFlag
   /**< Number of clock cycles to wait between SPI words. */
   qapi_SPIM_CS_Mode_t SPIM_CS_Mode;
   /**< CS mode to be used for the SPI core. */
   qbool_t loopback_Mode;
   /**< Normally 0. If set, the SPI controller will enable Loopback mode; used primarily for testing. */
} qapi_SPIM_Config_t;

/**
* SPI transfer type.
*
* This type specifies the address and length of the buffer for an SPI transaction.
*/
typedef struct
{
   uint8_t *tx_buf;
   /**< Buffer address for transmitting data. */
   uint8_t *rx_buf;
   /**< Buffer address for receiving data. */
   uint32_t len; 
   /**< Size in bytes. No alignment requirements; the arbitrary length data can be transferred. */
}qapi_SPIM_Descriptor_t;

/** @} */ /* end_addtogroup qapi_spi_master */

#ifdef QAPI_TXM_SOURCE_CODE      // KERNEL_MODE_DEFS
/*==================================================================================

                            FUNCTION DECLARATIONS

==================================================================================*/

/** @addtogroup qapi_spi_master
@{ */

/*==================================================================================
  FUNCTION      qapi_SPIM_Open
==================================================================================*/
/**
*  Initializes the SPI Master.
*
*  This function initializes internal data structures along with associated
*  static data. In any operating mode, this function should be called before
*  calling any other SPI master API.
*
*  @param[in]  instance    SPI instance specified by #qapi_SPIM_Instance_t.
*  @param[out] spi_Handle  Pointer to a location in which to store the driver handle.
*
*  @return
*  - QAPI_OK                           -- Module initialized successfully.
*  - QAPI_SPIM_ERROR_INVALID_PARAM     -- Invalid instance or handle parameter.
*  - QAPI_SPIM_ERROR_OPEN_FAILURE      -- Failed to Open SPIM.
*/
qapi_Status_t qapi_SPIM_Open (qapi_SPIM_Instance_t instance, void **spi_Handle);

/*==================================================================================
  FUNCTION      qapi_SPIM_Power_On
==================================================================================*/
/**
*  Enables the SPI Hardware resources for an SPI transaction.
*
*  This function enables all resources required for a successful SPI transaction. This 
*  includes clocks, power resources and pin multiplex functions. This function should 
*  be called before a transfer or a batch of SPI transfers.
* 
*  @param[in] spi_Handle  Driver handle returned by qapi_SPIM_Open().
*
*  @return
*  - QAPI_OK                             -- SPI master enabled successfully.
*  - QAPI_SPIM_ERROR_INVALID_PARAM       -- Invalid handle parameter.
*  - QAPI_SPIM_ERROR_POWER_ON_FAILURE    -- Failure in Power On.
*
*  @dependencies
*  - Before calling this API qapi_SPIM_Open() API must be called.
*/
qapi_Status_t qapi_SPIM_Power_On (void *spi_Handle);

/*==================================================================================
  FUNCTION      qapi_SPIM_Power_Off
==================================================================================*/
/**
*  Disables the SPI Hardware resources for an SPI transaction.
*
*  This function turns off all resources used by the SPI master. This includes clocks, 
*  power resources, and GPIOs. This function should be called to put the SPI master
*  in its lowest possible power state.
* 
*  @param[in] spi_Handle  Driver handle returned by qapi_SPIM_Open().
*
*  @return
*  - QAPI_OK                             -- SPI master disabled successfully.
*  - QAPI_SPIM_ERROR_INVALID_PARAM       -- Invalid handle parameter.
*  - QAPI_SPIM_ERROR_POWER_OFF_FAILURE   -- Failure in Power Off.
*
*  @dependencies
*  - Before calling this API qapi_SPIM_Open() and qapi_SPIM_Power_On() API must be called.
*/
qapi_Status_t qapi_SPIM_Power_Off (void *spi_Handle);

/*==================================================================================
  FUNCTION      qapi_SPIM_Full_Duplex
==================================================================================*/
/**
*  Performs a data transfer over the SPI bus.
*
*  This function performs an asynchronous transfer over the SPI bus. Transfers can be 
*  one-directional or bi-directional. A callback is generated upon transfer completion.
* 
*  @param[in] spi_Handle  Driver handle returned by qapi_SPIM_Open().
*  @param[in] config      Pointer to the SPI configuration structure described by
                          #qapi_SPIM_Config_t.
*  @param[in] desc        Pointer to the structure described by
                          #qapi_SPIM_Descriptor_t. The descriptor can have NULL Tx OR
                          Rx buffers if a half duplex transfer is selected.
*  @param[in] num_Descriptors  Number of descriptor pointers the client wants to process.
*  @param[in] c_Fn        Callback function to be invoked when the SPI transfer 
                          completes succesfully or with an error.
*  @param[in] c_Ctxt      Pointer to a client object that will be returned as an
                          argument to c_Fn.
*  @param[in] get_timestamp Boolean variable to indicate whether a tranasaction timestamp needs to be provided;
						  this is not supported for the QUPv2 version.
*  @return
*  - QAPI_OK                             -- SPI master was enabled successfully.
*  - QAPI_SPIM_ERROR_INVALID_PARAM       -- One or more invalid parameters.
*  - QAPI_SPIM_ERROR_TRANSFER_CONFIG_FAIL-- Failure in Transfer Configuration.
*  - QAPI_SPIM_ERROR_TRANSFER_FAILURE    -- Failure in Transfer.
*
*  @dependencies
*  - Before calling this API qapi_SPIM_Open() and qapi_SPIM_Power_On() API must be called.
*/
qapi_Status_t qapi_SPIM_Full_Duplex (void *spi_Handle, qapi_SPIM_Config_t *config, 
                                     qapi_SPIM_Descriptor_t *desc, uint32_t num_Descriptors,
									 qapi_SPIM_Callback_Fn_t c_Fn, void *c_Ctxt, qbool_t get_timestamp);

/*==================================================================================
  FUNCTION      qapi_SPIM_Close
==================================================================================*/
/**
*  Closes the SPI master.
*
*  This function frees all internal data structures and closes the SPI master interface.
*  The handle returned by qapi_SPIM_Open() is then rendered invalid.
*
*  @param[in] spi_handle  Driver handle returned by qapi_SPIM_Open().
*
*  @return
*  - QAPI_OK                             -- SPI driver closed successfully.
*  - QAPI_SPIM_ERROR_INVALID_PARAM       -- One or more invalid parameters.
*
*  @dependencies
*  - Before calling this API qapi_SPIM_Open() and qapi_SPIM_Power_On() API must be called.
*/
qapi_Status_t qapi_SPIM_Close (void *spi_handle);

/** @} */ /* end_addtogroup qapi_spi_master */

#elif defined  QAPI_TXM_MODULE     // USER_MODE_DEFS
#define qapi_SPIM_Open(instance, spi_Handle)  \
                                                                    (UINT) _txm_module_system_call4( \
                                                                    TXM_QAPI_SPI_OPEN, \
																	(ULONG) instance, \
																	(ULONG) spi_Handle,(ULONG) 0, (ULONG) 0)
#define qapi_SPIM_Power_On(spi_Handle)       \
                                                                    (UINT) _txm_module_system_call4( \
                                                                    TXM_QAPI_SPI_POWER_ON, \
                                                                    (ULONG) spi_Handle,(ULONG) 0, (ULONG) 0, \
                                                                    (ULONG) 0)	
#define qapi_SPIM_Power_Off(spi_Handle)      \
                                                                    (UINT) _txm_module_system_call4( \
                                                                    TXM_QAPI_SPI_POWER_OFF, \
                                                                    (ULONG) spi_Handle,(ULONG) 0,(ULONG) 0, \
                                                                    (ULONG) 0)
#define qapi_SPIM_Full_Duplex(spi_Handle, config, desc, \
                               num_Descriptors, c_Fn, c_Ctxt, \
                               get_timestamp) \
                                                                    (UINT) _txm_module_system_call8( \
                                                                    TXM_QAPI_SPI_FULL_DUPLEX, \
                                                                    (ULONG)spi_Handle, (ULONG)config, \
                                                                    (ULONG)desc, (ULONG) num_Descriptors, \
									                                (ULONG) c_Fn, (ULONG)c_Ctxt, (ULONG) get_timestamp, \
																	(ULONG) qapi_spi_cb_uspace_dispatcher )																		
//QuectelModifyFlag
#define qapi_SPIM_Close(spi_Handle)          \
                                                                   (UINT) _txm_module_system_call4( \
                                                                    TXM_QAPI_SPI_CLOSE, \
                                                                    (ULONG) spi_Handle, (ULONG) 0,(ULONG) 0, \
                                                                    (ULONG) 0)	


//add static to avoid redefinition
static int qapi_spi_cb_uspace_dispatcher(UINT cb_id, void *app_cb,
									UINT cb_param1, UINT cb_param2,
									UINT cb_param3, UINT cb_param4,
									UINT cb_param5, UINT cb_param6,
									UINT cb_param7, UINT cb_param8)
{
        qapi_SPIM_Callback_Fn_t (*pfn_app_cb1) (uint32_t, void *);

        if(cb_id == TXM_QAPI_SPI_CB_NOTIFY) // custom cb-type1
        {
                pfn_app_cb1 = (qapi_SPIM_Callback_Fn_t (*)(uint32_t, void *))app_cb;
                (pfn_app_cb1)((uint32_t)cb_param1, (void *)cb_param2);

        }
		return 0;
}


#else   // DEF_END

#error "No QAPI flags defined"

#endif // DEF end for QAPI_TXM_MODULE or QAPI_TXM_SOURCE_CODE


#endif	
