#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h" 
#include "UART_PRINT.h" 
#include "TIMER.h"
#include "get_adc.h"
#include "sh10.h"
#include <string.h>

#define MAX_SEND_BUF_LEN  128  
#define MAX_RECV_BUF_LEN  128
static uint8 pTxData[MAX_SEND_BUF_LEN]; //�������߷��ͻ������Ĵ�С
static uint8 pRxData[MAX_RECV_BUF_LEN]; //�������߽��ջ������Ĵ�С
uint16 pTxlen = 0;
uint16 pRxlen = 0;
#define MAX_UART_SEND_BUF_LEN  128
#define MAX_UART_RECV_BUF_LEN  128
uint8 uTxData[MAX_UART_SEND_BUF_LEN]; //���崮�ڷ��ͻ������Ĵ�С
uint8 uRxData[MAX_UART_RECV_BUF_LEN]; //���崮�ڽ��ջ������Ĵ�С
uint16 uTxlen = 0;
uint16 uRxlen = 0;


/*****��Ե�ͨѶ��ַ����******/
#define RF_CHANNEL                13           // Ƶ�� 11~26
#define PAN_ID                    0x0013      //����id 
#define MY_ADDR                   0xacef      // ����ģ���ַ
#define SEND_ADDR                 0x1234     //���͵�ַ
/**************************************************/
static basicRfCfg_t basicRfConfig;
uint8   APP_SEND_DATA_FLAG;
/******************************************/

/**************************************************/
// ����RF��ʼ��
void ConfigRf_Init(void)
{
    basicRfConfig.panId       =   PAN_ID;        //zigbee��ID������
    basicRfConfig.channel     =   RF_CHANNEL;    //zigbee��Ƶ������
    basicRfConfig.myAddr      =  MY_ADDR;   //���ñ�����ַ
    basicRfConfig.ackRequest  =   TRUE;          //Ӧ���ź�
    while(basicRfInit(&basicRfConfig) == FAILED); //���zigbee�Ĳ����Ƿ����óɹ�
    basicRfReceiveOn();                // ��RF
}

/********************MAIN************************/
void main(void)    
{   
    uint8 status;
    uint8 flag=0;
    halBoardInit();  //ģ�������Դ�ĳ�ʼ��
    ConfigRf_Init(); //�����շ����������ó�ʼ�� 
   while(1)
   {
      status = basicRfPacketIsReady();
      if(status==1)
      {
         basicRfReceive(pRxData,1,NULL);
         if(pRxData[0]==0xff)
         { 
           pRxData[0]=0;
           flag++;
           if(flag>=9)
           {flag=0;}
         }
      }
      
      switch(flag){
             case 1:halLedSet(2);break;
             case 2:halLedSet(1);break;
             case 3:halLedSet(4);break;
             case 4:halLedSet(3);break;
             case 5:halLedClear(3);break;
             case 6:halLedClear(4);break;
             case 7:halLedClear(1);break;
             case 8:halLedClear(2);break;
             }
   }
}
/************************main end ****************************/