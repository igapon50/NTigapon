#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"

#define DATA_LENGTH          32  /*!<Data buffer length for test buffer*/
#define RW_TEST_LENGTH       16  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/

#define I2C_EXAMPLE_SLAVE_SCL_IO     26    /*!<gpio number for i2c slave clock  */
#define I2C_EXAMPLE_SLAVE_SDA_IO     25    /*!<gpio number for i2c slave data */
#define I2C_EXAMPLE_SLAVE_NUM I2C_NUM_0    /*!<I2C port number for slave dev */
#define I2C_EXAMPLE_SLAVE_TX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave tx buffer size */
#define I2C_EXAMPLE_SLAVE_RX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave rx buffer size */

#define ESP_SLAVE_ADDR 0x08         /*!< ESP32 slave address, you can set any 7bit value */

static const char *I2C_TAG = "I2C";
/**
 * @brief i2c slave initialization
 */
static void i2c_example_slave_init()
{
	int i2c_slave_port = I2C_EXAMPLE_SLAVE_NUM;
	i2c_config_t conf_slave;
	conf_slave.sda_io_num = I2C_EXAMPLE_SLAVE_SDA_IO;
	conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf_slave.scl_io_num = I2C_EXAMPLE_SLAVE_SCL_IO;
	conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf_slave.mode = I2C_MODE_SLAVE;
	conf_slave.slave.addr_10bit_en = 0;
	conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;
	i2c_param_config(i2c_slave_port, &conf_slave);
	i2c_driver_install(i2c_slave_port, conf_slave.mode,
		I2C_EXAMPLE_SLAVE_RX_BUF_LEN,
		I2C_EXAMPLE_SLAVE_TX_BUF_LEN, 0);
}

static void i2c_test_task(void* arg)
{
	uint32_t task_idx = (uint32_t) arg;
	uint8_t* data = (uint8_t*) malloc(DATA_LENGTH);
	while(1){
		int size;
		size = i2c_slave_read_buffer( I2C_EXAMPLE_SLAVE_NUM, data, RW_TEST_LENGTH, 1000 / portTICK_RATE_MS);
		if(0 < size){
			ESP_LOGI(I2C_TAG, "read slave[%c]", data[0]);
		}
	}
}

void app_main()
{
	i2c_example_slave_init();
	xTaskCreate(i2c_test_task, "i2c_test_task_0", 1024 * 2, (void* ) 0, 10, NULL);
}
