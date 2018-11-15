#ifndef _TwoStickControlMotor_h
#define _TwoStickControlMotor_h

#define DEF_centervalue 'H'
#define DEF_mov_magnification 10
#define DEF_rol_magnification 10

// ゲームコントローラのスティック二本操作によるモーター個別の制御変数クラス
// Arduinoから使用するClass
class TwoStickControlMotor{
private:
    char m_centervalue;
    int m_mov_magnification;
    int m_rol_magnification;
    // wheel1とwheel2は、wheel3とwheel4に対して、正回転の向きが逆になるので負号を逆にする
    // 移動方向をwheel3とwheel4の向きにそろえるので、wheel1とwheel2の負号を逆にする
    char m_older_L_X; // 1/3
    //               0 15 補正前 補正後
    // wheel1 Left : b  f + -
    // wheel2 Right: f  b - +
    // wheel3 Left : b  f + +
    // wheel4 Right: f  b - -
    char m_older_L_Y; // 1/3
    //               0 15 補正前 補正後
    // wheel1 Left : f  b - +
    // wheel2 Right: f  b - +
    // wheel3 Left : f  b - -
    // wheel4 Right: f  b - -
    // wheel1とwheel2は、wheel3とwheel4に対して、正回転の向きが逆になるので負号を逆にする
    // ロールはwheel1とwheel2の向きにそろえるので、wheel3とwheel4の負号を逆にする
    char m_older_R_X; // 1/3
    //               0 15 補正前 補正後
    // wheel1 Left : b  f + +
    // wheel2 Right: b  f + +
    // wheel3 Left : f  b - +
    // wheel4 Right: f  b - +
//    char m_older_R_Y = m_centervalue; // 未使用

public:
  TwoStickControlMotor():
  m_centervalue(DEF_centervalue)
  ,m_mov_magnification(DEF_mov_magnification)
  ,m_rol_magnification(DEF_rol_magnification)
  ,m_older_L_X(m_centervalue)
  ,m_older_L_Y(m_centervalue)
  ,m_older_R_X(m_centervalue){}

  bool update(String str){ // スティックの傾きを4文字の引数で指定する
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

  int getULSpeedMMPS(){ // wheel1モーターの目標スピードを取得する
    return(m_mov_magnification * (-((int)m_older_L_X - (int)m_centervalue) + ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

  int getLLSpeedMMPS(){ // wheel2モーターの目標スピードを取得する
    return(m_mov_magnification * (((int)m_older_L_X - (int)m_centervalue) + ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

  int getLRSpeedMMPS(){ // wheel3モーターの目標スピードを取得する
    return(m_mov_magnification * (((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

  int getURSpeedMMPS(){ // wheel4モーターの目標スピードを取得する
    return(m_mov_magnification * (-((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

protected:

};

#endif
