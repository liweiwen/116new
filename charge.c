#include "HT66F0185.h"
#include "cmp116new.h"
#include "charge.h"

uchar charge_sta;//充电状态
uchar charge_pwm;//调节充电电压
uchar Charge_cnt;//充电计数
uchar Charge_cnt2;//充电计数2
uchar Charge_cnt3;//充电计数3
uchar I_CHARGE_SET;//充电电流阀值
uchar Input_cnt[2];// 输入信号检测计时
uchar JudgeQC2_0Cnt1;// 检测QC2.0计数1
uchar JudgeQC2_0Cnt2;// 检测QC2.0计数2
uchar uc9VChargeCnt;// 9V充电计时
uchar InVolSta;// 0:充电器不支持QC2.0 1:充电器支持9V充电 2:充电器支持12V充电
uchar I_CHARGE_offset;// 充电电流校正值


//PWM初始化
void Init_CHARGE_PT0(void)
{
    _tmpc = 0x01;//PTP0
    _tm0c1 = 0b10101000;//PWM模式，高有效
    _tm0c0 = 0b00010000;//运行,Fsys,ON,NC,NC,NC 
    tp0_ccrp = 0x01;      //PWM频率Fsys/256=16M/256=62.5KHz
    tp0_ccra = 0x00;      //占空比ccra/ccrp=64/256=25%
    _t0on = 1;      //开始计数ON
}

//调节PWM
void AdjustPWM(uint current, uint target, uint offset)
{
	if(current < (target - offset))
	{
		if(charge_pwm < CHARGE_MAX_PWM)
		{
			charge_pwm++;
		}
	}
	else if(current > target)
	{
		if(charge_pwm > 0)
		{
			charge_pwm--;
		}
	}
}

//输入信号检测
void Check_Signal(void)
{
	if(VIN_IN == 0)
	{
		if(InVol >= V_IN_3_0V)
		{
			Input_cnt[1] = 0;
			if(++Input_cnt[0] >= 2)
			{
				f_in_ok = 1;
			}
		}
		else if(InVol <= V_IN_2_0V)
		{
			Input_cnt[0] = 0;
			if(++Input_cnt[1] >= 2)
			{
				f_in_ok = 0;
				f_charge = 0;
				f_charge_ok = 0;
				f_charge_en = 0;
				f_QC2_0 = 0;
				f_judgeQC20_ok = 0;
				charge_sta = C_STANDBY;
				uc9VChargeCnt = 0;
				InVolSta = 0;
				Charge_cnt3 = 0;
			}
		}
	}
	else
	{
		f_in_ok = 0;
		f_charge = 0;
		f_charge_ok = 0;
		f_charge_en = 0;
		f_QC2_0 = 0;
		f_judgeQC20_ok = 0;
		charge_sta = C_STANDBY;
		uc9VChargeCnt = 0;
		InVolSta = 0;
		Charge_cnt3 = 0;
	}
}

//判断是否支持QC2.0
void JudgeQC2_0(void)
{
	if((f_in_ok) && (f_charge_ok == 0) && (InVol >= V_IN_3_0V))
	{
		USB_D_PLUS_EN1 = 1;
		USB_D_MINUS_EN2 = 1;
		USB_D_MINUS_EN3 = 0;
		if((QC_D_Vol >= QC_MINUS_0_4V) && (QC_D_Vol <= QC_MINUS_0_8V))
		{
			JudgeQC2_0Cnt1++;
		}
		if(++JudgeQC2_0Cnt2 >= T_2S)
		{
			if(QC_D_Vol <= QC_MINUS_0_2V)
			{
				if(JudgeQC2_0Cnt1 >= T_1S)
				{
					f_QC2_0 = 1;
				}
				else f_QC2_0 = 0;
				JudgeQC2_0Cnt1 = 0;	
			}
			else f_QC2_0 = 0;
			JudgeQC2_0Cnt2 = 0;
			f_judgeQC20_ok = 1;
		}
	}
	else
	{
		f_QC2_0 = 0;
		f_judgeQC20_ok = 0;
		JudgeQC2_0Cnt1 = 0;
		JudgeQC2_0Cnt2 = 0;
		f_charge_en = 0;
		USB_D_PLUS_EN1 = 0;
		USB_D_MINUS_EN2 = 1;
		USB_D_MINUS_EN3 = 0;
	}
}

//QC2.0充电控制
void QC2_0_Charge(void)
{
	if((f_QC2_0) && (InVolSta == CHARGE_5V) && (f_bat_low == 0))
	{
		USB_D_PLUS_EN1 = 0;
		USB_D_MINUS_EN2 = 0;
		USB_D_MINUS_EN3 = 1;
		if(++uc9VChargeCnt > T_2S)
		{
			if((InVol >= V_IN_7V) && (InVol <= V_IN_10V))
			{
				f_charge_en = 1;
				InVolSta = CHARGE_9V;
			}
			else
			{
				USB_D_PLUS_EN1 = 1;
				USB_D_MINUS_EN2 = 1;
				USB_D_MINUS_EN3 = 1;
				f_charge_en = 1;
				InVolSta = CHARGE_5V;
			}
		}
		else
		{
			uc9VChargeCnt++;
		}
	}
	else if(InVolSta == CHARGE_5V)
	{
		USB_D_PLUS_EN1 = 0;
		USB_D_MINUS_EN2 = 1;
		USB_D_MINUS_EN3 = 0;
		uc9VChargeCnt = 0;
		f_charge_en = 1;
		InVolSta = CHARGE_5V;
	}
}

