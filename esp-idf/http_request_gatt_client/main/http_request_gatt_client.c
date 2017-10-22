#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
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

static const char *GATTC_TAG = "ESP32-THETA";

//0F291746-0C80-4726-87A7-3C501FD3B4B6 Bluetooth Control Command スリープOK
//EBAFB2F0-0E0F-40A2-A84F-E2F098DC13C3 Auth Bluetooth Device
#define REMOTE_SERV_UUID_BLUETOOTH_CONTROL_COMMAND {0xb6,0xb4,0xd3,0x1f,0x50,0x3c,0xa7,0x87,0x26,0x47,0x80,0x0c,0x46,0x17,0x29,0x0f,}
#define REMOTE_CHAR_UUID_AUTH_BLUETOOTH_DEVICE {0xc3,0x13,0xdc,0x98,0xf0,0xe2,0x4f,0xa8,0xa2,0x40,0x0f,0x0e,0xf0,0xb2,0xaf,0xeb,}
//#define REMOTE_WRITE_UUID_AUTH_BLUETOOTH_DEVICE {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}

//9A5ED1C5-74CC-4C50-B5B6-66A48E7CCFF1 Camera Information スリープOK
//0D2FC4D5-5CB3-4CDE-B519-445E599957D8 Serial Number String
#define REMOTE_SERV_UUID_CAMERA_INFORMATION {0xF1,0xCF,0x7C,0x8E,0xA4,0x66,0xB6,0xB5,0x50,0x4C,0xCC,0x74,0xC5,0xD1,0x5E,0x9A,}
#define REMOTE_CHAR_UUID_SERIAL_NUMBER_STRING {0x0D,0xD8,0x57,0x99,0x59,0x5E,0x44,0x19,0xB5,0xDE,0x4C,0xB3,0x5C,0xD5,0xC4,0x2F,}

//1D0F3602-8DFB-4340-9045-513040DAD991 Shooting Control Command スリープNA
//FEC1805C-8905-4477-B862-BA5E447528A5 Take Picture
#define REMOTE_SERV_UUID_SHOOTING_CONTRAL {0x91,0xD9,0xDA,0x40,0x30,0x51,0x45,0x90,0x40,0x43,0xFB,0x8D,0x02,0x36,0x0F,0x1D,}
#define REMOTE_CHAR_UUID_TAKE_PICTURE_CHAR {0xA5,0x28,0x75,0x44,0x5E,0xBA,0x62,0xB8,0x77,0x44,0x05,0x89,0x5C,0x80,0xC1,0xFE,}

#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0

static const char remote_device_name[] = THETA_DEVICE_NAME;

static bool ble_connect    = false;
static bool ble_get_server = false;
static bool ble_auth = false;
static esp_gattc_char_elem_t *char_elem_result   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result = NULL;

/* eclare static functions */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_a_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

esp_gatt_if_t gattc_if_to_write = ESP_GATT_IF_NONE;
uint16_t conn_id_to_write;
esp_gatt_srvc_id_t srvc_id_to_write;
esp_gatt_id_t descr_id_to_write;

static esp_bt_uuid_t remote_filter_service_uuid = {
	.len = ESP_UUID_LEN_128,
	.uuid.uuid128 = REMOTE_SERV_UUID_BLUETOOTH_CONTROL_COMMAND,
};
static esp_bt_uuid_t remote_filter_char_uuid = {
	.len = ESP_UUID_LEN_128,
	.uuid.uuid128 = REMOTE_CHAR_UUID_AUTH_BLUETOOTH_DEVICE,
};

static esp_bt_uuid_t remote_filter_shooting_contral_service_uuid = {
	.len = ESP_UUID_LEN_128,
	.uuid.uuid128 = REMOTE_SERV_UUID_SHOOTING_CONTRAL,
};
static esp_bt_uuid_t remote_filter_take_picture_char_uuid = {
	.len = ESP_UUID_LEN_128,
	.uuid.uuid128 = REMOTE_CHAR_UUID_TAKE_PICTURE_CHAR,
};

static esp_ble_scan_params_t ble_scan_params = {
	.scan_type              = BLE_SCAN_TYPE_ACTIVE,
	.own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
	.scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
	.scan_interval          = 0x50,
	.scan_window            = 0x30
};

struct gattc_profile_inst {
	esp_gattc_cb_t gattc_cb;
	uint16_t gattc_if;
	uint16_t app_id;
	uint16_t conn_id;
	uint16_t service_start_handle;
	uint16_t service_end_handle;
	uint16_t char_handle;
	esp_bd_addr_t remote_bda;
};

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
	[PROFILE_A_APP_ID] = {
		.gattc_cb = gattc_profile_a_event_handler,
		.gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
	}
};

