#include "HT66F0185.h"
#include "cmp116new.h"
#include "comm.h"
#include "charge.h"

uchar T_6MS_cnt;//6ms计数
uchar T_20MS_cnt;//20ms计数
uint  Out5VCur1_tmp;// 5V输出电流采样缓存1
uint  Out5VCur1;// 5V输出电流1
uint  Cur5V_SC1;// 5V输出采样值1
uint  Out5VCur1_offset;// 5V输出电流零漂1
uchar Short5VNum1;// 5V短路计时1
uint  Out5VCur2_tmp;// 5V输出电流采样缓存2
uint  Out5VCur2;// 5V输出电流2
uint  Cur5V_SC2;// 5V输出采样值2
uint  Out5VCur2_offset;// 5V输出电流零漂2
uchar Short5VNum2;// 5V短路计时2
uint  ChargeCur_tmp;//充电电流采样缓存
uint  LowPowerDelay;//低功耗计时
uchar BAT_AD;
uchar BAT_cnt;
uint  BatVol_tmp[4];// 电池电压采样缓存
uint  BatVol[3];// 每一节的电池电压
uint  BatVol_real;// 电池电压
uchar T250msCnt;// 250MS计时
uchar ledcnt;// LED闪烁计时
uchar key_time;//按键计时
uchar OC5V_cnt1;// 5V过流计时1
uchar OC5V_cnt2;// 5V过流计时2
uchar OC5V_cnt3;// 5V过流计时3
uchar Bat_clear_cnt;//电池电压检测清0计数
uchar PowerOn_cnt;
DEFINE_ISR(isr_mf1f,0x10);//多功能中断
DEFINE_ISR(isr_tb1, 0x20);//定时器

_bits flag_0;
_bits flag_1;
_bits flag_2;
_bits flag_3;
_bits flag_4;

void delay(uint i)
{
	while(i)
	{
		i--;
		_clrwdt();
		
	}
}

void bat_led_off(void)
{
	LED1 = 0;
	LED2 = 0;
	LED3 = 0;
    LED4 = 0;
}

void bat_led1_on(void)
{
	LED1 = 1;
	LED2 = 0;
	LED3 = 0;
	LED4 = 0;
}

void bat_led1_2_on(void)
{
	LED1 = 1;
	LED2 = 1;
	LED3 = 0;
	LED4 = 0;
}

void bat_led1_2_3_on(void)
{
	LED1 = 1;
	LED2 = 1;
	LED3 = 1;
	LED4 = 0;
}

void bat_led_on(void)
{
	LED1 = 1;
	LED2 = 1;
	LED3 = 1;
	LED4 = 1;
}

//IO初始化
void InitIO(void)
{
    _cpc |= 0x08;// PA3配置为普通IO口
    _pac0  = 0;// PWM_OUT 充电PWM
    _pac1  = 1;// VIN_IN
    _pac2  = 0;// 5V_EN2 5V输出使能2 
    _pac3  = 1;// /KEY按键
    _pac4  = 1;// BAT_SCAN电池电压 AN3
    _pac5  = 1;// V_BOOST AN4
    _pac6  = 1;// NTC AN5
    _pac7  = 1;// QC2.0_D- AN6
    _pawu = 0x00;//唤醒使能 1:使能 0:除能  按键唤醒
    _papu = 0x0A;// PA3按键上拉，PA1 充电输入
    _pa   = 0x00;

    _cpc &= 0x7F;// PB5和PB6配置为普通IO口
    _pbc0 = 1;// VIN_VOL充电电压 AN0
    _pbc1 = 1;// 5V_ISN2 5V输出2电流 AN1
    _pbc2 = 1;// CHAR_ISN 充电电流 AN2
    _pbc3 = 1;// 5V_ISN1 5V输出1电流 AN7
    _pbc4 = 0;// COMM 通讯
    _pbc5 = 0;// LIGHT_EN 手电灯控制
    _pbc6 = 0;// NTC_EN
    _pbpu = 0x10;  //PB4 COMM通讯口上拉
    _pb  = 0x00;
    
    _pcc0 = 0;// BAT1_EN
    _pcc1 = 0;// BAT2_EN
    _pcc2 = 0;// BAT3_EN
    _pcc3 = 0;// 5V_EN1 5V输出使能1
    _pcc4 = 0;// EN1
    _pcc5 = 0;// EN2
    _pcc6 = 0;// EN3
    _pcpu = 0x00;//
    _pc  = 0;

    _pdc0 = 0;// LED4
    _pdc1 = 0;// LED3
    _pdc2 = 0;// LED2
    _pdc3 = 0;// LED1
    _pdpu = 0x00;
    _pd  = 0;
	BAT1_EN=1;
    BAT2_EN=0;
    BAT3_EN=0;
    NTC_EN = 1;
}

