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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to use C99 designated initializers.
 *
 * References:
 * http://www.drdobbs.com/the-new-c-x-macros/184401387
 */

#include <stdio.h>

#define COLOR_TABLE \
X(red, "red") \
X(green, "green") \
X(blue, "blue")

#define X(a, b) a,
enum COLOR {
	COLOR_TABLE
};
#undef X

#define X(a, b) b,
char *color_name[] = {
	COLOR_TABLE
};
#undef X

int main(int argc, char** argv, char** envp) {
	enum COLOR c = red;
	printf("c=%s\n", color_name[c]);
	return EXIT_SUCCESS;
}
