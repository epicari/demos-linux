/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2017 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <ace/OS_NS_stdio.h>
#include <ace/streams.h>
#include <ace/Malloc_Base.h>	// To get ACE_Allocator
#include <ace/Message_Queue.h>
#include <ace/Read_Buffer.h>
#include <ace/Thread_Manager.h>
#include <ace/Service_Config.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Tail_buffer-modified.cpp - insert the zeo length message into
 * the head of the queue.
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lACE -ldl -lrt -lpthread
 */

// Global thread manager.
static ACE_Thread_Manager thr_mgr;

// Make the queue be capable of being *very* large.
static const long max_queue=LONG_MAX;

// The consumer dequeues a message from the ACE_Message_Queue, writes
// the message to the stderr stream, and deletes the message. The
// producer sends a 0-sized message to inform the consumer to stop
// reading and exit.
static void *consumer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {	// Keep looping, reading a message out of the queue, until we
	// timeout or get a message with a length==0, which signals us to
	// quit.
	while(true) {
		ACE_Message_Block *mb;
		// Read from the head
		if(msg_queue->dequeue_tail(mb)==-1) {
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
	return(0);
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
			if(msg_queue->enqueue_head(mb)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
			break;
		}
		// Enqueue the message in fifo order.
		else {
			// Allocate a new message, but have it "borrow" its memory
			// from the buffer.
			ACE_NEW_RETURN(mb, ACE_Message_Block(rb.size(), ACE_Message_Block::MB_DATA, 0, buffer), 0);
			mb->wr_ptr(rb.size());

			ACE_DEBUG((LM_DEBUG, "enqueueing message of size %d\n", rb.size()));
			// Enqueue into the tail
			if(msg_queue->enqueue_tail(mb)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
		}
	}
	// Now read all the items from the tail (therefore we get reverse order)
	consumer(msg_queue);

	cout << "Program end. Why we did not get any output ???" << endl;
	cout << "Hint: what was the 1st line read by the consumer?" << endl;
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// Message queue.
	ACE_Message_Queue<ACE_MT_SYNCH> msg_queue(max_queue);
	if(thr_mgr.spawn(ACE_THR_FUNC(producer), (void *)&msg_queue, THR_NEW_LWP|THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn"), 1);
	}
	// Wait for producer and consumer threads to exit.
	thr_mgr.wait();
	return EXIT_SUCCESS;
}
