#ifndef _CMP116NEW_H_
#define _CMP116NEW_H_

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;
typedef unsigned char u8;
typedef unsigned int  u16;
typedef unsigned long u32;

typedef struct
{
    uchar b0 : 1;
    uchar b1 : 1;
    uchar b2 : 1;
    uchar b3 : 1;
    uchar b4 : 1;
    uchar b5 : 1;
    uchar b6 : 1;
    uchar b7 : 1;
} _bits;

#define AN_VIN     	    	0//AN0
#define AN_OUT2_5V_I        1//AN1
#define AN_CHARGE_I         2//AN2
#define AN_BAT_VOL         	3//AN3
#define AN_BOOST        	4//AN4
#define AN_NTC     	    	5//AN5
#define AN_QC_D      		6//AN6
#define AN_OUT1_5V_I        7//AN7


#define VIN_IN            	_pa1// 充电输入电压信号
#define BAT1_EN            	_pc0// 采样电池电压1使能
#define BAT2_EN             _pc1// 采样电池电压2使能
#define BAT3_EN             _pc2// 采样电池电压3使能
#define OUT1_5V_EN          _pc3// 5V输出使能1
#define OUT2_5V_EN          _pa2// 5V输出使能2
#define KEY_VALUE           _pa3// 按键检测
#define NTC_EN              _pb6// 温度采样使能
#define LED_CTRL            _pb5// LED手电灯控制
#define LED1             	_pd3// LED1
#define LED2             	_pd2// LED2
#define LED3             	_pd1// LED3
#define LED4             	_pd0// LED4


#define LIGHT_ALWAYS_OFF    0//一直灭
#define LIGHT_ALWAYS_ON     1//一直亮
#define LIGHT_STROBE_ON     2//爆闪
#define LIGHT_SOS_ON        3//SOS


extern _bits flag_0;
extern _bits flag_1;
extern _bits flag_2;
extern _bits flag_3;
extern _bits flag_4;

#define f_low_power         flag_0.b0// 低功耗模式 1:低功耗 0:正常
#define f_12v_lp       		flag_0.b1// 12V低功耗标志
#define f_sampling_ok       flag_0.b2// 采样完成 1:完成 0:没有
#define f_charge_ok         flag_0.b3// 充电完成 1:完成 0:没有
#define f_20ms              flag_0.b4// 20ms标志
#define f_led_2ms       	flag_0.b5// LED显示2ms标志
#define f_charge            flag_0.b6// 电池充电 1:充电 0:其它
#define f_in_ok             flag_0.b7// 输入正常 1:正常 0:没有

#define f_bat_err        	flag_1.b0// 电池故障
#define f_bat_ovp           flag_1.b1// 电池过压 1:过压 0:正常
#define f_bat_uvp           flag_1.b2// 电池欠压 1:欠压 0:正常
#define f_5v_lp1            flag_1.b3// 5V低功耗1 1:低功耗 0:正常
#define f_5v_ocp1        	flag_1.b4// 5V过流1 1:过流 0:没有
#define f_5v_ocp2           flag_1.b5// 5V过流2 1:过流 0:没有
#define f_bat_ok            flag_1.b6// 电池正常
#define f_5v_lp2           	flag_1.b7// 5V低功耗2 1:低功耗 0:正常


#define f_key               flag_2.b0// 检测到按键脚为低
#define f_keypress          flag_2.b1// 确定有按键按下
#define f_shortkey          flag_2.b2// 短按键标志
#define f_longkey           flag_2.b3// 长按键标志
#define f_keyok             flag_2.b4// 一次按键检测完成标志
#define f_light       	 	flag_2.b5// 手电灯 1:手电灯 0:没有
#define f_250MS_flash       flag_2.b6// 250ms标志
#define f_500MS_flash       flag_2.b7// 500ms标志


#define f_recv              flag_3.b0// 正在接收数据
#define f_start             flag_3.b1// 准备发送数据
#define f_bit          		flag_3.b2// 数据“1”标志
#define f_send           	flag_3.b3// 有数据发送标志
#define f_rec_ok            flag_3.b4// 一次数据接收完成
#define f_charge_temp       flag_3.b5// 充电温度异常标志
#define f_discharge_temp    flag_3.b6// 放电温度异常标志
#define f_temp_err          flag_3.b7// NTC故障（短路或开路）