void InitTbc0_1(void)
{
    _tbc = 0b11010100;//fsys/4 tb1=2^13 tb0=2^12
    _tb1e = 1;//定时器1溢出中断使能
}

void InitSTM_TP1(void)
{
    _intc1 = 0x01;//多功能中断1使能
    _tm1c0 = 0b00000000;//fsys/4
    _tm1c1 = 0b11000001;//定时/计数器,与CCRA比较
    ptm1_ccrd = 0x00;
    ptm1_ccra = 0xFF;
    _t1on = 1;
    _t1ae = 1;
}

void InitPara(void)
{
    _bp=0;
    _mp1=0x80;
    for(_tblp=0x80;_tblp<0xff;_tblp++)
    {
        _iar1=0;
        _mp1++;
    }
    _mp1 = 0xFF;
    _tblp = 0xFF;
    _iar1=0;

    _bp=1;
    _mp1=0x80;
    for(_tblp=0x80;_tblp<0xff;_tblp++)
    {
        _iar1=0;
        _mp1++;
    }
    _mp1 = 0xFF;
    _tblp = 0xFF;
    _iar1=0;
    PowerOn_cnt = T_500MS;
    LowPowerDelay = T_30S;
}


//ADC采样初始化
void InitADC(void)
{
	_acerl = 0xFF;//采样管脚AN0-AN7
	_sadc0 = 0x00; //清零
	_sadc1 = 0b00000100;//Fsys/16
	_sadc2 = 0xCB;//基准电压3.12V
	_sadc0 = 0x33;//使能ADC，AN3
}

//获取adc值
uint getadc(uchar ad_ch)
{
	uint temp = 0;
	uchar i;
	_sadc0 = 0x30 | ad_ch;
	for(i = 0; i < AVERAGE_NUM; i++)
	{
		_start = 0;
		_start = 1;
		_start = 0;
		while(_adbz);
		temp += sado;
	}
	return(temp/AVERAGE_NUM);
}

//获取各节电压
void set_BAT_AD(uchar BAT_ch)
{
    switch(BAT_ch)
    {
    	case 0:
    	{
    		BAT1_EN=1;
    		BAT2_EN=0;
    		BAT3_EN=0;
    	}
    	break;
    	case 1:
    	{
    		BAT1_EN=0;
    		BAT2_EN=1;
    		BAT3_EN=0;
    	}
    	break;
    	case 2:
    	{
    		BAT1_EN=0;
    		BAT2_EN=0;
    		BAT3_EN=1;
    	}
    	break;
    	case 3:
    	{
    		BAT1_EN=0;
    		BAT2_EN=0;
    		BAT3_EN=0;
    	}
    	break;
    	default:
    	{
    		BAT1_EN=0;
    		BAT2_EN=0;
    		BAT3_EN=0;
    	}
    	break;
    }
}


