/*************************************************************************
 * Title:	Data Queue Header File
 *
 * File:	queue.h
 * Author:	Ashley
 *
 * Maintain a buffer of data in a circular buffer or queue.
 *
 *************************************************************************/
#if !defined( INCLUDED_QUEUE_H )
#define INCLUDED_QUEUE_H

#include "zscdefine.h"


// declare the queue data type
typedef void volatile *Queue;

// The size of the internal queue structure
#define QUEUE_INTERNAL_SIZE (4)

// The size of the internal save state
#define QUEUE_SAVE_STATE_SIZE (3)

// Allocate a Queue object
//
// This macro is used to allow the consumers of a Queue to allocate a 
// block of RAM to store the queue into without violating the encapsulation
// of the data.
// 
// name = the variable name to use.
// dataLen = the number of bytes to allocate in the queue
//
// This will ensure word alignment of the allocated data
#define QUEUE_ALLOCATE_DATA( name, dataLen ) \
	volatile BYTE QueueData_##name[ (dataLen) + 1 ]; \
	volatile U32 QueueStruct_##name[ QUEUE_INTERNAL_SIZE ];

//
// Allocate Queue data for multiple queues.
//
#define QUEUE_ALLOCATE_MULTIPLE_DATA( name, dataLen, number ) \
	volatile BYTE QueueData_##name[(number)][ (dataLen) + 1 ]; \
	volatile U32 QueueStruct_##name[(number)][ QUEUE_INTERNAL_SIZE ];	

// Initialise the Queue data
//
// This macro is used to initialise the queue without violating the encapsulation
// eg:
//
//		QUEUE_ALLOCATE_DATA( RxQueue, 16 )
//
//		void Init()
//		{
//			Queue q;
//			q = QUEUE_INITIALISE_DATA( RxQueue, 16 );
//		}
#define QUEUE_INITIALISE_DATA( name, dataLen ) Queue_Initialise( QueueStruct_##name, QueueData_##name, (dataLen) )

//
// Initialise one of a multiple (array) allocated queues
// Must call for each of the number of queues allocated.
//
#define QUEUE_INITIALISE_MULTIPLE_DATA( name, dataLen, number ) Queue_Initialise( QueueStruct_##name[ (number) ], QueueData_##name[ (number) ], (dataLen) )

//
// The ability to save and "rewind" queues is useful for replaying frames
//
// Define a type to store the "state" of the type so it can be restored
//
typedef WORD volatile Queue_SavedState[QUEUE_SAVE_STATE_SIZE];

//------------------------------------------------------------------------
// Function Delcarations
//------------------------------------------------------------------------

// Initialise the queue - NOTE: do not call directly, use QUEUE_INITIALISE_DATA
extern Queue Queue_Initialise( Queue q, volatile BYTE *pAllocatedData, WORD dataLen );

// Add a byte on to the end of the queue. Returns zero if failed (out of space)
// otherwise it return non-zero to indicate success.
extern BYTE Queue_Push( Queue q, BYTE data );

// Add a byte to the end of the queue and return a pointer to it so that it
// can be modified later. This is needed for building variable length
// frames in AISG so we can update the length fields in the payload.
// Note, NULL is returned if the queue is full, otherwise a pointer 
// is returned that may be used to modifiy the written byte.
extern volatile BYTE* Queue_PushPlaceholderValue( Queue q, BYTE data );

// Remove a byte from the start of the queue. If the queue is empty
// an undefined value will be returned.
extern BYTE Queue_Pop( Queue q );

// Get the byte at the start of the queue, but do not remove it from the queue.
// If the queue is empty an undefined value will be returned.
extern BYTE Queue_Peek( Queue q );

// Empty the Queue
extern void Queue_Clear( Queue q );

// Returns non-zero if the queue is empty
extern BYTE Queue_IsEmpty( Queue q );

// Returns non-zero if the queue is full
extern BYTE Queue_IsFull( Queue q );

// Returns the number of bytes remaining in the queue
extern WORD Queue_GetLength( Queue q );

// Returns the maximum number of bytes the queue can contain
extern WORD Queue_GetMaxLength( Queue q );

// Save the queue state into the save location
extern void Queue_SaveState( Queue q, Queue_SavedState pSaveState );

// Restore the queue state from the saved state.
// NOTE: this will cause unpredictable results if any Push operations have
// been preformed.
extern void Queue_RestoreState( Queue q, Queue_SavedState pFromState );

#endif // INCLUDED_QUEUE_H
