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
static uint8 pRxData[MAX_RECV_BUF_LEN]; //�������߷��ͻ������Ĵ�С
#define MAX_UART_SEND_BUF_LEN  128
#define MAX_UART_RECV_BUF_LEN  128
uint8 uTxData[MAX_UART_SEND_BUF_LEN]; //���崮�ڷ��ͻ������Ĵ�С
uint8 uRxData[MAX_UART_RECV_BUF_LEN]; //���崮�ڽ��ջ������Ĵ�С
uint16 uTxlen = 0;
uint16 uRxlen = 0;



/*****��Ե�ͨѶ��ַ����******/
#define RF_CHANNEL                20           // Ƶ�� 11~26
#define PAN_ID                    0x3040      //����id 
#define MY_ADDR                   0x0001      // ����ģ���ַ
#define SEND_ADDR                 0x1234     //���͵�ַ
//#define yyyy_ADDR                 0X0003     //������ַ
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
{   uint8 flag=0;
    uint8 status,len; 
    uint8 butff[] = {1,2,3,4};
    halBoardInit();  //ģ�������Դ�ĳ�ʼ��
    ConfigRf_Init(); //�����շ����������ó�ʼ�� 
//    halLedSet(1);
//    halLedSet(2);
    Timer4_Init();
    Timer4_On();
    while(1)
    {  
//      status = basicRfPacketIsReady();
//      len = halUartRxLen();
//      halUartWrite(butff,4);
//      if(status==1)
//      {
//         len = basicRfReceive(pRxData,MAX_UART_SEND_BUF_LEN,NULL);
//         halUartWrite(pRxData, len);
//         status=0;
//         len=0;
//      }
    
//      if(len!=0)
//      {
        
//         len = halUartRead(uRxData, len);
//         halUartWrite(uRxData, len);
      
         if(status = basicRfSendPacket(SEND_ADDR,butff,4))
         {
           halLedSet(1);
         }
         else
         {
           halLedClear(1);
         }
//         status = basicRfSendPacket(yyyy_ADDR,uRxData,len);
//         len=0;
//      }
    }
}
