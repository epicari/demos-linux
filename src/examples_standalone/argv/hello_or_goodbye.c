/*
 * This file is part of the linuxapi project.
 * Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * The linuxapi package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The linuxapi package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <string.h>	// for strcmp(3)

/*
 * This is the most basic hello, world program for C
 */

// the arguments to main which are not used DO NOT
// create a compilation error even with all warnings turned
// on
// also main DOES NOT have to return int
int main(int argc, char** argv, char** envp) {
	if(strcmp(argv[0],"./hello.out")==0) {
		printf("Hello, World!\n");
		return EXIT_SUCCESS;
	}
	if(strcmp(argv[0],"./goodbye.out")==0) {
		printf("Goodbye, World!\n");
		return EXIT_SUCCESS;
	}
	printf("What do you want from me?!?\n");
	return EXIT_SUCCESS;
}