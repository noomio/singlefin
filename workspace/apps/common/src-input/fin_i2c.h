#if !defined(FIN_API_I2C_H_INCLUDED)
#define FIN_API_I2C_H_INCLUDED

#define I2C_DEFAULT_FREQ	100  // 100kHz

struct i2c_list_entry {
    const char *key;
    uint32_t value;
 };

typedef enum fin_i2c_num{
	I2C1,
	I2C2,
	I2C_MAX_NO
} fin_i2c_num_t;


int fin_i2c_transfer(fin_i2c_num_t i2c_num, uint16_t addr, uint8_t *tx_data, size_t tx_data_size, 
	uint8_t *rx_data, size_t rx_data_size, uint32_t delay_us);
int fin_i2c_start(fin_i2c_num_t i2c_num);
int fin_i2c_stop(fin_i2c_num_t i2c_num);
int fin_i2c_config(fin_i2c_num_t i2c_num);
int fin_i2c_deconfig(fin_i2c_num_t i2c_num);
int fin_i2c_set_frequency(fin_i2c_num_t i2c_num, uint32_t frequency);
int fin_i2c_power_down(fin_i2c_num_t i2c_num);
int fin_i2c_power_up(fin_i2c_num_t i2c_num);

#endif  /* FIN_API_I2C_H_INCLUDED */