static void gattc_profile_a_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
	esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;
	uint16_t conn_id = 0;
	switch (event){
	case ESP_GATTC_REG_EVT:
		ESP_LOGI(GATTC_TAG, "REG_EVT");
		esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
		if(scan_ret){
			ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
		}
		break;
	case ESP_GATTC_CONNECT_EVT:{
		//p_data->connect.status always be ESP_GATT_OK
		ESP_LOGI(GATTC_TAG, "ESP_GATTC_CONNECT_EVT conn_id %d, if %d, status %d", p_data->connect.conn_id, gattc_if, p_data->connect.status);
		gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
		memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
		ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
		esp_log_buffer_hex(GATTC_TAG, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, sizeof(esp_bd_addr_t));
//		esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
//		if(mtu_ret){
//			ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
//		}
		esp_ble_gattc_search_service(gattc_if, p_data->connect.conn_id, &remote_filter_service_uuid);
		break;
	}
	case ESP_GATTC_OPEN_EVT:
		if(param->open.status != ESP_GATT_OK){
			ESP_LOGE(GATTC_TAG, "open failed, status %d", p_data->open.status);
			break;
		}
		ESP_LOGI(GATTC_TAG, "open success");
		break;
		case ESP_GATTC_CFG_MTU_EVT:
		if(param->cfg_mtu.status != ESP_GATT_OK){
			ESP_LOGE(GATTC_TAG,"config mtu failed, error status = %x", param->cfg_mtu.status);
		}
		ESP_LOGI(GATTC_TAG, "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
//		esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &remote_filter_service_uuid);
		break;
	case ESP_GATTC_SEARCH_RES_EVT:
		ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_RES_EVT");
		esp_gatt_srvc_id_t *srvc_id =(esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
		conn_id = p_data->search_res.conn_id;
		ble_get_server = true;
		gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
		gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
		ESP_LOGI(GATTC_TAG, "SEARCH RES: conn_id = %x\n", conn_id);
		if(srvc_id->id.uuid.len == ESP_UUID_LEN_16){
			ESP_LOGI(GATTC_TAG, "UUID16: %x\n", srvc_id->id.uuid.uuid.uuid16);
		}else if(srvc_id->id.uuid.len == ESP_UUID_LEN_32){
			ESP_LOGI(GATTC_TAG, "UUID32: %x\n", srvc_id->id.uuid.uuid.uuid32);
		}else if(srvc_id->id.uuid.len == ESP_UUID_LEN_128){
			ESP_LOGI(GATTC_TAG, "UUID128:");
			esp_log_buffer_hex(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, sizeof(srvc_id->id.uuid.uuid.uuid128));
		} else {
			ESP_LOGE(GATTC_TAG, "UNKNOWN LEN %d\n", srvc_id->id.uuid.len);
		}
		break;
	case ESP_GATTC_SEARCH_CMPL_EVT:
		if(p_data->search_cmpl.status != ESP_GATT_OK){
			ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
			break;
		}
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT");
		if(ble_auth){
			if (ble_get_server){
				uint16_t count = 0;
				ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT - step1");
				esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
					p_data->search_cmpl.conn_id,
					ESP_GATT_DB_CHARACTERISTIC,
					gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
					gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
					INVALID_HANDLE,
					&count);
				ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT - step2");
				if (status != ESP_GATT_OK){
					ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
				}
				if (count > 0){
					ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT - step3");
					char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(char_elem_result) * count);
					if (!char_elem_result){
						ESP_LOGE(GATTC_TAG, "gattc no mem");
					}else{
						ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT - step4");
						status = esp_ble_gattc_get_char_by_uuid( gattc_if,
							p_data->search_cmpl.conn_id,
							gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
							gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
							remote_filter_take_picture_char_uuid,
							char_elem_result,
							&count);
						if (status != ESP_GATT_OK){
							ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
						}
						ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT - step5");
						if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_WRITE)){
							gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;
							ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT - step6");
#if 1
							int8_t svalue = 1;
							uint8_t value = (uint8_t)svalue;
#else
							uint8_t value = 1;
#endif
							status = esp_ble_gattc_write_char(gattc_if,
								p_data->search_cmpl.conn_id,
								char_elem_result[0].char_handle,
								sizeof(value),
								&value,
								ESP_GATT_WRITE_TYPE_NO_RSP,
								ESP_GATT_AUTH_REQ_NONE);
							if (status != ESP_GATT_OK){
								ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char error");
							}else{
								ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char OK");
							}
						}
					}
					/* free char_elem_result */
					free(char_elem_result);
				}else{
					ESP_LOGE(GATTC_TAG, "no char found");
				}
			}
		}else{
			if (ble_get_server){
				uint16_t count = 0;
				esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
					p_data->search_cmpl.conn_id,
					ESP_GATT_DB_CHARACTERISTIC,
					gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
					gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
					INVALID_HANDLE,
					&count);
				if (status != ESP_GATT_OK){
					ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
				}
				if (count > 0){
					char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(char_elem_result) * count);
					if (!char_elem_result){
						ESP_LOGE(GATTC_TAG, "gattc no mem");
					}else{
						status = esp_ble_gattc_get_char_by_uuid( gattc_if,
							p_data->search_cmpl.conn_id,
							gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
							gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
							remote_filter_char_uuid,
							char_elem_result,
							&count);
						if (status != ESP_GATT_OK){
							ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
						}else{
							ESP_LOGI(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid OK");
						}
						if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_WRITE)){
							gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;
							status = esp_ble_gattc_write_char(gattc_if,
								p_data->search_cmpl.conn_id,
								char_elem_result[0].char_handle,
								sizeof(AUTH_UUID),
								(uint8_t *)AUTH_UUID,
								ESP_GATT_WRITE_TYPE_NO_RSP,
								ESP_GATT_AUTH_REQ_NONE);
							if (status != ESP_GATT_OK){
								ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char error");
							}else{
								ESP_LOGI(GATTC_TAG, "esp_ble_gattc_write_char OK");
							}
						}
					}
					/* free char_elem_result */
					free(char_elem_result);
				}else{
					ESP_LOGE(GATTC_TAG, "no char found");
				}
			}
		}
		break;
	case ESP_GATTC_REG_FOR_NOTIFY_EVT:
		ESP_LOGI(GATTC_TAG, "ESP_GATTC_REG_FOR_NOTIFY_EVT");
