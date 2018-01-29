#import <Arduino.h>
#include "TA7291P.h"

class TA7291P{
// 東芝製モーター制御ICをArduinoから使用するClass
//---------------------------
// Arduino Pinアサイン
//   D0とD1は、シリアル通信に使用する
//   D8～D13は、USB Host Sheildに使用する
//   A4とA5は、SPI通信に使用する
//   D2～D7が、TA7291Pに使用可能なので以下の通り接続する
//     モーター1
//       D2-IN2
//       D3(PWM)-Vref
//       D4-IN1
//     モーター2
//       D5-IN2
//       D6(PWM)-Vref
//       D7-IN1
//---------------------------
// TA7291P Pinアサイン
//   Vcc  7 ロジック側電源端子
//   Vs 8 出力側電源端子
//   Vref 4 〇制御電源端子
//   GND  1 GND
//   IN1  5 〇入力端子
//   IN2  6 〇入力端子
//   OUT1 2 出力端子
//   OUT2 10  出力端子
//   OPEN1 3  接続無し
//   OPEN2 9  接続無し
//   ※Vref <Vccにする必要がある
//---------------------------
public:
    //モーター1、int in1 = 2, int in2 = 4, int vref = 3
    //モーター2、int in1 = 5, int in2 = 7, int vref = 6
    TA7291P(int in1 = 2, int in2 = 4, int vref = 3){
      m_in1 = in1;
      m_in2 = in2;
      m_vref = vref;
    }
    void init(void){
      pinMode(m_in1,OUTPUT); //信号用ピン
      pinMode(m_in2,OUTPUT); //信号用ピン
      pinMode(m_vref,OUTPUT); //制御用ピン
    }
    int operator ++() {
      m_val += m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
      return(m_val);
    }
    void operator ++(int n){
      m_val += m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
    }
    int operator --() {
      m_val -= m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
      return(m_val);
    }
    void operator --(int n){
      m_val -= m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
    }
    void setStepval(int step = 1){
      if(0 < step && step < (m_maxval - m_minval)/2 ){
        m_step = step;
      }
    }
    int getStepval(void){
      return(m_step);
    }
private:
    int m_in1;
    int m_in2;
    int m_vref;
    const int m_maxval = MAX_MOTER_VAL;
    const int m_minval = MIN_MOTER_VAL;
    int m_val = STOP_MOTER_VAL;
    int m_step = 1;

protected:

};