#define f_bat_low           flag_4.b0// 电池电压过低
#define f_BatVol_err		flag_4.b1// 电池电压检测故障
#define f_2ms				flag_4.b2// 计时2ms标志
#define f_charge_en         flag_4.b3// 可以开始充电
#define f_judgeQC20_ok      flag_4.b4// 判断充电是否支持QC2.0完成
#define f_QC2_0          	flag_4.b5// 支持QC2.0标志
#define f_9v_charge			flag_4.b6// 支持QC2.0 9V充电标志
#define f_6ms				flag_4.b7// 计时6ms标志

#define nokey	        0
#define shortkey        1
#define longkey         2

#define AVERAGE_NUM         4//采样次数
   
#define KEY_LONGPRESS_OK    60// 3S

//25mR电流计算放大倍数2^12*0.025/5=20.48=1A（5V输出）
#define OVER_CUR_5V         70// 3.2A
#define SHORT_CUR_5V        74// 3.4A
#define OVER_CUR_12V        51// 2.5A
#define SHORT_CUR_12V       55// 2.7A
#define OUT_I_2_6A        	53// 2.6A
#define OUT_I_4_2A        	86// 4.2A
#define OUT_I_5_2A        	106// 5.2A
#define CUR_5V_200MA        2//0.2A

//2.048ms为计时基数
#define T_6MS               3// 6ms
#define T_20MS              10// 20ms
#define T_30S               14648// 30S

//20ms为计时基数
#define T_80MS              4// 80ms
#define T_200MS             10// 200MS
#define T_250MS             12// 250MS
#define T_500MS             25// 500MS
#define T_600MS             29// 600MS
#define T_1S                49// 1S
#define T_1_25S             61// 1.25S
#define T_2S                98// 2S
#define T_4S                197// 4S
#define T_5S                244// 5S
#define T_7S                350// 7S
#define T_10S               500// 7S
#define T20_30S             1465// 30S

//250ms为计时基数
#define T_LED_250MS         1// 250MS
#define T_LED_500MS         2// 500MS
#define T_LED_1S            4// 1S
#define T_LED_2S            8// 2S

//160ms为计时基数
#define T_temp_3S           20// 3S
#define OC_5VOS_TIME        2// 320ms

#define CHARGE_OCP          882// 57摄氏度       
#define CHARGE_UCP          3132// 0摄氏度
#define CHARGE_RES_OCP      1017// 52摄氏度
#define CHARGE_RES_UCP      2939// 5摄氏度
#define DISCHARGE_OCP       882// 57摄氏度        
#define DISCHARGE_UCP       3705// -20摄氏度
#define DISCHARGE_RES_OCP   1017// 52摄氏度
#define DISCHARGE_RES_UCP   3594// -15摄氏度
#define NO_RT   			50//探头漏焊
#define SHORT_RT   			4050//探头短路
//电压计算放大倍数2^12/(5*3)=273=1V
//电压计算放大倍数2^12/(5*3)=273.07=1V
#define BATSOC_0        2594// 9.5V
#define BATSOC_25       3043// 11.15V
#define BATSOC_50       3114// 11.41V
#define BATSOC_75       3199// 11.72V
#define BATSOC_100      3412// 12.5V
#define DISCHARGE_OFFSET 0	// 0V

#define BAT_0_03V       7// 0.03
#define BAT_0_05V       13// 0.05
#define BAT_0_1V        27// 0.1
#define BAT_0_5V        136// 0.5
#define BAT_1_0V        273// 1.0
#define BAT_2_0V        546// 2.0V
#define BAT_2_5V        682// 2.5V
#define BAT_3_0V        819// 3.0V
#define BAT_3_05V       832// 3.05V
#define BAT_3_1V        835// 3.1V
#define BAT_3_2V        873// 3.2V
#define BAT_3_4V        928// 3.4V
#define BAT_3_8V        1037// 3.8V
#define BAT_4_1V        1119// 4.1V
#define BAT_4_14V       1130// 4.14V
#define BAT_4_15V       1133// 4.15V
#define BAT_4_17V       1139// 4.17V
#define BAT_4_18V       1141// 4.18V
#define BAT_4_19V       1144// 4.19V
#define BAT_4_2V        1147// 4.2V
#define BAT_4_22V       1152// 4.22V
#define BAT_4_23V       1155// 4.23V
#define BAT_9_0V        2457// 9V
#define BAT_9_6V        2621// 9.6V
#define BAT_10_2V       2785// 10.2V
#define BAT_12_0V       3276// 12.0V
#define BAT_12_3V       3358// 12.3V
#define BAT_12_35V      3372// 12.35V
#define BAT_12_4V       3385// 12.4V
#define BAT_12_42V      3390// 12.42V
#define BAT_12_45V      3400// 12.45V
#define BAT_12_47V      3404// 12.47V
#define BAT_12_5V       3413// 12.5V
#define BAT_12_55V      3427// 12.55V
#define BAT_12_6V       3440// 12.6V   
//3.12V基准电压计算放大倍数2^12/(3.12)=1312.8=1V
#define bat_2_0V        2632// 2.0V