//ADC采样
void SamplingADC(void)
{
	_adcen = 1;
	uint temp;
	NTC_tmp += getadc(AN_NTC);
	QC_D_Vol_tmp += getadc(AN_QC_D);
	InVol_tmp += getadc(AN_VIN);
	V_BOOST_tmp += getadc(AN_BOOST);
	temp = getadc(AN_CHARGE_I);
	ChargeCur_tmp += temp;
	Cur5V_SC1 = getadc(AN_OUT1_5V_I);
	Out5VCur1_tmp += Cur5V_SC1;
	Cur5V_SC2 = getadc(AN_OUT2_5V_I);
	Out5VCur2_tmp += Cur5V_SC2;
		
	BatVol_tmp[BAT_AD] += getadc(AN_BAT_VOL);
	if(f_charge)
	{
		if(BatVol_tmp[BAT_AD] > (temp/(BAT_AD + 1)))
		{
			BatVol_tmp[BAT_AD] -= temp/(BAT_AD + 1);
		}
		else BatVol_tmp[BAT_AD] = 0;
	}

	if(++BAT_AD >= 4)
	{
		if(++BAT_cnt >= 4)
		{
			BAT_cnt = 0;
			//检测电池故障
			if(BatVol_tmp[3] > 1000)
			{
				if(++Bat_clear_cnt >= 5)
				{
					Bat_clear_cnt = 0;
					f_BatVol_err = 1;
				}
				else
				{
					Bat_clear_cnt = 0;
					f_BatVol_err = 0;
				}
			}
			
			//第三节电池电压
			if(BatVol_tmp[2] > (BatVol_tmp[1]*2/3))
			{
				BatVol[2] = (BatVol_tmp[2]*3) - (BatVol_tmp[1]*2);
				BatVol[2] /= 12;
			}
			else BatVol[2] = 0;
			
			//第二节电池电压
			if(BatVol_tmp[1] > (BatVol_tmp[0]/2))
			{
				BatVol[1] = (BatVol_tmp[1]*2) - BatVol_tmp[0];
				BatVol[1] /= 12;
			}
			else BatVol[1] = 0;
			
			//第一节电池电压
			BatVol[0] = BatVol_tmp[0] / 12;
			
			//总电压
			BatVol_real = BatVol_tmp[2]/4;
			
			BatVol_tmp[0] = 0;
			BatVol_tmp[1] = 0;
			BatVol_tmp[2] = 0;
			BatVol_tmp[3] = 0;
		}
		BAT_AD = 0;
	}
	set_BAT_AD(BAT_AD);
	if(Cur5V_SC1 > temp)
    {
		Cur5V_SC1 = Cur5V_SC1 - temp;
    }
    else Cur5V_SC1 = 0;
    if(Cur5V_SC1 >= SHORT_CUR1)
    {
        Short5VNum1++;
        if(Short5VNum1 >= 2)
        {
            f_5v_ocp1 = 1;
        }
    }
    if(Cur5V_SC2 > temp)
    {
		Cur5V_SC2 = Cur5V_SC2 - temp;
    }
    else Cur5V_SC2 = 0;
    if(Cur5V_SC2 >= SHORT_CUR2)
    {
        Short5VNum2++;
        if(Short5VNum2 >= 2)
        {
            f_5v_ocp2 = 1;
        }
    }
    
    if(++ADC_cnt >= 8)  //采样8次
    {
        ADC_cnt = 0;
        NTC_VALUE = NTC_tmp >> 3;
        InVol = InVol_tmp >> 3;
        V_BOOST = V_BOOST_tmp >> 3;
        QC_D_Vol = QC_D_Vol_tmp >> 3;
        ChargeCur = ChargeCur_tmp >> 3;
        if(f_charge)
        {
        	if(Out5VCur1_tmp > ChargeCur_tmp)
        	{
        		Out5VCur1_tmp = Out5VCur1_tmp - ChargeCur_tmp;
        	}
        	else Out5VCur1_tmp = 0;
        	if(Out5VCur2_tmp > ChargeCur_tmp)
        	{
        		Out5VCur2_tmp = Out5VCur2_tmp - ChargeCur_tmp;
        	}
        	else Out5VCur2_tmp = 0;
        }
        Out5VCur1 = Out5VCur1_tmp >> 3;
        Out5VCur2 = Out5VCur2_tmp >> 3;
        if(f_in_ok == 0)
        {
            ChargeCurOffset = ChargeCur;   //ChargeCurOffset充电漏电流
        }
		// 充电电流去零漂
        if(ChargeCur > ChargeCurOffset)
        {
            ChargeCur = ChargeCur - ChargeCurOffset;
        }
        else
        {
            ChargeCur = 0;
        }
        // 5V输出电流去零漂
        if(PowerOn_cnt)
        {
            Out5VCur1_offset = Out5VCur1; 
            Out5VCur2_offset = Out5VCur2; 
        }
        
        if(Out5VCur1 > Out5VCur1_offset)
        {
            Out5VCur1 -= Out5VCur1_offset;
        }
        else
        {
            Out5VCur1 = 0;
        }
        if(Out5VCur2 > Out5VCur2_offset)
        {
            Out5VCur2 -= Out5VCur2_offset;
        }
        else
        {
            Out5VCur2 = 0;
        }
        
        ChargeCur_tmp = 0;
        Out5VCur1_tmp = 0;
        Out5VCur2_tmp = 0;
        InVol_tmp = 0;
        NTC_tmp = 0;
        QC_D_Vol_tmp = 0;
        V_BOOST_tmp = 0;
        f_sampling_ok = 1;
    }
    _adcen = 0;
}
	
