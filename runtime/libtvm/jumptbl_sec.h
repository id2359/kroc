/*
tvm - jumptbl_sec.h
The Transterpreter - a portable virtual machine for Transputer bytecode
Copyright (C) 2004-2008 Christian L. Jacobsen, Matthew C. Jadud

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
 * jumptbl_sec.h - header for jumptbl_sec.c
 *
 * Copyright (C) 2004    Matt C. Jadud and Christian L. Jacobsen
 */

#ifndef JUMPTBL_SEC_H
#define JUMPTBL_SEC_H

extern const unsigned int secondaries_min;
extern const unsigned int secondaries_max;
extern int (*const secondaries[])(tvm_ectx_t *);

#endif /* JUMPTBL_SEC_H */
