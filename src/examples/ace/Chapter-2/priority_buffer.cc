/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

#include<firstinclude.h>
#include<ace/OS_NS_stdio.h>
#include<ace/Malloc_Base.h> // To get ACE_Allocator
#include<ace/Message_Queue.h>
#include<ace/Read_Buffer.h>
#include<ace/Thread_Manager.h>
#include<ace/Service_Config.h>
#include<stdlib.h> // for EXIT_SUCCESS

/*
* This short program prints the contents of stdin to stdout sorted by
* the length of each line via the use of an ASX Message_Queue. It
* illustrates how priorities can be used for ACE Message_Queues.
*
* EXTRA_CMDS=pkg-config --cflags --libs ACE
*/

// Make the queue be capable of being *very* large.
static const long max_queue=LONG_MAX;

// The consumer dequeues a message from the ACE_Message_Queue, writes
// the message to the stderr stream, and deletes the message. The
// producer sends a 0-sized message to inform the consumer to stop
// reading and exit.
static void *consumer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {
	// Keep looping, reading a message out of the queue, until we
	// timeout or get a message with a length==0, which signals us to
	// quit.
	while(true) {
		ACE_Message_Block *mb;
		if(msg_queue->dequeue_head(mb)==-1) {
			break;
		}
		size_t length=mb->length();
		if(length>0) {
			ACE_OS::puts(mb->rd_ptr());
		}
		// Free up the buffer memory and the Message_Block.
		ACE_Allocator::instance()->free(mb->rd_ptr());
		mb->release();
		if(length==0) {
			break;
		}
	}
	return(NULL);
}


// The producer reads data from the stdin stream, creates a message,
// and then queues the message in the message list, where it is
// removed by the consumer thread. A 0-sized message is enqueued when
// there is no more data to read. The consumer uses this as a flag to
// know when to exit.
static void *producer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {
	ACE_Read_Buffer rb(ACE_STDIN);
	// Keep reading stdin, until we reach EOF.
	while(true) {
		// Allocate a new buffer.
		char *buffer=rb.read('\n');
		ACE_Message_Block *mb;
		if(buffer==0) {
			// Send a 0-sized shutdown message to the other thread and
			// exit.
			ACE_NEW_RETURN(mb, ACE_Message_Block((size_t)0), 0);
			if(msg_queue->enqueue_tail(mb)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
			break;
		} else {
			// Enqueue the message in priority order.
			// Allocate a new message, but have it "borrow" its memory
			// from the buffer.
			ACE_NEW_RETURN(mb, ACE_Message_Block(rb.size(), ACE_Message_Block::MB_DATA, 0, buffer), 0);
			// Setting the priority according to message length.
			mb->msg_priority(rb.size());
			mb->wr_ptr(rb.size());

			ACE_DEBUG((LM_DEBUG, "enqueueing message of size %d\n", mb->msg_priority()));

			// Enqueue in priority order.
			if(msg_queue->enqueue_prio(mb)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
		}
	}
	// Now read all the items out in priority order (i.e., ordered by
	// the size of the lines!).
	//consumer(msg_queue);
	return(NULL);
}

// Global thread manager.
static ACE_Thread_Manager thr_mgr;

// Spawn off one thread that copies stdin to stdout in order of the
// size of each line.
int ACE_TMAIN(int argc,ACE_TCHAR** argv,ACE_TCHAR** envp) {
	// first way to run this app is this way:
	/*
	ACE_Message_Queue<ACE_MT_SYNCH> msg_queue(max_queue);
	producer(&msg_queue);
	*/
	// second way to run this app - one thread in the background:
	/*
	ACE_Message_Queue<ACE_MT_SYNCH> msg_queue(max_queue);
	if(thr_mgr.spawn(ACE_THR_FUNC(producer),(void *)&msg_queue,THR_NEW_LWP|THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn"), 1);
	}
	// Wait for producer and consumer threads to exit.
	thr_mgr.wait();
	*/
	// third way: two threads in the background
	ACE_Message_Queue<ACE_MT_SYNCH> msg_queue(max_queue);
	if(thr_mgr.spawn(ACE_THR_FUNC(producer),(void *)&msg_queue,THR_NEW_LWP|THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn"), 1);
	}
	if(thr_mgr.spawn(ACE_THR_FUNC(consumer),(void *)&msg_queue,THR_NEW_LWP|THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn"), 1);
	}
	thr_mgr.wait();
	// Wait for producer and consumer threads to exit.
	return EXIT_SUCCESS;
}
