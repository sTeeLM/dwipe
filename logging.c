/*
 *  logging.c:  Logging facilities for dwipe.
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


#include "dwipe.h"
#include "context.h"
#include "method.h"
#include "prng.h"
#include "options.h"
#include "logging.h"


void dwipe_log( dwipe_log_t level, const char* format, ... )
{
/**
 *  Writes a message to the program log file.
 *
 */


	/* A result buffer. */
	int r;

	/* A time buffer. */
	time_t t;

	/* A pointer to the system time struct. */
	struct tm* p;

	/* The log file pointer. */
	FILE* fp;

	/* The log file descriptor. */
	int fd;

	/* The variable argument pointer. */
	va_list ap;

	/* Get the current time. */
	t = time( NULL );
	p = gmtime( &t );

	/* Open the log file for appending. */
	fp = fopen( DWIPE_KNOB_LOGFILE, "a" );

	if( fp == NULL )
	{
		fprintf( stderr, "dwipe_log: Unable to open '%s' for logging.\n", DWIPE_KNOB_LOGFILE );
		return;
	}
	
	/* Get the file descriptor of the log file. */
	fd = fileno( fp );

	/* Block and lock. */
	r = flock( fd, LOCK_EX );

	if( r != 0 )
	{
		perror( "dwipe_log: flock:" );
		fprintf( stderr, "dwipe_log: Unable to lock '%s' for logging.\n", DWIPE_KNOB_LOGFILE );
		return;
	}

	/* Print the date. The rc script uses the same format. */
	fprintf( fp, "[%i/%02i/%02i %02i:%02i:%02i] dwipe: ", \
	  1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec );

	switch( level )
	{

		case DWIPE_LOG_NONE:
			/* Do nothing. */
			break;

		case DWIPE_LOG_DEBUG:
			fprintf( fp, "debug: " );
			break;

		case DWIPE_LOG_INFO:
			fprintf( fp, "info: " );
			break;

		case DWIPE_LOG_NOTICE:
			fprintf( fp, "notice: " );
			break;

		case DWIPE_LOG_WARNING:
			fprintf( fp, "warning: " );
			break;

		case DWIPE_LOG_ERROR:
			fprintf( fp, "error: " );
			break;

		case DWIPE_LOG_FATAL:
			fprintf( fp, "fatal: " );
			break;

		case DWIPE_LOG_SANITY:
			/* TODO: Request that the user report the log. */
			fprintf( fp, "sanity: " );
			break;

		default:
			fprintf( fp, "level %i: ", level );

	}

	/* Fetch the argument list. */
	va_start( ap, format );

	/* Print the event. */
	vfprintf( fp, format, ap );

/*
	if( level >= DWIPE_LOG_WARNING )
	{
		vfprintf( stderr, format, ap );
	}
*/

	/* Release the argument list. */
	va_end( ap );

	/* Append a newline. */
	fprintf( fp, "\n" );

/*
	if( level >= DWIPE_LOG_WARNING )
	{
		fprintf( stderr, "\n" );
	}
*/

	/* Unlock the file. */
	r = flock( fd, LOCK_UN );

	if( r != 0 )
	{
		perror( "dwipe_log: flock:" );
		fprintf( stderr, "Error: Unable to unlock '%s' after logging.\n", DWIPE_KNOB_LOGFILE );
	}

	/* Close the stream. */
	r = fclose( fp );

	if( r != 0 )
	{
		perror( "dwipe_log: fclose:" );
		fprintf( stderr, "Error: Unable to close '%s' after logging.\n", DWIPE_KNOB_LOGFILE );
	}

} /* dwipe_log */


void dwipe_perror( int dwipe_errno, const char* f, const char* s )
{
/**
 * Wrapper for perror().
 *
 * We may wish to tweak or squelch this later. 
 *
 */

	dwipe_log( DWIPE_LOG_ERROR, "%s: %s: %s", f, s, strerror( dwipe_errno ) );

} /* dwipe_perror */

/* eof */