//电池容量灯显示
void bat_led_ctrl(void)
{ 	
    if(f_keyok)
    {
    	f_keyok = 0;
    	f_5v_ocp1 = 0;
    	Short5VNum1 = 0;
    	f_5v_ocp2 = 0;
    	Short5VNum2 = 0;
    	f_temp_err = 0;
    	f_bat_err = 0;
    	f_BatVol_err = 0;
  
    }
    else if((f_charge_temp && f_in_ok) || f_discharge_temp || f_BatVol_err)	
    {
    	if(f_250MS_flash)
        {
            bat_led_off();
        }
        else 
        {
        	bat_led_on();
        }
    }
    else if((f_bat_uvp) && (f_charge==0))//电池电压低,LED1闪烁，其余熄灭
    {
        if(f_250MS_flash)
        {
            bat_led1_on();
        }
        else
        {
        	bat_led_off();
        }
    }
    else
    {
        switch(BatLevel)
        {
            case 0: //电量低于25%LED1亮，其余熄灭；充电LED1闪烁
            {
                if((BatVol_real >= (BATSOC_25+35))||(BatLevel < SaveBatLevel))
                {
                    BatLevel = 1;
                }
                
                if(f_charge)
                {
                    if(f_500MS_flash)
                    {
            			bat_led1_on();
        			}
        			else
        			{
        				bat_led_off();
                	}
                }	
                else
                {
                    bat_led1_on();
                }
            }
            break;

            case 1://电量25%-50%LED1、2亮，其余熄灭；充电LED2闪烁
            {
                if((BatVol_real >= (BATSOC_50+18))||(BatLevel < SaveBatLevel))
                {
                    BatLevel = 2;
                }
                else if(BatVol_real < (BATSOC_25-DISCHARGE_OFFSET))
                {
                    BatLevel = 0;
                }
                if(f_charge)
                {
                    if(f_500MS_flash)
                    {
                    	bat_led1_on();
                    }
                    else
                    {
                    	bat_led1_2_on();
                    }
                }
                else
                {
                	bat_led1_2_on();
                }
            }
            break;

            case 2://电量50%-75%LED1、2、3亮，其余熄灭；充电LED3闪烁
            {
                if((BatVol_real >= (BATSOC_75+25)) || (BatLevel < SaveBatLevel))
                {
                    BatLevel = 3;
                }
                else if(BatVol_real < (BATSOC_50-DISCHARGE_OFFSET))
                {
                    BatLevel = 1;
                }
                if(f_charge)
                {
                    if(f_500MS_flash)
                    {  
                    	bat_led1_2_on();
                    }
                    else
                    {
                    	bat_led1_2_3_on();
                    }
                }
                else
                {
                	bat_led1_2_3_on();
                }
            }
            break;

            case 3://电量75%-100%LED1、2、3、4亮；充电LED4闪烁
            {
                if(BatVol_real < (BATSOC_75-DISCHARGE_OFFSET))
                {
                    BatLevel = 2;
                }
                if(f_charge)
                {
                    if(f_500MS_flash)
                    {
                        bat_led_on();
                    }
                    else
                    {
                    	bat_led1_2_3_on();
                    }
                }
                else
                {
                    bat_led_on();
                }
            }
            break;

            default:
            {
                bat_led_off();
            }
            break;
        }
        if(BatLevel>=SaveBatLevel)
        {
            SaveBatLevel = 0;
        }
    }
}

