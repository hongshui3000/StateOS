/******************************************************************************

    @file    StateOS: osmessagebuffer.h
    @author  Rajmund Szymanski
    @date    28.05.2018
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __STATEOS_MSG_H
#define __STATEOS_MSG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : message buffer
 *
 ******************************************************************************/

typedef struct __msg msg_t, * const msg_id;

struct __msg
{
	tsk_t  * queue; // inherited from stream buffer
	void   * res;   // allocated message buffer object's resource
	unsigned count; // inherited from stream buffer
	unsigned limit; // inherited from stream buffer

	unsigned head;  // inherited from stream buffer
	unsigned tail;  // inherited from stream buffer
	char   * data;  // inherited from stream buffer

	unsigned size;  // size of the first message in the buffer
};

/******************************************************************************
 *
 * Name              : _MSG_INIT
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : message buffer data
 *
 * Return            : message buffer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _MSG_INIT( _limit, _data ) { 0, 0, 0, _limit, 0, 0, _data, 0 }

/******************************************************************************
 *
 * Name              : _MSG_DATA
 *
 * Description       : create a message buffer data
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : message buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _MSG_DATA( _limit ) (char[_limit]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_MSG
 *
 * Description       : define and initialize a message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

#define             OS_MSG( msg, limit )                                \
                       char msg##__buf[limit];                           \
                       msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                       msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : static_MSG
 *
 * Description       : define and initialize a static message buffer object
 *
 * Parameters
 *   msg             : name of a pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

#define         static_MSG( msg, limit )                                \
                static char msg##__buf[limit];                           \
                static msg_t msg##__msg = _MSG_INIT( limit, msg##__buf ); \
                static msg_id msg = & msg##__msg

/******************************************************************************
 *
 * Name              : MSG_INIT
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : message buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_INIT( limit ) \
                      _MSG_INIT( limit, _MSG_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : MSG_CREATE
 * Alias             : MSG_NEW
 *
 * Description       : create and initialize a message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : pointer to message buffer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                MSG_CREATE( limit ) \
             & (msg_t) MSG_INIT  ( limit )
#define                MSG_NEW \
                       MSG_CREATE
#endif

/******************************************************************************
 *
 * Name              : msg_init
 *
 * Description       : initialize a message buffer object
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : message buffer data
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_init( msg_t *msg, unsigned limit, void *data );

/******************************************************************************
 *
 * Name              : msg_create
 * Alias             : msg_new
 *
 * Description       : create and initialize a new message buffer object
 *
 * Parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 * Return            : pointer to message buffer object (message buffer successfully created)
 *   0               : message buffer not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

msg_t *msg_create( unsigned limit );

__STATIC_INLINE
msg_t *msg_new( unsigned limit ) { return msg_create(limit); }

/******************************************************************************
 *
 * Name              : msg_kill
 *
 * Description       : reset the message buffer object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_kill( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_delete
 *
 * Description       : reset the message buffer object and free allocated resource
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void msg_delete( msg_t *msg );

/******************************************************************************
 *
 * Name              : msg_waitUntil
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait until given timepoint while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *   time            : timepoint value
 *
 * Return            : number of bytes read from the message buffer
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_waitUntil( msg_t *msg, void *data, unsigned size, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_waitFor
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait for given duration of time while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *   delay           : duration of time (maximum number of ticks to wait while the message buffer object is empty)
 *                     IMMEDIATE: don't wait if the message buffer object is empty
 *                     INFINITE:  wait indefinitely while the message buffer object is empty
 *
 * Return            : number of bytes read from the message buffer
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_waitFor( msg_t *msg, void *data, unsigned size, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_wait
 *
 * Description       : try to transfer data from the message buffer object,
 *                     wait indefinitely while the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the message buffer
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_wait( msg_t *msg, void *data, unsigned size ) { return msg_waitFor(msg, data, size, INFINITE); }

/******************************************************************************
 *
 * Name              : msg_take
 * ISR alias         : msg_takeISR
 *
 * Description       : try to transfer data from the message buffer object,
 *                     don't wait if the message buffer object is empty
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to write buffer
 *   size            : size of write buffer
 *
 * Return            : number of bytes read from the message buffer
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_take( msg_t *msg, void *data, unsigned size );

__STATIC_INLINE
unsigned msg_takeISR( msg_t *msg, void *data, unsigned size ) { return msg_take(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_sendUntil
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait until given timepoint while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *   time            : timepoint value
 *
 * Return            : number of bytes written to the message buffer
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_sendUntil( msg_t *msg, const void *data, unsigned size, cnt_t time );

/******************************************************************************
 *
 * Name              : msg_sendFor
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait for given duration of time while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *   delay           : duration of time (maximum number of ticks to wait while the message buffer object is full)
 *                     IMMEDIATE: don't wait if the message buffer object is full
 *                     INFINITE:  wait indefinitely while the message buffer object is full
 *
 * Return            : number of bytes written to the message buffer
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned msg_sendFor( msg_t *msg, const void *data, unsigned size, cnt_t delay );

/******************************************************************************
 *
 * Name              : msg_send
 *
 * Description       : try to transfer data to the message buffer object,
 *                     wait indefinitely while the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return            : number of bytes written to the message buffer
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned msg_send( msg_t *msg, const void *data, unsigned size ) { return msg_sendFor(msg, data, size, INFINITE); }

/******************************************************************************
 *
 * Name              : msg_give
 * ISR alias         : msg_giveISR
 *
 * Description       : try to transfer data to the message buffer object,
 *                     don't wait if the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return            : number of bytes written to the message buffer
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_give( msg_t *msg, const void *data, unsigned size );

__STATIC_INLINE
unsigned msg_giveISR( msg_t *msg, const void *data, unsigned size ) { return msg_give(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_push
 * ISR alias         : msg_pushISR
 *
 * Description       : try to transfer data to the message buffer object,
 *                     remove the oldest data if the message buffer object is full
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *   data            : pointer to read buffer
 *   size            : size of read buffer
 *
 * Return
 * Return            : number of bytes written to the message buffer
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned msg_push( msg_t *msg, const void *data, unsigned size );

__STATIC_INLINE
unsigned msg_pushISR( msg_t *msg, const void *data, unsigned size ) { return msg_push(msg, data, size); }

/******************************************************************************
 *
 * Name              : msg_count
 * ISR alias         : msg_countISR
 *
 * Description       : return the size of the first message in the buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of data contained in the message buffer
 *
 ******************************************************************************/

