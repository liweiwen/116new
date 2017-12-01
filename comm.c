#include "HT66F0185.h"
#include "cmp116new.h"
#include "comm.h"

uchar RegAdd;
uchar RegCnt;
uchar RegTmp;
uint  CommTemp;
uint  RegRx;

//与电瓶夹通信
void comm_to_cable(void)
{
	if(f_rec_ok)
	{
		f_rec_ok = 0;
		LowPowerDelay = T_30S;
		if(RegRx == 0xa150)
			StartSend(senddata);
	}
}

//启动发送
void StartSend(uint data)
{
	f_recv = 0;
	f_bit = 0;
	f_start = 0;
	f_send = 1;
	PinTxRx = 0;
	PinTxRx_Path = 0;
	CommTemp = data;
	RegAdd = 0;
	RegCnt = 0;
}

//数据收发
void CommData(void)
{
	if(RegAdd < 0xff)
		RegAdd++;
	if(f_send)
	{
		//发送头码
		if(f_start == 0)
		{
			if(RegAdd == Head_Low)
					PinTxRx = 1;
			else if(RegAdd >= Head_Length)
			{
				PinTxRx = 0;
				f_start = 1;//头码发送完成
				RegAdd = 0;
				if(CommTemp & TxBit)
				{
					f_bit = 1;//数据为‘1’
				}
			}
		}
		else
		{
			if(f_bit)
			{
				if(RegAdd == BIT1_Low)
				{
					PinTxRx = 1;
				}
			}
			else
			{
				if(RegAdd == BIT0_Low)
				{
					PinTxRx = 1;
				}
			}
			if(RegAdd >= BIT_Length)
			{
				PinTxRx = 0;
				RegAdd = 0;
				if(++RegCnt < TxNum)
				{
					CommTemp <<= 1;
					if(CommTemp & TxBit)
					{
						f_bit = 1;
					}
					else
					{
						f_bit = 0;
					}
				}
				else
				{
					f_send = 0;//发送完成
					PinTxRx_Path = 1;
				}
			}
			
		}
	}
	else
	{
		if(f_recv)
		{
			if(f_start == 0)
			{
				if(f_bit == 0)
				{
					if(PinTxRx)
					{
						if((RegAdd > Head_H_min) && (RegAdd < Head_H_max))
						{
							f_bit = 1;
						}
						else
							f_recv = 0;//头码检测失败
					}
				}
				else
				{
					if(PinTxRx == 0)
					{
						if((RegAdd > Head_W_min) && (RegAdd < Head_W_max))
						{
							f_recv = 1;//检测到头码
							RegCnt = 0;
							CommTemp = 0;
						}
						else
						{
							f_recv = 0;
						}
						f_bit = 0;
						RegAdd = 0;
					}
				}
			}
			else
			{
				if(f_bit == 0)
				{
					if(PinTxRx)
					{
						RegTmp = RegAdd;
						f_bit = 1;
					}
				}
				else
				{
					if(PinTxRx == 0)
					{
						if((RegAdd > BIT_W_min)&&(RegAdd < BIT_W_max))
						{
							RegTmp <<= 1;
							CommTemp <<= 1;
							if(RegTmp < RegAdd)
							{
								CommTemp |= RxBit;//判断0/1
							}
							if(++RegCnt > TxNum)
							{
								RegRx = CommTemp;
								f_start = 0;
								f_rec_ok = 1;//接收完成
							}
						}
						else
						{
							f_start = 0;
						}
						f_bit = 0;
						RegAdd = 0;	
					}
				}
			}
			
		}
		else
		{
			if(PinTxRx == 0)
			{
				f_bit = 0;
				f_start = 0;
				f_recv = 1;
				RegAdd = 0;
			}
		}
	}
}

					
					
				
		
