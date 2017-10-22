/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

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

/* Constants that aren't configurable in menuconfig */
//#define WEB_SERVER "example.com"
#define WEB_SERVER "192.168.1.1"
#define WEB_PORT "80"

#define AUTH_UUID "00000000-0000-0000-0000-000000000000"
static bool wifi_register_uuid    = false;
static const char *WIFI_TAG = "WiFi-THETA";

#if 0
static const char *REQUEST_MAIN_takePicture = "{\r\n"
"	\"name\":\"camera.takePicture\"\r\n"
"}\r\n";
#endif

static const char *REQUEST_MAIN_getOptions__bluetoothPower = "{\r\n"
"	\"name\":\"camera.getOptions\",\r\n"
"	\"parameters\":{\r\n"
"		\"optionNames\":[\r\n"
"			\"_bluetoothPower\",\r\n"
"			\"_bluetoothPowerSupport\"\r\n"
"		]\r\n"
"	}\r\n"
"}\r\n";

static const char *REQUEST_MAIN_setOptions__bluetoothPower = "{\r\n"
"	\"name\":\"camera.setOptions\",\r\n"
"	\"parameters\":{\r\n"
"		\"options\":{\r\n"
"			\"_bluetoothPower\":\"ON\"\r\n"
"		}\r\n"
"	}\r\n"
"}\r\n";

static const char *REQUEST_MAIN_setBluetoothDevice = "{\r\n"
"	\"name\":\"camera._setBluetoothDevice\",\r\n"
"	\"parameters\":{\"uuid\":\""AUTH_UUID"\"}\r\n"
"}\r\n";

void margeString_POST_Request(char *dststr, size_t dstlen, const char *str1){
	static const char *REQUEST_HEADER_1 = "POST /osc/commands/execute HTTP/1.1\r\n"
	"Connection: keep-alive\r\n"
	"Content-Type: application/json\r\n"
	"Content-Length: ";
	static const char *REQUEST_HEADER_2 = "\r\nHost: "WEB_SERVER"\r\n"
	"User-Agent: Apache-HttpClient/4.5.3 (Java/1.8.0_144)\r\n"
	"Authorization: Basic cmVjZXB0b3I6cmVjZXB0b3I=\r\n"
	"\r\n";

	//パラメータノーチェック
	size_t length = strlen(str1);
	snprintf(dststr, dstlen, "%s%d%s%s", REQUEST_HEADER_1, length, REQUEST_HEADER_2, str1);
	return;
}

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

bool initialize_requests(int s){
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
		if(write(s, buffer, strlen(buffer)) < 0){
			ESP_LOGE(WIFI_TAG, "... socket send failed");
			close(s);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			return(false);
		}
		ESP_LOGI(WIFI_TAG, "... socket send success");
		struct timeval receiving_timeout;
		receiving_timeout.tv_sec = 5;
		receiving_timeout.tv_usec = 0;
		if(setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0){
			ESP_LOGE(WIFI_TAG, "... failed to set socket receiving timeout");
			close(s);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			return(false);
		}
		ESP_LOGI(WIFI_TAG, "... set socket receiving timeout success");
		/* Read HTTP response */
		do{
			bzero(recv_buf, sizeof(recv_buf));
			r = read(s, recv_buf, sizeof(recv_buf)-1);
			for(int i = 0; i < r; i++){
				putchar(recv_buf[i]);
			}
		}while(r > 0);
		ESP_LOGI(WIFI_TAG, "... done reading from socket. Last read return=%d\r\n", r);
	}
	return(true);
}

static void http_get_task(void *pvParameters)
{
	const struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *res;
	struct in_addr *addr;
	int s;
	
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

		s = socket(res->ai_family, res->ai_socktype, 0);
		if(s < 0){
			ESP_LOGE(WIFI_TAG, "... Failed to allocate socket.");
			freeaddrinfo(res);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(WIFI_TAG, "... allocated socket");
		if(connect(s, res->ai_addr, res->ai_addrlen) != 0){
			ESP_LOGE(WIFI_TAG, "... socket connect failed errno=%d", errno);
			close(s);
			freeaddrinfo(res);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(WIFI_TAG, "... connected");
		freeaddrinfo(res);

		if(false == initialize_requests(s)){
			continue;
		}
		wifi_register_uuid = true;
		close(s);
    	while(wifi_register_uuid){
			ESP_LOGI(WIFI_TAG, "vTaskDelay... ");
			vTaskDelay(1000 / portTICK_PERIOD_MS);
    	}
		ESP_LOGI(WIFI_TAG, "Starting again!");
	}
}



void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
}
