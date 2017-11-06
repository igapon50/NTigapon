#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "driver/i2c.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#define AUTH_UUID "00000000-0000-0000-0000-000000000000"
#include "thetav2_1.h"

#define THETA_DEVICE_NAME "00101594"
//#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_SSID "THETAYL"THETA_DEVICE_NAME".OSC"
//#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD
#define EXAMPLE_WIFI_PASS THETA_DEVICE_NAME

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static bool wifi_register_uuid    = false;
static const char *WIFI_TAG = "WiFi-THETA";

#define DATA_LENGTH          512  /*!<Data buffer length for test buffer*/
#define RW_TEST_LENGTH       129  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/
#define I2C_EXAMPLE_SLAVE_SCL_IO     26    /*!<gpio number for i2c slave clock  */
#define I2C_EXAMPLE_SLAVE_SDA_IO     25    /*!<gpio number for i2c slave data */
#define I2C_EXAMPLE_SLAVE_NUM I2C_NUM_0    /*!<I2C port number for slave dev */
#define I2C_EXAMPLE_SLAVE_TX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave tx buffer size */
#define I2C_EXAMPLE_SLAVE_RX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave rx buffer size */

#define ESP_SLAVE_ADDR 0x08         /*!< ESP32 slave address, you can set any 7bit value */

static const char *I2C_TAG = "I2C";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id){
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		/* This is a workaround as ESP32 WiFi libs don't currently
		auto-reassociate. */
		wifi_register_uuid = false;
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		break;
	default:
		break;
	}
	return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(WIFI_TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

bool initialize_requests(int socket){
	int r;
	char recv_buf[64];
	char buffer[1024] = {'\0'};
	const char *requests[] = {
		REQUEST_MAIN_setBluetoothDevice,
		REQUEST_MAIN_setOptions__bluetoothPower,
		REQUEST_MAIN_getOptions__bluetoothPower,
//		REQUEST_MAIN_takePicture,
		NULL
	};
	for(int no = 0 ; requests[no] != NULL ; no++){
		margeString_POST_Request(buffer, 1024, requests[no]);
		ESP_LOGI(WIFI_TAG, "%s", buffer);
		if(write(socket, buffer, strlen(buffer)) < 0){
			ESP_LOGE(WIFI_TAG, "... socket send failed");
			close(socket);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			return(false);
		}
		ESP_LOGI(WIFI_TAG, "... socket send success");
		struct timeval receiving_timeout;
		receiving_timeout.tv_sec = 5;
		receiving_timeout.tv_usec = 0;
		if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0){
			ESP_LOGE(WIFI_TAG, "... failed to set socket receiving timeout");
			close(socket);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			return(false);
		}
		ESP_LOGI(WIFI_TAG, "... set socket receiving timeout success");
		/* Read HTTP response */
		do{
			bzero(recv_buf, sizeof(recv_buf));
			r = read(socket, recv_buf, sizeof(recv_buf)-1);
			for(int i = 0; i < r; i++){
				putchar(recv_buf[i]);
			}
		}while(r > 0);
		ESP_LOGI(WIFI_TAG, "... done reading from socket. Last read return=%d\r\n", r);
	}
	return(true);
}

bool command_requests(int socket){
	int r;
	char recv_buf[64];
	char buffer[1024] = {'\0'};
	const char *requests[] = {
		REQUEST_MAIN_takePicture,
		NULL
	};
	for(int no = 0 ; requests[no] != NULL ; no++){
		margeString_POST_Request(buffer, 1024, requests[no]);
		ESP_LOGI(WIFI_TAG, "%s", buffer);
		if(write(socket, buffer, strlen(buffer)) < 0){
			ESP_LOGE(WIFI_TAG, "... socket send failed");
			close(socket);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			return(false);
		}
		ESP_LOGI(WIFI_TAG, "... socket send success");
		struct timeval receiving_timeout;
		receiving_timeout.tv_sec = 5;
		receiving_timeout.tv_usec = 0;
		if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0){
			ESP_LOGE(WIFI_TAG, "... failed to set socket receiving timeout");
			close(socket);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			return(false);
		}
		ESP_LOGI(WIFI_TAG, "... set socket receiving timeout success");
		/* Read HTTP response */
		do{
			bzero(recv_buf, sizeof(recv_buf));
			r = read(socket, recv_buf, sizeof(recv_buf)-1);
			for(int i = 0; i < r; i++){
				putchar(recv_buf[i]);
			}
		}while(r > 0);
		ESP_LOGI(WIFI_TAG, "... done reading from socket. Last read return=%d\r\n", r);
	}
	return(true);
}

static void http_task(void *pvParameters)
{
	const struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *res;
	struct in_addr *addr;
	int socket;
	
	while(1){
		/* Wait for the callback to set the CONNECTED_BIT in the
		event group.
		*/
		xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
			false, true, portMAX_DELAY);
		ESP_LOGI(WIFI_TAG, "Connected to AP");

		int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

		if(err != 0 || res == NULL){
			ESP_LOGE(WIFI_TAG, "DNS lookup failed err=%d res=%p", err, res);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}

		/* Code to print the resolved IP.
		Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
		addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
		ESP_LOGI(WIFI_TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

		socket = socket(res->ai_family, res->ai_socktype, 0);
		if(socket < 0){
			ESP_LOGE(WIFI_TAG, "... Failed to allocate socket.");
			freeaddrinfo(res);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(WIFI_TAG, "... allocated socket");
		if(connect(socket, res->ai_addr, res->ai_addrlen) != 0){
			ESP_LOGE(WIFI_TAG, "... socket connect failed errno=%d", errno);
			close(socket);
			freeaddrinfo(res);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(WIFI_TAG, "... connected");
		freeaddrinfo(res);

//		if(false == initialize_requests(socket)){
//			continue;
//		}
		wifi_register_uuid = true;
		while(wifi_register_uuid){
			uint8_t data[DATA_LENGTH] = {'\0'};
			int size;
			size = i2c_slave_read_buffer( I2C_EXAMPLE_SLAVE_NUM, data, RW_TEST_LENGTH, 1000 / portTICK_RATE_MS);
			if(0 < size){
				ESP_LOGI(I2C_TAG, "read slave[%c]", data[0]);
				if(command_requests(socket)){
					ESP_LOGI(I2C_TAG, "command_request() success");
				}else{
					ESP_LOGE(I2C_TAG, "command_request() fail");
				}
			}
    	}
		close(socket);
		ESP_LOGI(WIFI_TAG, "Starting again!");
	}
}

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

void app_main()
{
	// Initialize NVS.
	esp_err_t ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES){
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );
	i2c_example_slave_init();
	initialise_wifi();
	xTaskCreate(&http_task, "http_task", 4096, NULL, 5, NULL);
	while(true != wifi_register_uuid){
		ESP_LOGI(WIFI_TAG, "vTaskDelay... ");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