#if 0
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "REG FOR NOTIFY failed: error status = %d", p_data->reg_for_notify.status);
        }else{
            uint16_t count = 0;
            uint16_t notify_en = 1;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         ESP_GATT_DB_DESCRIPTOR,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0){
                descr_elem_result = malloc(sizeof(descr_elem_result) * count);
                if (!descr_elem_result){
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                }else{
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         p_data->reg_for_notify.handle,
                                                                         notify_descr_uuid,
                                                                         descr_elem_result,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                    }

                    /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                    if (count > 0 && descr_elem_result[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                     descr_elem_result[0].handle,
                                                                     sizeof(notify_en),
                                                                     (uint8_t *)&notify_en,
                                                                     ESP_GATT_WRITE_TYPE_RSP,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                    }

                    /* free descr_elem_result */
                    free(descr_elem_result);
                }
            }
            else{
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }

        }
#endif
		break;
	case ESP_GATTC_NOTIFY_EVT:
		ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, receive notify value:");
		esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);
		break;
	case ESP_GATTC_WRITE_DESCR_EVT:
		if(p_data->write.status != ESP_GATT_OK){
			ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
			break;
		}
		ESP_LOGI(GATTC_TAG, "write descr success ");
#if 0
		esp_gatt_status_t status = esp_ble_gattc_write_char(gattc_if,
			gl_profile_tab[PROFILE_A_APP_ID].conn_id,
			gl_profile_tab[PROFILE_A_APP_ID].char_handle,
			sizeof(AUTH_UUID),
			(uint8_t *)AUTH_UUID,
			ESP_GATT_WRITE_TYPE_NO_RSP,
			ESP_GATT_AUTH_REQ_NONE);
		if(status != ESP_GATT_OK){
			ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char error");
		}else{
			ESP_LOGI(GATTC_TAG, "esp_ble_gattc_write_char OK");
		}
