/*
 *  pass.h: Routines that read and write patterns to block devices.
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

#ifndef PASS_H_
#define PASS_H_

int dwipe_random_pass  ( dwipe_context_t* c );
int dwipe_random_verify( dwipe_context_t* c );
int dwipe_static_pass  ( dwipe_context_t* c, dwipe_pattern_t* pattern );
int dwipe_static_verify( dwipe_context_t* c, dwipe_pattern_t* pattern );

#endif /* PASS_H_ */

/* eof */
