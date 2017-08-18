#ifndef _TwoButtonControlMotor_h
#define _TwoButtonControlMotor_h

class TwoButtonControlMotor{
// 前進、後進によるモーター制御変数クラス
// Arduinoから使用するClass
public:
    //ステップ値、最小値、停止値、最大値の設定
    void init(int step = 1, int minval = 0, int stopval = 90, int maxval = 180){
      m_val = m_step = step;
      m_minval = minval;
      m_stopval = stopval;
      m_maxval = maxval;
    }

    //m_minvalからm_maxvalの範囲内でm_stepずつm_valを加速する
    int operator ++() {
      m_val += m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
      return(m_val);
    }

    //m_minvalからm_maxvalの範囲内でm_stepずつm_valを加速する
    void operator ++(int n){
      m_val += m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
    }

    //m_minvalからm_maxvalの範囲内でm_stepずつm_valを減速する
    int operator --() {
      m_val -= m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
      return(m_val);
    }

    //m_minvalからm_maxvalの範囲内でm_stepずつm_valを減速する
    void operator --(int n){
      m_val -= m_step;
      if(m_val < m_minval){
        m_val = m_minval;
      }else if(m_maxval < m_val){
        m_val = m_maxval;
      }
    }

    //加速、減速時のステップを指定した値で更新する、0から(m_maxval - m_minval)/2の範囲内で指定する
    void setStepval(int step = 1){
      if(0 < step && step < (m_maxval - m_minval)/2 ){
        m_step = step;
      }
    }

    //現在の値を取得する
    int getValue(void){
      return(m_val);
    }

    //加速、減速時のステップを取得する
    int getStepval(void){
      return(m_step);
    }

    //停止値を取得する
    int getStopval(void){
      return(m_stopval);
    }

    //停止値にm_valをm_stepずつ近づける
    int Stop(void){
      if(m_val < m_stopval){
        operator++();
        if(m_val > m_stopval){
          m_val = m_stopval;
        }
      }else if(m_stopval < m_val){
        operator--();
        if(m_stopval > m_val){
          m_val = m_stopval;
        }
      }
      return(m_val);
    }

    int QuickStop(void){
      return(m_val = m_stopval);
    }
private:
    int m_minval = 0, m_stopval = 90, m_maxval = 180;
    int m_val = m_stopval;
    int m_step = 1;

protected:

};

#endif