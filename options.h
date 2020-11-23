/*
 *  options.h: Command line processing routines for dwipe.
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


#ifndef OPTIONS_H_
#define OPTIONS_H_

/* Program knobs. */
#define DWIPE_KNOB_ENTROPY                "/dev/urandom"
#define DWIPE_KNOB_IDENTITY_SIZE          512
#define DWIPE_KNOB_LABEL_SIZE             512
#define DWIPE_KNOB_LOADAVG                "/proc/loadavg"
#define DWIPE_KNOB_LOG_BUFFERSIZE         1024                /* Maximum length of a log event. */
#define DWIPE_KNOB_PARTITIONS             "/proc/partitions"
#define DWIPE_KNOB_PARTITIONS_PREFIX      "/dev/"
#define DWIPE_KNOB_PRNG_STATE_LENGTH      512                 /* 128 words */
#define DWIPE_KNOB_SCSI                   "/proc/scsi/scsi"
#define DWIPE_KNOB_SLEEP                  1
#define DWIPE_KNOB_STAT                   "/proc/stat"
#define DBAN_VERSION                      "2.2.1"

/* FIXME: This should be a command line option. */
#define DWIPE_KNOB_LOGFILE "/var/log/dban/dwipe.txt"

/* Function prototypes for loading options from the environment and command line. */
int dwipe_options_parse( int argc, char** argv );
void dwipe_options_log( void );

typedef struct /* dwipe_options_t */
{
	int            autonuke;  /* Do not prompt the user for confirmation when set.          */
	char*          banner;    /* The product banner shown on the top line of the screen.    */
	dwipe_method_t method;    /* A function pointer to the wipe method that will be used.   */
	dwipe_prng_t*  prng;      /* The pseudo random number generator implementation.         */
	int            rounds;    /* The number of times that the wipe method should be called. */
	int            sync;      /* A flag to indicate whether writes should be sync'd.        */
	dwipe_verify_t verify;    /* A flag to indicate whether writes should be verified.      */
    char*          exclude;
} dwipe_options_t;

extern dwipe_options_t dwipe_options;

#endif /* OPTIONS_H_ */

/* eof */
