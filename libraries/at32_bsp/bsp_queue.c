/*************************************************************************
* Title:	Data Queue Implementation
*
* File:	queue.c
* Author:	zsc
*
* Maintain a buffer of data in a circular buffer or queue.
*
*************************************************************************/
#include "zscdefine.h"
#include "bsp_queue.h"

// Define the internal representation of a queue
typedef struct
{
	// NOTE: If you change this structure, make sure you change the 
	// QUEUE_INTERNAL_RESERVED_SPACE define in queue.h or you will cause 
	// program execution errors.
	volatile U32 Length;	// total length of queue
	volatile U32 Head;		// head position (index)
	volatile U32 Tail;		// tail position (index)
	volatile BYTE *Data;
} QueueImp;

#define UNDEFINED_VALUE (0xCD)

//
// Initilise the Queue into the allocated memory 
// and return the opaque queue object;
//
Queue Queue_Initialise( Queue q, volatile BYTE *pAllocatedData, WORD dataLen )
{
	QueueImp *qi = (QueueImp*)q;
	if( NULL == pAllocatedData || NULL == q ) return NULL;

	qi->Data = pAllocatedData;
	qi->Length = dataLen;
	qi->Head = 0;
	qi->Tail = 0;

	return (Queue)q;
}

//
// Add a byte on to the end of the queue. Returns zero if failed (out of space)
// otherwise it return non-zero to indicate success.
//
BYTE Queue_Push( Queue q, BYTE data )
{	
	// compute the new head location
	WORD nhead = ((QueueImp*)q)->Head+1;

	if( NULL == q ) return false;

	if( nhead == ((QueueImp*)q)->Length )
	{
		nhead = 0;
	}

	if( nhead == ((QueueImp*)q)->Tail )  // Full
	{
		return false;	// return zero for failure
	}	
	else
	{
		// Store the data
		((QueueImp*)q)->Data[ ((QueueImp*)q)->Head ] = data;
		((QueueImp*)q)->Head = nhead;
	}

	// return non-zero for success
	return true;
}

// Add a byte to the end of the queue and return a pointer to it so that it
// can be modified later. This is needed for building variable length
// frames in AISG so we can update the length fields in the payload.
// Note, NULL is returned if the queue is full, otherwise a pointer 
// is returned that may be used to modifiy the written byte.
volatile BYTE* Queue_PushPlaceholderValue( Queue q, BYTE data )
{
	volatile BYTE *pData;

	// compute the new head location
	WORD nhead = ((QueueImp*)q)->Head+1;

	if( NULL == q ) return false;

	if( nhead == ((QueueImp*)q)->Length )
	{
		nhead = 0;
	}

	if( nhead == ((QueueImp*)q)->Tail )  // Full
	{
		return NULL;	// return NULL for failure
	}	
	else
	{
		// Store the data
		((QueueImp*)q)->Data[ ((QueueImp*)q)->Head ] = data;

		// save the pointer
		pData = &((QueueImp*)q)->Data[ ((QueueImp*)q)->Head ];

		// update the head
		((QueueImp*)q)->Head = nhead;

		// return the pointer
		return pData;
	}


}

// Remove a byte from the start of the queue. If the queue is empty
// an undefined value will be returned.
BYTE Queue_Pop( Queue q )
{

	// setup a return byte. Set to a value that is not likely to be used
	// to help debugging	
	BYTE res = UNDEFINED_VALUE; 

	if( NULL == q ) return UNDEFINED_VALUE;

	if( !Queue_IsEmpty(q) )
	{
		// get the current byte
		res = ((QueueImp*)q)->Data[ ((QueueImp*)q)->Tail ];

		// increment and wrap the tail over the end of the buffer if needed
		((QueueImp*)q)->Tail++;

		if( ((QueueImp*)q)->Tail == ((QueueImp*)q)->Length )
		{
			((QueueImp*)q)->Tail = 0;
		}
	}

	// return the byte
	return res;	
}

// Get the byte at the start of the queue, but do not remove it from the queue.
// If the queue is empty an undefined value will be returned.
BYTE Queue_Peek( Queue q )
{
	if( NULL == q ) return UNDEFINED_VALUE;

	if( !Queue_IsEmpty(q) )
	{
		return ((QueueImp*)q)->Data[ ((QueueImp*)q)->Tail ];
	} 
	else
	{
		return UNDEFINED_VALUE;
	}
}

// Empty the Queue
void Queue_Clear( Queue q )
{
	if( NULL == q ) return;

	((QueueImp*)q)->Head = 0;
	((QueueImp*)q)->Tail = 0;
}

// Returns non-zero if the queue is empty
BYTE Queue_IsEmpty( Queue q )
{
	if( NULL == q ) return true;

	// If the head and tail values are the same, the queue is empty
	if( ((QueueImp*)q)->Head == ((QueueImp*)q)->Tail)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

// Returns non-zero if the queue is full
BYTE Queue_IsFull( Queue q )
{
	// If the next head value is the same as the tail
	// the queue is full
	WORD nhead = ((QueueImp*)q)->Head + 1;

	if( NULL == q ) return false;

	if(nhead == ((QueueImp*)q)->Length)
	{
		nhead = 0;
	}

	if(nhead == ((QueueImp*)q)->Tail)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Returns the number of bytes remaining in the queue
WORD Queue_GetLength( Queue q )
{
	if( NULL == q ) return 0;

	// If the head is after the tail, return the difference
	// otherwise adjust for the wrap around the end of the array.	
	if( ((QueueImp*)q)->Head >= ((QueueImp*)q)->Tail)
	{
		return ((QueueImp*)q)->Head - ((QueueImp*)q)->Tail;
	}
	else 
	{
		return ((QueueImp*)q)->Length - ( ((QueueImp*)q)->Tail - ((QueueImp*)q)->Head );
	}
}

// Returns the maximum number of bytes the queue can contain
WORD Queue_GetMaxLength( Queue q )
{
	if( NULL == q ) return 0;

	// note that we allocate an extra byte so we can deal with the
	// full case simply by pointing to an unused location and still be
	// able to contain all the data requested by the user.
	return ((QueueImp*)q)->Length-1;
}

// Save the queue state into the save location
void Queue_SaveState( Queue q, Queue_SavedState pSaveState )
{
	if( q != NULL && pSaveState != NULL )
	{
		pSaveState[0] = ((QueueImp*)q)->Length;
		pSaveState[1] = ((QueueImp*)q)->Head;
		pSaveState[2] = ((QueueImp*)q)->Tail;
	}	
}

// Restore the queue state from the saved state.
// NOTE: this will cause unpredictable results if any Push operations have
// been preformed.
void Queue_RestoreState( Queue q, Queue_SavedState pFromState )
{
	if( q != NULL && pFromState != NULL )
	{
		((QueueImp*)q)->Length = pFromState[0];
		((QueueImp*)q)->Head = 	pFromState[1];
		((QueueImp*)q)->Tail = 	pFromState[2];
	}		
}
