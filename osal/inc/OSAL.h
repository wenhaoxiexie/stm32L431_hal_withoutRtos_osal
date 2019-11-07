#ifndef OSAL_H
#define OSAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "comdef.h"

#define OSAL_MSG_NEXT(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->next

#define OSAL_MSG_Q_INIT(q_ptr)      *(q_ptr) = NULL

#define OSAL_MSG_Q_EMPTY(q_ptr)     (*(q_ptr) == NULL)

#define OSAL_MSG_Q_HEAD(q_ptr)      (*(q_ptr))

#define OSAL_MSG_LEN(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->len

#define OSAL_MSG_ID(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->dest_id

#define INTS_ALL    0xFF

#define HAL_ENTER_CRITICAL_SECTION()  {__disable_irq();}	
#define HAL_EXIT_CRITICAL_SECTION()  {__enable_irq();} 

#define  HAL_DISABLE_INTERRUPTS()  {__disable_irq();}	
#define HAL_ENABLE_INTERRUPTS()  {__enable_irq();} 
	
typedef struct
{
  void   *next;
  uint16 len;
  uint8  dest_id;
} osal_msg_hdr_t;

typedef struct
{
  uint8  event;
  uint8  status;
} osal_event_hdr_t;

typedef struct
{
	osal_event_hdr_t evt;
	uint8 id;
	uint32 data;
}osal_example_t;

	typedef void * osal_msg_q_t;
  extern uint8 * osal_msg_allocate(uint16 len );
  extern uint8 osal_msg_deallocate( uint8 *msg_ptr );
  extern uint8 osal_msg_send( uint8 destination_task, uint8 *msg_ptr );
  extern uint8 osal_msg_push_front( uint8 destination_task, uint8 *msg_ptr );
  extern uint8 *osal_msg_receive( uint8 task_id );
  extern osal_event_hdr_t *osal_msg_find(uint8 task_id, uint8 event);
  extern void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr );
  extern uint8 osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, uint8 max );
  extern void *osal_msg_dequeue( osal_msg_q_t *q_ptr );
  extern void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr );
  extern void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr );
  extern uint8 osal_set_event( uint8 task_id, uint16 event_flag );
  extern uint8 osal_clear_event( uint8 task_id, uint16 event_flag );
  extern uint8 osal_isr_register( uint8 interrupt_id, void (*isr_ptr)( uint8* ) );
  extern uint8 osal_int_enable( uint8 interrupt_id );
  extern uint8 osal_int_disable( uint8 interrupt_id );
  extern uint8 osal_init_system( void );
  extern void osal_start_system( void );
  extern void osal_run_system( void );
  extern uint8 osal_self( void );
  extern int osal_strlen( char *pString );
  extern void *osal_memcpy( void*, const void GENERIC *, unsigned int );
  extern void *osal_memdup( const void GENERIC *src, unsigned int len );
  extern void *osal_revmemcpy( void*, const void GENERIC *, unsigned int );
  extern uint8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );
  extern void *osal_memset( void *dest, uint8 value, int len );
  extern uint16 osal_build_uint16( uint8 *swapped );
  extern uint32 osal_build_uint32( uint8 *swapped, uint8 len );
  extern uint16 osal_rand( void );
  extern uint8* osal_buffer_uint32( uint8 *buf, uint32 val );
  extern uint8* osal_buffer_uint24( uint8 *buf, uint24 val );
  extern uint8 osal_isbufset( uint8 *buf, uint8 val, uint8 len );
	extern  void *osal_mem_alloc( uint16 size );
  extern  void osal_mem_free( void *ptr );

#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
