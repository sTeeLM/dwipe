/*
 *  options.c:  Command line processing routines for dwipe. 
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

/* The global options struct. */
dwipe_options_t dwipe_options;

void dwipe_options_usage()
{
    fprintf(stderr, "%s\n", dwipe_options.banner);
    fprintf(stderr, "Usage: dwipe [OPTION]... [dev file]\n");
    fprintf(stderr, "Options: \n");
    fprintf(stderr, "    -a|--autonuke : default off\n");
    fprintf(stderr, "         Do not prompt the user for confirmation when set.\n");
    fprintf(stderr, "    -m|--method [dod|dod3pass|gutmann|ops2|random|zero] : default dod3pass\n");
    fprintf(stderr, "         The wipe method that will be used.\n");
    fprintf(stderr, "    -p|--prng [twister|isaac] : default twister\n");
    fprintf(stderr, "         The pseudo random number generator implementation.\n");
    fprintf(stderr, "    -r|--rounds : default 1\n");
    fprintf(stderr, "         The number of times that the wipe method should be called.\n");
    fprintf(stderr, "    -s|--sync   : default off\n");
    fprintf(stderr, "         A flag to indicate whether writes should be sync'd.\n");
    fprintf(stderr, "    -v|--verify [off|last|all] : default last\n");
    fprintf(stderr, "         A flag to indicate whether writes should be verified.\n");
    fprintf(stderr, "    -e|--exclude [dev] :\n");
    fprintf(stderr, "         Device should survive from dwipe\n");
    fprintf(stderr, "    -h|--help   :\n");
    fprintf(stderr, "         Show this help\n");
}

int dwipe_options_parse( int argc, char** argv )
{
	extern char* optarg;  /* The working getopt option argument. */
	extern int   optind;  /* The working getopt index into argv. */
	extern int   optopt;  /* The last unhandled getopt option.   */
	extern int   opterr;  /* The last getopt error number.       */

	extern dwipe_prng_t dwipe_twister;
	extern dwipe_prng_t dwipe_isaac;

	/* The maximum banner size, including the null. */
	const int dwipe_banner_size = 81;

	/* The getopt() result holder. */
	int dwipe_opt;

	/* Array index variable. */
	int i;

	/* The list of acceptable short options. */
	char dwipe_options_short [] = "ahm:p:r:sv:e:";

	/* The list of acceptable long options. */
	static struct option dwipe_options_long [] =
	{
		/* Set when the user wants to wipe without a confirmation prompt. */
		{ "autonuke", no_argument, 0, 0 },

		/* A GNU standard option. Corresponds to the 'h' short option. */
		{ "help", no_argument, 0, 'h' },

		/* The wipe method. Corresponds to the 'm' short option. */
		{ "method", required_argument, 0, 'm' },

		/* The Pseudo Random Number Generator. */
		{ "prng", required_argument, 0, 'p' },

		/* The number of times to run the method. */
		{ "rounds", required_argument, 0, 'r' },

		/* A flag to indicate whether the devices whould be opened in sync mode. */
		{ "sync", no_argument, 0, 0 },

		/* Verify that wipe patterns are being written to the device. */
		{ "verify", required_argument, 0, 0 },

		/* Verify that wipe patterns are being written to the device. */
		{ "exclude", required_argument, 0, 0 },

		/* Requisite padding for getopt(). */
		{ 0, 0, 0, 0 }
	};

	/* Note that COLS isn't available until ncurses is initialized. */
	dwipe_options.banner = malloc( dwipe_banner_size );

	/* Set the default product banner. */
	snprintf( dwipe_options.banner, dwipe_banner_size, "Darik's Wipe version %s(modified by sTeeL<steel.mental@gmail.com>)", DBAN_VERSION );


	/* Set default options. */
	dwipe_options.autonuke = 0;
	dwipe_options.method   = &dwipe_dodshort;
	dwipe_options.prng     = &dwipe_twister;
	dwipe_options.rounds   = 1;
	dwipe_options.sync     = 0;
	dwipe_options.verify   = DWIPE_VERIFY_LAST;
    dwipe_options.exclude  = NULL;


	/* Parse command line options. */
	while( 1 )
	{
		/* Get the next command line option with (3)getopt. */
		dwipe_opt = getopt_long( argc, argv, dwipe_options_short, dwipe_options_long, &i );

		/* Break when we have processed all of the given options. */
		if( dwipe_opt < 0 ) { break; }

		switch( dwipe_opt )
		{
            case 'a':
                dwipe_options.autonuke = 1;
                break;

            case 'h':
                dwipe_options_usage();
                exit( 0 );

            case 'm':  /* Method option. */

				if( strcmp( optarg, "dod522022m" ) == 0 || strcmp( optarg, "dod" ) == 0 )
				{
					dwipe_options.method = &dwipe_dod522022m;
					break;
				}

				if( strcmp( optarg, "dodshort" ) == 0 || strcmp( optarg, "dod3pass" ) == 0 )
				{
					dwipe_options.method = &dwipe_dodshort;
					break;
				}

				if( strcmp( optarg, "gutmann" ) == 0 )
				{
					dwipe_options.method = &dwipe_gutmann;
					break;
				}

				if( strcmp( optarg, "ops2" ) == 0 )
				{
					dwipe_options.method = &dwipe_ops2;
					break;
				}

				if(  strcmp( optarg, "random" ) == 0
				  || strcmp( optarg, "prng"   ) == 0
				  || strcmp( optarg, "stream" ) == 0
				  )
				{
					dwipe_options.method= &dwipe_random;
					break;
				}

				if( strcmp( optarg, "zero" ) == 0 || strcmp( optarg, "quick" ) == 0 )
				{
					dwipe_options.method = &dwipe_zero;
					break;
				}

				/* Else we do not know this wipe method. */
				fprintf( stderr, "Error: Unknown wipe method '%s'.\n", optarg );
				exit( EINVAL );


			case 'p':  /* PRNG option. */

				if(  strcmp( optarg, "mersenne" ) == 0
				  || strcmp( optarg, "twister"  ) == 0
				  )
				{
					dwipe_options.prng = &dwipe_twister;
					break;
				}

				if( strcmp( optarg, "isaac" ) == 0 )
				{
					dwipe_options.prng = &dwipe_isaac;
					break;
				}

				/* Else we do not know this PRNG. */
				fprintf( stderr, "Error: Unknown prng '%s'.\n", optarg );
				exit( EINVAL );


			case 'r':  /* Rounds option. */

				if( sscanf( optarg, " %i", &dwipe_options.rounds ) != 1 \
				    || dwipe_options.rounds < 1
				  )
				{
					fprintf( stderr, "Error: The rounds argument must be a postive integer.\n" );
					exit( EINVAL );
				}

				break;

            case 's':
                dwipe_options.sync = 1;
                break;

            case 'v':
                if( strcmp( optarg, "0" ) == 0 || strcmp( optarg, "off" ) == 0 )
                {
                        dwipe_options.verify = DWIPE_VERIFY_NONE;
                        break;
                }

                if( strcmp( optarg, "1" ) == 0 || strcmp( optarg, "last" ) == 0 )
                {
                        dwipe_options.verify = DWIPE_VERIFY_LAST;
                        break;
                }

                if( strcmp( optarg, "2" ) == 0 || strcmp( optarg, "all" ) == 0 )
                {
                        dwipe_options.verify = DWIPE_VERIFY_ALL;
                        break;
                }
                /* Else we do not know this verification level. */
                fprintf( stderr, "Error: Unknown verification level '%s'.\n", optarg );
				exit( EINVAL );

            case 'e':
                dwipe_options.exclude = optarg;
                break;

			default:

				/* Bogus command line argument. */
				/* TODO: Print usage. */
				fprintf( stderr, "Error: Unknown short option.\n" );
				exit( EINVAL );
			
		} /* method */

	} /* command line options */

	/* Return the number of options that were processed. */
	return optind;

} /* dwipe_options_parse */


