/**************************************************************************************************
  Filename:       OSAL_SampleSw.c
  Revised:        $Date: 2014-06-03 16:29:28 -0700 (Tue, 03 Jun 2014) $
  Revision:       $Revision: 38778 $

  Description:    This file contains all the settings and other functions
                  that the user should set and change.


  Copyright 2006-2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "ZComDef.h"
#include "hal_drivers.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"

#if defined ( MT_TASK )
  #include "MT.h"
  #include "MT_TASK.h"
#endif

#include "nwk.h"
#include "APS.h"
#include "ZDApp.h"
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  #include "ZDNwkMgr.h"
#endif
#if defined ( ZIGBEE_FRAGMENTATION )
  #include "aps_frag.h"
#endif

#include "app_sample.h"

#if (defined OTA_CLIENT) && (OTA_CLIENT == TRUE)
  #include "zcl_ota.h"
#endif
/*********************************************************************
 * GLOBAL VARIABLES
 */

// The order in this table must be identical to the task initialization calls below in osalInitTask.
const pTaskEventHandlerFn tasksArr[] = {
  macEventLoop,
  nwk_event_loop,
  Hal_ProcessEvent,
#if defined( MT_TASK )
  MT_ProcessEvent,
#endif
  APS_event_loop,
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_ProcessEvent,
#endif
  ZDApp_event_loop,
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_event_loop,
#endif
//  zcl_event_loop,
  appSample_event_loop,
#if (defined OTA_CLIENT) && (OTA_CLIENT == TRUE)
  zclOTA_event_loop
#endif
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks( void )
{
  uint8 taskID = 0;
  /*
  osal_mem_alloc()�ú�����OSAL�е��ڴ��������
  ��һ���洢���亯��������ָ��һ�������ָ�룬
  �����Ǳ����仺��Ĵ�С����tasksCnt�Ķ�������
  const uint8tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
  tasksEventsָ�򱻷�����ڴ�ռ䣬����ע��
  tasksArr[]����ָ���������ϵ��һһ��Ӧ�ġ�
  tasksEvents����ָ���һ���������������ڴ�ռ�
*/
  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  
  //��������ڴ�ռ�ȫ������Ϊ0��tasksCnt������ * ��������ռ���ڴ�ռ䣨4byte��
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));
  //�������Z-StackЭ��ջ�У���MAC�㵽ZDO��ĳ�ʼ�����������еĲ������������ID������ID�������ε�����
  macTaskInit( taskID++ );//mac_ID = 0
  nwk_init( taskID++ );//nwk_ID = 1
  Hal_Init( taskID++ );//Hal_ID = 2
#if defined( MT_TASK )
  MT_TaskInit( taskID++ );//mt_ID = 3
#error  //�޶���
#endif
  APS_Init( taskID++ );//APS_ID =4
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_Init( taskID++ );//ZDO_ID =5
#endif
  ZDApp_Init( taskID++ );//ZDO_ID =6
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_Init( taskID++ );//ZDO_ID =7
#endif
//  zcl_Init( taskID++ );
  appSample_Init( taskID++ );
#if (defined OTA_CLIENT) && (OTA_CLIENT == TRUE)
  zclOTA_Init( taskID );
#error  //�޶���
#endif
}

/*********************************************************************
*********************************************************************/