//电池电压处理
void BatVol_ctrl(void)
{
    //电池欠压
    if((BatVol_real <= BAT_9_0V) || (BatVol[0] <= BAT_3_0V)
    ||(BatVol[1] <= BAT_3_0V) || (BatVol[2] <= BAT_3_0V))
    {
        BatUVPCnt++;
        if(BatUVPCnt >= T_5S)
        {
            BatUVPCnt = 0;
            f_bat_uvp = 1;
            senddata |= 0x0002;
        }
    }
    else
    {
        BatUVPCnt = 0;
        f_bat_ok = 1;
        if(f_bat_uvp)
        {
            if(((f_charge) || (f_charge_ok)) && (BatVol_real >= BAT_10_2V) && (BatVol[0] >= BAT_3_4V)
                && (BatVol[1] >= BAT_3_4V) && (BatVol[2] >= BAT_3_4V))
            {
                f_bat_uvp = 0;
                senddata &= 0xFFFD;
            }
        }
    }

    //电池过压
    if((BatVol[0] >= BAT_4_17V) || (BatVol[1] >= BAT_4_17V) || (BatVol[2] >= BAT_4_17V))
    {
        BatOVPCnt++;
        if(BatOVPCnt >= T_2S)
        {
            BatOVPCnt = 0;
            f_bat_ovp = 1;
        }
    }
    else if((BatVol[0] <= BAT_4_14V)&&(BatVol[1] <= BAT_4_14V)&&(BatVol[2] <= BAT_4_14V))
    {
        BatOVPCnt = 0;
        f_bat_ovp = 0;
    }
    
    //电池电压过低，涓流充电
    if((BatVol[0] <= BAT_3_0V) || (BatVol[1] <= BAT_3_0V) || (BatVol[2] <= BAT_3_0V))
    {
        if(++BatLVPCnt >= T_5S)
        {
            BatLVPCnt = 0;
           	f_bat_low = 1;
        }
        if((BatVol[0] <= BAT_2_0V) || (BatVol[1] <= BAT_2_0V) || (BatVol[2] <= BAT_2_0V))
        {
        	if(++BatNVPCnt >= T_10S)
        	{
        		f_bat_err = 1;
        	}
        }
        else
        {
        	BatNVPCnt = 0;
        	f_bat_ok = 1;
        }
    }
    else if((BatVol[0] >= BAT_3_05V)&&(BatVol[1] >= BAT_3_05V)&&(BatVol[2] >= BAT_3_05V)&&f_bat_low)
    {
    	BatNVPCnt = 0;
        BatLVPCnt = 0;
        f_bat_low = 0;
        f_bat_err = 0;
        I_CHARGE_SET = I_CHARGE_2A;
        I_CHARGE_offset = I_CHARGE_0_15;
    }
}

//温度处理
void NTC_VALUE_ctrl(void)
{
   //温度(充电保护)
    if((NTC_VALUE <= CHARGE_RES_UCP)&&(NTC_VALUE >= CHARGE_RES_OCP))
    {
        f_charge_temp = 0;
    }
    else if((NTC_VALUE >= CHARGE_UCP)||(NTC_VALUE <= CHARGE_OCP))
    {
        if(f_charge)
        {
            f_charge_temp = 1;
        }
    }

    //温度(放电保护)
    if((NTC_VALUE >= DISCHARGE_RES_OCP)&&(NTC_VALUE<(SHORT_RT-25)))
    {
        f_discharge_temp = 0;
        senddata &= 0xFFFE;
    }
    else if((NTC_VALUE <= DISCHARGE_OCP)||(NTC_VALUE>SHORT_RT))
    {
        f_discharge_temp = 1;
        f_light = 0;
        senddata |= 0x0001;
    }
}


