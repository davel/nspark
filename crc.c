
/*
 * calculate Cyclic Redundancy Check
 *
 * Algorithm by: David Schwaderer, Published in PC Tech Journal April 1985.
 *
 * $Header: crc.c 1.6 95/07/13 $
 * $Log:	crc.c,v $
 * Revision 1.6  95/07/13  xx:xx:xx  BB
 * Minor change for use with Borland C/C++ 1,3 and 4
 *
 * Revision 1.5  94/12/12  17:32:03  arb
 * Added support for ArcFS outputsize checking
 *
 * Revision 1.4  92/12/07  17:17:39  duplain
 * reformatted source.
 *
 * Revision 1.3  92/11/06  12:05:55  duplain
 * Credit author of algorithm, now I know his name.
 *
 * Revision 1.2  92/11/06  11:56:47  duplain
 * Slight change to CRC calculation code, no functional changes.
 *
 * Revision 1.1  92/09/29  18:02:16  duplain
 * Initial revision
 *
 */

#include <stdio.h>
#include "spark.h"
#include "main.h"
#include "arcfs.h"

Word crc;
static Word crctable[256] = {
	0x00000000, 0x0000c0c1, 0x0000c181, 0x00000140,
	0x0000c301, 0x000003c0, 0x00000280, 0x0000c241,
	0x0000c601, 0x000006c0, 0x00000780, 0x0000c741,
	0x00000500, 0x0000c5c1, 0x0000c481, 0x00000440,
	0x0000cc01, 0x00000cc0, 0x00000d80, 0x0000cd41,
	0x00000f00, 0x0000cfc1, 0x0000ce81, 0x00000e40,
	0x00000a00, 0x0000cac1, 0x0000cb81, 0x00000b40,
	0x0000c901, 0x000009c0, 0x00000880, 0x0000c841,
	0x0000d801, 0x000018c0, 0x00001980, 0x0000d941,
	0x00001b00, 0x0000dbc1, 0x0000da81, 0x00001a40,
	0x00001e00, 0x0000dec1, 0x0000df81, 0x00001f40,
	0x0000dd01, 0x00001dc0, 0x00001c80, 0x0000dc41,
	0x00001400, 0x0000d4c1, 0x0000d581, 0x00001540,
	0x0000d701, 0x000017c0, 0x00001680, 0x0000d641,
	0x0000d201, 0x000012c0, 0x00001380, 0x0000d341,
	0x00001100, 0x0000d1c1, 0x0000d081, 0x00001040,
	0x0000f001, 0x000030c0, 0x00003180, 0x0000f141,
	0x00003300, 0x0000f3c1, 0x0000f281, 0x00003240,
	0x00003600, 0x0000f6c1, 0x0000f781, 0x00003740,
	0x0000f501, 0x000035c0, 0x00003480, 0x0000f441,
	0x00003c00, 0x0000fcc1, 0x0000fd81, 0x00003d40,
	0x0000ff01, 0x00003fc0, 0x00003e80, 0x0000fe41,
	0x0000fa01, 0x00003ac0, 0x00003b80, 0x0000fb41,
	0x00003900, 0x0000f9c1, 0x0000f881, 0x00003840,
	0x00002800, 0x0000e8c1, 0x0000e981, 0x00002940,
	0x0000eb01, 0x00002bc0, 0x00002a80, 0x0000ea41,
	0x0000ee01, 0x00002ec0, 0x00002f80, 0x0000ef41,
	0x00002d00, 0x0000edc1, 0x0000ec81, 0x00002c40,
	0x0000e401, 0x000024c0, 0x00002580, 0x0000e541,
	0x00002700, 0x0000e7c1, 0x0000e681, 0x00002640,
	0x00002200, 0x0000e2c1, 0x0000e381, 0x00002340,
	0x0000e101, 0x000021c0, 0x00002080, 0x0000e041,
	0x0000a001, 0x000060c0, 0x00006180, 0x0000a141,
	0x00006300, 0x0000a3c1, 0x0000a281, 0x00006240,
	0x00006600, 0x0000a6c1, 0x0000a781, 0x00006740,
	0x0000a501, 0x000065c0, 0x00006480, 0x0000a441,
	0x00006c00, 0x0000acc1, 0x0000ad81, 0x00006d40,
	0x0000af01, 0x00006fc0, 0x00006e80, 0x0000ae41,
	0x0000aa01, 0x00006ac0, 0x00006b80, 0x0000ab41,
	0x00006900, 0x0000a9c1, 0x0000a881, 0x00006840,
	0x00007800, 0x0000b8c1, 0x0000b981, 0x00007940,
	0x0000bb01, 0x00007bc0, 0x00007a80, 0x0000ba41,
	0x0000be01, 0x00007ec0, 0x00007f80, 0x0000bf41,
	0x00007d00, 0x0000bdc1, 0x0000bc81, 0x00007c40,
	0x0000b401, 0x000074c0, 0x00007580, 0x0000b541,
	0x00007700, 0x0000b7c1, 0x0000b681, 0x00007640,
	0x00007200, 0x0000b2c1, 0x0000b381, 0x00007340,
	0x0000b101, 0x000071c0, 0x00007080, 0x0000b041,
	0x00005000, 0x000090c1, 0x00009181, 0x00005140,
	0x00009301, 0x000053c0, 0x00005280, 0x00009241,
	0x00009601, 0x000056c0, 0x00005780, 0x00009741,
	0x00005500, 0x000095c1, 0x00009481, 0x00005440,
	0x00009c01, 0x00005cc0, 0x00005d80, 0x00009d41,
	0x00005f00, 0x00009fc1, 0x00009e81, 0x00005e40,
	0x00005a00, 0x00009ac1, 0x00009b81, 0x00005b40,
	0x00009901, 0x000059c0, 0x00005880, 0x00009841,
	0x00008801, 0x000048c0, 0x00004980, 0x00008941,
	0x00004b00, 0x00008bc1, 0x00008a81, 0x00004a40,
	0x00004e00, 0x00008ec1, 0x00008f81, 0x00004f40,
	0x00008d01, 0x00004dc0, 0x00004c80, 0x00008c41,
	0x00004400, 0x000084c1, 0x00008581, 0x00004540,
	0x00008701, 0x000047c0, 0x00004680, 0x00008641,
	0x00008201, 0x000042c0, 0x00004380, 0x00008341,
	0x00004100, 0x000081c1, 0x00008081, 0x00004040
};

void
calccrc(Byte byte)
{
	if (crcsize-- > 0)
		/* BB added cast to next line to stop Borland compiler
		   from complaining */
		/* crc = ((crc >> 8) & 0xff) ^ crctable[(crc ^ byte) & 0xff]; */
#ifdef __MSDOS__
		crc = ((crc >> 8) & 0xff) ^ crctable[(int) ((crc ^ byte) & 0xff)];
#else
		crc = ((crc >> 8) & 0xff) ^ crctable[(crc ^ byte) & 0xff];
#endif							/* __MSDOS__ */
}
