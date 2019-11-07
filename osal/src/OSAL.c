#include <string.h>
#include "stdlib.h"
#include "comdef.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "usart.h"

osal_msg_q_t osal_qHead;

static uint8 activeTaskID = TASK_NO_TASK;

static uint8 osal_msg_enqueue_push( uint8 destination_task, uint8 *msg_ptr, uint8 urgent );

int osal_strlen( char *pString )
{
  return (int)( strlen( pString ) );
}

void *osal_memcpy( void *dst, const void GENERIC *src, unsigned int len )
{
  uint8 *pDst;
  const uint8 GENERIC *pSrc;

  pSrc = src;
  pDst = dst;

  while ( len-- )
    *pDst++ = *pSrc++;

  return ( pDst );
}

void *osal_revmemcpy( void *dst, const void GENERIC *src, unsigned int len )
{
  uint8 *pDst;
  const uint8 GENERIC *pSrc;

  pSrc = src;
  pSrc += (len-1);
  pDst = dst;

  while ( len-- )
    *pDst++ = *pSrc--;

  return ( pDst );
}

void *osal_memdup( const void GENERIC *src, unsigned int len )
{
  uint8 *pDst;

  pDst = osal_mem_alloc( len );
  if ( pDst )
  {
    VOID osal_memcpy( pDst, src, len );
  }

  return ( (void *)pDst );
}

uint8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len )
{
  const uint8 GENERIC *pSrc1;
  const uint8 GENERIC *pSrc2;

  pSrc1 = src1;
  pSrc2 = src2;

  while ( len-- )
  {
    if( *pSrc1++ != *pSrc2++ )
      return FALSE;
  }
  return TRUE;
}

void *osal_memset( void *dest, uint8 value, int len )
{
  return memset( dest, value, len );
}

uint8 * osal_msg_allocate( uint16 len )
{
  osal_msg_hdr_t *hdr;

  if ( len == 0 )
    return ( NULL );

  hdr = (osal_msg_hdr_t *) osal_mem_alloc( (short)(len + sizeof( osal_msg_hdr_t )) );
  if ( hdr )
  {
    hdr->next = NULL;
    hdr->len = len;
    hdr->dest_id = TASK_NO_TASK;
    return ( (uint8 *) (hdr + 1) );
  }
  else
    return ( NULL );
}

uint8 osal_msg_deallocate( uint8 *msg_ptr )
{
  uint8 *x;

  if ( msg_ptr == NULL )
    return ( INVALID_MSG_POINTER );

  if ( OSAL_MSG_ID( msg_ptr ) != TASK_NO_TASK )
    return ( MSG_BUFFER_NOT_AVAIL );

  x = (uint8 *)((uint8 *)msg_ptr - sizeof( osal_msg_hdr_t ));

  osal_mem_free( (void *)x );

  return ( SUCCESS );
}

uint8 osal_msg_send( uint8 destination_task, uint8 *msg_ptr )
{
  return ( osal_msg_enqueue_push( destination_task, msg_ptr, FALSE ) );
}

uint8 osal_msg_push_front( uint8 destination_task, uint8 *msg_ptr )
{
  return ( osal_msg_enqueue_push( destination_task, msg_ptr, TRUE ) );
}

static uint8 osal_msg_enqueue_push( uint8 destination_task, uint8 *msg_ptr, uint8 push )
{
  if ( msg_ptr == NULL )
  {
    return ( INVALID_MSG_POINTER );
  }

  if ( destination_task >= tasksCnt )
  {
    osal_msg_deallocate( msg_ptr );
    return ( INVALID_TASK );
  }

  // Check the message header
  if ( OSAL_MSG_NEXT( msg_ptr ) != NULL ||
       OSAL_MSG_ID( msg_ptr ) != TASK_NO_TASK )
  {
    osal_msg_deallocate( msg_ptr );
    return ( INVALID_MSG_POINTER );
  }

  OSAL_MSG_ID( msg_ptr ) = destination_task;

  if ( push == TRUE )
  {
    // prepend the message
    osal_msg_push( &osal_qHead, msg_ptr );
  }
  else
  {
    // append the message
    osal_msg_enqueue( &osal_qHead, msg_ptr );
  }

  // Signal the task that a message is waiting
  osal_set_event( destination_task, SYS_EVENT_MSG );

  return ( SUCCESS );
}