void KeyScan(void)
{
	if(KEY_VALUE == 0)
    {
        if(f_key == 0)
        {
        	f_key = 1;
        }
        else
        {
        	LowPowerDelay = T_30S;
        	f_keypress = 1;
        	if((++key_time >= KEY_LONGPRESS_OK) && (f_longkey == 0))
        	{
        		f_longkey = 1;
        		f_keyok = 1;
        		key_time = 0;
        		key_state = longkey;     
        	}
        }
    }
    else
    {
    	f_key = 0;      
    	key_time = 0;
    	if((f_keypress == 1) && (f_longkey == 0))
    	{
    		f_longkey = 0;
    		f_keyok = 1;
    		f_keypress = 0;
    		key_state = shortkey;
    	}
    	f_keypress = 0;
    	f_longkey = 0;
    }    
        	
    switch (key_state)
    {
    	case shortkey:
    	{
    		if(light_sta != LIGHT_ALWAYS_OFF)     
            {
            	if(++light_sta >= 4)   
            	{
            		light_sta = LIGHT_ALWAYS_OFF;
            	}
            }
    	}
    	break;
    	
    	case longkey:
        {
            if((f_light == 0) && (f_bat_uvp == 0))
            {
                f_light = 1;
                light_sta = LIGHT_ALWAYS_ON;      
                light_cnt = 0;
                light_time = 0;
            }
            else if(f_light)
            {
            	f_light = 0;
                light_sta = LIGHT_ALWAYS_OFF;      
                light_cnt = 0;
                light_time = 0;
                LED_CTRL = 0;
            }
        }
        break;

        default: key_state = nokey;
        break;
    }
    key_state = nokey;
}

void led_ctrl(void)
{
    if((f_light) && (f_bat_uvp == 0))
    {
        switch(light_sta)
        {
        	case LIGHT_ALWAYS_OFF:
            {
                LED_CTRL = 0;
            	f_light = 0;
            }
            break;
            
            case LIGHT_ALWAYS_ON:
            {
                LED_CTRL = 1;
            }
            break;
            
            case LIGHT_STROBE_ON:
            {
                if(++light_cnt > T_80MS)
                {
                    light_cnt = 0;
                    LED_CTRL = ~LED_CTRL;
                }
            }
            break;
            
            case LIGHT_SOS_ON:
            {
                if((light_time >= 6) && (light_time <= 12))
                {
                    if(++light_cnt >= T_600MS)
                    {
                        light_cnt = 0;
                        light_time++;
                        LED_CTRL = ~LED_CTRL;
                    }
                }
                else if(light_time >= 18)
                {
                    LED_CTRL=0;
                    if(++light_cnt >= T_2S)
                    {
                        light_time = 0;
                    }
                }
                else if(++light_cnt >= T_250MS)
                {
                    light_cnt = 0;
                    light_time++;
                    LED_CTRL = ~LED_CTRL;
                }
            }
            break;
            
            default: break;
        }
    }
    else if(f_bat_uvp)
    {
        LED_CTRL = 0;
        f_light = 0;
    }
}

