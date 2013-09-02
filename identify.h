/*
 * identify.h: Definitions and tables from 'hdparm-4.5/identify.c' for dwipe.
 *
 * Command line interface to get/set hard disk parameters
 * by Mark S. Lord (C) 1994-2001 -- freely distributable
 * You are free to distribute this software under the terms of the BSD License.
 * On Debian systems, the complete text of the BSD License can be found in
 * /usr/share/common-licenses/BSD
 *
 */

#if 0 /* Unused by dwipe. */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#endif

#include <linux/types.h>

/* device types */
/* ------------ */
#define NO_DEV                  0xffff
#define ATA_DEV                 0x0000
#define ATAPI_DEV               0x0001

/* word definitions */
/* ---------------- */
#define GEN_CONFIG		0   /* general configuration */
#define LCYLS			1   /* number of logical cylinders */
#define CONFIG			2   /* specific configuration */
#define LHEADS			3   /* number of logical heads */
#define TRACK_BYTES		4   /* number of bytes/track (ATA-1) */
#define SECT_BYTES		5   /* number of bytes/sector (ATA-1) */
#define LSECTS			6   /* number of logical sectors/track */
#define START_SERIAL_NUM        10  /* ASCII serial number */
#define END_SERIAL_NUM          19
#define BUF_TYPE		20  /* buffer type (ATA-1) */
#if 0  /* Conflicts with libbb. */
#define BUF_SIZE		21  /* buffer size (ATA-1) */
#endif /* Conflicts with libbb. */
#define RW_LONG			22  /* extra bytes in R/W LONG command */
#define START_FW_REV            23  /* ASCII firmware revision */
#define END_FW_REV		26 
#define START_MODEL_NUM		27  /* ASCII model number */
#define END_MODEL_NUM		46
#define SECTOR_XFER_MAX	        47  /* r/w multiple: max sectors xfered */
#define DWORD_IO		48  /* can do double-word IO (ATA-1) */
#define CAPAB_0			49  /* capabilities */
#define CAPAB_1			50
#define PIO_MODE		51  /* max PIO mode supported (obsolete)*/
#define DMA_MODE		52  /* max Singleword DMA mode supported (obsolete)*/
#define WHATS_VALID		53  /* what fields are valid */
#define LCYLS_CUR		54  /* current logical cylinders */
#define LHEADS_CUR		55  /* current logical heads */
#define LSECTS_CUR	        56  /* current logical sectors/track */
#define CAPACITY_LSB		57  /* current capacity in sectors */
#define CAPACITY_MSB		58
#define SECTOR_XFER_CUR		59  /* r/w multiple: current sectors xfered */
#define LBA_SECTS_LSB		60  /* LBA: total number of user */
#define LBA_SECTS_MSB		61  /*      addressable sectors */
#define SINGLE_DMA		62  /* singleword DMA modes */
#define MULTI_DMA		63  /* multiword DMA modes */
#define ADV_PIO_MODES		64  /* advanced PIO modes supported */
				    /* multiword DMA xfer cycle time: */
#define DMA_TIME_MIN		65  /*   - minimum */
#define DMA_TIME_NORM		66  /*   - manufacturer's recommended	*/
				    /* minimum PIO xfer cycle time: */
#define PIO_NO_FLOW		67  /*   - without flow control */
#define PIO_FLOW		68  /*   - with IORDY flow control */
#define PKT_REL			71  /* typical #ns from PKT cmd to bus rel */
#define SVC_NBSY		72  /* typical #ns from SERVICE cmd to !BSY */
#define CDR_MAJOR		73  /* CD ROM: major version number */
#define CDR_MINOR		74  /* CD ROM: minor version number */
#define QUEUE_DEPTH		75  /* queue depth */
#define MAJOR			80  /* major version number */
#define MINOR			81  /* minor version number */
#define CMDS_SUPP_0		82  /* command/feature set(s) supported */
#define CMDS_SUPP_1		83
#define CMDS_SUPP_2		84
#define CMDS_EN_0		85  /* command/feature set(s) enabled */
#define CMDS_EN_1		86
#define CMDS_EN_2		87
#define ULTRA_DMA		88  /* ultra DMA modes */
				    /* time to complete security erase */
#define ERASE_TIME		89  /*   - ordinary */
#define ENH_ERASE_TIME		90  /*   - enhanced */
#define ADV_PWR			91  /* current advanced power management level*/
#define PSWD_CODE		92  /* master password revision code	*/
#define HWRST_RSLT		93  /* hardware reset result */
#define RM_STAT 		127  /* removable media status notification feature set support */
#define SECU_STATUS		128  /* security status */
#define START_VENDOR_SPECIFIC	129  /* vendor specific */
#define END_VENDOR_SPECIFIC	159
#define CFA_PWR_MODE		160  /* CFA power mode 1 */
#define INTEGRITY		255  /* integrity word */

/* bit definitions within the words */
/* -------------------------------- */

/* many words are considered valid if bit 15 is 0 and bit 14 is 1 */
#define VALID			0xc000
#define VALID_VAL		0x4000
/* many words are considered invalid if they are either all-0 or all-1 */
#define NOVAL_0			0x0000
#define NOVAL_1			0xffff

