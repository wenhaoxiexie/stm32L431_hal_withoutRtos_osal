#include "OSAL.h"
#include "osal_bufmgr.h"

#define START_PTR( bd_ptr )  ( (bd_ptr) + 1 )
#define END_PTR( bd_ptr )    ( (uint8 *)START_PTR( bd_ptr ) + (bd_ptr)->payload_len )

typedef struct bm_desc
{
  struct bm_desc *next_ptr;
  uint16          payload_len;
} bm_desc_t;

static bm_desc_t *bm_list_ptr = NULL;

static bm_desc_t *bm_desc_from_payload ( uint8 *payload_ptr );

void *osal_bm_alloc( uint16 size )
{
  bm_desc_t     *bd_ptr;

  HAL_ENTER_CRITICAL_SECTION();

  bd_ptr = osal_mem_alloc( sizeof( bm_desc_t ) + size );

  if ( bd_ptr != NULL )
  {
    // set the buffer descriptor info
    bd_ptr->payload_len  = size;

    // add item to the beginning of the list
    bd_ptr->next_ptr = bm_list_ptr;
    bm_list_ptr = bd_ptr;

    // return start of the buffer
    bd_ptr = START_PTR( bd_ptr );
  }

  HAL_EXIT_CRITICAL_SECTION();

  return ( (void *)bd_ptr );
}

void osal_bm_free( void *payload_ptr )
{
  bm_desc_t *loop_ptr;
  bm_desc_t *prev_ptr;

  HAL_ENTER_CRITICAL_SECTION();

  prev_ptr = NULL;

  loop_ptr = bm_list_ptr;
  while ( loop_ptr != NULL )
  {
    if ( payload_ptr >= (void *)START_PTR( loop_ptr ) &&
         payload_ptr <= (void *)END_PTR( loop_ptr) )
    {
      // unlink item from the linked list
      if ( prev_ptr == NULL )
      {
        // it's the first item on the list
        bm_list_ptr = loop_ptr->next_ptr;
      }
      else
      {
        prev_ptr->next_ptr = loop_ptr->next_ptr;
      }

      // free the memory
      osal_mem_free( loop_ptr );

      // we're done here
      break;
    }

    // move on to next item
    prev_ptr = loop_ptr;
    loop_ptr = loop_ptr->next_ptr;
  }

  HAL_EXIT_CRITICAL_SECTION();

  return;
}

void *osal_bm_adjust_header( void *payload_ptr, int16 size )
{
  bm_desc_t *bd_ptr;
  uint8 *new_payload_ptr;

  bd_ptr = bm_desc_from_payload( (uint8 *)payload_ptr );
  if ( bd_ptr != NULL )
  {
    new_payload_ptr = (uint8 *)( (uint8 *)payload_ptr - size );

    // make sure the new payload is within valid range
    if ( new_payload_ptr >= (uint8 *)START_PTR( bd_ptr ) &&
         new_payload_ptr <= (uint8 *)END_PTR( bd_ptr ) )
    {
      // return new payload pointer
      return ( (void *)new_payload_ptr );
    }
  }

  // return original value
  return ( payload_ptr );
}

void *osal_bm_adjust_tail( void *payload_ptr, int16 size )
{
  bm_desc_t *bd_ptr;
  uint8 *new_payload_ptr;

  bd_ptr = bm_desc_from_payload( (uint8 *)payload_ptr );
  if ( bd_ptr != NULL )
  {
    new_payload_ptr = (uint8 *)END_PTR( bd_ptr ) - size;

    // make sure the new payload is within valid range
    if ( new_payload_ptr >= (uint8 *)START_PTR( bd_ptr ) &&
         new_payload_ptr <= (uint8 *)END_PTR( bd_ptr ) )
    {
      // return new payload pointer
      return ( (void *)new_payload_ptr );
    }
  }

  // return original value
  return ( payload_ptr );
}

static bm_desc_t *bm_desc_from_payload ( uint8 *payload_ptr )
{
  bm_desc_t *loop_ptr;

  loop_ptr = bm_list_ptr;
  while ( loop_ptr != NULL )
  {
    if ( payload_ptr >= (uint8 *)START_PTR( loop_ptr ) &&
         payload_ptr <= (uint8 *)END_PTR( loop_ptr) )
    {
      // item found
      break;
    }

    // move on to next item
    loop_ptr = loop_ptr->next_ptr;
  }

  return ( loop_ptr );
}
