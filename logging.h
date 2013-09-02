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


#ifndef LOGGING_H_
#define LOGGING_H_

typedef enum dwipe_log_t_
{
	DWIPE_LOG_NONE = 0,
	DWIPE_LOG_DEBUG,     /* TODO:  Very verbose logging.             */
	DWIPE_LOG_INFO,      /* TODO:  Verbose logging.                  */
	DWIPE_LOG_NOTICE,    /* Most logging happens at this level.      */
	DWIPE_LOG_WARNING,   /* Things that the user should know about.  */
	DWIPE_LOG_ERROR,     /* Non-fatal errors that result in failure. */
	DWIPE_LOG_FATAL,     /* Errors that cause the program to exit.   */
	DWIPE_LOG_SANITY     /* Programming errors.                      */
} dwipe_log_t;

void dwipe_log( dwipe_log_t level, const char* format, ... );
void dwipe_perror( int dwipe_errno, const char* f, const char* s );

#endif /* LOGGING_H_ */

/* eof */
