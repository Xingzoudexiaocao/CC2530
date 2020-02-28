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
#define MY_ADDR                   0x1234      // ����ģ���ַ
#define SEND_ADDR                 0x0001     //���͵�ַ
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
    basicRfConfig.myAddr      =   MY_ADDR;   //���ñ�����ַ
    basicRfConfig.ackRequest  =   TRUE;          //Ӧ���ź�
    while(basicRfInit(&basicRfConfig) == FAILED); //���zigbee�Ĳ����Ƿ����óɹ�
    basicRfReceiveOn();                // ��RF
}

/********************MAIN************************/
void main(void)    
{   
    uint8 status,len,flag=0;
    halBoardInit();  //ģ�������Դ�ĳ�ʼ��
    ConfigRf_Init(); //�����շ����������ó�ʼ�� 
    halLedClear(3);
    halLedClear(4);
    halLedClear(1);
    halLedClear(2);
    
    while(1)
    {  
//      status = basicRfPacketIsReady();
//      len = halUartRxLen();
//      if(status==1)
//      {
//         len = basicRfReceive(pRxData,MAX_UART_SEND_BUF_LEN,NULL);
//         halUartWrite(pRxData, len);
//         status=0;
//         len=0;
//         halLedSet(1);
//         halLedSet(2);
//         halLedSet(3);
//      }
//    
//      if(len!=0)
//      {
//        
//         len = halUartRead(uRxData, len);
// //        halUartWrite(uRxData, len);
//         status = basicRfSendPacket(SEND_ADDR,uRxData,len);
//         len=0;
//      }
         if(status = basicRfPacketIsReady())
         {
           halLedSet(1);
         }
         else
         {
           halLedClear(1);
         }
    }
}
