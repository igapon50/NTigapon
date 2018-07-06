#ifndef _TwoStickControlMotor_h
#define _TwoStickControlMotor_h

class TwoStickControlMotor{
// ゲームコントローラのスティック二本操作によるモーター制御変数クラス
// Arduinoから使用するClass
public:
    //初期化処理
    void init(char centervalue = 'H', int magnification = 7){
      m_centervalue = centervalue;
      m_magnification = magnification;
    }

  bool update(String str){
    if(4 == str.length()){
      m_older_L_X = str.charAt(0);
      m_older_L_Y = str.charAt(1);
      m_older_R_X = str.charAt(2);
//      m_older_R_Y = str.charAt(3); // 未使用
    }else{
      return(false);
    }
    return(true);
  }

//  unsigned int setMotorAll(unsigned int speedMMPS=0,bool dir=DIR_ADVANCE); //全てのモーターのスピードと方向をセットする
// 引数dirには、DIR_ADVANCE/DIR_BACKOFFが指摘できる
//    unsigned int Omni4WD::wheelULSetSpeedMMPS(unsigned int speedMMPS,bool dir); //
//    unsigned int Omni4WD::wheelLLSetSpeedMMPS(unsigned int speedMMPS,bool dir); //
//    unsigned int Omni4WD::wheelLRSetSpeedMMPS(unsigned int speedMMPS,bool dir); //
//    unsigned int Omni4WD::wheelURSetSpeedMMPS(unsigned int speedMMPS,bool dir); //
  int getULSpeedMMPS(){ // wheel1
    return(m_magnification * (((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue) + ((int)m_older_R_X - (int)m_centervalue)));
  }

  int getLLSpeedMMPS(){ // wheel2
    return(m_magnification * (-((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue) + ((int)m_older_R_X - (int)m_centervalue)));
  }

  int getLRSpeedMMPS(){ // wheel3
    return(m_magnification * (((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue) - ((int)m_older_R_X - (int)m_centervalue)));
  }

  int getURSpeedMMPS(){ // wheel4
    return(m_magnification * (-((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue) - ((int)m_older_R_X - (int)m_centervalue)));
  }

private:
    char m_centervalue = 'H';
    int m_magnification = 7;
    char m_older_L_X = m_centervalue; // 1/3
    //               0 15
    // wheel1 Left : b  f
    // wheel2 Right: f  b
    // wheel3 Left : b  f
    // wheel4 Right: f  b
    char m_older_L_Y = m_centervalue; // 1/3
    //               0 15
    // wheel1 Left : f  b
    // wheel2 Right: f  b
    // wheel3 Left : f  b
    // wheel4 Right: f  b
    char m_older_R_X = m_centervalue; // 1/3
    //               0 15
    // wheel1 Left : b  f
    // wheel2 Right: b  f
    // wheel3 Left : f  b
    // wheel4 Right: f  b
//    char m_older_R_Y = m_centervalue; // 未使用

protected:

};

#endif
