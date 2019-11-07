#include "OSAL.h"
#include "OSAL_Tasks.h"

#include "simpleBLEPeripheral.h"

const pTaskEventHandlerFn tasksArr[] =
{
  SimpleBLEPeripheral_ProcessEvent                               
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

void osalInitTasks( void )
{
  uint8 taskID = 0;

  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  SimpleBLEPeripheral_Init( taskID );
}