#endif
		break;
	case ESP_GATTC_SRVC_CHG_EVT:{
		esp_bd_addr_t bda;
		memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
		ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:");
		esp_log_buffer_hex(GATTC_TAG, bda, sizeof(esp_bd_addr_t));
		break;
	}
	case ESP_GATTC_WRITE_CHAR_EVT:
		if(p_data->write.status != ESP_GATT_OK){
			ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
			break;
		}
		if(ble_auth){
			ESP_LOGI(GATTC_TAG, "write char success ");
		}else{
			ble_auth = true;
			ble_get_server = false;
			ESP_LOGI(GATTC_TAG, "write AUTH_BLUETOOTH_DEVICE success ");
			esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &remote_filter_shooting_contral_service_uuid);
		}
		break;
	case ESP_GATTC_DISCONNECT_EVT:
		ble_connect = false;
		ble_get_server = false;
		ble_auth = false;
		ESP_LOGI(GATTC_TAG, "ESP_GATTC_DISCONNECT_EVT, status = %d", p_data->disconnect.status);
		break;
	default:
		break;
	}
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	uint8_t *adv_name = NULL;
	uint8_t adv_name_len = 0;
	switch (event) {
	case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
		//the unit of the duration is second
		uint32_t duration = 30;
		esp_ble_gap_start_scanning(duration);
		break;
	}
	case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
		//scan start complete event to indicate scan start successfully or failed
		if(param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS){
			ESP_LOGE(GATTC_TAG, "scan start failed, error status = %x", param->scan_start_cmpl.status);
			break;
		}
		ESP_LOGI(GATTC_TAG, "scan start success");
		break;
	case ESP_GAP_BLE_SCAN_RESULT_EVT: {
		esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
		switch (scan_result->scan_rst.search_evt) {
		case ESP_GAP_SEARCH_INQ_RES_EVT:
			esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
			ESP_LOGI(GATTC_TAG, "searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
			adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
				ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
			ESP_LOGI(GATTC_TAG, "searched Device Name Len %d", adv_name_len);
			esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
			ESP_LOGI(GATTC_TAG, "\n");
			if(adv_name != NULL){
				if(strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0){
					ESP_LOGI(GATTC_TAG, "searched device %s\n", remote_device_name);
					if(ble_connect == false){
						ble_connect = true;
						ESP_LOGI(GATTC_TAG, "connect to the remote device.");
						esp_ble_gap_stop_scanning();
						esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
					}
				}
			}
			break;
		case ESP_GAP_SEARCH_INQ_CMPL_EVT:
			break;
		default:
			break;
		}
		break;
	}
	case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
		if(param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
			ESP_LOGE(GATTC_TAG, "scan stop failed, error status = %x", param->scan_stop_cmpl.status);
			break;
		}
		ESP_LOGI(GATTC_TAG, "stop scan successfully");
		break;
	case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
		if(param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
			ESP_LOGE(GATTC_TAG, "adv stop failed, error status = %x", param->adv_stop_cmpl.status);
			break;
		}
		ESP_LOGI(GATTC_TAG, "stop adv successfully");
		break;
	case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
		ESP_LOGI(GATTC_TAG, "update connetion params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
			param->update_conn_params.status,
			param->update_conn_params.min_int,
			param->update_conn_params.max_int,
			param->update_conn_params.conn_int,
			param->update_conn_params.latency,
			param->update_conn_params.timeout);
		break;
	default:
		break;
	}
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
	/* If event is register event, store the gattc_if for each profile */
	if(event == ESP_GATTC_REG_EVT){
		if(param->reg.status == ESP_GATT_OK){
			gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
		}else{
			ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d",
				param->reg.app_id,
				param->reg.status);
			return;
		}
	}

	/* If the gattc_if equal to profile A, call profile A cb handler,
	* so here call each profile's callback */
	do{
		int idx;
		for(idx = 0; idx < PROFILE_NUM; idx++){
			if(gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
				gattc_if == gl_profile_tab[idx].gattc_if){
				if(gl_profile_tab[idx].gattc_cb){
					gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
				}
			}
		}
	}while(0);
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
	initialise_wifi();
	xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);

	while(true != wifi_register_uuid){
		ESP_LOGI(GATTC_TAG, "vTaskDelay... ");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	ret = esp_bt_controller_init(&bt_cfg);
	if(ret){
		ESP_LOGE(GATTC_TAG, "%s initialize controller failed, error code = %x\n", __func__, ret);
		return;
	}

	ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
	if(ret){
		ESP_LOGE(GATTC_TAG, "%s enable controller failed, error code = %x\n", __func__, ret);
		return;
	}

	ret = esp_bluedroid_init();
	if(ret){
		ESP_LOGE(GATTC_TAG, "%s init bluetooth failed, error code = %x\n", __func__, ret);
		return;
	}

	ret = esp_bluedroid_enable();
	if(ret){
		ESP_LOGE(GATTC_TAG, "%s enable bluetooth failed, error code = %x\n", __func__, ret);
		return;
	}

	//register the  callback function to the gap module
	ret = esp_ble_gap_register_callback(esp_gap_cb);
	if(ret){
		ESP_LOGE(GATTC_TAG, "%s gap register failed, error code = %x\n", __func__, ret);
		return;
	}

	//register the callback function to the gattc module
	ret = esp_ble_gattc_register_callback(esp_gattc_cb);
	if(ret){
		ESP_LOGE(GATTC_TAG, "%s gattc register failed, error code = %x\n", __func__, ret);
		return;
	}

	ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
	if(ret){
		ESP_LOGE(GATTC_TAG, "%s gattc app register failed, error code = %x\n", __func__, ret);
	}
}