uint8 *osal_msg_receive( uint8 task_id )
{
  osal_msg_hdr_t *listHdr;
  osal_msg_hdr_t *prevHdr = NULL;
  osal_msg_hdr_t *foundHdr = NULL;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  // Point to the top of the queue
  listHdr = osal_qHead;

  // Look through the queue for a message that belongs to the asking task
  while ( listHdr != NULL )
  {
    if ( (listHdr - 1)->dest_id == task_id )
    {
      if ( foundHdr == NULL )
      {
        // Save the first one
        foundHdr = listHdr;
      }
      else
      {
        // Second msg found, stop looking
        break;
      }
    }
    if ( foundHdr == NULL )
    {
      prevHdr = listHdr;
    }
    listHdr = OSAL_MSG_NEXT( listHdr );
  }

  // Is there more than one?
  if ( listHdr != NULL )
  {
    // Yes, Signal the task that a message is waiting
    osal_set_event( task_id, SYS_EVENT_MSG );
  }
  else
  {
    // No more
    osal_clear_event( task_id, SYS_EVENT_MSG );
  }

  // Did we find a message?
  if ( foundHdr != NULL )
  {
    // Take out of the link list
    osal_msg_extract( &osal_qHead, foundHdr, prevHdr );
  }

  // Release interrupts
  HAL_EXIT_CRITICAL_SECTION();

  return ( (uint8*) foundHdr );
}

osal_event_hdr_t *osal_msg_find(uint8 task_id, uint8 event)
{
  osal_msg_hdr_t *pHdr;

  HAL_ENTER_CRITICAL_SECTION();  // Hold off interrupts.

  pHdr = osal_qHead;  // Point to the top of the queue.

  // Look through the queue for a message that matches the task_id and event parameters.
  while (pHdr != NULL)
  {
    if (((pHdr-1)->dest_id == task_id) && (((osal_event_hdr_t *)pHdr)->event == event))
    {
      break;
    }

    pHdr = OSAL_MSG_NEXT(pHdr);
  }

  HAL_EXIT_CRITICAL_SECTION();  // Release interrupts.

  return (osal_event_hdr_t *)pHdr;
}


void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr )
{
  void *list;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  OSAL_MSG_NEXT( msg_ptr ) = NULL;
  // If first message in queue
  if ( *q_ptr == NULL )
  {
    *q_ptr = msg_ptr;
  }
  else
  {
    // Find end of queue
    for ( list = *q_ptr; OSAL_MSG_NEXT( list ) != NULL; list = OSAL_MSG_NEXT( list ) );

    // Add message to end of queue
    OSAL_MSG_NEXT( list ) = msg_ptr;
		
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();
}

void *osal_msg_dequeue( osal_msg_q_t *q_ptr )
{
  void *msg_ptr = NULL;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  if ( *q_ptr != NULL )
  {
    // Dequeue message
    msg_ptr = *q_ptr;
    *q_ptr = OSAL_MSG_NEXT( msg_ptr );
    OSAL_MSG_NEXT( msg_ptr ) = NULL;
    OSAL_MSG_ID( msg_ptr ) = TASK_NO_TASK;
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();

  return msg_ptr;
}


void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr )
{

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  // Push message to head of queue
  OSAL_MSG_NEXT( msg_ptr ) = *q_ptr;
  *q_ptr = msg_ptr;

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();
}

void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr )
{

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  if ( msg_ptr == *q_ptr )
  {
    // remove from first
    *q_ptr = OSAL_MSG_NEXT( msg_ptr );
  }
  else
  {
    // remove from middle
    OSAL_MSG_NEXT( prev_ptr ) = OSAL_MSG_NEXT( msg_ptr );
  }
  OSAL_MSG_NEXT( msg_ptr ) = NULL;
  OSAL_MSG_ID( msg_ptr ) = TASK_NO_TASK;

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();
}

