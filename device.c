/*
 *  device.c:  Device routines for dwipe.
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

#include <netinet/in.h>

#include "dwipe.h"
#include "context.h"
#include "method.h"
#include "options.h"
#include "identify.h"
#include "scsicmds.h"
#include "logging.h"

void dwipe_device_strsize(char * buffer, int buflen, loff_t size)
{
    long double real = size;
    long double real1 = size;
    if(real > 1024*1024*1024*1024.0 ) { /* 1TiB */
        real /= 1024*1024*1024*1024.0;
        real1 /= 1000*1000*1000*1000.0;
        snprintf(buffer, buflen, "Size: %.2Lf TiB(%.2Lf TB)", real, real1);
    } else if(real > 1024*1024*1024.0 ) { /* 1GiB */
        real /= 1024*1024*1024.0;
        real1 /= 1000*1000*1000.0;
        snprintf(buffer, buflen, "Size: %.2Lf GiB(%.2Lf GB)", real, real1);
    } else if(real > 1024*1024.0 ) { /* 1MiB */
        real /= 1024*1024.0;
        real1 /= 1000*1000.0;
        snprintf(buffer, buflen, "Size: %.2Lf MiB(%.2Lf KB)", real, real1);
    } else if(real > 1024*1024.0 ) { /* 1KiB */
        real /= 1024.0;
        real1 /= 1000.0;
        snprintf(buffer, buflen, "Size: %.2Lf KiB(%.2Lf KB)", real, real1);
    } else {
        snprintf(buffer, buflen, "Size: %.2Lf B", real);
    }
}

void dwipe_device_identify( dwipe_context_t* c )
{
	/**
	 * Gets device information and creates the context label.
	 *
	 * @parameter  c         A pointer to a device context.
	 * @modifies   c->label  The menu description that the user sees.
	 *
	 */
	 
	char buffer [FILENAME_MAX];
	FILE* fp = NULL;
    int size;

	/* Names in the partition file do not have the '/dev/' prefix. */
	char dprefix [] = DWIPE_KNOB_PARTITIONS_PREFIX;

	/* Allocate memory for the label. */
 	c->label = malloc( DWIPE_KNOB_LABEL_SIZE );
 	if(!c->label)
        return;

    /* read /sys/class/block/%s/device/vendor */
    snprintf(buffer, sizeof(buffer), "/sys/class/block/%s/device/vendor",  &(c->device_name[strlen(dprefix)]));
    fp = fopen( buffer, "r" );

    if( fp == NULL )
    {
            fprintf( stderr, "Error: Unable to open '%s'.\n", buffer );
            goto err;
    }

    if( fgets( buffer, sizeof(buffer), fp ) != NULL )
    {
            strncpy( c->label, buffer, DWIPE_KNOB_LABEL_SIZE );
            size = strlen(c->label);
            c->label[size - 1] = ' ';
    }
    else
    {
            strncpy( c->label, "Uninitialized Device", DWIPE_KNOB_LABEL_SIZE );
            goto err;
    }

    fclose(fp);
    fp = NULL;

	/* read /sys/class/block/%s/device/model */
    snprintf(buffer, sizeof(buffer), "/sys/class/block/%s/device/model",  &(c->device_name[strlen(dprefix)]));
    fp = fopen( buffer, "r" );
    if( fp == NULL )
    {
            fprintf( stderr, "Error: Unable to open '%s'.\n", buffer );
            goto err;
    }

    if( fgets( buffer, sizeof(buffer), fp ) != NULL )
    {
            strncpy( c->label + size, buffer, DWIPE_KNOB_LABEL_SIZE - size );
            size = strlen(c->label);
            c->label[size - 1] = ' ';
    }
    else
    {
            strncpy( c->label, "Uninitialized Device", DWIPE_KNOB_LABEL_SIZE );
            goto err;
    }
    fclose(fp);
    fp = NULL;

    /* /sys/class/block/%s/device/rev */
    snprintf(buffer, sizeof(buffer), "/sys/class/block/%s/device/rev",  &(c->device_name[strlen(dprefix)]));
    fp = fopen( buffer, "r" );
    if( fp == NULL )
    {
            fprintf( stderr, "Error: Unable to open '%s'.\n", buffer );
            goto err;
    }

    if( fgets( buffer, sizeof(buffer), fp ) != NULL )
    {
            strncpy( c->label + size, buffer, DWIPE_KNOB_LABEL_SIZE - size );
            size = strlen(c->label);
            c->label[size - 1] = ' ';
    }
    else
    {
            strncpy( c->label, "Uninitialized Device", DWIPE_KNOB_LABEL_SIZE );
            goto err;
    }
    fclose(fp);
    fp = NULL;

    dwipe_device_strsize(c->label + size, DWIPE_KNOB_LABEL_SIZE - size, c->device_size);

err:
    if( fp != NULL )
    {
        fclose(fp);
        fp = NULL;
    }

} /* dwipe_device_identify */


