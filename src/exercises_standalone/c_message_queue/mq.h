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

#ifndef __mq_h
#define __mq_h

#include <firstinclude.h>

// our message queue is obfuscated so that the user will not tempted
// to fiddle with it's innards
typedef struct _mq *mq;

int mq_init(mq* m, const int maxsize);
int mq_destroy(mq m);
int mq_put(mq m, void* message);
void* mq_get(mq m);

#endif // __mq_h
