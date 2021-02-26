
#ifndef __920X_I2C_H__
#define __920X_I2C_H__

#define I2C_DEFAULT_FREQ	100  // 100kHz

struct i2c_list_entry {
    const char *key;
    uint32_t value;
 };

typedef enum {
	I2C1,
	I2C2,
	I2C_MAX_NO
} i2c_num_t;


int i2c_transfer(i2c_num_t i2c_num, uint16_t addr, uint8_t *tx_data, size_t tx_data_size, 
	uint8_t *rx_data, size_t rx_data_size, uint32_t delay_us);
int i2c_start(i2c_num_t i2c_num);
int i2c_stop(i2c_num_t i2c_num);
int i2c_config(i2c_num_t i2c_num);
int i2c_deconfig(i2c_num_t i2c_num);
int i2c_set_frequency(i2c_num_t i2c_num, uint32_t frequency);
int i2c_power_down(i2c_num_t i2c_num);
int i2c_power_up(i2c_num_t i2c_num);


#endif