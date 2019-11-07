#include "OSAL.h"
#include "simpleBLEPeripheral.h"
#include "usart.h"

static uint8 simpleBLEPeripheral_TaskID;

static void simpleBLEPeripheral_ProcessOSALMsg( osal_event_hdr_t *pMsg );

void SimpleBLEPeripheral_Init( uint8 task_id )
{
  simpleBLEPeripheral_TaskID = task_id;

  osal_set_event( simpleBLEPeripheral_TaskID, SBP_START_DEVICE_EVT );

		
	// ≤‚ ‘œ˚œ¢
	for(uint8 i=0;i<10;i++)
	{
		osal_example_t* test =(osal_example_t*)osal_msg_allocate(sizeof(osal_example_t));
	
		if(test)
		{
			
			test->evt.event=2;
			test->evt.status=0;
			test->id=1;
			test->data=i;
					
			osal_msg_send(simpleBLEPeripheral_TaskID, (uint8 *)test);
		}
		
	}
}

uint16 SimpleBLEPeripheral_ProcessEvent( uint8 task_id, uint16 events )
{
  VOID task_id; 

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( simpleBLEPeripheral_TaskID )) != NULL )
    {
      simpleBLEPeripheral_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      VOID osal_msg_deallocate( pMsg );
    }

    return (events ^ SYS_EVENT_MSG);
  }

	if ( events & SBP_START_DEVICE_EVT )
	{
		printf(">>>>>event: SBP_START_DEVICE_EVT  \r\n");
		
		return (events ^ SBP_START_DEVICE_EVT);
	}
	
		
  return 0;
}

static void simpleBLEPeripheral_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
		case 1:
			break;
		case 2:
				printf(" id = %d °¢data = %d \r\n",((osal_example_t*)pMsg)->id,((osal_example_t*)pMsg)->data);
			break;
		default:
			break;
  }
}