uint8 osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, uint8 max )
{
  void *list;
  uint8 ret = FALSE;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  // If first message in queue
  if ( *q_ptr == NULL )
  {
    *q_ptr = msg_ptr;
    ret = TRUE;
  }
  else
  {
    // Find end of queue or max
    list = *q_ptr;
    max--;
    while ( (OSAL_MSG_NEXT( list ) != NULL) && (max > 0) )
    {
      list = OSAL_MSG_NEXT( list );
      max--;
    }

    // Add message to end of queue if max not reached
    if ( max != 0 )
    {
      OSAL_MSG_NEXT( list ) = msg_ptr;
      ret = TRUE;
    }
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();

  return ret;
}

uint8 osal_set_event( uint8 task_id, uint16 event_flag )
{
  if ( task_id < tasksCnt )
  {
    HAL_ENTER_CRITICAL_SECTION();    // Hold off interrupts
    tasksEvents[task_id] |= event_flag;  // Stuff the event bit(s)
    HAL_EXIT_CRITICAL_SECTION();     // Release interrupts
    return ( SUCCESS );
  }
  else
  {
    return ( INVALID_TASK );
  }
}

uint8 osal_clear_event( uint8 task_id, uint16 event_flag )
{
  if ( task_id < tasksCnt )
  {
    HAL_ENTER_CRITICAL_SECTION();    // Hold off interrupts
    tasksEvents[task_id] &= ~(event_flag);   // Clear the event bit(s)
    HAL_EXIT_CRITICAL_SECTION();     // Release interrupts
    return ( SUCCESS );
  }
   else
  {
    return ( INVALID_TASK );
  }
}


uint8 osal_isr_register( uint8 interrupt_id, void (*isr_ptr)( uint8* ) )
{
  // Remove these statements when functionality is complete
  (void)interrupt_id;
  (void)isr_ptr;
  return ( SUCCESS );
}


uint8 osal_int_enable( uint8 interrupt_id )
{

  if ( interrupt_id == INTS_ALL )
  {
    HAL_ENABLE_INTERRUPTS();
    return ( SUCCESS );
  }
  else
  {
    return ( INVALID_INTERRUPT_ID );
  }
}

uint8 osal_int_disable( uint8 interrupt_id )
{

  if ( interrupt_id == INTS_ALL )
  {
    HAL_DISABLE_INTERRUPTS();
    return ( SUCCESS );
  }
  else
  {
    return ( INVALID_INTERRUPT_ID );
  }
}

uint8 osal_init_system( void )
{
  // Initialize the Memory Allocation System
  //osal_mem_init();

  // Initialize the message queue
  osal_qHead = NULL;

  // Initialize the timers
  //osalTimerInit();

  // Initialize the Power Management System
//osal_pwrmgr_init();

  // Initialize the system tasks.
  osalInitTasks();

  // Setup efficient search for the first free block of heap.
  //osal_mem_kick();

  return ( SUCCESS );
}


void osal_start_system( void )
{
	for(;;)
	{
		osal_run_system();
	}
}

void osal_run_system( void )
{
  uint8 idx = 0;

  do{
     if (tasksEvents[idx])  // Task is highest priority that is ready.
     {
       break;
     }
    }while (++idx < tasksCnt);

  if (idx < tasksCnt)
  {
    uint16 events;

    HAL_ENTER_CRITICAL_SECTION();
    events = tasksEvents[idx];
    tasksEvents[idx] = 0;  // Clear the Events for this task.
    HAL_EXIT_CRITICAL_SECTION();

    activeTaskID = idx;
    events = (tasksArr[idx])( idx, events );
    activeTaskID = TASK_NO_TASK;

    HAL_ENTER_CRITICAL_SECTION();
    tasksEvents[idx] |= events;  // Add back unprocessed events to the current task.
    HAL_EXIT_CRITICAL_SECTION();
  }
}


uint8 osal_isbufset( uint8 *buf, uint8 val, uint8 len )
{
  uint8 x;

  if ( buf == NULL )
  {
    return ( FALSE );
  }

  for ( x = 0; x < len; x++ )
  {
    // Check for non-initialized value
    if ( buf[x] != val )
    {
      return ( FALSE );
    }
  }
  return ( TRUE );
}

uint8 osal_self( void )
{
  return ( activeTaskID );
}

void *osal_mem_alloc( uint16 size )
{
	return calloc(1,size);
}

void osal_mem_free( void *ptr )
{
	if(ptr==NULL)
		return ;
	free(ptr);
}

