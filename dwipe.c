/*  vi: tabstop=3
 *
 *  dwipe.c:  Darik's Wipe.
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


#include <time.h>

#include "dwipe.h"
#include "context.h"
#include "method.h"
#include "prng.h"
#include "options.h"
#include "device.h"
#include "logging.h"
#include "gui.h"

#ifdef BB_DWIPE
#include "mt19937ar-cok.c"
#include "isaac_rand.c"
#include "gui.c"
#include "options.c"
#include "device.c"
#include "pass.c"
#include "method.c"
#include "logging.c"
#include "prng.c"
#endif

#include <sys/ioctl.h>  /* FIXME: Twice Included */
#include <sys/shm.h>
#include <wait.h>


#ifdef BB_DWIPE
int dwipe_main( int argc, char** argv )
#else
int main( int argc, char** argv )
#endif
{
	int dwipe_optind;       /* The result of dwipe_options().                    */
	int dwipe_enumerated;   /* The number of contexts that have been enumerated. */
	int dwipe_error = 0;    /* An error counter.                                 */
	int dwipe_pid;          /* The fork() result holder.                         */
	int dwipe_selected = 0; /* The number of contexts that have been selected.   */
	int dwipe_shmid;        /* A shared memory handle for the context array.     */
	int dwipe_wait  = 0;    /* The number of child processes that have returned. */
    
    /* Exclude device by command */
    dwipe_context_t * dwipe_exclude_device = NULL;

	/* The list of device filenames. */
	char** dwipe_names = NULL;
	
	/* Used to write-out the result file. */
	char dwipe_result_file [FILENAME_MAX];
	FILE* dwipe_result_fp;

	/* The entropy source file handle. */
	int dwipe_entropy; 

	/* The generic index variables. */
	int i;
	int j;

	/* The generic result buffer. */
	int r;

	/* The array of enumerated contexts. */
	dwipe_context_t* c1;

	/* The array of contexts that will actually be wiped. */
	dwipe_context_t* c2;

	dwipe_log( DWIPE_LOG_NOTICE, "Program loaded." );

	/* Open the entropy source. */
	dwipe_entropy = open( DWIPE_KNOB_ENTROPY, O_RDONLY );

	/* Check the result. */
	if( dwipe_entropy < 0 )
	{
		dwipe_perror( errno, __FUNCTION__, "open" );
		dwipe_log( DWIPE_LOG_FATAL, "Unable to open entropy source %s.", DWIPE_KNOB_ENTROPY );
		return errno;
	}

	dwipe_log( DWIPE_LOG_NOTICE, "Opened entropy source '%s'.", DWIPE_KNOB_ENTROPY );

	/* Parse command line options. */
	dwipe_optind = dwipe_options_parse( argc, argv );


	if( dwipe_optind == argc )
	{
		/* File names were not given by the user.  Scan for devices. */
		dwipe_enumerated = dwipe_device_scan( &dwipe_names );

		if( dwipe_enumerated == 0 )
		{
			dwipe_log( DWIPE_LOG_ERROR, "Storage devices not found." );
			return -1;
		}

		else
		{
			dwipe_log( DWIPE_LOG_INFO, "Automatically enumerated %i devices.", dwipe_enumerated );
		}
	}

	else
	{
		/* Recall that all file names have been moved to the end of the argv. */
		dwipe_names = &argv[ dwipe_optind ];

		/* Determine the number of command line arguments that were file names. */
		dwipe_enumerated = argc - dwipe_optind;
	}

	/* Allocate memory for the first context array. */
	c1 = malloc( dwipe_enumerated * sizeof( dwipe_context_t ) );

	/* Check the allocation. */
	if( ! c1 )
	{
		dwipe_perror( errno, __FUNCTION__, "malloc" );
		dwipe_log( DWIPE_LOG_FATAL, "Unable to create the array of enumeration contexts." );
		return errno;
	}

	/* Zero the allocation. */
	memset( c1, 0, dwipe_enumerated * sizeof( dwipe_context_t ) );

	/* Create a context struct for each device. */
	for( i = 0; i < dwipe_enumerated; i++ )
	{
		/* A result buffer for the BLKGETSIZE64 ioctl. */
		u64 size64;

		/* Set the entropy source. */
		c1[i].entropy_fd = dwipe_entropy;

		/* Get the file name. */	
		c1[i].device_name = dwipe_names[i];

		/* Open the file for reads and writes. */
		c1[i].device_fd = open( c1[i].device_name, O_RDWR );

		/* Check the open() result. */
		if( c1[i].device_fd < 0 )
		{
			dwipe_perror( errno, __FUNCTION__, "open" );
			dwipe_log( DWIPE_LOG_WARNING, "Unable to open device '%s'. in rw mode", c1[i].device_name );
			c1[i].select = DWIPE_SELECT_DISABLED;
            if((c1[i].device_fd = open( c1[i].device_name, O_RDONLY )) < 0) 
            {
			    dwipe_perror( errno, __FUNCTION__, "open" );
			    dwipe_log( DWIPE_LOG_WARNING, "Unable to open device '%s'. in ro mode", c1[i].device_name );
                dwipe_device_identify(&c1[i]);
                continue;
            }
		}

		/* Stat the file. */
		if( fstat( c1[i].device_fd, &c1[i].device_stat ) != 0 ) 
		{
			dwipe_perror( errno, __FUNCTION__, "fstat");
			dwipe_log( DWIPE_LOG_ERROR, "Unable to stat file '%s'.", c1[i].device_name );
			dwipe_error++;
			continue;
		}

		/* Check that the file is a block device. */
		if( ! S_ISBLK( c1[i].device_stat.st_mode ) )
		{
			dwipe_log( DWIPE_LOG_ERROR, "'%s' is not a block device.", c1[i].device_name );
			dwipe_error++;
			continue;
		}

		/* Lock the file for exclusive access. */
		/* TODO: Determine why the LOCK_NB flag is not being honored. */
/* 
		if( flock( c1[i].device_fd, LOCK_EX | LOCK_NB ) != 0 )
		{
			dwipe_perror( errno, __FUNCTION__, "flock" );
			dwipe_log( DWIPE_LOG_ERROR, "Unable to lock the '%s' file.", c1[i].device_name );
			dwipe_error++;
			continue;
		}
*/

		/* Do sector size and block size checking. */

		if( ioctl( c1[i].device_fd, BLKSSZGET, &c1[i].sector_size ) == 0 )
		{
			dwipe_log( DWIPE_LOG_INFO, "Device '%s' has sector size %i.", c1[i].device_name,  c1[i].sector_size );

			if( ioctl( c1[i].device_fd, BLKBSZGET, &c1[i].block_size ) == 0 )
			{
				if( c1[i].block_size != c1[i].sector_size )
				{
					dwipe_log( DWIPE_LOG_WARNING, "Changing '%s' block size from %i to %i.", c1[i].device_name, c1[i].block_size, c1[i].sector_size );
					if( ioctl( c1[i].device_fd, BLKBSZSET, &c1[i].sector_size ) == 0 )
					{
						c1[i].block_size = c1[i].sector_size;
					}

					else
					{
						dwipe_log( DWIPE_LOG_WARNING, "Device '%s' failed BLKBSZSET ioctl.", c1[i].device_name );
					}
				}
			}
			else
			{
				dwipe_log( DWIPE_LOG_WARNING, "Device '%s' failed BLKBSZGET ioctl.", c1[i].device_name );
				c1[i].block_size  = 0;
			}
		}

		else
		{
			dwipe_log( DWIPE_LOG_WARNING, "Device '%s' failed BLKSSZGET ioctl.", c1[i].device_name );
			c1[i].sector_size = 0;
			c1[i].block_size  = 0;
		}


		/* The st_size field is zero for block devices. */
		/* ioctl( c1[i].device_fd, BLKGETSIZE64, &c1[i].device_size ); */

		/* Seek to the end of the device to determine its size. */
		c1[i].device_size = lseek( c1[i].device_fd, 0, SEEK_END );

		/* Also ask the driver for the device size. */
		/* if( ioctl( c1[i].device_fd, BLKGETSIZE64, &size64 ) ) */
		if( ioctl( c1[i].device_fd, _IOR(0x12,114,size_t), &size64 ) )
		{
			/* The ioctl failed. */
			fprintf( stderr, "Error: BLKGETSIZE64 failed  on '%s'.\n", c1[i].device_name );
			dwipe_log( DWIPE_LOG_ERROR, "BLKGETSIZE64 failed  on '%s'.\n", c1[i].device_name );
			dwipe_error++;
		}

		/* Check whether the two size values agree. */
		if( c1[i].device_size != size64 )
		{
			/* This could be caused by the linux last-odd-block problem. */
			fprintf( stderr, "Error: Last-odd-block detected on '%s'.\n", c1[i].device_name );
			dwipe_log( DWIPE_LOG_ERROR, "Last-odd-block detected on '%s'.", c1[i].device_name  );
			dwipe_error++;
		}

		if( c1[i].device_size == (loff_t)-1 )
		{
			/* We cannot determine the size of this device. */
			dwipe_perror( errno, __FUNCTION__, "lseek" );
			dwipe_log( DWIPE_LOG_ERROR, "Unable to determine the size of '%s'.", c1[i].device_name );
			dwipe_error++;
		}

		else
		{
			/* Reset the file pointer. */
			r = lseek( c1[i].device_fd, 0, SEEK_SET );
	
			if( r == (loff_t)-1 )
			{
				dwipe_perror( errno, __FUNCTION__, "lseek" );
				dwipe_log( DWIPE_LOG_ERROR, "Unable to reset the '%s' file offset.", c1[i].device_name );
				dwipe_error++;
			}
		}

		if( c1[i].device_size == 0 )
		{
			dwipe_log( DWIPE_LOG_ERROR, "Device '%s' is size %llu.", c1[i].device_name, c1[i].device_size );
			dwipe_error++;
			continue;
		}

		else
		{
			dwipe_log( DWIPE_LOG_INFO, "Device '%s' is size %llu.", c1[i].device_name,  c1[i].device_size );
		}

		/* Try to get detailed information about this device. */
		dwipe_device_identify( &c1[i] );

        if(dwipe_options.exclude && !strcmp(c1[i].device_name, dwipe_options.exclude)) 
        {
            c1[i].select = DWIPE_SELECT_SKIPPED;
            dwipe_log( DWIPE_LOG_INFO, "Device '%s' is excluded.", c1[i].device_name);

            if(c1[i].device_part == 0) // if device is a disk, exclude all partations
            {
                dwipe_exclude_device = &c1[i]; // save dwipe_exclude_device
            }
        }

        // trick: disk always processed before it's partations
        if(dwipe_exclude_device) 
        {
            if(c1[i].device_type       == dwipe_exclude_device->device_type
                && c1[i].device_host   == dwipe_exclude_device->device_host
                && c1[i].device_bus    == dwipe_exclude_device->device_bus
                && c1[i].device_target == dwipe_exclude_device->device_target
                && c1[i].device_lun    == dwipe_exclude_device->device_lun
                && c1[i].device_part > 0)
            {
                c1[i].select = DWIPE_SELECT_SKIPPED;
                dwipe_log( DWIPE_LOG_INFO, "Device '%s' is excluded.", c1[i].device_name);
            }
        }

		if( dwipe_options.autonuke )
		{
			/* When the autonuke option is set, select all disks. */
			if( c1[i].device_part == 0 && c1[i].select != DWIPE_SELECT_DISABLED && c1[i].select != DWIPE_SELECT_SKIPPED)
				{ c1[i].select = DWIPE_SELECT_TRUE; }
			else if( c1[i].select != DWIPE_SELECT_DISABLED && c1[i].select != DWIPE_SELECT_SKIPPED) 
				{ c1[i].select = DWIPE_SELECT_TRUE_PARENT; }
		}
		else
		{
			/* The user must manually select devices. */
            if(c1[i].select != DWIPE_SELECT_DISABLED && c1[i].select != DWIPE_SELECT_SKIPPED)
			    c1[i].select = DWIPE_SELECT_FALSE;
		}

		/* Set the PRNG implementation. */
		c1[i].prng = dwipe_options.prng;
		c1[i].prng_seed.length = 0;
		c1[i].prng_seed.s      = 0;
		c1[i].prng_state       = 0;

	} /* file arguments */

	/* Check for initialization errors. */
	if( dwipe_error ) { return -1; }

	/* Start the ncurses interface. */
	dwipe_gui_init();
	
	
	if( dwipe_options.autonuke == 1 )
	{
		/* Print the options window. */
		dwipe_gui_options();
	}

	else
	{
		/* Get device selections from the user. */
		dwipe_gui_select( dwipe_enumerated, c1 );
	}

	/* Count the number of selected contexts. */
	for( i = 0 ; i < dwipe_enumerated ; i++ )
	{
		if( c1[i].select == DWIPE_SELECT_TRUE )
		{
			dwipe_selected += 1;
		}
	}

	/* Allocate shared memory for the array of selected contexts. */ 
	dwipe_shmid = shmget( IPC_PRIVATE, dwipe_selected * sizeof( dwipe_context_t ), S_IRUSR | S_IWUSR );

	/* Check the allocation result. */
	if( dwipe_shmid < 0 )
	{
		dwipe_perror( errno, __FUNCTION__, "shmget" );
		dwipe_log( DWIPE_LOG_FATAL, "Unable to allocate shared memory for the context array." );
		dwipe_log( DWIPE_LOG_FATAL, "The return was %l for size %l.", dwipe_shmid, dwipe_selected * sizeof( dwipe_context_t ) );
		dwipe_gui_free();
		return errno;
	}

	/* Attach the shared memory to the context array pointer. */
	c2 = (dwipe_context_t*)( shmat( dwipe_shmid, 0, 0 ) ); 

	/* Check the attachment result. */
	if( c2 < 0 )
	{
		dwipe_perror( errno, __FUNCTION__, "shmat" );
		dwipe_log( DWIPE_LOG_FATAL, "Unable to attach shared memory." );
		dwipe_gui_free();
		return errno;
	}

	/* Populate the array of selected contexts. */
	for( i = 0, j = 0 ; i < dwipe_enumerated ; i++ )
	{
		if( c1[i].select == DWIPE_SELECT_TRUE )
		{
			/* Copy the context. */
			c2[j++] = c1[i];
		}

		else
		{
			/* Close the device file descriptor. */
			close( c1[i].device_fd );

			/* Release private resources. */
			free( c1[i].device_name );
			free( c1[i].label       );
		}

	} /* for */

	/* We're done with the array of enumerated contexts. */
	free( c1 );


	for( i = 0 ; i < dwipe_selected ; i++ )
	{
		if( c2[i].select )
		{
			/* Fork a child process. */
			dwipe_pid = fork();

			if( dwipe_pid )
			{
				/* The parent puts the child process number in its context. */
				c2[i].pid = dwipe_pid;
			}

			else
			{
				/* The child invokes the wipe method and exits. */
				return dwipe_options.method( &c2[i] );
			}
		}

		else
		{
			c2[i].pid = -1;
			dwipe_wait += 1;
		}

	} /* forking */

	/* Change the terminal mode to non-blocking input. */
	nodelay( stdscr, 0 );
	halfdelay( DWIPE_KNOB_SLEEP * 10 );
	
	while( dwipe_wait < dwipe_selected )
	{
		/* Sleeping is handled by the getch block. */

		/* Sleep between update intervals. */
		/* sleep( DWIPE_KNOB_SLEEP );      */

		/* Enumerate child processes. */
		for( i = 0 ; i < dwipe_selected ; i++ )
		{
			if( c2[i].pid > 0 )
			{
				c2[i].result = waitpid( c2[i].pid, &c2[i].status, WNOHANG ); 

				/* Check for fatal errors. */
				if( c2[i].result < 0 )
				{
					dwipe_perror( errno, __FUNCTION__, "waitpid" );
					dwipe_gui_free();
					return errno;
				}

				if( c2[i].result > 0 )
				{
					/* The child has been reaped. */
					c2[i].pid = 0;

					if( WIFEXITED( c2[i].status ) )
					{
						/* The child returned normally. */
						c2[i].result = WEXITSTATUS( c2[i].status );
					}

					else
					{
						/* The child was killed.  Remember the signal. */
						c2[i].signal = WIFSIGNALED( c2[i].status ) ? WTERMSIG( c2[i].status ) : 0;
					}

					/* Increment the number of children that have returned. */
					dwipe_wait += 1;

				} /* child reaped */

			} /* child active */
				  
		} /* child waitpid */

		/* Show the user what is happening. */
		dwipe_gui_status( dwipe_selected, c2 );

	} /* while */

	/* TODO: Fanfare. */
	dwipe_pid = getch();

	/* Release the gui. */
	dwipe_gui_free();

	dwipe_log( DWIPE_LOG_NOTICE, "Wipe finished." );

	for( i = 0 ; i < dwipe_selected ; i++ )
	{
		snprintf( dwipe_result_file, sizeof(dwipe_result_file), "%s.result", c2[i].device_name );
		dwipe_result_fp = fopen( dwipe_result_file, "w" );
		fprintf( dwipe_result_fp, "DWIPE_LABEL='%s'\n", c2[i].label );
		fprintf( dwipe_result_fp, "DWIPE_METHOD='%s'\n", dwipe_method_label( dwipe_options.method) );
		fprintf( dwipe_result_fp, "DWIPE_ROUNDS='%i'\n", dwipe_options.rounds );
		
		if( dwipe_options.verify == DWIPE_VERIFY_NONE )
		{
			fprintf( dwipe_result_fp, "DWIPE_VERIFY='off'\n" );
		}
		if( dwipe_options.verify == DWIPE_VERIFY_ALL  )
		{
			fprintf( dwipe_result_fp, "DWIPE_VERIFY='all'\n" );
		}
		if( dwipe_options.verify == DWIPE_VERIFY_LAST )
		{
			fprintf( dwipe_result_fp, "DWIPE_VERIFY='last'\n" );
		}
		
		if( c2[i].result < 0 )
		{
			dwipe_log( DWIPE_LOG_NOTICE, "Wipe of device '%s' failed.", c2[i].device_name );
			fprintf( dwipe_result_fp, "DWIPE_RESULT='fail'\n" );
		}

		if( c2[i].result == 0 )
		{
			dwipe_log( DWIPE_LOG_NOTICE, "Wipe of device '%s' succeeded.", c2[i].device_name );
			fprintf( dwipe_result_fp, "DWIPE_RESULT='pass'\n" );
		}

		if( c2[i].result > 0 )
		{
			dwipe_log( DWIPE_LOG_NOTICE, "Wipe of device '%s' incomplete.", c2[i].device_name );
			fprintf( dwipe_result_fp, "DWIPE_RESULT='fail'\n" );
		}
		
		fclose( dwipe_result_fp );
	}

	for( i = 0 ; i < dwipe_selected ; i++ )
	{
		/* Check for fatal errors. */
		if( c2[i].result < 0 ){ return -1; }
	}

	for( i = 0 ; i < dwipe_selected ; i++ )
	{
		/* Check for non-fatal errors. */
		if( c2[i].result > 0 ){ return 1; }
	}

	/* Success. The shared memory will be released when we exit. */
	return 0;

} /* main */

/* eof */
