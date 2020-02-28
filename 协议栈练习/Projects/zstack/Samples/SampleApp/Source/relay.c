#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "ZDApp.h"

#include "NewProcessApp.h"

#include "OnBoard.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "get_adc.h"
#include "iocc2530.h"
// This list should be filled with Application specific Cluster IDs.

//#define SEND_DATA_EVENT  0X01//��Ӷ�ʱʱ�¼�
#define relay (P2_0) //�̵�������
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
devStates_t NewProcessAPP_NwkState;   //***//����ڵ�״̬�ı���


//******************************************************************************
void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pckt );//������Ϣ������
//void NewProcessAPP_SendTheMessage( void );                     //���ݷ��ͺ���
void relay_scan(uint8 *buffer);
//******************************************************************************


void NewProcessApp_Init( uint8 task_id )//�����ʼ������
{
  halUARTCfg_t uartConfig;   
  //hal_adc_Init();                                 //adc�ɼ���ʼ��
  NewProcessAPP_TaskID           = task_id;       //��ʼ���������ȼ�
  NewProcessAPP_NwkState         = DEV_INIT;      //**���豸״̬��ʼ��ΪDEV_INIT����ʾ�ڵ�û���ӵ�����
  NewProcessAPP_TransID          = 0;             //���������ݰ���ų�ʼ��Ϊ0
  
  NewProcessAPP_epDesc.endPoint  = NEWPROCESSAPP_ENDPOINT;//�Խڵ����������г�ʼ��
  NewProcessAPP_epDesc.task_id   = &NewProcessAPP_TaskID;
  NewProcessAPP_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&NewProcessAPP_SimpleDesc;
  NewProcessAPP_epDesc.latencyReq = noLatencyReqs;

  //ʹ��afRegister�������ڵ�����������ע��ſ���ʹ��OSAL����
  afRegister( &NewProcessAPP_epDesc );
   //*********************uart���ñ�********************//  
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.callBackFunc         = NULL;//����ص�����
  HalUARTOpen(0,&uartConfig);
  P2SEL &=~0X01;
  P2DIR |=0X01;
  relay=0;
}

uint16 NewProcessApp_ProcessEvent(uint8 task_id,uint16 events)//��Ϣ������
{

  afIncomingMSGPacket_t *MSGpkt;//����һ��ָ�������Ϣ�ṹ���ָ��MSGpkt
   
   if( events & SYS_EVENT_MSG)
   {   //ʹ��osal_msg_receive��������Ϣ�����Ͻ�����Ϣ������Ϣ�а������յ��������ݰ�
       //��׼ȷ˵�ǰ�����ָ����յ����������ݰ���ָ�룩
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(NewProcessAPP_TaskID);
      while( MSGpkt)
      {
        switch(MSGpkt->hdr.event)
        {
           case ZDO_STATE_CHANGE: //**�Խ��յ�����Ϣ�����ж�,�������������к�������
	NewProcessAPP_NwkState = (devStates_t)(MSGpkt->hdr.status);//**��ȡ�ڵ��豸����
	if(NewProcessAPP_NwkState == DEV_END_DEVICE)
	{ //**�Խڵ��豸���ͽ����жϣ�������ն˽ڵ�DEV_END_DEVICEִ����һ�� 
	//   osal_set_event(NewProcessAPP_TaskID,SEND_DATA_EVENT);//*
	   
	}
	
	break;
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
//   {
//      NewProcessAPP_SendTheMessage();//**������Ϣ������
//      osal_start_timerEx(NewProcessAPP_TaskID,SEND_DATA_EVENT,2000);
//      return (events ^ SEND_DATA_EVENT);//*
//   }
   return 0;
}
//void NewProcessAPP_SendTheMessage(void)//���߷��ͺ���
//{  
//   uint8 state=0;       //״̬����
//   uint16 chk=0XFF;
//   unsigned char theMessageData[5] = "     ";
//   state=get_swsensor();
//   
//   if(state==1)
//   {
//     theMessageData[3]=0XFF;
//   }
//   else if(state==0)
//   {
//     theMessageData[3]=0X00;
//   }
//   
//      theMessageData[0]=0XFE;
//      theMessageData[1]=0X0E;
//      theMessageData[2]=0X02;
//      chk=0X0E+0X02+theMessageData[3];
//      theMessageData[4]=(chk<<8)>>8;
//      
//   afAddrType_t my_DstAddr;
//   my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;//��ʼ��Ϊ����
//   my_DstAddr.endPoint = NEWPROCESSAPP_ENDPOINT;//��ʼ���˿ں�
//   my_DstAddr.addr.shortAddr = 0x0000;//Э������ַ����Э������������
//   
//   AF_DataRequest(&my_DstAddr,//������Ŀ�Ľڵ�������ַ���������ݸ�ʽ����㲥���������ಥ
//	     &NewProcessAPP_epDesc,//ָ���ڵ�ķ��Ͷ˿ں�
//	     NEWPROCESSAPP_PERIODIC_CLUSTERID,//�����
//	     5,//�������ݴ�С
//	     theMessageData,//���ݻ���
//	     &NewProcessAPP_TransID,//ָ����ָ�룬ÿ����һ�μ�һ
//	     AF_DISCV_ROUTE, 
//	     AF_DEFAULT_RADIUS);
//                  HalLedBlink(HAL_LED_1,3,50,50);
//                 //HalLedBlink(HAL_LED_2,0,50,500);
//	     
//}

void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
   unsigned char buffer[6] = "   ";
   //uint16 chk;
   switch( pkt->clusterId)
   { 
     case NEWPROCESSAPP_PERIODIC_CLUSTERID:
       osal_memcpy(buffer,pkt->cmd.Data,5);//���յ������ݿ�����buffer��
       relay_scan(buffer);
       break;
         
  default:
    break;
   }
}
void relay_scan(uint8 *buffer)
{
    HalUARTWrite(0,buffer,5);
#if defined (RELAY1 )   
    if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X03)&&(buffer[3]==0X11)&&(buffer[4]==0x22))
      {
        HalLedBlink( HAL_LED_1, 1, 50, 50 );
        //HalUARTWrite(0,"yes",3);
        relay=1;
      }
    if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X03)&&(buffer[3]==0X22)&&(buffer[4]==0x33))
      {
        HalLedBlink( HAL_LED_2, 1, 50, 50 );
        //HalUARTWrite(0,"no",2);
        relay=0;
      }
#endif
      if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X04)&&(buffer[3]==0X11)&&(buffer[4]==0x23))
      {
        HalLedBlink( HAL_LED_1, 1, 50, 50 );
        //HalUARTWrite(0,"yes",3);
        relay=1;
      }
    if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X04)&&(buffer[3]==0X22)&&(buffer[4]==0x34))
      {
        HalLedBlink( HAL_LED_2, 1, 50, 50 );
        //HalUARTWrite(0,"no",2);
        relay=0;
      }
}