unsigned msg_count( msg_t *msg );

__STATIC_INLINE
unsigned msg_countISR( msg_t *msg ) { return msg_count(msg); }

/******************************************************************************
 *
 * Name              : msg_space
 * ISR alias         : msg_spaceISR
 *
 * Description       : return the amount of free space in the message buffer
 *
 * Parameters
 *   msg             : pointer to message buffer object
 *
 * Return            : amount of free space in the message buffer
 *
 ******************************************************************************/

unsigned msg_space( msg_t *msg );

__STATIC_INLINE
unsigned msg_spaceISR( msg_t *msg ) { return msg_space(msg); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseMessageBuffer
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *   data            : message buffer data
 *
 * Note              : for internal use
 *
 ******************************************************************************/

struct baseMessageBuffer : public __msg
{
	 explicit
	 baseMessageBuffer( const unsigned _limit, char * const _data ): __msg _MSG_INIT(_limit, _data) {}
	~baseMessageBuffer( void ) { assert(queue == nullptr); }

	void     kill     ( void )                                            {        msg_kill     (this);                       }
	unsigned waitUntil(       void *_data, unsigned _size, cnt_t _time  ) { return msg_waitUntil(this, _data, _size, _time);  }
	unsigned waitFor  (       void *_data, unsigned _size, cnt_t _delay ) { return msg_waitFor  (this, _data, _size, _delay); }
	unsigned wait     (       void *_data, unsigned _size )               { return msg_wait     (this, _data, _size);         }
	unsigned take     (       void *_data, unsigned _size )               { return msg_take     (this, _data, _size);         }
	unsigned takeISR  (       void *_data, unsigned _size )               { return msg_takeISR  (this, _data, _size);         }
	unsigned sendUntil( const void *_data, unsigned _size, cnt_t _time  ) { return msg_sendUntil(this, _data, _size, _time);  }
	unsigned sendFor  ( const void *_data, unsigned _size, cnt_t _delay ) { return msg_sendFor  (this, _data, _size, _delay); }
	unsigned send     ( const void *_data, unsigned _size )               { return msg_send     (this, _data, _size);         }
	unsigned give     ( const void *_data, unsigned _size )               { return msg_give     (this, _data, _size);         }
	unsigned giveISR  ( const void *_data, unsigned _size )               { return msg_giveISR  (this, _data, _size);         }
	unsigned push     ( const void *_data, unsigned _size )               { return msg_push     (this, _data, _size);         }
	unsigned pushISR  ( const void *_data, unsigned _size )               { return msg_pushISR  (this, _data, _size);         }
	unsigned count    ( void )                                            { return msg_count    (this);                       }
	unsigned countISR ( void )                                            { return msg_countISR (this);                       }
	unsigned space    ( void )                                            { return msg_space    (this);                       }
	unsigned spaceISR ( void )                                            { return msg_spaceISR (this);                       }
};

/******************************************************************************
 *
 * Class             : MessageBuffer
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored bytes)
 *
 ******************************************************************************/

template<unsigned _limit>
struct MessageBufferT : public baseMessageBuffer
{
	explicit
	MessageBufferT( void ): baseMessageBuffer(sizeof(data_), data_) {}

	private:
	char data_[_limit];
};

/******************************************************************************
 *
 * Class             : MessageBuffer
 *
 * Description       : create and initialize a message buffer object
 *
 * Constructor parameters
 *   limit           : size of a buffer (max number of stored objects)
 *   T               : class of an object
 *
 ******************************************************************************/

template<unsigned _limit, class T>
struct MessageBufferTT : public baseMessageBuffer
{
	explicit
	MessageBufferTT( void ): baseMessageBuffer(sizeof(data_), data_) {}

	private:
	char data_[_limit*(sizeof(unsigned)+sizeof(T))-sizeof(unsigned)];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_MSG_H