int dwipe_is_real(char * name)
{
	/* if /sys/class/block/sda/device/vendor exist ? */
	char b [FILENAME_MAX];

	snprintf(b, sizeof(b), "/sys/class/block/%s/device/vendor", name);
    if(!access(b, F_OK)) {
        dwipe_log( DWIPE_LOG_INFO, "'%s' is a real device.", name );
        return 1;
    }
    dwipe_log( DWIPE_LOG_INFO, "'%s' is not a real device.", name );
	return 0;
}

int dwipe_device_scan( char*** device_names )
{
	/**
	 * Scans the the filesystem for storage device names.
	 *
	 * @parameter device_names  A reference to a null array pointer.
	 * @modifies  device_names  Populates device_names with an array of strings.
	 * @returns                 The number of strings in the device_names array.
	 *
	 */

	/* The partitions file pointer.  Usually '/proc/partitions'. */
	FILE* fp;

	/* The input buffer. */
	char b [FILENAME_MAX];

	/* Buffer for the major device number. */
	int dmajor;

	/* Buffer for the minor device number. */
	int dminor;

	/* Buffer for the device block count.  */
	loff_t dblocks;

	/* Buffer for the device file name.    */
	char dname [FILENAME_MAX];

	/* Names in the partition file do not have the '/dev/' prefix. */
	char dprefix [] = DWIPE_KNOB_PARTITIONS_PREFIX;

	/* The number of devices that have been found. */
	int dcount = 0;
 
	/* Open the partitions file. */
	fp = fopen( DWIPE_KNOB_PARTITIONS, "r" );

	if( fp == NULL )
	{
		perror( "dwipe_device_scan: fopen" );
		fprintf( stderr, "Error: Unable to open the partitions file '%s'.\n", DWIPE_KNOB_PARTITIONS );
		exit( errno );
	}

	/* Copy the device prefix into the name buffer. */
	strcpy( dname, dprefix );

	/* Sanity check: If device_name is non-null, then it is probably being used. */
	if( *device_names != NULL )
	{
		fprintf( stderr, "Sanity Error: dwipe_device_scan: Non-null device_names pointer.\n" );
		exit( -1 );
	}

	/* Read every line in the partitions file. */
	while( fgets( b, sizeof( b ), fp ) != NULL )
	{
		/* Scan for a device line. */
		if( sscanf( b, "%i %i %lli %s", &dmajor, &dminor, &dblocks, &dname[ strlen( dprefix ) ] ) == 4 )
		{

			/* skip raid and partation */
			if(!dwipe_is_real(&dname[ strlen( dprefix ) ])) {
				continue;
			}

			/* Increment the device count. */
			dcount += 1;

			/* TODO: Check whether the device numbers are sensible. */

			/* Allocate another name pointer. */
			*device_names = realloc( *device_names, dcount * sizeof(char*) );

			/* Allocate the device name string. */
			(*device_names)[ dcount -1 ] = malloc( strlen( dname ) +1 );
			
			/* Copy the buffer into the device name string. */
			strcpy( (*device_names)[ dcount -1 ], dname );

		} /* if sscanf */

	} /* while fgets */

	/* Pad the array with a null pointer. */
	*device_names = realloc( *device_names, ( dcount +1 ) * sizeof(char*) );
	(*device_names)[ dcount ] = NULL;

	/* Return the number of devices that were found. */
	return dcount;

} /* dwipe_device_scan */

/* eof */