/* word 0: gen_config */
#define NOT_ATA			0x8000	
#define NOT_ATAPI		0x4000	
#define MEDIA_REMOVABLE		0x0080
#define DRIVE_NOT_REMOVABLE	0x0040
#define INCOMPLETE		0x0004
#define CFA_SUPPORT_VAL		0x848a	/* 848a=CFA feature set support */
#define DRQ_RESPONSE_TIME	0x0060
#define DRQ_3MS_VAL		0x0000
#define DRQ_INTR_VAL		0x0020
#define DRQ_50US_VAL		0x0040
#define PKT_SIZE_SUPPORTED	0x0003
#define PKT_SIZE_12_VAL		0x0000
#define PKT_SIZE_16_VAL		0x0001
#define EQPT_TYPE		0x1f00
#define SHIFT_EQPT		8

#define CDROM 0x0005
const char *pkt_str[] = {
	"Direct-access device",			/* word 0, bits 12-8 = 00 */
	"Sequential-access device",		/* word 0, bits 12-8 = 01 */
	"Printer",				/* word 0, bits 12-8 = 02 */
	"Processor",				/* word 0, bits 12-8 = 03 */
	"Write-once device",			/* word 0, bits 12-8 = 04 */
	"CD-ROM",				/* word 0, bits 12-8 = 05 */
	"Scanner",				/* word 0, bits 12-8 = 06 */
	"Optical memory",			/* word 0, bits 12-8 = 07 */
	"Medium changer",			/* word 0, bits 12-8 = 08 */
	"Communications device",		/* word 0, bits 12-8 = 09 */
	"ACS-IT8 device",			/* word 0, bits 12-8 = 0a */
	"ACS-IT8 device",			/* word 0, bits 12-8 = 0b */
	"Array controller",			/* word 0, bits 12-8 = 0c */
	"Enclosure services",			/* word 0, bits 12-8 = 0d */
	"Reduced block command device",		/* word 0, bits 12-8 = 0e */
	"Optical card reader/writer",		/* word 0, bits 12-8 = 0f */
	"",					/* word 0, bits 12-8 = 10 */
	"",					/* word 0, bits 12-8 = 11 */
	"",					/* word 0, bits 12-8 = 12 */
	"",					/* word 0, bits 12-8 = 13 */
	"",					/* word 0, bits 12-8 = 14 */
	"",					/* word 0, bits 12-8 = 15 */
	"",					/* word 0, bits 12-8 = 16 */
	"",					/* word 0, bits 12-8 = 17 */
	"",					/* word 0, bits 12-8 = 18 */
	"",					/* word 0, bits 12-8 = 19 */
	"",					/* word 0, bits 12-8 = 1a */
	"",					/* word 0, bits 12-8 = 1b */
	"",					/* word 0, bits 12-8 = 1c */
	"",					/* word 0, bits 12-8 = 1d */
	"",					/* word 0, bits 12-8 = 1e */
	"Unknown",				/* word 0, bits 12-8 = 1f */
};
const char *ata1_cfg_str[] = {			/* word 0 in ATA-1 mode */
	"reserved",				/* bit 0 */
	"hard sectored",			/* bit 1 */
	"soft sectored",			/* bit 2 */
	"not MFM encoded ",			/* bit 3 */
	"head switch time > 15us",		/* bit 4 */
	"spindle motor control option",		/* bit 5 */
	"fixed drive",				/* bit 6 */
	"removable drive",			/* bit 7 */
	"disk xfer rate <= 5Mbs",		/* bit 8 */
	"disk xfer rate > 5Mbs, <= 10Mbs",	/* bit 9 */
	"disk xfer rate > 5Mbs",		/* bit 10 */
	"rotational speed tol.",		/* bit 11 */
	"data strobe offset option",		/* bit 12 */
	"track offset option",			/* bit 13 */
	"format speed tolerance gap reqd",	/* bit 14 */
	"ATAPI"					/* bit 14 */
};

/* word 1: number of logical cylinders */
#define LCYLS_MAX		0x3fff /* maximum allowable value */

/* word 2: specific configureation 
 * (a) require SET FEATURES to spin-up
 * (b) require spin-up to fully reply to IDENTIFY DEVICE
 */
#define STBY_NID_VAL		0x37c8  /*     (a) and     (b) */
#define STBY_ID_VAL		0x738c	/*     (a) and not (b) */
#define PWRD_NID_VAL 		0x8c73	/* not (a) and     (b) */
#define PWRD_ID_VAL		0xc837	/* not (a) and not (b) */

/* words 47 & 59: sector_xfer_max & sector_xfer_cur */
#define SECTOR_XFER		0x00ff  /* sectors xfered on r/w multiple cmds*/
#define MULTIPLE_SETTING_VALID  0x0100  /* 1=multiple sector setting is valid */

/* word 49: capabilities 0 */
#define STD_STBY  	  	0x2000  /* 1=standard values supported (ATA);
					   0=vendor specific values */
#define IORDY_SUP		0x0800  /* 1=support; 0=may be supported */
#define IORDY_OFF		0x0400  /* 1=may be disabled */
#define LBA_SUP			0x0200  /* 1=Logical Block Address support */
#define DMA_SUP			0x0100  /* 1=Direct Memory Access support */
#define DMA_IL_SUP		0x8000  /* 1=interleaved DMA support (ATAPI) */
#define CMD_Q_SUP		0x4000  /* 1=command queuing support (ATAPI) */
#define OVLP_SUP		0x2000  /* 1=overlap operation support (ATAPI) */
#define SWRST_REQ		0x1000  /* 1=ATA SW reset required (ATAPI, obsolete */

