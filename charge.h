#ifndef _CHARGE_H_
#define _CHARGE_H_
typedef unsigned char uchar;

#define USB_D_PLUS_EN1       _pc4// 充电QC2.0 D+低电平控制
#define USB_D_MINUS_EN2      _pc5// 充电QC2.0 D-高电平控制
#define USB_D_MINUS_EN3      _pc6// 充电QC2.0 D-低电平控制
//充电状态
#define C_STANDBY       0//待机
#define C_CHARGE_CC     1//恒流
#define C_CHARGE_CV     2//恒压
#define C_CHARGE_STOP   3//充停
//电压计算放大倍数2^12/5=819.2=1V
#define QC_MINUS_0_2V   164     
#define QC_MINUS_0_4V   328
#define QC_MINUS_0_8V   655
//50mR电流计算放大倍数2^12*0.05/5=40.96=1A
#define I_CHARGE_1_5A   61// 1A
#define I_CHARGE_1A     41// 1A
#define I_CHARGE_2A     84// 2A
#define I_CHARGE_3A     123// 3A
#define I_CHARGE_0_05   2 // 0.05A
#define I_CHARGE_0_15   6 // 0.15A
#define I_CHARGE_0_2    8// 0.2A
#define I_CHARGE_0_3    12// 0.3A
#define I_CHARGE_0_5A   21// 0.5A
#define I_CHARGE_0_5    21// 0.5A
#define I_CHARGE_0_6    25// 0.6A
#define I_CHARGE_OFFSET 6// 0.15A  
#define V_CHARGE_OFFSET 30//0.11V

//以20ms*8=160ms为时基计时
#define charge_delay     200//32s
#define charge_16s       100//16s
#define charge_2s        12//2s
#define charge_4s        25//2s

#define CHARGE_MAX_PWM      210// 充电最大PWM值
//QC2.0充电电压状态
#define CHARGE_FREE         0// 空闲
#define CHARGE_5V           0// 5V充电
#define CHARGE_9V           1// 9V充电
#define CHARGE_12V          2// 12V充电

//电压计算放大倍数2^12/(5*3)=273=1V
#define V_IN_2_0V		546//3.0V
#define V_IN_3_0V		819//3.0V
#define V_IN_3_2V		873//3.2V
#define V_IN_3_5V		955//3.5V
#define V_IN_4_85V		1324//4.85V
#define V_IN_4_92V		1343//4.92V
#define V_IN_6V         1638// 6V 
#define V_IN_7V         1911// 7V
#define V_IN_8V         2184// 8V
#define V_IN_9V         2457// 9V
#define V_IN_10V        2730// 10V  
#define V_IN_11V        3003// 11V 
#define V_IN_13_6V      3712// 13.6V 
#define V_IN_14V        3822// 14V 
#define V_CHARGE_OFFSET  30// 0.11V 
//充电升压电压计算放大倍数2^12/(5*4)=204.8=1V
#define V_13_6V		2785//13.6V

void Check_Signal(void);
void Charge_ctrl(void);
void JudgeQC2_0(void);
void QC2_0_Charge(void);
void Init_CHARGE_PT0(void);
extern uchar charge_sta;
extern uchar Charge_cnt;
extern uchar I_CHARGE_SET;
extern uchar I_CHARGE_offset;

#endif
