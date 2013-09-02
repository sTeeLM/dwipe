/*
 *  prng.c: Pseudo Random Number Generator abstractions for dwipe.
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
 */


#include "dwipe.h"
#include "prng.h"
#include "logging.h"

#include "mt19937ar-cok.h"
#include "isaac_rand.h"

dwipe_prng_t dwipe_twister =
{
	"Mersenne Twister (mt19937ar-cok)",
	dwipe_twister_init,
	dwipe_twister_read
};

dwipe_prng_t dwipe_isaac =
{
	"ISAAC (rand.c 20010626)",
	dwipe_isaac_init,
	dwipe_isaac_read
};



int dwipe_twister_init( DWIPE_PRNG_INIT_SIGNATURE )
{
	if( *state == NULL )
	{
		/* This is the first time that we have been called. */
		*state = malloc( sizeof( twister_state_t ) );
	}
	twister_init( (twister_state_t*)*state, (u32*)( seed->s ), seed->length / sizeof( u32 ) );
	return 0;
}

int dwipe_twister_read( DWIPE_PRNG_READ_SIGNATURE )
{
	u32 ii;
	u32 words = count / sizeof( u32 );
	u32 remain = count % sizeof( u32 );

	/* Twister returns 4-bytes per call, so cast the buffer into words. */
	for( ii = 0; ii < words; ++ii )
	{
		((u32*)buffer)[ii] = twister_genrand_int32( (twister_state_t*)*state );
	}

	/* Fill the buffer tail if the count is not evenly divided by the size of u32. */
	for( ii = 1; ii <= remain; ++ii )
	{
		/* Notice how three bytes are discarded by doing this. */
		((u8*)buffer)[count-ii] = twister_genrand_int32( (twister_state_t*)*state );
	}
	 
	return 0;
}



int dwipe_isaac_init( DWIPE_PRNG_INIT_SIGNATURE )
{
	int count;
	randctx* isaac_state = *state;

	if( *state == NULL )
	{
		/* This is the first time that we have been called. */
		*state = malloc( sizeof( randctx ) );
		isaac_state = *state;

		/* Check the memory allocation. */
		if( isaac_state == 0 )
		{
				dwipe_perror( errno, __FUNCTION__, "malloc" );
				dwipe_log( DWIPE_LOG_FATAL, "Unable to allocate memory for the isaac state." );
				return -1;
		}
	}


	/* Take the minimum of the isaac seed size and available entropy. */
	if( sizeof( isaac_state->randrsl ) < seed->length ) 
	{
		count = sizeof( isaac_state->randrsl );	
	}
	else
	{
		memset( isaac_state->randrsl, 0, sizeof( isaac_state->randrsl ) );
		count = seed->length;
	}

	if( count == 0 )
	{
		/* Start ISACC without a seed. */
		randinit( isaac_state, 0 );
	}
	else
	{
		/* Seed the ISAAC state with entropy. */
		memcpy( isaac_state->randrsl, seed->s, count );

		/* The second parameter indicates that randrsl is non-empty. */
		randinit( isaac_state, 1 );
	}

	return 0;
}

int dwipe_isaac_read( DWIPE_PRNG_READ_SIGNATURE )
{
	return 0;
}

/* eof */