/* word 50: capabilities 1 */
#define MIN_STANDBY_TIMER	0x0001  /* 1=device specific standby timer value minimum */

/* words 51 & 52: PIO & DMA cycle times */
#define MODE			0xff00  /* the mode is in the MSBs */

/* word 53: whats_valid */
#define OK_W88     	   	0x0004	/* the ultra_dma info is valid */
#define OK_W64_70		0x0002  /* see above for word descriptions */
#define OK_W54_58		0x0001  /* current cyl, head, sector, cap. info valid */

/*word 63,88: dma_mode, ultra_dma_mode*/
#define MODE_SEL		0xff00
#define MODE4_SEL		0x1000  /* 1=seleced mode */
#define MODE3_SEL		0x0800  /* 1=seleced mode */
#define MODE2_SEL		0x0400  /* 1=seleced mode */
#define MODE1_SEL		0x0200  /* 1=seleced mode */
#define MODE0_SEL		0x0100  /* 1=seleced mode */
#define MODE_SUP		0x00ff
#define MODE_MAX		7

/* word 64: PIO transfer modes */
#define PIO_SUP			0x0003
#define PIO_MODE_MAX		10

/* word 75: queue_depth */
#define DEPTH_BITS		0x001f  /* bits used for queue depth */

/* words 80-81: version numbers */
/* NOVAL_0 or  NOVAL_1 means device does not report version */

/* word 81: minor version number */
#define MINOR_MAX		0x17
const char *minor_str[] = {				/* word 81 value: */
	"device does not report version",		/* 0x0000	*/
	"ATA-1 X3T9.2 781D prior to revision 4",	/* 0x0001	*/
	"ATA-1 published, ANSI X3.221-1994",		/* 0x0002	*/
	"ATA-1 X3T9.2 781D revision 4",			/* 0x0003	*/
	"ATA-2 published, ANSI X3.279-1996",		/* 0x0004	*/
	"ATA-2 X3T10 948D prior to revision 2k",	/* 0x0005	*/
	"ATA-3 X3T10 2008D revision 1",			/* 0x0006	*/
	"ATA-2 X3T10 948D revision 2k",			/* 0x0007	*/
	"ATA-3 X3T10 2008D revision 0",			/* 0x0008	*/
	"ATA-2 X3T10 948D revision 3",			/* 0x0009	*/
	"ATA-3 published, ANSI X3.298-199x",		/* 0x000a	*/
	"ATA-3 X3T10 2008D revision 6",			/* 0x000b	*/
	"ATA-3 X3T13 2008D revision 7 and 7a",		/* 0x000c	*/
	"ATA/ATAPI-4 X3T13 1153D revision 6",		/* 0x000d	*/
	"ATA/ATAPI-4 T13 1153D revision 13",		/* 0x000e	*/
	"ATA/ATAPI-4 X3T13 1153D revision 7",		/* 0x000f	*/
	"ATA/ATAPI-4 T13 1153D revision 18",		/* 0x0010	*/
	"ATA/ATAPI-4 T13 1153D revision 15",		/* 0x0011	*/
	"ATA/ATAPI-4 published, ANSI NCITS 317-1998",	/* 0x0012	*/
	"reserved",					/* 0x0013	*/
	"ATA/ATAPI-4 T13 1153D revision 14",		/* 0x0014	*/
	"ATA/ATAPI-5 T13 1321D revision 1",		/* 0x0015	*/
	"reserved",					/* 0x0016	*/
	"ATA/ATAPI-4 T13 1153D revision 17",		/* 0x0017	*/
	"reserved"					/* 0x0018-0xfffe*/
};
const char actual_ver[] = { 
			/* word 81 value: */
	0,		/* 0x0000	WARNING: 	*/
	1,		/* 0x0001	WARNING: 	*/
	1,		/* 0x0002	WARNING: 	*/
	1,		/* 0x0003	WARNING: 	*/
	2,		/* 0x0004	WARNING:   This array 		*/
	2,		/* 0x0005	WARNING:   corresponds 		*/
	3,		/* 0x0006	WARNING:   *exactly*		*/
	2,		/* 0x0007	WARNING:   to the ATA/		*/
	3,		/* 0x0008	WARNING:   ATAPI version	*/
	2,		/* 0x0009	WARNING:   listed in	 	*/
	3,		/* 0x000a	WARNING:   the 		 	*/
	3,		/* 0x000b	WARNING:   minor_str 		*/
	3,		/* 0x000c	WARNING:   array		*/
	4,		/* 0x000d	WARNING:   above.		*/
	4,		/* 0x000e	WARNING:  			*/
	4,		/* 0x000f	WARNING:   if you change 	*/
	4,		/* 0x0010	WARNING:   that one,      	*/
	4,		/* 0x0011	WARNING:   change this one	*/
	4,		/* 0x0012	WARNING:   too!!!        	*/
	0,		/* 0x0013	WARNING:	*/
	4,		/* 0x0014	WARNING:	*/
	5,		/* 0x0015	WARNING:	*/
	0,		/* 0x0016	WARNING:	*/
	4,		/* 0x0017	WARNING:	*/
	0		/* 0x0018-0xfffe    		*/
};

/* words 82-84: cmds/feats supported */
#define CMDS_W83		0x01ff  /* word 83: defined command locations*/
#define CMDS_W82		0x77ff  /* word 82: defined command locations*/
#define NUM_CMD_FEAT_STR	48