void dwipe_options_log( void )
{
/**
 *  Prints a manifest of options to the log.
 *
 */

	dwipe_log( DWIPE_LOG_NOTICE, "Program options are set as follows..." );

	if( dwipe_options.autonuke )
	{
		dwipe_log( DWIPE_LOG_NOTICE, "  autonuke = %i (on)", dwipe_options.autonuke );
	}

	else
	{
		dwipe_log( DWIPE_LOG_NOTICE, "  autonuke = %i (off)", dwipe_options.autonuke );
	}


	dwipe_log( DWIPE_LOG_NOTICE, "  banner   = %s", dwipe_options.banner );
	dwipe_log( DWIPE_LOG_NOTICE, "  method   = %s", dwipe_method_label( dwipe_options.method ) );
	dwipe_log( DWIPE_LOG_NOTICE, "  rounds   = %i", dwipe_options.rounds );
	dwipe_log( DWIPE_LOG_NOTICE, "  sync     = %i", dwipe_options.sync );
	dwipe_log( DWIPE_LOG_NOTICE, "  exclude  = %s", dwipe_options.exclude == NULL ? "none" :  dwipe_options.exclude);

	switch( dwipe_options.verify )
	{
		case DWIPE_VERIFY_NONE:
			dwipe_log( DWIPE_LOG_NOTICE, "  verify   = %i (off)", dwipe_options.verify );
			break;

		case DWIPE_VERIFY_LAST:
			dwipe_log( DWIPE_LOG_NOTICE, "  verify   = %i (last pass)", dwipe_options.verify );
			break;

		case DWIPE_VERIFY_ALL:
			dwipe_log( DWIPE_LOG_NOTICE, "  verify   = %i (all passes)", dwipe_options.verify );
			break;

		default:
			dwipe_log( DWIPE_LOG_NOTICE, "  verify   = %i", dwipe_options.verify );
			break;
	}

} /* dwipe_options_log */

/* eof */
