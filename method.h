/*
 *  methods.c: Method implementations for dwipe.
 *
 *  Copyright Darik Horn <dajhorn-dban@vanadac.com>.
 *
 *  This program is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, version 2.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 *  Ave, Cambridge, MA 02139, USA.
 *
 */


#ifndef METHOD_H_
#define METHOD_H_

/* The argument list for dwipe methods. */
#define DWIPE_METHOD_SIGNATURE dwipe_context_t* c

typedef enum dwipe_verify_t_
{
	DWIPE_VERIFY_NONE = 0,  /* Do not read anything back from the device. */
	DWIPE_VERIFY_LAST,      /* Check the last pass.                       */
	DWIPE_VERIFY_ALL,       /* Check all passes.                          */
} dwipe_verify_t;


/* The typedef of the function that will do the wipe. */
typedef int(*dwipe_method_t)( DWIPE_METHOD_SIGNATURE );

typedef struct /* dwipe_pattern_t */
{
	int   length;  /* Length of the pattern in bytes, -1 means random.  */
	char* s;       /* The actual bytes of the pattern.                  */
} dwipe_pattern_t;

const char* dwipe_method_label( dwipe_method_t method );
int dwipe_runmethod( DWIPE_METHOD_SIGNATURE, dwipe_pattern_t* patterns );

int dwipe_dod522022m( DWIPE_METHOD_SIGNATURE );
int dwipe_dodshort( DWIPE_METHOD_SIGNATURE );
int dwipe_gutmann( DWIPE_METHOD_SIGNATURE );
int dwipe_ops2( DWIPE_METHOD_SIGNATURE );
int dwipe_random( DWIPE_METHOD_SIGNATURE );
int dwipe_zero( DWIPE_METHOD_SIGNATURE );

#endif /* METHOD_H_ */

/* eof */