void OC_SC_ctrl(void)
{
	OVER_CUR1 = OVER_CUR_5V;
	SHORT_CUR1 = SHORT_CUR_5V;
	OVER_CUR2 = OVER_CUR_5V;
	SHORT_CUR2 = SHORT_CUR_5V;
    // 5V过流或者短路判断
	if(Out5VCur1 >= OVER_CUR1)
    {
        if(++OC5V_cnt1 >= OC_5VOS_TIME)
       	{
         	  f_5v_ocp1 = 1;
       	}
    }
    else
    {
      	 OC5V_cnt1 = 0;
    }
    if(Out5VCur2 >= OVER_CUR2)
    {
        if(++OC5V_cnt2 >= OC_5VOS_TIME)
        {
         	  f_5v_ocp2 = 1;
        }
    }
    else
    {
      	 OC5V_cnt2 = 0;
   	}
    //QC3.0
   	if(((Out5VCur1 > OUT_I_2_6A) && (Out5VCur1 < OVER_CUR1)) || ((Out5VCur2 > OUT_I_2_6A) && (Out5VCur2 < OVER_CUR2)))
    {
    	if(++OC5V_cnt3 >= T_500MS)
    	{
    		OC5V_cnt3 = 0;
    		if((Out5VCur1+Out5VCur2) > OUT_I_4_2A)
    		{
    			if(Out5VCur1 > Out5VCur2)
    			{
    				f_5v_ocp2 = 1;
    			}
    			else f_5v_ocp1 = 1;
    		}
    	}
   	}
}


void out_ctrl(void)
{
    // 5V输出控制
    if((f_bat_uvp == 0) && (f_discharge_temp == 0) && (f_bat_err == 0) && (f_BatVol_err == 0))
    {
        if(f_5v_ocp1 == 0)
        {
        	OUT1_5V_EN = 1;
        }
        else OUT1_5V_EN = 0;
        if(f_5v_ocp2 == 0)
        {
        	OUT2_5V_EN = 1;
        }
        else OUT2_5V_EN = 0;
    }
    else
    {
        OUT1_5V_EN = 0;
        OUT2_5V_EN = 0;
    }
}

void low_power_mode(void)
{
		if(Out5VCur1 <= CUR_5V_200MA)
    	{
        	if(outlp_cnt[0] < T20_30S)
        	{
            	outlp_cnt[0]++;
        	}
        	else
        	{
            	f_5v_lp1 = 1;
        	}
        	outlp_cnt[1] = 0;
    	}
    	else
    	{
        	if(outlp_cnt[1] < T_250MS)
        	{
            	outlp_cnt[1]++;
        	}
        	else
        	{
            	f_5v_lp1 = 0;
            	LowPowerDelay = T_30S;
            	outlp_cnt[0] = 0;
        	}
    	}
    	if(Out5VCur2 <= CUR_5V_200MA)
    	{
        	if(outlp_cnt[2] < T20_30S)
        	{
            	outlp_cnt[2]++;
        	}
        	else
        	{
            	f_5v_lp2 = 1;
        	}
        	outlp_cnt[3] = 0;
    	}
    	else
    	{
        	if(outlp_cnt[3] < T_250MS)
        	{
            	outlp_cnt[3]++;
        	}
        	else
        	{
            	f_5v_lp2 = 0;
            	LowPowerDelay = T_30S;
            	outlp_cnt[2] = 0;
        	}
    	}
    if(f_5v_lp1 == 0)return;
    if(f_5v_lp2 == 0)return;
    if(f_light)return;
    if(LowPowerDelay)return;
    if((f_in_ok)&&(f_charge_ok == 0))return;
    NTC_EN = 0;
    f_low_power = 1;
    charge_sta = C_STANDBY;
    SaveBatLevel = BatLevel;
    BatLevel = 0;
    OUT1_5V_EN = 0;
    OUT2_5V_EN = 0;
    BAT1_EN = 0;
    BAT2_EN = 0;
    BAT3_EN = 0;
    LED_CTRL = 0;
    bat_led_off();
    PinTxRx_Path = 1;
    //关定时器中断
    _t1on = 0;
    _t1ae = 0;
    _tb1e = 0;
    _t0on = 0;
    _wdtc = 0b10101011;//关看门狗
    _smod = 0;
    _nop();_nop();_nop();
    _nop();_nop();_nop();
}

