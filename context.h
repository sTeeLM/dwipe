/*
 *  context.h: The internal state representation of dwipe.
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

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "prng.h"

typedef enum dwipe_device_t_
{
	DWIPE_DEVICE_UNKNOWN = 0,   /* Unknown device. */
	DWIPE_DEVICE_IDE,
	DWIPE_DEVICE_SCSI,
	DWIPE_DEVICE_COMPAQ,        /* Unimplemented.  */
	DWIPE_DEVICE_USB,           /* Unimplemented.  */
	DWIPE_DEVICE_IEEE1394       /* Unimplemented.  */
} dwipe_device_t;

typedef enum dwipe_pass_t_
{
	DWIPE_PASS_NONE = 0,     /* Not running.                     */
	DWIPE_PASS_WRITE,        /* Writing patterns to the device.  */
	DWIPE_PASS_VERIFY,       /* Verifying a pass.                */
	DWIPE_PASS_FINAL_BLANK,  /* Filling the device with zeros.   */
	DWIPE_PASS_FINAL_OPS2    /* Special case for dwipe_ops2.     */
} dwipe_pass_t;

typedef enum dwipe_select_t_
{
	DWIPE_SELECT_NONE = 0,     /* Unused.                                                                  */
	DWIPE_SELECT_TRUE,         /* Wipe this device.                                                        */
	DWIPE_SELECT_TRUE_PARENT,  /* A parent of this device has been selected, so the wipe is implied.       */
	DWIPE_SELECT_FALSE,        /* Do not wipe this device.                                                 */
	DWIPE_SELECT_FALSE_CHILD,  /* A child of this device has been selected, so we can't wipe this device.  */
	DWIPE_SELECT_DISABLED,     /* Do not wipe this device and do not allow it to be selected.              */
	DWIPE_SELECT_SKIPPED       /* Skipped */
} dwipe_select_t;


#define DWIPE_KNOB_SPEEDRING_SIZE         30
#define DWIPE_KNOB_SPEEDRING_GRANULARITY  10

typedef struct dwipe_speedring_t_
{
	u64    bytes[DWIPE_KNOB_SPEEDRING_SIZE];
	u64    bytestotal;
	u64    byteslast;
	time_t times[DWIPE_KNOB_SPEEDRING_SIZE];
	time_t timestotal;
	time_t timeslast;
	u32    position;
} dwipe_speedring_t;


typedef struct dwipe_context_t_
{
	int               block_size;    /* The soft block size reported the device.                    */
	int               device_bus;    /* The device bus number.                                      */
	int               device_fd;     /* The file descriptor of the device file being wiped.         */
	int               device_host;   /* The host number.                                            */
	struct hd_driveid device_id;     /* The WIN_IDENTIFY data for IDE drives.                       */
	int               device_lun;    /* The device logical unit number.                             */
	int               device_major;  /* The major device number.                                    */
	int               device_minor;  /* The minor device number.                                    */
	int               device_part;   /* The device partition or slice number.                       */
	char*             device_name;   /* The device file name.                                       */
	loff_t            device_size;   /* The device size in bytes.                                   */
	struct stat       device_stat;   /* The device file state from fstat().                         */
	dwipe_device_t    device_type;   /* Indicates an IDE, SCSI, or Compaq SMART device.             */
	int               device_target; /* The device target.                                          */
	u64               eta;           /* The estimated number of seconds until method completion.    */
	int               entropy_fd;    /* The entropy source. Usually /dev/urandom.                   */
	char*             label;         /* The string that we will show the user.                      */
	int               pass_count;    /* The number of passes performed by the working wipe method.  */
	u64               pass_done;     /* The number of bytes that have already been i/o'd.           */
	u64               pass_errors;   /* The number of errors across all passes.                     */
	u64               pass_size;     /* The total number of i/o bytes across all passes.            */
	dwipe_pass_t      pass_type;     /* The type of the current working pass.                       */
	int               pass_working;  /* The current working pass.                                   */
	pid_t             pid;           /* The process that has been assigned to do the wipe.          */
	dwipe_prng_t*     prng;          /* The PRNG implementation.                                    */
	dwipe_entropy_t   prng_seed;     /* The random data that is used to seed the PRNG.              */
	void*             prng_state;    /* The private internal state of the PRNG.                     */
	int               result;        /* The process return value.                                   */
	int               round_count;   /* The number of rounds performed by the working wipe method.  */
	u64               round_done;    /* The number of bytes that have already been i/o'd.           */
	u64               round_errors;  /* The number of errors across all rounds.                     */
	u64               round_size;    /* The total number of i/o bytes across all rounds.            */
	double            round_percent; /* The percentage complete across all rounds.                  */
	int               round_working; /* The current working round.                                  */
	int               sector_size;   /* The hard sector size reported by the device.                */
	dwipe_select_t    select;        /* Indicates whether this device should be wiped.              */
	int               signal;        /* Set when the child is killed by a signal.                   */
	dwipe_speedring_t speedring;     /* Ring buffer for computing the rolling throughput average.   */
	int               status;        /* The last process status value from waitpid().               */
	short             sync_status;   /* A flag to indicate when the method is syncing.              */
	u64               throughput;    /* Average throughput in bytes per second.                     */
	u64               verify_errors; /* The number of verification errors across all passes.        */
} dwipe_context_t;

#endif /* CONTEXT_H_ */

/* eof */
