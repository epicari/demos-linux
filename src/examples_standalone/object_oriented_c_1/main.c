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

#include "rectangle.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv, char** envp) {
	rectangle* r=(rectangle*)malloc(sizeof(rectangle));
	rectangle_set_width(r,5);
	rectangle_set_height(r,6);
	printf("the area of the rectangle is %d\n", rectangle_area(r));
	return 0;
}
