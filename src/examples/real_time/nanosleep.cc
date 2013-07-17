/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <time.h>	// for nanosleep(2), timespec
#include <us_helper.h>	// for getticks(), get_mic_diff()
#include <sched_utils.h>// for sched_run_priority(), SCHED_FIFO_LOW_PRIORITY:const, SCHED_FIFO_MID_PRIORITY:const, SCHED_FIFO_HIGH_PRIORITY:const

/*
 * This is an example showing how to sleep for very short periods of
 * time and proves that it is indeed so...
 *
 * As you can see there is a diff of about 50-100 micro between the time
 * to sleep and the real time slept. This is MUCH less than the HZ
 * value for linux (ranging from 100HZ to 1000HZ which yields 10000 micro
 * to 1000 micro which are much bigger values.
 *
 * In order to actually prove a low latency for this call
 * there is still a need to split this
 * program into two threads the higher priority of which runs this
 * code and the lower keeping the CPU occupied all the time (by doing
 * some calculation). According to expection the difference from the
 * expected sleep time should remain about 100 micro.
 *
 * Where does this 100 micro come from ? The answer lies in the accuracy
 * of the interrupt handler, it's clock and the time to reschedule...
 * The real time patch to the kernel can reduce this further...
 *
 * Take note that nanosleep does not involve signals and so is 'signal
 * safe' and on machines with a good rtc will use it. read more in the
 * documentation.
 *
 * Note that the real time thread gets much better latency of just a
 * few (5?) micros at worst.
 *
 * I've had problems with this example in the past on ubuntu since
 * the cpu_freq functions returned 0 as the cpu frequency. This
 * problem has disappeared.
 *
 * the -lcpufreq is for the get_mic_diff
 * EXTRA_LINK_FLAGS=-lcpufreq -lpthread
 *
 * TODO:
 * - use usleep and sleep also and compare the results to those of nanosleep.
 * - use other ways than getticks() to measure how much time has passed and
 * compare results.
 */

void* measure(void* val) {
	const unsigned int repeats=100;
	for(unsigned int i=100; i<2000; i+=100) {
		unsigned long sum=0;
		for(unsigned int j=0; j<repeats; j++) {
			timespec t;
			t.tv_sec=0;
			t.tv_nsec=1000 * i;
			ticks_t start=getticks();
			nanosleep(&t, NULL);
			ticks_t end=getticks();
			sum+=get_mic_diff(start, end);
		}
		printf("time expired for nanosleep(2) of %u micros is %lu diff is %lu\n", i, sum / repeats, sum / repeats - i);
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	printf("SCHED_OTHER running\n");
	sched_run_priority(measure, NULL, 0, SCHED_OTHER);
	printf("low priority running\n");
	sched_run_priority(measure, NULL, SCHED_FIFO_LOW_PRIORITY, SCHED_FIFO);
	printf("mid priority running\n");
	sched_run_priority(measure, NULL, SCHED_FIFO_MID_PRIORITY, SCHED_FIFO);
	printf("high priority running\n");
	sched_run_priority(measure, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	return EXIT_SUCCESS;
}
