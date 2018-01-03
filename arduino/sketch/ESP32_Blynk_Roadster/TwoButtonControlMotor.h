#ifndef _TwoButtonControlMotor_h
#define _TwoButtonControlMotor_h

#define DEF_MIN_VAL 0
#define DEF_STOP_VAL 90
#define DEF_MAX_VAL 180
#define DEF_STEP_VAL 1
#define DEF_MIN_TRIM_VAL (DEF_STOP_VAL-15)
#define DEF_MAX_TRIM_VAL (DEF_STOP_VAL+15)

class TwoButtonControlMotor{
// 前進、後進によるモーター制御変数クラス
// Arduinoから使用するClass
public:
	//初期化処理(ステップ値、最小値、停止値、最大値の設定)
	void init(int step = DEF_STEP_VAL, int minval = DEF_MIN_VAL, int stopval = DEF_STOP_VAL, int maxval = DEF_MAX_VAL){
		m_val = m_step = m_defstep = step;
		m_minval = m_defminval = minval;
		m_stopval = m_defstopval = stopval;
		m_maxval = m_defmaxval = maxval;
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

	//現在の値を設定する
	bool setValue(int val){
		if(m_minval <= val && val <= m_maxval){
			m_val = val;
			return(true);
		}
		return(false);
	}

	//現在の値を取得する
	int getValue(void){
		return(m_val);
	}

	//加速、減速時のステップを指定した値で更新する、0から(m_maxval - m_minval)/2の範囲内で指定する
	void setStepval(int step = DEF_STEP_VAL){
		if(0 < step && step < (m_maxval - m_minval)/2 ){
			m_step = step;
		}
	}

	//加速、減速時のステップを取得する
	int getStepval(void){
		return(m_step);
	}

	//最小値、停止値、最大値の設定を1度増加する
	void upTrim(void){
		if(m_stopval < DEF_MAX_TRIM_VAL){
			m_minval++;
			m_stopval++;
			m_maxval++;
		}
	}

	//最小値、停止値、最大値の設定を1度減少する
	void downTrim(void){
		if(DEF_MIN_TRIM_VAL < m_stopval){
			m_minval--;
			m_stopval--;
			m_maxval--;
		}
	}

	//最小値、停止値、最大値を設定する
	void setTrim(int trim){
		m_trim = trim;
		if(trim < DEF_MAX_TRIM_VAL){
			if(DEF_MIN_TRIM_VAL < trim){
				m_minval = m_defminval + trim;
				m_stopval = m_defstopval + trim;
				m_maxval = m_defmaxval + trim;
			}else{
				m_minval = m_defminval + DEF_MIN_TRIM_VAL;
				m_stopval = m_defstopval + DEF_MIN_TRIM_VAL;
				m_maxval = m_defmaxval + DEF_MIN_TRIM_VAL;
			}
		}else{
			m_minval = m_defminval + DEF_MAX_TRIM_VAL;
			m_stopval = m_defstopval + DEF_MAX_TRIM_VAL;
			m_maxval = m_defmaxval + DEF_MAX_TRIM_VAL;
		}
	}

	//最小値、停止値、最大値の設定を取得する
	int getTrim(void){
		return(m_trim);
	}

	//最小値を取得する
	int getMinval(void){
		return(m_minval);
	}
	//最大値を取得する
	int getMaxval(void){
		return(m_maxval);
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

	//緊急停止
	int QuickStop(void){
		return(m_val = m_stopval);
	}

private:
	int m_defminval = DEF_MIN_VAL;
	int m_defstopval = DEF_STOP_VAL;
	int m_defmaxval = DEF_MAX_VAL;
	int m_defstep = DEF_STEP_VAL;
	int m_minval = m_defminval;
	int m_stopval = m_defstopval;
	int m_maxval = m_defmaxval;
	int m_val = m_stopval;
	int m_step = m_defstep;
	int m_trim = 0;

protected:

};

#endif