const char *cmd_feat_str[] = { 
	"",					/* word 82 bit 15: obsolete  */
	"NOP cmd",				/* word 82 bit 14 */
	"READ BUFFER cmd",			/* word 82 bit 13 */
	"WRITE BUFFER cmd",			/* word 82 bit 12 */
	"",					/* word 82 bit 11: obsolete  */
	"Host Protected Area feature set",	/* word 82 bit 10 */
	"DEVICE RESET cmd",			/* word 82 bit  9 */
	"SERVICE interrupt",			/* word 82 bit  8 */
	"release interrupt",			/* word 82 bit  7 */
	"look-ahead",				/* word 82 bit  6 */
	"write cache",				/* word 82 bit  5 */
	"PACKET command feature set",		/* word 82 bit  4 */
	"Power Management feature set",		/* word 82 bit  3 */
	"Removable Media feature set",		/* word 82 bit  2 */
	"Security Mode feature set",		/* word 82 bit  1 */
	"SMART feature set",			/* word 82 bit  0 */
						/* --------------*/
	"",					/* word 83 bit 15: !valid bit */
	"",					/* word 83 bit 14:  valid bit */
	"",					/* word 83 bit 13:  reserved */
	"",					/* word 83 bit 12:  reserved */
	"",					/* word 83 bit 11:  reserved */
	"",					/* word 83 bit 10:  reserved */
	"",					/* word 83 bit  9:  reserved */
	"SET MAX security extension",		/* word 83 bit  8 */
	"Address offset used (large drive)",	/* word 83 bit  7 */
	"SET FEATURES subcommand required to spinup after power up",
	"Power-Up In Standby feature set",	/* word 83 bit  5 */
	"Removable Media Status Notification feature set",
	"Advanced Power Management feature set",/* word 83 bit  3 */
	"CFA feature set",			/* word 83 bit  2 */
	"READ/WRITE DMA QUEUED",		/* word 83 bit  1 */
	"DOWNLOAD MICROCODE cmd", 		/* word 83 bit  0 */
						/* --------------*/
	"",					/* word 84 bit 15: !valid bit */
	"",					/* word 84 bit 14:  valid bit */
	"",					/* word 84 bit 13:  reserved */
	"",					/* word 84 bit 12:  reserved */
	"",					/* word 84 bit 11:  reserved */
	"",					/* word 84 bit 10:  reserved */
	"",					/* word 84 bit  9:  reserved */
	"",					/* word 84 bit  8:  reserved */
	"",					/* word 84 bit  7:  reserved */
	"",					/* word 84 bit  6:  reserved */
	"",					/* word 84 bit  5:  reserved */
	"",					/* word 84 bit  4:  reserved */
	"",					/* word 84 bit  3:  reserved */
	"",					/* word 84 bit  2:  reserved */
	"",					/* word 84 bit  1:  reserved */
	""					/* word 84 bit  0:  reserved */
};


/* words 85-87: cmds/feats enabled */
/* use cmd_feat_str[] to display what commands and features have
 * been enabled with words 85-87 
 */

/* words 89, 90, SECU ERASE TIME */
#define ERASE_BITS		0x00ff

/* word 92: master password revision */
/* NOVAL_0 or  NOVAL_1 means no support for master password revision */

/* word 93: hw reset result */
#define CBLID			0x2000  /* CBLID status */
#define RST0			0x0001  /* 1=reset to device #0 */
#define DEV_DET			0x0006  /* how device num determined */
#define JUMPER_VAL		0x0002  /* device num determined by jumper */
#define CSEL_VAL		0x0004  /* device num determined by CSEL_VAL */

/* word 127: removable media status notification feature set support */
#define RM_STAT_BITS 		0x0003
#define RM_STAT_SUP		0x0001
	
/* word 128: security */
#define SECU_ENABLED	0x0002
#define SECU_LEVEL		0x0010
#define NUM_SECU_STR	6
const char *secu_str[] = {
	"supported",			/* word 128, bit 0 */
	"enabled",			/* word 128, bit 1 */
	"locked",			/* word 128, bit 2 */
	"frozen",			/* word 128, bit 3 */
	"expired: security count",	/* word 128, bit 4 */
	"supported: enhanced erase"	/* word 128, bit 5 */
};

/* word 160: CFA power mode */
#define VALID_W160		0x8000  /* 1=word valid */
#define PWR_MODE_REQ		0x2000  /* 1=CFA power mode req'd by some cmds*/
#define PWR_MODE_OFF		0x1000  /* 1=CFA power moded disabled */
#define MAX_AMPS		0x0fff  /* value = max current in ma */

/* word 255: integrity */
#define SIG			0x00ff  /* signature location */
#define SIG_VAL			0x00A5  /* signature value */

#if 0 /* Unused by dwipe. */

/* Compiler note:
 * 	For some totally bizarre reason, the compiler won't pass an unsigned
 *	short (__u16 == unsigned short on my machine) to an unsigned short in
 *	a function without a type conversion.  It doesn't have that problem
 *	with unsigned int so I changed mode_loop to use __u32 instead of __u16.
 *	TBD: change it back if/when the compiler gets fixed.
 */
__u8 mode_loop(__u32 mode_sup, __u32 mode_sel, int cc, __u8 *have_mode);

