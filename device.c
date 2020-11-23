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
#include <string.h>
#include <scsi/scsi.h>

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

int dwipe_is_partition(const char * name)
{
    /* if /sys/class/block/sda1/device/partition exist ? */
    char b [FILENAME_MAX];

    snprintf(b, sizeof(b), "/sys/class/block/%s/partition", name);
    dwipe_log( DWIPE_LOG_INFO, "testing device '%s'.", b );
    if(!access(b, F_OK)) {
        dwipe_log( DWIPE_LOG_INFO, "'%s' is a partition.", name );
        return 1;
    }
    dwipe_log( DWIPE_LOG_INFO, "'%s' is not a partition.", name );
    return 0;
}


struct my_scsi_idlun {
    int four_in_one;    /* 4 separate bytes of info compacted into 1 int */
    int host_unique_id; /* distinguishes adapter cards from same supplier */
};

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
    int size = 0;
    struct my_scsi_idlun sg_scsi;

    dwipe_log( DWIPE_LOG_INFO, "dwipe_device_identify '%s' .", c->device_name );

    /* Names in the partition file do not have the '/dev/' prefix. */
    const char dprefix [] = DWIPE_KNOB_PARTITIONS_PREFIX;

    /* Allocate memory for the label. */
    c->label = malloc( DWIPE_KNOB_LABEL_SIZE );
    if(!c->label)
        return;
    memset(c->label, 0, DWIPE_KNOB_LABEL_SIZE);

    if(ioctl(c->device_fd, SCSI_IOCTL_GET_IDLUN, &sg_scsi) != 0) 
    {
        dwipe_log( DWIPE_LOG_ERROR, "Error: Probe device %s SCSI ID error %d on fd %d.\n", c->device_name, errno, c->device_fd);
        goto err;
    } 
    else 
    {
        // (scsi_device_id | (lun << 8) | (channel << 16) | (host_no << 24))
        c->device_host = ((sg_scsi.four_in_one) & 0xFF000000) >> 24;
        c->device_bus = ((sg_scsi.four_in_one)  & 0x00FF0000) >> 16;
        c->device_lun = ((sg_scsi.four_in_one)  & 0x0000FF00) >> 8;
        c->device_target = ((sg_scsi.four_in_one) & 0xFF);

        dwipe_log( DWIPE_LOG_INFO, "Device %s at [%08x:%08x] = [Host:Channel:TargetId:Lun] [%d:%d:%d:%d].", 
            c->device_name, 
            sg_scsi.four_in_one,
            sg_scsi.host_unique_id,
            c->device_host, c->device_bus, c->device_target, c->device_lun);
    }

    if(dwipe_is_partition(&(c->device_name[strlen(dprefix)])))
    {
        strncpy( c->label, "  |------PARTATION ", DWIPE_KNOB_LABEL_SIZE);
        size = strlen(c->label);
        c->label[size - 1] = ' ';
        dwipe_log( DWIPE_LOG_ERROR, "size0 is %d", size);

        snprintf(buffer, sizeof(buffer), "/sys/class/block/%s/partition",  &(c->device_name[strlen(dprefix)]));
        buffer[FILENAME_MAX - 1] = 0;

        fp = fopen( buffer, "r" );
        if( fp == NULL )
        {
            dwipe_log( DWIPE_LOG_ERROR, "Error: Unable to open '%s'.", buffer );
            goto err;
        }

        if( fgets( buffer, sizeof(buffer), fp ) != NULL )
        {
            strncpy( c->label + size, buffer, DWIPE_KNOB_LABEL_SIZE - size );
            size = strlen(c->label);
            c->label[size - 1] = ' ';
            dwipe_log( DWIPE_LOG_ERROR, "size1 is %d", size);

            if( sscanf (buffer, "%i", &(c->device_part)) != 1) 
            {
                dwipe_log( DWIPE_LOG_ERROR, "Error: Unable to read device partition number'%s'.", c->device_name );
                goto err;
            }
        }
        else
        {
            strncpy( c->label, "Uninitialized Device", DWIPE_KNOB_LABEL_SIZE );
            goto err;
        }

        fclose(fp);
        fp = NULL;

    } else {

        /* read /sys/class/block/%s/device/vendor */
        snprintf(buffer, sizeof(buffer), "/sys/class/block/%s/device/vendor",  &(c->device_name[strlen(dprefix)]));
        buffer[FILENAME_MAX - 1] = 0;

        fp = fopen( buffer, "r" );
        if( fp == NULL )
        {
            dwipe_log( DWIPE_LOG_ERROR, "Error: Unable to open '%s'.", buffer );
            goto err;
        }

        if( fgets( buffer, sizeof(buffer), fp ) != NULL )
        {
            strncpy( c->label, buffer, DWIPE_KNOB_LABEL_SIZE );
            size = strlen(c->label);
            c->label[size - 1] = ' ';
            dwipe_log( DWIPE_LOG_ERROR, "size2 is %d", size);
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
        buffer[FILENAME_MAX - 1] = 0;

        fp = fopen( buffer, "r" );
        if( fp == NULL )
        {
            dwipe_log( DWIPE_LOG_ERROR, "Error: Unable to open '%s'.", buffer );
            goto err;
        }

        if( fgets( buffer, sizeof(buffer), fp ) != NULL )
        {
            strncpy( c->label + size, buffer, DWIPE_KNOB_LABEL_SIZE - size );
            size = strlen(c->label);
            c->label[size - 1] = ' ';
            dwipe_log( DWIPE_LOG_ERROR, "size3 is %d", size);
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
        buffer[FILENAME_MAX - 1] = 0;

        fp = fopen( buffer, "r" );
        if( fp == NULL )
        {
            dwipe_log( DWIPE_LOG_ERROR, "Error: Unable to open '%s'.", buffer );
            goto err;
        }

        if( fgets( buffer, sizeof(buffer), fp ) != NULL )
        {
            strncpy( c->label + size, buffer, DWIPE_KNOB_LABEL_SIZE - size );
            size = strlen(c->label);
            c->label[size - 1] = ' ';
            dwipe_log( DWIPE_LOG_ERROR, "size4 is %d", size);
        }
        else
        {
            strncpy( c->label, "Uninitialized Device", DWIPE_KNOB_LABEL_SIZE );
            goto err;
        }
        fclose(fp);
        fp = NULL;
    }

    dwipe_log( DWIPE_LOG_ERROR, "size5 is %d", size);
    dwipe_device_strsize(c->label + size, DWIPE_KNOB_LABEL_SIZE - size, c->device_size);
    c->label[DWIPE_KNOB_LABEL_SIZE - 1] = 0;

    return;

err:
    if( fp != NULL )
    {
        fclose(fp);
        fp = NULL;
    }
    if(c->label) 
    {
        c->label[DWIPE_KNOB_LABEL_SIZE - 1] = 0;
    }
} /* dwipe_device_identify */


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
        dwipe_log( DWIPE_LOG_ERROR, "Error: Unable to open the partitions file '%s'.\n", DWIPE_KNOB_PARTITIONS );
        exit( errno );
    }

    /* Copy the device prefix into the name buffer. */
    strcpy( dname, dprefix );

    /* Sanity check: If device_name is non-null, then it is probably being used. */
    if( *device_names != NULL )
    {
        dwipe_log( DWIPE_LOG_ERROR, "Sanity Error: dwipe_device_scan: Non-null device_names pointer.\n" );
        exit( -1 );
    }

    /* Read every line in the partitions file. */
    while( fgets( b, sizeof( b ), fp ) != NULL )
    {
        /* Scan for a device line. */
        if( sscanf( b, "%i %i %lli %s", &dmajor, &dminor, &dblocks, &dname[ strlen( dprefix ) ] ) == 4 )
        {
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