//50-温度正常，电压正常；51-过温，电压正常；52-温度正常，欠压；53-过温，欠压
#define CableType   0xA150;		//A150-3C电瓶夹;A250-4C电瓶夹;A350-4F电瓶夹;

#define Batstate0   0x50;		//温度正常，电压正常
#define Batstate1   0x51;		//过温，电压正常
#define Batstate2   0x52;		//温度正常，欠压
#define Batstate3   0x53;		//过温，欠压

static volatile uint ptm1_ccra __attribute__ ((at(0x3B)));
static volatile uint ptm1_ccrd __attribute__ ((at(0x39)));
static volatile uint tp0_ccrp __attribute__ ((at(0x35)));
static volatile uint tp0_ccra __attribute__ ((at(0x33)));
static volatile uint  sado __attribute__ ((at(0x20)));//ADC采样返回值

static  volatile uint  outlp_cnt[4] __attribute__ ((at(0x186)));// 5V进入省电模式计时
static  volatile uint  ChargeCurOffset __attribute__ ((at(0x190)));// 充电电流零漂
static  volatile uint  ChargeCur __attribute__ ((at(0x192)));// 充电电流
static  volatile uint  senddata __attribute__ ((at(0x194)));// 低功耗计时
static  volatile uchar BatLevel __attribute__ ((at(0x196)));// 电池电量
static  volatile uchar SaveBatLevel __attribute__ ((at(0x197)));// 保存电池灯状态
static  volatile uchar BatUVPCnt __attribute__ ((at(0x198)));// 电池欠压计时
static  volatile uchar BatLVPCnt __attribute__ ((at(0x199)));// 电池低压计时
static  volatile uchar BatNVPCnt __attribute__ ((at(0x19A)));// 电池无压计时
static  volatile uchar BatOVPCnt __attribute__ ((at(0x19B)));// 电池过压计时
static  volatile uchar key_state __attribute__ ((at(0x19E)));// 按键状态
static  volatile uchar light_sta __attribute__ ((at(0x1A0)));// 手电灯状态
static  volatile uchar light_cnt __attribute__ ((at(0x1A1)));// 手电灯亮计时
static  volatile uchar light_time __attribute__ ((at(0x1A2)));// 手电灯亮计数
static  volatile uchar ADC_cnt __attribute__ ((at(0x1A3)));// 采样计数
static  volatile uchar OVER_CUR1 __attribute__ ((at(0x1A4)));// QC3.0输出1过流点
static  volatile uchar OVER_CUR2 __attribute__ ((at(0x1A5)));// QC3.0输出2过流点
static  volatile uchar SHORT_CUR1 __attribute__ ((at(0x1A6)));// QC3.0输出1短路点
static  volatile uchar SHORT_CUR2 __attribute__ ((at(0x1A7)));// QC3.0输出2短路点
static  volatile uint  V_BOOST_tmp __attribute__ ((at(0x1A8)));// 充电升压电压采样缓存
static  volatile uint  V_BOOST __attribute__ ((at(0x1AA)));// 充电升压电压
static  volatile uint  QC_D_Vol_tmp __attribute__ ((at(0x1AC)));// QC2.0输入D-电压缓存
static  volatile uint  QC_D_Vol __attribute__ ((at(0x1AE)));// QC2.0输入D-电压值
static  volatile uint  NTC_tmp __attribute__ ((at(0x1B0)));// QC2.0输入D-电压缓存
static  volatile uint  NTC_VALUE __attribute__ ((at(0x1B2)));// QC2.0输入D-电压值
static  volatile uchar NTC_cnt __attribute__ ((at(0x1B4)));// QC3.0输出2短路点
static  volatile uint  InVol_tmp __attribute__ ((at(0x1B5)));// QC2.0输入D-电压缓存
static  volatile uint  InVol __attribute__ ((at(0x1B7)));// QC2.0输入D-电压值


extern uint	 BatVol_real;
extern uint  LowPowerDelay;

#endif