void wake_up(void)
{
	
    _smod = 1;
    _nop();_nop();_nop();
    _nop();_nop();_nop();
    NTC_EN = 1;
    PinTxRx_Path = 1;
    f_low_power = 0;
    f_bat_ok = 0;
    _wdtc = 0b01010011;//开看门狗
    _tb1e = 1;
    _t0on = 1;//启动定时器
    _t1on = 1;
    _t1ae = 1;
    //检测休眠信号恢复
    LowPowerDelay = T_30S;
    PowerOn_cnt = T_500MS;
    BAT1_EN = 1;
    BAT2_EN = 0;
    BAT3_EN = 0;
    outlp_cnt[0] = 0;
    outlp_cnt[1] = 0;
    outlp_cnt[2] = 0;
    outlp_cnt[3] = 0;
    //过流或者短路恢复
    f_5v_ocp1 = 0;
    Short5VNum1 = 0;
    f_5v_ocp2 = 0;
    Short5VNum2 = 0;
}


void main()
{
 	_emi = 0;
    _smod = 0x01;//fsys = fh(8M RC振荡器)
    _wdtc = 0x53;//使能看门狗，2^14分频2^14/16M==1024us溢出
    InitIO();
    InitADC();
    InitTbc0_1();
    Init_CHARGE_PT0();
    InitSTM_TP1();
    _emi = 1;
    InitPara();
	senddata = CableType;
	I_CHARGE_SET = I_CHARGE_2A;
    I_CHARGE_offset = I_CHARGE_0_15;
    delay(50000);
    BatVol[0] = getadc(AN_BAT_VOL);
    if(BatVol[0] <= bat_2_0V)
    {
       	f_bat_err = 1;
       	_sadc2 = 0x00;  //Vdd 
       	   
    }
    else
    {
   		f_bat_ok = 1;
   		tp0_ccra = 0x40;
   		_sadc2 = 0x00;  //Vdd 
    }
    while(1)
    {
        if(f_low_power == 0)
        {
        	_clrwdt();
        	if(f_6ms)
            {
            	SamplingADC();
            	f_6ms = 0;
            }
            if(f_20ms)
            {
                f_20ms = 0;
                Check_Signal();
                KeyScan();
                
                if(PowerOn_cnt)
                {
                    PowerOn_cnt--;
                    bat_led_on();
                }
                else
                {
                    if(f_sampling_ok)
                    {
                        f_sampling_ok = 0;
                        OC_SC_ctrl();
                        NTC_VALUE_ctrl();
                        if((f_charge_temp == 0)&&(f_BatVol_err == 0)&&(f_bat_err == 0)&&(f_bat_ok)&&f_charge_en)//
                        {
                    		Charge_ctrl();
                    	}
                    	else
                    	{
                    		f_charge = 0;
                    		f_charge_ok = 0;
                    		Charge_cnt = 0;
                    		tp0_ccra = 0;
                    		charge_sta = C_STANDBY;
                    	}
                    }
                    if(!f_judgeQC20_ok)
					{
						JudgeQC2_0();
					}
					else 
					{
						QC2_0_Charge();
					}
					bat_led_ctrl();
                    out_ctrl();
                    led_ctrl();
                	comm_to_cable();
                    BatVol_ctrl();
                    low_power_mode();
                }
            }
        }
		
        if(f_low_power)
        {
            if((PinTxRx == 0) || (KEY_VALUE == 0) || ((VIN_IN == 0) && (f_charge_ok == 0)) || ((VIN_IN) && (f_charge_ok)))
            {
                wake_up();
            }
        }  
    }
}

void isr_tb1(void)// 时基定时器1，2ms
{
	f_2ms = 1;
	if(++T_6MS_cnt >= T_6MS)
	{
		T_6MS_cnt = 0;
		f_6ms = 1;
	}
    if(++T_20MS_cnt >= T_20MS)
    {
    	T_20MS_cnt = 0;
    	f_20ms = 1;
    	if(++T250msCnt >= T_250MS)
    	{
    		f_250MS_flash ^= 1;
        	T250msCnt = 0;
        	if(++ledcnt >= T_LED_500MS)
			{
				ledcnt = 0;
				f_500MS_flash ^= 1;
			}
    	}                                                                                                              
    }
    f_led_2ms ^= 1;
    if(LowPowerDelay)
    {
        LowPowerDelay--;
    }
}

void isr_mf1f(void)// 多功能中断,定时计数
{
    _t1af = 0;
    CommData();
}
