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

int hastler_moter_front(const int val = STOP_MOTER_VAL){
	int val_front;
	if(val < MIN_MOTER_VAL){
		val_front = MIN_MOTER_VAL;
	}else if(MAX_MOTER_VAL < val){
		val_front = MAX_MOTER_VAL;
	}else{
		val_front = val;
	}

	//前輪
	//静止／正転／逆転の状態に分けてプログラムする
	if(STOP_MOTER_VAL == val_front){ //静止:255
		//LOW,LOWでデジタル出力
		digitalWrite(digitalOutPin_front1,LOW);
		digitalWrite(digitalOutPin_front2,LOW);
	}else if(STOP_MOTER_VAL < val_front){ //正転:256~510
		//HIGH,LOWでデジタル出力
		digitalWrite(digitalOutPin_front1,HIGH);
		digitalWrite(digitalOutPin_front2,LOW);
		//val_frontが大きいほど出力値も大きくなる
		analogWrite(analogOutPin_front,val_front - STOP_MOTER_VAL); //出力値:1~255
	}else{ //逆転:0~254
		//LOW,HIGHでデジタル出力
		digitalWrite(digitalOutPin_front1,LOW);
		digitalWrite(digitalOutPin_front2,HIGH);
		//val_frontが小さいほど出力値は大きくなる
		analogWrite(analogOutPin_front,STOP_MOTER_VAL - val_front); //出力値:1~255
	}
 return(val_front);
}

int hastler_moter_back(const int val = STOP_MOTER_VAL){
	int val_back;
	if(val < MIN_MOTER_VAL){
		val_back = MIN_MOTER_VAL;
	}else if(MAX_MOTER_VAL < val){
		val_back = MAX_MOTER_VAL;
	}else{
		val_back = val;
	}

	//後輪
	//静止／正転／逆転の状態に分けてプログラムする
	if(STOP_MOTER_VAL == val_back){ //静止:255
		//LOW,LOWでデジタル出力
		digitalWrite(digitalOutPin_back1,LOW);
		digitalWrite(digitalOutPin_back2,LOW);
	}else if(STOP_MOTER_VAL < val_back){ //正転:256~510
		//HIGH,LOWでデジタル出力
		digitalWrite(digitalOutPin_back1,HIGH);
		digitalWrite(digitalOutPin_back2,LOW);
		//val_backが大きいほど出力値も大きくなる
		analogWrite(analogOutPin_back,val_back - STOP_MOTER_VAL); //出力値:1~255
	}else{ //逆転:0~254
		//LOW,HIGHでデジタル出力
		digitalWrite(digitalOutPin_back1,LOW);
		digitalWrite(digitalOutPin_back2,HIGH);
		//val_backが小さいほど出力値は大きくなる
		analogWrite(analogOutPin_back,STOP_MOTER_VAL - val_back); //出力値:1~255
	}
 return(val_back);
}
