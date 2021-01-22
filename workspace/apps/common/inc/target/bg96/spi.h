
#ifndef __BG96_SPI_H__
#define __BG96_SPI_H__


struct spi_list_entry {
    const char *key;
    uint32_t value;
 };


#define SPI_DEFAULT_FREQ	10000000L  // 100kHz

typedef enum {
	SPI1,
	SPI2,
	SPI_MAX_NO
} spi_num_t;

typedef enum{
	SPI_CS_DEASSERT,
	SPI_CS_KEEP_ASSERTED,
	SPI_CS_MODE_INVALID = 0x7FFFFFFF
} spi_cs_mode_t;

typedef enum{
	SPI_CS_ACTIVE_LOW,
	SPI_CS_ACTIVE_HIGH
} spi_cs_polarity_t;

typedef enum{
	SPI_BYTE_ORDER_NATIVE = 0,
	SPI_BYTE_ORDER_LITTLE_ENDIAN = 0,
	SPI_BYTE_ORDER_BIG_ENDIAN
}spi_endian_t;

typedef enum{
	SPI_MODE_0, //CPOL = 0, CPHA = 0.
	SPI_MODE_1, //CPOL = 0, CPHA = 1.
	SPI_MODE_2, //CPOL = 1, CPHA = 0.
	SPI_MODE_3 //CPOL = 1, CPHA = 1.
} spi_mode_t;

typedef enum{
	SPI_BPW_3 = 3,
	SPI_BPW_4,
	SPI_BPW_5,
	SPI_BPW_6,
	SPI_BPW_7,
	SPI_BPW_8,
	SPI_BPW_9,
	SPI_BPW_10,
	SPI_BPW_11,
	SPI_BPW_12,
	SPI_BPW_13,
	SPI_BPW_14,
	SPI_BPW_15,
	SPI_BPW_16,
	SPI_BPW_17,
	SPI_BPW_18,
	SPI_BPW_19,
	SPI_BPW_20,
	SPI_BPW_21,
	SPI_BPW_22,
	SPI_BPW_23,
	SPI_BPW_24,
	SPI_BPW_25,
	SPI_BPW_26,
	SPI_BPW_27,
	SPI_BPW_28,
	SPI_BPW_29,
	SPI_BPW_30,
	SPI_BPW_31
}spi_bits_per_word_t;



int spi_power_down(spi_num_t spi_num);
int spi_power_up(spi_num_t spi_num);
int spi_config(spi_num_t spi_num);
int spi_deconfig(spi_num_t spi_num);
int spi_set_frequency(spi_num_t spi_num, uint32_t frequency);
int spi_set_endianess(spi_num_t spi_num, spi_endian_t endian);
int spi_set_bit_per_word(spi_num_t spi_num, spi_bits_per_word_t bpw);
int spi_set_mode(spi_num_t spi_num, spi_mode_t mode);
int spi_set_inter_word_delay(spi_num_t spi_num, uint8_t delay);
int spi_set_loopback(spi_num_t spi_num, bool enable);
int spi_set_chip_select_mode(spi_num_t spi_num, spi_cs_mode_t mode);
int spi_set_chip_select_polarity(spi_num_t spi_num, spi_cs_polarity_t polarity);
int spi_set_chip_select_delay(spi_num_t spi_num, uint8_t delay);
int spi_set_slaves_num(spi_num_t spi_num, uint8_t num);
int spi_send(spi_num_t spi_num, uint8_t *tx_buf, size_t tx_len);
int spi_send_receive(spi_num_t spi_num, uint8_t *tx_buf, uint8_t *rx_buf, size_t rx_len);
void spi_config_dump(spi_num_t spi_num);
const char *spi_get_name(spi_num_t spi_num);

#endif