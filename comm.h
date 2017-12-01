#ifndef _COMM_H_
#define _COMM_H_


#define PinTxRx         _pb4//模拟发送端口
#define PinTxRx_Path    _pbc4//设置输入输出口


//Tx define 64us
#define Head_Low    62// 3968us
#define Head_Length  78// 4992us
#define BIT1_Low    5 // 320us
#define BIT0_Low    10// 640us
#define BIT_Length   15// 960us
#define TxNum       16// 16位
#define TxBit       0x8000

//Rx define 64us
#define Head_H_min  52      // 3.328ms
#define Head_H_max  72      // 4.608ms
#define Head_W_min  68      // 4.624ms
#define Head_W_max  88      // 5.632ms
#define BIT_W_min   10      // 640us
#define BIT_W_max   20      // 1280us
#define RxBit       0x0001


void comm_to_cable(void);
void StartSend(uint data);
void CommData(void);

extern uint RegRx;

#endif
