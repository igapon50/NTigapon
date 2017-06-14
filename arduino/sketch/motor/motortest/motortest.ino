const int analogInPin = A0;
int sensorValue = 0;        // value read from the pot
const int digitalOutPin_front1 = 2;
const int digitalOutPin_front2 = 4;
const int analogOutPin_front = 3;
const int digitalOutPin_back1 = 7;
const int digitalOutPin_back2 = 8;
const int analogOutPin_back = 9;

void setup(){
  pinMode(digitalOutPin_front1,OUTPUT); //信号用ピン
  pinMode(digitalOutPin_front2,OUTPUT); //信号用ピン
  pinMode(digitalOutPin_back1,OUTPUT); //信号用ピン
  pinMode(digitalOutPin_back2,OUTPUT); //信号用ピン
  pinMode(11,INPUT); //スイッチ
  Serial.begin(115200);
}

void loop(){
  int val_front = 255,val_back = 255;
  sensorValue = analogRead(analogInPin);
  Serial.print("sensor = ");
  Serial.print(sensorValue);
  val_front = val_back = sensorValue/2;
//  if(sensorValue > 800){
//    val=5*51+256; // ゆっくりめで回転
//  }else{
//    val=255; //止める
//  }
  
  Serial.print("\t val_front = ");
  Serial.println(val_front);
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
  
  Serial.print("\t val_back = ");
  Serial.println(val_back);
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
