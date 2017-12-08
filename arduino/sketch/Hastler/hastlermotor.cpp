#import <Arduino.h>
#include "hastlermotor.h"

const int digitalOutPin_front1 = 2;
const int digitalOutPin_front2 = 4;
const int analogOutPin_front = 3;
const int digitalOutPin_back1 = 5;
const int digitalOutPin_back2 = 7;
const int analogOutPin_back = 6;

void hastler_motor_init(){
	pinMode(digitalOutPin_front1,OUTPUT); //信号用ピン
	pinMode(digitalOutPin_front2,OUTPUT); //信号用ピン
	pinMode(analogOutPin_front,OUTPUT); //制御用ピン
	pinMode(digitalOutPin_back1,OUTPUT); //信号用ピン
	pinMode(digitalOutPin_back2,OUTPUT); //信号用ピン
	pinMode(analogOutPin_back,OUTPUT); //制御用ピン
}

void hastler_moter_front(const int val = STOP_MOTER_VAL){
	int val_front;
	if(val < MIN_MOTER_VAL){
		Serial.print("\nhastler_moter_front input val = ");
		Serial.println(val);
		val_front = MIN_MOTER_VAL;
	}else if(MAX_MOTER_VAL < val){
		Serial.print("\nhastler_moter_front input val = ");
		Serial.println(val);
		val_front = MAX_MOTER_VAL;
	}else{
		val_front = val;
	}

	Serial.print(" val_front = ");
	Serial.print(val_front);
	//前輪
	//静止／正転／逆転の状態に分けてプログラムする
	if(val_front>=255 && val_front<=256){ //静止:255~256
		//LOW,LOWでデジタル出力
		digitalWrite(digitalOutPin_front1,LOW);
		digitalWrite(digitalOutPin_front2,LOW);
	}else if(val_front>256){        //正転:257~511
		//HIGH,LOWでデジタル出力
		digitalWrite(digitalOutPin_front1,HIGH);
		digitalWrite(digitalOutPin_front2,LOW);
		//val_frontが大きいほど出力値も大きくなる
		analogWrite(analogOutPin_front,val_front-256); //出力値:1~255
	}else{                    //逆転:0~254
		//LOW,HIGHでデジタル出力
		digitalWrite(digitalOutPin_front1,LOW);
		digitalWrite(digitalOutPin_front2,HIGH);
		//val_frontが小さいほど出力値は大きくなる
		analogWrite(analogOutPin_front,255-val_front); //出力値:1~255
	}
}

void hastler_moter_back(const int val = STOP_MOTER_VAL){
	int val_back;
	if(val < MIN_MOTER_VAL){
		Serial.print("\nhastler_moter_back input val = ");
		Serial.println(val);
		val_back = MIN_MOTER_VAL;
	}else if(MAX_MOTER_VAL < val){
		Serial.print("\nhastler_moter_back input val = ");
		Serial.println(val);
		val_back = MAX_MOTER_VAL;
	}else{
		val_back = val;
	}

	Serial.print(" val_back = ");
	Serial.print(val_back);
	//後輪
	//静止／正転／逆転の状態に分けてプログラムする
	if(val_back>=255 && val_back<=256){ //静止:255~256
		//LOW,LOWでデジタル出力
		digitalWrite(digitalOutPin_back1,LOW);
		digitalWrite(digitalOutPin_back2,LOW);
	}else if(val_back>256){        //正転:257~511
		//HIGH,LOWでデジタル出力
		digitalWrite(digitalOutPin_back1,HIGH);
		digitalWrite(digitalOutPin_back2,LOW);
		//val_backが大きいほど出力値も大きくなる
		analogWrite(analogOutPin_back,val_back-256); //出力値:1~255
	}else{                    //逆転:0~254
		//LOW,HIGHでデジタル出力
		digitalWrite(digitalOutPin_back1,LOW);
		digitalWrite(digitalOutPin_back2,HIGH);
		//val_backが小さいほど出力値は大きくなる
		analogWrite(analogOutPin_back,255-val_back); //出力値:1~255
	}
}