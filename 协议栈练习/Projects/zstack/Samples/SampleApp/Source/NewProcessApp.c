#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "ZDApp.h"

#include "NewProcessApp.h"

#include "OnBoard.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"//���uart.h�ļ�
// ���б�Ӧ��д�ض���Ӧ�ó����Ⱥ��ID.

//#define SEND_DATA_EVENT  0X01//��Ӷ�ʱʱ�¼�



const cId_t NewProcessAPP_ClusterList[NEWPROCESSAPP_MAX_CLUSTERS] =
{
  NEWPROCESSAPP_PERIODIC_CLUSTERID
};
//��������һ��ZigBee�豸�ڵ㣬��Ϊ���豸������

const SimpleDescriptionFormat_t NewProcessAPP_SimpleDesc =
{
  NEWPROCESSAPP_ENDPOINT,              //  int Endpoint;
  NEWPROCESSAPP_PROFID,                //  uint16 AppProfId[2];
  NEWPROCESSAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  NEWPROCESSAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  NEWPROCESSAPP_FLAGS,                 //  int   AppFlags:4;
  NEWPROCESSAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)NewProcessAPP_ClusterList,  //  uint8 *pAppInClusterList;
  0,           
  (cId_t *)NULL    
};

endPointDesc_t NewProcessAPP_epDesc;  //�ڵ�������
uint8 NewProcessAPP_TaskID;           //�������ȼ�
uint8 NewProcessAPP_TransID;          //���ݷ������к�
unsigned char uartbuf[128];           //���ڷ��ͻ�����
//******************************************************************************
void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pckt );//������Ϣ������
void NewProcessAPP_SendTheMessage( void );                     //���ݷ��ͺ���
void rxCB(uint8 port,uint8 event);                             //���ڷ��ͺ���
void NewProcessAPP_key();
//******************************************************************************


void NewProcessApp_Init( uint8 task_id )//�����ʼ������
{
  halUARTCfg_t uartConfig;                                   //*
  NewProcessAPP_TaskID            = task_id;                  //��ʼ���������ȼ�
  NewProcessAPP_TransID           = 0;                        //���������ݰ���ų�ʼ��Ϊ0
  
  NewProcessAPP_epDesc.endPoint   = NEWPROCESSAPP_ENDPOINT;   //�Խڵ����������г�ʼ��
  NewProcessAPP_epDesc.task_id    = &NewProcessAPP_TaskID;
  NewProcessAPP_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&NewProcessAPP_SimpleDesc;
  NewProcessAPP_epDesc.latencyReq = noLatencyReqs;

  //ʹ��afRegister�������ڵ�����������ע��ſ���ʹ��OSAL����
  afRegister( &NewProcessAPP_epDesc );
  //*********************uart���ñ�********************//  
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.callBackFunc         = rxCB;//����ص�����
  HalUARTOpen(0,&uartConfig);
}


uint16 NewProcessApp_ProcessEvent(uint8 task_id,uint16 events)
{

  afIncomingMSGPacket_t *MSGpkt;//����һ��ָ�������Ϣ�ṹ���ָ��MSGpkt
   
   if( events & SYS_EVENT_MSG)
   {   
/*ʹ��osal_msg_receive��������Ϣ�����Ͻ�����Ϣ������Ϣ�а������յ��������ݰ�
׼ȷ˵�ǰ�����ָ����յ����������ݰ���ָ�룩*******************************/
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(NewProcessAPP_TaskID);
      while( MSGpkt)
      {
        switch(MSGpkt->hdr.event)
        {
           case KEY_CHANGE:
	
           case AF_INCOMING_MSG_CMD: //�Խ��յ�����Ϣ�����ж�,����������ź���������к�������
	NewProcessAPP_MessageMSGCB(MSGpkt);//������Ϣ������
	break;
        default:    //������case x:����ִ��ʱִ��default��
          break;
        }
        osal_msg_deallocate((uint8 *)MSGpkt);//������Ϣ������ɣ��ͷ���Ϣ��ռ�ô洢�ռ�
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( NewProcessAPP_TaskID );
        //������ɺ���ʹ��osal_msg_receive��������Ϣ�����Ͻ�����Ϣ
      }
      return (events ^ SYS_EVENT_MSG);
      
      
   }
//   if(events & SEND_DATA_EVENT)  //*
//      {
//      NewProcessAPP_SendTheMessage();//**������Ϣ������
//      osal_start_timerEx(NewProcessAPP_TaskID,SEND_DATA_EVENT,100);
//      return (events ^ SEND_DATA_EVENT);//*
//      }
   return 0;
}

void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
   unsigned char buffer[5] = "   ";
   switch( pkt->clusterId)
   { 
     case NEWPROCESSAPP_PERIODIC_CLUSTERID:
       osal_memcpy(buffer,pkt->cmd.Data,5);//���յ������ݿ�����buffer��
       HalUARTWrite(0,buffer,5);
       HalLedBlink( HAL_LED_1, 1, 50, 50 );
       break;
  default:
    break;
   }
}
void rxCB(uint8 port,uint8 event)//*���ڻص�����
{
  
  if ((event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT)))
   {
   //HalUARTWrite(0,"����һ��",8);
   HalLedBlink( HAL_LED_2, 1, 50, 50 );
   NewProcessAPP_SendTheMessage();
 
  }
}
void NewProcessAPP_SendTheMessage(void)//���߷��ͺ���
{  
  
   unsigned char theMessageData[5] = "     ";
   HalUARTRead(0,theMessageData,5);
   HalLedBlink(HAL_LED_2,3,50,50);
   
   afAddrType_t my_DstAddr;
   my_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;//��ʼ��Ϊ�㲥
   my_DstAddr.endPoint = NEWPROCESSAPP_ENDPOINT;//��ʼ���˿ں�
   my_DstAddr.addr.shortAddr = 0xFFFF;//��������
   
   AF_DataRequest(&my_DstAddr,//������Ŀ�Ľڵ�������ַ���������ݸ�ʽ����㲥���������ಥ
	     &NewProcessAPP_epDesc,//ָ���ڵ�ķ��Ͷ˿ں�
	     NEWPROCESSAPP_PERIODIC_CLUSTERID,//�����
	     5,//�������ݴ�С
	     theMessageData,//���ݻ���
	     &NewProcessAPP_TransID,//ָ����ָ�룬ÿ����һ�μ�һ
	     AF_DISCV_ROUTE, 
	     AF_DEFAULT_RADIUS);
   
                  HalLedBlink(HAL_LED_2,3,50,50);
                 //HalLedBlink(HAL_LED_2,0,50,500);
	   
    
}






