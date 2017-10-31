#ifndef _thetav2_1_h
#define _thetav2_1_h

#ifndef AUTH_UUID
#define AUTH_UUID "00000000-0000-0000-0000-000000000000"
#endif
#define WEB_SERVER "192.168.1.1"
#define WEB_PORT "80"

static const char *REQUEST_MAIN_takePicture = "{\r\n"
"	\"name\":\"camera.takePicture\"\r\n"
"}\r\n";

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

//サンプル
// param s : socket
// return true:成功
// return false:失敗
#if 0
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
#endif

#endif