//控制充电电流
void Charge_ctrl(void)
{
	switch(InVolSta)
	{
		case CHARGE_5V:
		{
			if(InVol > V_IN_4_85V)
			{
				I_CHARGE_SET = I_CHARGE_2A;
			}
			else if(InVol < V_IN_3_2V)
			{
				I_CHARGE_SET = I_CHARGE_1A;
				if(++Charge_cnt3 >= charge_4s)
				{
					I_CHARGE_SET = I_CHARGE_0_5A;
				}
			}
			else
				Charge_cnt3 = 0;
		}
		break;
		case CHARGE_9V:
		{
			if(InVol > V_IN_8V)
			{
				I_CHARGE_SET = I_CHARGE_2A;
			}
			else if(InVol < V_IN_6V)
			{
				I_CHARGE_SET = I_CHARGE_1A;
				if(++Charge_cnt3 >= charge_4s)
				{
					InVolSta = CHARGE_5V;
				}
			}
			else
				Charge_cnt3 = 0;
		}
		break;
		
		default:
		{
			if(InVol > V_IN_4_85V)
			{
				I_CHARGE_SET = I_CHARGE_2A;
			}
			else if(InVol < V_IN_3_2V)
			{
				I_CHARGE_SET = I_CHARGE_1A;
				if(++Charge_cnt3 >= charge_4s)
				{
					I_CHARGE_SET = I_CHARGE_0_5A;
				}
			}
			else
				Charge_cnt3 = 0;
		}
		break;
	}
	if(f_bat_low)
	{
		I_CHARGE_SET = I_CHARGE_0_3;
		I_CHARGE_offset = I_CHARGE_0_05;
	}
	if(V_BOOST >= V_13_6V)
	{
		charge_sta = C_CHARGE_STOP;
	}
	switch(charge_sta)
	{
		case C_STANDBY:
		{
			f_charge = 0;
			if((f_in_ok) && (f_bat_ovp==0))
			{
				if(BatVol_real <= BAT_12_0V)
				{
					charge_sta = C_CHARGE_CC;
				}
				else if(BatVol_real < BAT_12_6V)
				{
					charge_sta = C_CHARGE_CV;
				}
				if(charge_sta)
				{
					f_charge = 1;
					Charge_cnt = 0;
					Charge_cnt2 = 0;
					charge_pwm = 0x40;
				}
			}
			else
			{
				f_charge = 0;
				charge_pwm = 0;
				if(f_bat_ovp)
				{
					charge_sta = C_CHARGE_STOP;
				}
			}
		}
		
		case C_CHARGE_CC:
		{
			LowPowerDelay = T_30S;
			AdjustPWM(ChargeCur, I_CHARGE_SET, I_CHARGE_offset);
			if((f_bat_ovp) || (f_in_ok == 0))
			{
				charge_sta = C_STANDBY;
			}
			if((ChargeCur <= I_CHARGE_0_2) && (f_bat_low == 0))
			{
				if(++Charge_cnt >= charge_delay)
				{
					Charge_cnt = 0;
					charge_sta = C_STANDBY;
				}
			}
			else
			{
				Charge_cnt = 0;
			}
			if(BatVol_real >= BAT_12_35V)
			{
				charge_sta = C_CHARGE_CV;
				Charge_cnt = 0;
			}
		}
		break;
		
		case C_CHARGE_CV:
		{
			LowPowerDelay = T_30S;
			AdjustPWM(BatVol_real, BAT_12_4V, V_CHARGE_OFFSET);
			if((f_bat_ovp) || (f_in_ok == 0))
			{
				charge_sta = C_STANDBY;
			}
			if((ChargeCur <= I_CHARGE_0_2) && (f_bat_low == 0))
			{
				if(++Charge_cnt >= charge_delay)
				{
					charge_sta = C_CHARGE_STOP;
				}
			}
			else
			{
				Charge_cnt = 0;
			}
			if(ChargeCur >= I_CHARGE_SET)
			{
				charge_sta = C_CHARGE_CC;
			}
		}
		break;
		
		case C_CHARGE_STOP:
		{
			f_charge_ok = 1;
			f_charge = 0;
			Charge_cnt = 0;
			charge_pwm = 0;
			if(f_in_ok == 0)
			{
				charge_sta = C_STANDBY;
			}
			//低压返回充电
			if(BatVol_real <= BAT_12_0V)
			{
				if(++Charge_cnt2 >= charge_16s)
				{
					Charge_cnt2 = 0;
					charge_sta = C_STANDBY;
					f_charge_ok = 0;
				}
			}
		}
		break;
		
		default:
		{
			charge_sta = C_STANDBY;
		}
		break;
	}
	if(charge_pwm >= CHARGE_MAX_PWM)
    {
        charge_pwm = CHARGE_MAX_PWM;
    }
    tp0_ccra = charge_pwm;
}

