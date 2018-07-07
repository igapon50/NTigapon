#ifndef _TwoStickControlMotor_h
#define _TwoStickControlMotor_h

class TwoStickControlMotor{
// ゲームコントローラのスティック二本操作によるモーター制御変数クラス
// Arduinoから使用するClass
public:
    //初期化処理
    void init(char centervalue = 'H', int mov_magnification = 10, int rol_magnification = 3){
      m_centervalue = centervalue;
      m_mov_magnification = mov_magnification;
      m_rol_magnification = rol_magnification;
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

  int getULSpeedMMPS(){ // wheel1
    return(m_mov_magnification * (-((int)m_older_L_X - (int)m_centervalue) + ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

  int getLLSpeedMMPS(){ // wheel2
    return(m_mov_magnification * (((int)m_older_L_X - (int)m_centervalue) + ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

  int getLRSpeedMMPS(){ // wheel3
    return(m_mov_magnification * (((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

  int getURSpeedMMPS(){ // wheel4
    return(m_mov_magnification * (-((int)m_older_L_X - (int)m_centervalue) - ((int)m_older_L_Y - (int)m_centervalue)) + m_rol_magnification * ((int)m_older_R_X - (int)m_centervalue));
  }

private:
    char m_centervalue = 'H';
    int m_mov_magnification = 10;
    int m_rol_magnification = 3;
    // wheel1とwheel2は、wheel3とwheel4に対して、正回転の向きが逆になるので負号を逆にする
    // 移動方向をwheel3とwheel4の向きにそろえるので、wheel1とwheel2の負号を逆にする
    char m_older_L_X = m_centervalue; // 1/3
    //               0 15 補正前 補正後
    // wheel1 Left : b  f + -
    // wheel2 Right: f  b - +
    // wheel3 Left : b  f + +
    // wheel4 Right: f  b - -
    char m_older_L_Y = m_centervalue; // 1/3
    //               0 15 補正前 補正後
    // wheel1 Left : f  b - +
    // wheel2 Right: f  b - +
    // wheel3 Left : f  b - -
    // wheel4 Right: f  b - -
    // wheel1とwheel2は、wheel3とwheel4に対して、正回転の向きが逆になるので負号を逆にする
    // ロールはwheel1とwheel2の向きにそろえるので、wheel3とwheel4の負号を逆にする
    char m_older_R_X = m_centervalue; // 1/3
    //               0 15 補正前 補正後
    // wheel1 Left : b  f + +
    // wheel2 Right: b  f + +
    // wheel3 Left : f  b - +
    // wheel4 Right: f  b - +
//    char m_older_R_Y = m_centervalue; // 未使用

protected:

};

#endif
