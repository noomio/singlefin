#if !defined(FIN_API_SPI_H_INCLUDED)
#define FIN_API_SPI_H_INCLUDED

struct spi_list_entry {
    const char *key;
    uint32_t value;
 };


#define SPI_DEFAULT_FREQ	1000000UL  // 1MHz, this is the min and anything less will return error when spi_config




#endif  /* FIN_API_SPI_H_INCLUDED */

