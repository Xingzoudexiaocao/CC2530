#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h" 
#include "TIMER.h"

#define MAX_SEND_BUF_LEN  128
#define MAX_RECV_BUF_LEN  128
static uint8 pTxData[MAX_SEND_BUF_LEN]; //�������߷��ͻ������Ĵ�С
static uint8 pRxData[MAX_RECV_BUF_LEN]; //�������߽��ջ������Ĵ�С

#define MAX_UART_SEND_BUF_LEN  128
#define MAX_UART_RECV_BUF_LEN  128
uint8 uTxData[MAX_UART_SEND_BUF_LEN]; //���崮�ڷ��ͻ������Ĵ�С
uint8 uRxData[MAX_UART_RECV_BUF_LEN]; //���崮�ڽ��ջ������Ĵ�С
uint16 uTxlen = 0;
uint16 uRxlen = 0;

 
/*****��Ե�ͨѶ��ַ����******/
#define RF_CHANNEL                13           // Ƶ�� 11~26
#define PAN_ID                    0x0013     //����id 
#define MY_ADDR                   0x1234      // ����ģ���ַ
#define SEND_ADDR                 0xacef     //���͵�ַ
/**************************************************/
#define sw1 (P1_2)
uint8   APP_SEND_DATA_FLAG;
static basicRfCfg_t basicRfConfig;
/******************************************/
void MyByteCopy(uint8 *dst, int dststart, uint8 *src, int srcstart, int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        *(dst+dststart+i)=*(src+srcstart+i);
    }
}
/****************************************************/
uint16 RecvUartData(void)
{   
    uint16 r_UartLen = 0;
    uint8 r_UartBuf[128]; 
    uRxlen=0; 
    r_UartLen = halUartRxLen();
    while(r_UartLen > 0)
    {
        r_UartLen = halUartRead(r_UartBuf, sizeof(r_UartBuf));
        MyByteCopy(uRxData, uRxlen, r_UartBuf, 0, r_UartLen);
        uRxlen += r_UartLen;
        halMcuWaitMs(5);   //������ӳٷǳ���Ҫ����Ϊ���Ǵ���������ȡ����ʱ����Ҫ��һ����ʱ����
        r_UartLen = halUartRxLen();       
    }   
    return uRxlen;
}
/**************************************************/
// ����RF��ʼ��
void ConfigRf_Init(void)
{
    basicRfConfig.panId       =   PAN_ID;        //zigbee��ID������
    basicRfConfig.channel     =   RF_CHANNEL;    //zigbee��Ƶ������
    basicRfConfig.myAddr      =   MY_ADDR;   //���ñ�����ַ
    basicRfConfig.ackRequest  =   TRUE;          //Ӧ���ź�
    while(basicRfInit(&basicRfConfig) == FAILED); //���zigbee�Ĳ����Ƿ����óɹ�
    basicRfReceiveOn();                // ��RF
}

/********************MAIN START************************/
void main(void)
{
    uint8 status;
    halBoardInit();  //ģ�������Դ�ĳ�ʼ��
    ConfigRf_Init(); //�����շ����������ó�ʼ�� 
    Timer4_Init();
    Timer4_On();
    P1SEL &=~0X04;
    P1DIR &=~0X04;
  while(1)
  { 
    
    
    if( GetSendDataFlag()==1)
    {
      if(sw1==0)
      {
        halLedToggle(3);
        pTxData[0]=0xff;
        status = basicRfSendPacket(SEND_ADDR,pTxData,1);

      }
      else
      {
        halLedClear(3);
      }
      
       halLedToggle(1);

       Timer4_On();
       
    }
   
  }
   
}
/************************MAIN END ****************************/