/* our main() routine: */
void identify (__u16 *id_supplied, const char *devname)
{

	char *id_file = NULL, fmt[]="/proc/ide/%s/identify";
	FILE *fl;
	__u16 val[256], ii, jj, kk;
	__u8  like_std = 1, std = 0;
	__u16 dev = NO_DEV, eqpt = NO_DEV;
	__u8  have_mode = 0, err_dma = 0;
	__u8  chksum = 0;
	__u32 ll, mm;

	if (id_supplied) {
		memcpy(val, id_supplied, sizeof(val));
	} else {
		id_file = calloc(1, strlen(devname)+1+strlen(fmt));
		sprintf(id_file, fmt, devname);

		/* open the file, read in all the info and close it */
		if (id_file == NULL) {
			fl = stdin;
		} else if(NULL == (fl = fopen(id_file, "r"))) {
     			perror(id_file);
			exit(errno);
		}
		for(ii = GEN_CONFIG; ii<=INTEGRITY; ii++) {
			unsigned int scratch;
			if(1 != fscanf(fl,"%04x",&scratch)) break;
			val[ii] = (__u16)scratch;
			chksum += val[ii] + (val[ii] >> 8);
		}
		fclose(fl);  
		if(ii < (INTEGRITY+1)) {
			fprintf(stderr,"Input file wrong format or length\n");
			exit(ii);
		}
	}
	chksum &= 0xff;

	/* check if we recognise the device type */
	printf("\n");
	if(!(val[GEN_CONFIG] & NOT_ATA)) {
		dev = ATA_DEV;
		if(val[GEN_CONFIG] & DRIVE_NOT_REMOVABLE)
			printf("non-");
		printf("removable ATA device, with ");
	} else if(val[GEN_CONFIG]==CFA_SUPPORT_VAL) {
		dev = ATA_DEV;
		like_std = 4;
		printf("CompactFlash ATA device, with ");
	} else if(!(val[GEN_CONFIG] & NOT_ATAPI)) {
		dev = ATAPI_DEV;
		eqpt = (val[GEN_CONFIG] & EQPT_TYPE) >> SHIFT_EQPT;
		printf("ATAPI %s, with ", pkt_str[eqpt]);
		like_std = 3;
	} else {
		printf("Unknown device type:\n\tbits 15&14 of general configuration word 0 both set to 1.\n");
		exit(0);
	}
	if(!(val[GEN_CONFIG] & MEDIA_REMOVABLE))
		printf("non-");
	printf("removable media\n");


	/* Info from the specific configuration word says whether or not the
	 * ID command completed correctly.  It is only defined, however in
	 * ATA/ATAPI-5; it is reserved (value theoretically 0) in prior 
	 * standards.  Since the values allowed for this word are extremely
	 * specific, it should be safe to check it now, even though we don't
	 * know yet what standard this device is using.
	 */
	if((val[CONFIG]==STBY_NID_VAL) || (val[CONFIG]==STBY_ID_VAL) ||
	   (val[CONFIG]==PWRD_NID_VAL) || (val[CONFIG]==PWRD_ID_VAL) ) {
	   	like_std = 5;
		if((val[CONFIG]==STBY_NID_VAL) || (val[CONFIG]==STBY_ID_VAL))
			printf("powers-up in standby; SET FEATURES subcmd spins-up.\n");
		if((val[CONFIG]==STBY_NID_VAL) || (val[CONFIG]==PWRD_NID_VAL) ||
		   (val[GEN_CONFIG] & INCOMPLETE)) 
			printf("\n\tWARNING: ID response incomplete.\n\tWARNING: Following data may be incorrect.\n\n");
	}

	/* output the model and serial numbers and the fw revision */
	if(val[START_MODEL_NUM]) {
		printf("\tModel Number:\t\t");
		for(ii = START_MODEL_NUM; ii<=END_MODEL_NUM; ii++) {
	        	printf("%c%c",(char)0x00ff&(val[ii]>>8),(char)val[ii]&0x00ff);
		}
		printf("\n");
	}
	if(val[START_SERIAL_NUM]) {
		printf("\tSerial Number:\t\t");
		for(ii = START_SERIAL_NUM; ii<=END_SERIAL_NUM; ii++) {
			printf("%c%c",(char)0x00ff&(val[ii]>>8),(char)val[ii]&0x00ff);
		}
		printf("\n");
	}
	if(val[START_FW_REV]) {
		printf("\tFirmware Revision:\t");
		for(ii = START_FW_REV; ii<=END_FW_REV; ii++) {
			printf("%c%c",(char)0x00ff&(val[ii]>>8),(char)val[ii]&0x00ff);
		}
		printf("\n");
	}

	/* major & minor standards version number (Note: these words were not
	 * defined until ATA-3 & the CDROM std uses different words.) */
	printf("Standards:");
	if(eqpt != CDROM) {
		if(val[MINOR] && (val[MINOR] <=MINOR_MAX)){
			printf("\n\tUsed: ");
			std = actual_ver[val[MINOR]];
			printf("%s ",minor_str[val[MINOR]]);
			if(like_std < 3) like_std = 3;
		}
		if(val[MAJOR] && (val[MAJOR] !=NOVAL_1)) {
			printf("\n\tSupported: ");
			jj = val[MAJOR] >> 1;
			for(ii = 1; ii <15; ii++) {
				if(jj & 0x0001) {
					printf("%u ", ii);
					if(like_std < ii) like_std = ii;
				}
				jj >>= 1;
			}
			if(like_std < 3) like_std = 3;
		}
	} else {
		printf("\n");
		if(val[CDR_MINOR]) {
			printf("\tUsed: ");
			if(val[CDR_MINOR] == 9) 
				printf("ATAPI for CD-ROMs, SFF-8020i, r2.5\n");
			else
				printf("unrecognised.\n");
		}
		if(val[CDR_MAJOR] && (val[CDR_MAJOR] !=NOVAL_1)) {
			printf("\tSupported: CD-ROM ATAPI");
			jj = val[CDR_MAJOR] >> 1;
			for(ii = 1; ii <15; ii++) {
				if(jj & 0x0001) {
					printf("-%u ", ii);
				}
				jj >>= 1;
			}
			printf("\n");
		}
	}

	/* Figure out what standard the device is using if it hasn't told
	 * us.  If we know the std, check if the device is using any of
	 * the words from the next level up.  It happens.
	 */
	if(like_std < std) like_std = std;
	if(eqpt != CDROM) {
		if(((std == 4) || (!std && (like_std < 5))) &&
		   ((((val[INTEGRITY] & SIG) == SIG_VAL) && !chksum) ||
		    ((val[HWRST_RSLT] & VALID) == VALID_VAL) ||
		    (((val[CMDS_SUPP_1] & VALID) == VALID_VAL) &&
		     ((val[CMDS_SUPP_1] & CMDS_W83) > 0x001f)) ) ) {
			like_std = 5;
		}  else if(((std == 3) || (!std && (like_std < 4))) &&
			   ((((val[CMDS_SUPP_1] & VALID) == VALID_VAL) &&
			     (((val[CMDS_SUPP_1] & CMDS_W83) > 0x0000) ||  
			      ((val[CMDS_SUPP_0] & CMDS_W82) > 0x000f))) ||
			    ((val[CAPAB_1] & VALID) == VALID_VAL) ||
			    ((val[WHATS_VALID] & OK_W88) && val[ULTRA_DMA]) ||
			    ((val[RM_STAT] & RM_STAT_BITS) == RM_STAT_SUP) ) ) {
			like_std = 4;
		}  else if(((std == 2) || (!std && (like_std < 3))) &&
			   ((val[CMDS_SUPP_1] & VALID) == VALID_VAL) ) {
			like_std = 3;
		}  else if(((std == 1) || (!std && (like_std < 2))) &&
		   	   ((val[CAPAB_0] & (IORDY_SUP | IORDY_OFF)) ||
		   	    (val[WHATS_VALID] & OK_W64_70)) ) {
			like_std = 2;
		}
		if(!std)
			printf("\n\tLikely used: %u\n",like_std);
		else if(like_std > std) {
			printf("& some of %u\n",like_std);
		} else  printf("\n");
	}

	printf("Configuration:\n");
	/* more info from the general configuration word */
	if((eqpt != CDROM) && (like_std == 1)) {
		jj = val[GEN_CONFIG] >> 1;
		for(ii = 1; ii < 15; ii++) {
			if(jj & 0x0001) printf("\t%s\n",ata1_cfg_str[ii]);
			jj >>=1;
		}
	}
	if(dev == ATAPI_DEV) {
		printf("\tDRQ response: "); /* Data Request (DRQ) */
		switch(val[GEN_CONFIG] & DRQ_RESPONSE_TIME) {
		case DRQ_3MS_VAL : printf("3ms.\n"); break;
		case DRQ_INTR_VAL : printf("<=10ms with INTRQ\n"); break;
		case DRQ_50US_VAL : printf("50us.\n"); break;
		default : printf("unknown.\n"); break;
		}
		printf("\tPacket size: ");
		switch(val[GEN_CONFIG] & PKT_SIZE_SUPPORTED) {
		case PKT_SIZE_12_VAL : printf("12 bytes\n"); break;
		case PKT_SIZE_16_VAL : printf("16 bytes\n"); break;
		default : printf("Unknown\n"); break;
		}
	} else {
		jj = val[WHATS_VALID] & OK_W54_58;
		printf("\tLogical\t\tmax\tcurrent\n");
		printf("\tcylinders\t%u\t%u\n",val[LCYLS],jj?val[LCYLS_CUR]:0);
		printf("\theads\t\t%u\t%u\n",val[LHEADS],jj?val[LHEADS_CUR]:0);
		printf("\tsectors/track\t%u\t%u\n",val[LSECTS],jj?val[LSECTS_CUR]:0);
		printf("\tbytes/track:\t%u",val[TRACK_BYTES]);
		if(like_std != 1) printf("\t\t(obsolete)\n");
		else		  printf("\n");
		printf("\tbytes/sector:\t%u",val[SECT_BYTES]);
		if(like_std != 1) printf("\t\t(obsolete)\n");
		else		  printf("\n");
		if(jj) {
			mm = (__u32)val[CAPACITY_MSB] << 16 | val[CAPACITY_LSB];
			printf("\tcurrent sector capacity: %u\n",mm);
		} 
		ll = (__u32)val[LBA_SECTS_MSB] << 16 | val[LBA_SECTS_LSB];
		printf("\tLBA user addressable sectors = %u\n",ll);
	}

	/* hw support of commands (capabilities) */
	printf("Capabilities:\n");
	printf("\t");
	if(dev == ATAPI_DEV) {
		if(eqpt != CDROM) {
			if(val[CAPAB_0] & CMD_Q_SUP) printf("Cmd queuing, ");
		}
		if(val[CAPAB_0] & OVLP_SUP) printf("Cmd overlap, ");
	}
	if(val[CAPAB_0] & LBA_SUP) printf("LBA, ");
	if(like_std != 1) {
		printf("IORDY");
		if(!(val[CAPAB_0] & IORDY_SUP)) printf("(may be)");
		if(val[CAPAB_0] & IORDY_OFF) printf("(can");
		else			     printf("(cannot");
		printf(" be disabled)");
	} else printf("no IORDY"); 
	printf("\n");
	jj = 0;
	if(val[BUF_SIZE] && (val[BUF_SIZE] != NOVAL_1)) {
		printf("\tBuffer size: %.1fkB",(float)val[BUF_SIZE]/2);
		jj = 1;
	}
	if(val[RW_LONG]) {
		printf("\tECC bytes: %u",val[RW_LONG]);
		jj = 1;
	}
	if((eqpt != CDROM) && (like_std > 3)) {
		printf("\tQueue depth: %u",(val[QUEUE_DEPTH] & DEPTH_BITS)+1);
		jj = 1;
	}
	if(jj) printf("\n");
	if(dev == ATA_DEV) {
		if(like_std == 1) {
			if(val[BUF_TYPE]) printf("\tBuffer type: %04x.",val[BUF_TYPE]);
			printf("\tCan");
			if(!val[DWORD_IO]) printf("not");
			printf(" perform double-word IO\n");
		} else {
			printf("\tStandby timer values: spec'd by ");
			if(val[CAPAB_0] & STD_STBY) printf("standard");
			else 			    printf("Vendor");
			if((like_std > 3) && ((val[CAPAB_1] & VALID) == VALID_VAL)) {
			   	if(val[CAPAB_1] & MIN_STANDBY_TIMER) printf(", with ");
				else 				     printf(", no ");
				printf("device specific minimum\n");
			} else  printf("\n");
		}
		printf("\tr/w multiple sector transfer: ");
		if((like_std < 3) && !(val[SECTOR_XFER_MAX] & SECTOR_XFER)) {
			printf("not supported\n");
		} else {
			printf("Max = %u\t",val[SECTOR_XFER_MAX] & SECTOR_XFER);
			printf("Current = ");
			if(val[SECTOR_XFER_CUR] & MULTIPLE_SETTING_VALID)
				printf("%u\n",val[SECTOR_XFER_CUR] & SECTOR_XFER);
			else	printf("?\n");
		}
		if((like_std > 3) && (val[ADV_PWR])){
			printf("\tAdvanced power management level: %u\n",val[ADV_PWR]);
		}
	} else { /* ATAPI */
		if(eqpt != CDROM) {
			if(val[CAPAB_0] & SWRST_REQ) printf("\tATA sw reset required\n");
		}
		if(val[PKT_REL] || val[SVC_NBSY]) {
			printf("\tOverlap support:");
			if(val[PKT_REL]) printf(" %uus to release bus.",val[PKT_REL]);
			if(val[SVC_NBSY]) printf(" %uus to clear BSY after SERVICE cmd.",val[SVC_NBSY]);
			printf("\n");
		}
	}

	/* DMA stuff. Check that only one DMA mode is selected. */
	printf("\tDMA: ");
	if(!(val[CAPAB_0] & DMA_SUP)) {
		printf("not supported\n");
	} else {
		if(val[DMA_MODE] && !val[SINGLE_DMA] && !val[MULTI_DMA]) 
			printf(" sdma%u\n",(val[DMA_MODE] & MODE) >> 8);
		if(val[SINGLE_DMA]) {
			jj = val[SINGLE_DMA];   kk = val[SINGLE_DMA] >> 8;
			err_dma += mode_loop((__u32)jj,(__u32)kk,'s',&have_mode);
		}
		if(val[MULTI_DMA]) {
			jj = val[MULTI_DMA];   kk = val[MULTI_DMA] >> 8;
			err_dma += mode_loop((__u32)jj,(__u32)kk,'m',&have_mode);
		}
		if((val[WHATS_VALID] & OK_W88) && val[ULTRA_DMA]) {
			jj = val[ULTRA_DMA];   kk = val[ULTRA_DMA] >> 8;
			err_dma += mode_loop((__u32)jj,(__u32)kk,'u',&have_mode);
		}
		if(err_dma || !have_mode) printf("(?)");
		printf("\n");

		if((dev == ATAPI_DEV) && (eqpt != CDROM) && (val[CAPAB_0] & DMA_IL_SUP)) 
			printf("\t     Interleaved DMA support\n");

		if((val[WHATS_VALID] & OK_W64_70) && 
		   (val[DMA_TIME_MIN] || val[DMA_TIME_NORM])) {
			printf("\t     Cycle time:");
			if(val[DMA_TIME_MIN])
				printf(" min=%uns",val[DMA_TIME_MIN]);
			if(val[DMA_TIME_NORM])
				printf(" recommended=%uns",val[DMA_TIME_NORM]);
			printf("\n");
		}
	}

	/* Programmed IO stuff */
	printf("\tPIO: ");
	if((val[WHATS_VALID] & OK_W64_70) && (val[ADV_PIO_MODES] & PIO_SUP)) {
		jj = ((val[ADV_PIO_MODES] & PIO_SUP) << 3) | 0x0007;
		for(ii = 0; ii <= PIO_MODE_MAX ; ii++) {
			if(jj & 0x0001)
				printf("pio%u ",ii);
			jj >>=1;
		}
		printf("\n");
	} else if(((std < 5) || (eqpt == CDROM)) && (val[PIO_MODE] & MODE) ) {
		for(ii = 0; ii <= val[PIO_MODE]>>8; ii++) {
			printf("pio%u ",ii);
		}
		printf("\n");
	} else  printf("pio0\n");
	if(val[WHATS_VALID] & OK_W64_70) {
		if(val[PIO_NO_FLOW] || val[PIO_FLOW]) {
			printf("\t     Cycle time:");
			if(val[PIO_NO_FLOW])
				printf(" no flow control=%uns", val[PIO_NO_FLOW]);
			if(val[PIO_FLOW])
				printf("  IORDY flow control=%uns", val[PIO_FLOW]);
			printf("\n");
		}
	}

	if((val[CMDS_SUPP_1] & VALID) == VALID_VAL){
		printf("Commands/features:\n\tEnabled\tSupported:\n");
		jj = val[CMDS_SUPP_0];
		kk = val[CMDS_EN_0];
		for(ii = 0; ii < NUM_CMD_FEAT_STR; ii++) {
			if((jj & 0x8000) && (*cmd_feat_str[ii] != '\0')) {
				if(kk & 0x8000) printf("\t   *");
				else		printf("\t");
				printf("\t%s\n",cmd_feat_str[ii]);
			}
			jj <<=1; kk<<=1;
			if(ii%16 == 15) {
				jj = val[CMDS_SUPP_0+1+(ii/16)];
				kk = val[CMDS_EN_0+1+(ii/16)];
			}
		}
	}
	if((val[RM_STAT] & RM_STAT_BITS) == RM_STAT_SUP) 
		printf("\tRemovable Media Status Notification feature set supported\n");
	
	
	/* security */
	if((eqpt != CDROM) && (like_std > 3) && 
	   (val[SECU_STATUS] || val[ERASE_TIME] || val[ENH_ERASE_TIME])) {
		printf("Security: \n");
		if(val[PSWD_CODE] && (val[PSWD_CODE] != NOVAL_1))
			printf("\tMaster password revision code = %u\n",val[PSWD_CODE]);
		jj = val[SECU_STATUS];
		if(jj) {
			for(ii = 0; ii < NUM_SECU_STR; ii++) {
				if(!(jj & 0x0001)) printf("\tnot\t");
				else		   printf("\t\t");
				printf("%s\n",secu_str[ii]);
				jj >>=1;
			}
			if(val[SECU_STATUS] & SECU_ENABLED) {
				printf("\tSecurity level ");
				if(val[SECU_STATUS] & SECU_LEVEL) printf("maximum\n");
				else				  printf("high\n");
			}
		}
		jj =  val[ERASE_TIME]     & ERASE_BITS;
		kk =  val[ENH_ERASE_TIME] & ERASE_BITS;
		if(jj || kk) {
			printf("\t");
			if(jj) printf("%umin for SECURITY ERASE UNIT. ", jj==ERASE_BITS ? 508 : jj<<1);
			if(kk) printf("%umin for ENHANCED SECURITY ERASE UNIT.", kk==ERASE_BITS ? 508 : kk<<1);
			printf("\n");
		}
	}

	/* reset result */
	if((val[HWRST_RSLT] & VALID) == VALID_VAL) {
		printf("HW reset results:\n");
		if(val[HWRST_RSLT] & CBLID) printf("\tCBLID- above Vih\n");
		else			    printf("\tCBLID- below Vih\n");
		if(val[HWRST_RSLT] & RST0)  {
			printf("\tDevice num = 0");
			jj = val[HWRST_RSLT];
		} else {
			printf("\tDevice num = 1");
			jj = val[HWRST_RSLT] >> 8;
		}
		if((jj & DEV_DET) == JUMPER_VAL) 
			printf(" determined by the jumper");
		else if((jj & DEV_DET) == CSEL_VAL)
			printf(" determined by CSEL");
		printf("\n");
	}

	/* more stuff from std 5 */
	if((like_std > 4) && (eqpt != CDROM)) {
		if(val[CFA_PWR_MODE] & VALID_W160) {
			printf("CFA power mode 1:\n\t");
			if(val[CFA_PWR_MODE] & PWR_MODE_OFF) printf("dis");
			else				     printf("en");
			printf("abled");
			if(val[CFA_PWR_MODE] & PWR_MODE_REQ)
				printf(" and required by some commands");
			printf("\n");
			if(val[CFA_PWR_MODE] & MAX_AMPS)
				printf("\tMaximum current = %uma\n",val[CFA_PWR_MODE] & MAX_AMPS);
		}
		if((val[INTEGRITY] & SIG) == SIG_VAL) {
			printf("Checksum: ");
			if(chksum) printf("in");
			printf("correct\n");
		}
	}

	exit(0);
}
__u8 mode_loop(__u32 mode_sup, __u32 mode_sel, int cc, __u8 *have_mode) {
	__u16 ii;
	__u8 err_dma = 0;
	for(ii = 0; ii <= MODE_MAX; ii++) {
		if(mode_sel & 0x0001) {
			printf("*%cdma%u ",cc,ii);
			if(*have_mode) err_dma = 1;
			*have_mode = 1;
		} else if(mode_sup & 0x0001) {
			printf("%cdma%u ",cc,ii);
		}
		mode_sup >>=1;   mode_sel >>=1;
	}
	return err_dma;
}

#endif /* Unused by dwipe. */

/* eof */
