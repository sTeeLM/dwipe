/*
 *  prng.h: Pseudo Random Number Generator abstractions for dwipe.
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

#ifndef PRNG_H_
#define PRNG_H_

/* A chunk of random data. */
typedef struct /* dwipe_entropy_t */
{
	size_t  length;  /* Length of the entropy string in bytes.  */
	u8*     s;       /* The actual bytes of the entropy string. */
} dwipe_entropy_t;

#define DWIPE_PRNG_INIT_SIGNATURE void** state, dwipe_entropy_t* seed
#define DWIPE_PRNG_READ_SIGNATURE void** state, void* buffer, size_t count

/* Function pointers for PRNG actions. */
typedef int(*dwipe_prng_init_t)( DWIPE_PRNG_INIT_SIGNATURE );
typedef int(*dwipe_prng_read_t)( DWIPE_PRNG_READ_SIGNATURE );

/* The generic PRNG definition. */
typedef struct /* dwipe_prng_t */
{
	const char*       label;  /* The name of the pseudo random number generator. */
	dwipe_prng_init_t init;   /* Inialize the prng state with the seed.          */
	dwipe_prng_read_t read;   /* Read data from the prng.                        */
} dwipe_prng_t;

/* Mersenne Twister prototypes. */
int dwipe_twister_init( DWIPE_PRNG_INIT_SIGNATURE );
int dwipe_twister_read( DWIPE_PRNG_READ_SIGNATURE );

/* ISAAC prototypes. */
int dwipe_isaac_init( DWIPE_PRNG_INIT_SIGNATURE );
int dwipe_isaac_read( DWIPE_PRNG_READ_SIGNATURE );

#endif /* PRNG_H_ */

/* eof */
