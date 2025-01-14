/********************************************************************************
*  Look-Up-Table for converting characters from upper case to lower case	*
*  according to a user defined code page translation table			*
*-------------------------------------------------------------------------------*

[new]	25.09.96	for the new -i1 option: char/digit/rest matching
[chg]	22.09.96	metasymbol [TG]
[chg]	21.08.96	changed from subroutine to struct [TG]
	
	Tom Gries <gries@ibm.net>
[ini]	Mike Thomas <rmthomas@sciolus.cistron.nl>

*******************************************************/

#define INCL_DOSNLS     /* National Language Support values */
#define INCL_DOSERRORS  /* DOS error values                 */

#ifndef _WIN32

/* TG 28.04.04 

#include <os2.h>

Attention:
Load the OS/2 header files only, if you really compile for OS/2.
The directive "ifndef _WIN32" ... is NOT equivalent to "compile for OS/2"

*/

#endif

#ifdef __EMX__
#include <stdlib.h>
#include <dos.h>
#endif

#include <stdio.h>

#include "agrep.h"
#include "codepage.h"

char	CODEPAGE_STR[MAX_LINE_LEN];	/* holds the selected codepage identifier */
int	CODEPAGE;			/* corresponding number */
char	CP_MAPPING;			/* -i option = case(in)sensitive search ?
					   is one of 'i', 'a', '#' or 0	*/

unsigned char LUT[256];			/* THE LOOK-UP TABLE (filled in AGREP.C) */


int get_current_codepage(VOID)

#ifdef __EMX__

{
extern unsigned int _emx_env;

if ((_emx_env & 0x0200) == 0x0200) {	/* running under pure OS/2 2.x */

/* DosQueryCp allows a process to query its current process code page and the 
   prepared system code pages. 
 
   This example gets the current code page and up to three other prepared code 
   pages.
*/

    ULONG  aulCpList[8]  = {0},                /* Code page list        */
           ulBufSize     = 8 * sizeof(ULONG),  /* Size of output list   */
           ulListSize    = 0,                  /* Size of list returned */
           indx          = 0;                  /* Loop index            */
    APIRET rc            = NO_ERROR;           /* Return code           */

    rc = DosQueryCp(ulBufSize,      /* Length of output code page list  */
                    aulCpList,      /* List of code pages               */
                    &ulListSize);   /* Length of list returned          */
 
    if (rc != NO_ERROR) {
       printf("DosQueryCp error: return code = %u\n",rc);
       return -1;
    }
    else return aulCpList[0];
    
	/*    for (indx=0; indx < ulListSize/sizeof(ULONG); indx++)
          printf ("aulCpList[%u] = %u\n", indx, aulCpList[indx]); */

}	/* running under OS/2 */

else {

/* Issue software interrupt 0x21 via int86() when compiled with emx */
/* DOS only */

/* Go on only, if data and stack executable */
/* The -ac option under DOS must be set. See _int86() and exmbind descriptions. */

if ((_emx_env & 0x0800) == 0x0800) {

	/* check if running under pure DOS with data and stack executable */

    union REGS r;

      r.x.ax = 0x6601;
      _int86 (0x21, &r, &r);
      
      /* if the CARRY is not set, then the bx register tells us the codepage */
      if (r.e.eflags & 0x0001) return (-1);
      else return (r.x.bx);

}	/* running under DOS */
}	/* else */

}	/* end of get_current_codepage() */

#else			/* using a non-EMX compiler */
{ return(-1); }
#endif
 

/*	1: lowercase_accented()
	2: lowercase_not_accented()
	3: charactertype_one_of_#_1_a() 
*/

struct CODEPAGE_struct CP[CODEPAGES][CPSIZE] \
= {

{	/* CODEPAGE 437 ( intern CP[0] ) */
	/* The 128 characters above 7Fh of codepage 437 : */
	/* ���������������������������������������������������������������� */
	/* ���������������������������������������������������������������� */
	
	{0x00,0x00,0x00, 0} /* ... */, {0x01,0x01,0x01, 0} /* ... */,
	{0x02,0x02,0x02, 0} /* ... */, {0x03,0x03,0x03, 0} /* ... */,
	{0x04,0x04,0x04, 0} /* ... */, {0x05,0x05,0x05, 0} /* ... */,
	{0x06,0x06,0x06, 0} /* ... */, {0x07,0x07,0x07, 0} /* ... */,
	{0x08,0x08,0x08, 0} /* ... */, {0x09,0x09,0x09, 0} /* ... */,
	{0x0A,0x0A,0x0A, 0} /* ... */, {0x0B,0x0B,0x0B, 0} /* ... */,
	{0x0C,0x0C,0x0C, 0} /* ... */, {0x0D,0x0D,0x0D, 0} /* ... */,
	{0x0E,0x0E,0x0E, 0} /* ... */, {0x0F,0x0F,0x0F, 0} /* ... */,
	{0x10,0x10,0x10, 0} /* ... */, {0x11,0x11,0x11, 0} /* ... */,
	{0x12,0x12,0x12, 0} /* ... */, {0x13,0x13,0x13, 0} /* ... */,
	{0x14,0x14,0x14, 0} /* ... */, {0x15,0x15,0x15, 0} /* ... */,
	{0x16,0x16,0x16, 0} /* ... */, {0x17,0x17,0x17, 0} /* ... */,
	{0x18,0x18,0x18, 0} /* ... */, {0x19,0x19,0x19, 0} /* ... */,
	{0x1A,0x1A,0x1A, 0} /* ... */, {0x1B,0x1B,0x1B, 0} /* ... */,
	{0x1C,0x1C,0x1C, 0} /* ... */, {0x1D,0x1D,0x1D, 0} /* ... */,
	{0x1E,0x1E,0x1E, 0} /* ... */, {0x1F,0x1F,0x1F, 0} /* ... */,
	{0x20,0x20,0x23, 0} /*     */, {0x21,0x21,0x23, 0} /* !!! */,
	{0x22,0x22,0x23, 0} /* """ */, {0x23,0x23,0x23, 0} /* ### */,
	{0x24,0x24,0x23, 0} /* $$$ */, {0x25,0x25,0x23, 0} /* %%% */,
	{0x26,0x26,0x23, 0} /* &&& */, {0x27,0x27,0x23, 0} /* ''' */,
	{0x28,0x28,0x23, 0} /* ((( */, {0x29,0x29,0x23, 0} /* ))) */,
	{0x2A,0x2A,0x23, 0} /* *** */, {0x2B,0x2B,0x23, 0} /* +++ */,
	{0x2C,0x2C,0x23, 0} /* ,,, */, {0x2D,0x2D,0x23, 0} /* --- */,
	{0x2E,0x2E,0x23, 0} /* ... */, {0x2F,0x2F,0x23, 0} /* /// */,
	{0x30,0x30,0x31, 0} /* 000 */, {0x31,0x31,0x31, 0} /* 111 */,
	{0x32,0x32,0x31, 0} /* 222 */, {0x33,0x33,0x31, 0} /* 333 */,
	{0x34,0x34,0x31, 0} /* 444 */, {0x35,0x35,0x31, 0} /* 555 */,
	{0x36,0x36,0x31, 0} /* 666 */, {0x37,0x37,0x31, 0} /* 777 */,
	{0x38,0x38,0x31, 0} /* 888 */, {0x39,0x39,0x31, 0} /* 999 */,
	{0x3A,0x3A,0x23, 0} /* ::: */, {0x3B,0x3B,0x23, 0} /* ;;; */,
	{0x3C,0x3C,0x23, 0} /* <<< */, {0x3D,0x3D,0x23, 0} /* === */,
	{0x3E,0x3E,0x23, 0} /* >>> */, {0x3F,0x3F,0x23, 0} /* ??? */,
	{0x40,0x40,0x23, 0} /* @@@ */, {0x61,0x61,0x61, 0} /* Aaa */,
	{0x62,0x62,0x61, 0} /* Bbb */, {0x63,0x63,0x61, 0} /* Ccc */,
	{0x64,0x64,0x61, 0} /* Ddd */, {0x65,0x65,0x61, 0} /* Eee */,
	{0x66,0x66,0x61, 0} /* Fff */, {0x67,0x67,0x61, 0} /* Ggg */,
	{0x68,0x68,0x61, 0} /* Hhh */, {0x69,0x69,0x61, 0} /* Iii */,
	{0x6A,0x6A,0x61, 0} /* Jjj */, {0x6B,0x6B,0x61, 0} /* Kkk */,
	{0x6C,0x6C,0x61, 0} /* Lll */, {0x6D,0x6D,0x61, 0} /* Mmm */,
	{0x6E,0x6E,0x61, 0} /* Nnn */, {0x6F,0x6F,0x61, 0} /* Ooo */,
	{0x70,0x70,0x61, 0} /* Ppp */, {0x71,0x71,0x61, 0} /* Qqq */,
	{0x72,0x72,0x61, 0} /* Rrr */, {0x73,0x73,0x61, 0} /* Sss */,
	{0x74,0x74,0x61, 0} /* Ttt */, {0x75,0x75,0x61, 0} /* Uuu */,
	{0x76,0x76,0x61, 0} /* Vvv */, {0x77,0x77,0x61, 0} /* Www */,
	{0x78,0x78,0x61, 0} /* Xxx */, {0x79,0x79,0x61, 0} /* Yyy */,
	{0x7A,0x7A,0x61, 0} /* Zzz */, {0x5B,0x5B,0x23, 0} /* [[[ */,
	{0x5C,0x5C,0x23, 0} /* \\\ */, {0x5D,0x5D,0x23, 0} /* ]]] */,
	{0x5E,0x5E,0x23, 0} /* ^^^ */, {0x5F,0x5F,0x23, 0} /* ___ */,
	{0x60,0x60,0x23, 0} /* ``` */, {0x61,0x61,0x61, 0} /* aaa */,
	{0x62,0x62,0x61, 0} /* bbb */, {0x63,0x63,0x61, 0} /* ccc */,
	{0x64,0x64,0x61, 0} /* ddd */, {0x65,0x65,0x61, 0} /* eee */,
	{0x66,0x66,0x61, 0} /* fff */, {0x67,0x67,0x61, 0} /* ggg */,
	{0x68,0x68,0x61, 0} /* hhh */, {0x69,0x69,0x61, 0} /* iii */,
	{0x6A,0x6A,0x61, 0} /* jjj */, {0x6B,0x6B,0x61, 0} /* kkk */,
	{0x6C,0x6C,0x61, 0} /* lll */, {0x6D,0x6D,0x61, 0} /* mmm */,
	{0x6E,0x6E,0x61, 0} /* nnn */, {0x6F,0x6F,0x61, 0} /* ooo */,
	{0x70,0x70,0x61, 0} /* ppp */, {0x71,0x71,0x61, 0} /* qqq */,
	{0x72,0x72,0x61, 0} /* rrr */, {0x73,0x73,0x61, 0} /* sss */,
	{0x74,0x74,0x61, 0} /* ttt */, {0x75,0x75,0x61, 0} /* uuu */,
	{0x76,0x76,0x61, 0} /* vvv */, {0x77,0x77,0x61, 0} /* www */,
	{0x78,0x78,0x61, 0} /* xxx */, {0x79,0x79,0x61, 0} /* yyy */,
	{0x7A,0x7A,0x61, 0} /* zzz */, {0x7B,0x7B,0x23, 0} /* {{{ */,
	{0x7C,0x7C,0x23, 0} /* ||| */, {0x7D,0x7D,0x23, 0} /* }}} */,
	{0x7E,0x7E,0x23, 0} /* ~~~ */, {0x7F,0x7F,0x23, 0} /*  */,
	{0x87,0x63,0x61, 0} /* ��c */, {0x81,0x75,0x61, 0} /* ��u */,
	{0x82,0x65,0x61, 0} /* ��e */, {0x83,0x61,0x61, 0} /* ��a */,
	{0x84,0x61,0x61, 0} /* ��a */, {0x85,0x61,0x61, 0} /* ��a */,
	{0x86,0x61,0x61, 0} /* ��a */, {0x87,0x63,0x61, 0} /* ��c */,
	{0x88,0x65,0x61, 0} /* ��e */, {0x89,0x65,0x61, 0} /* ��e */,
	{0x8A,0x65,0x61, 0} /* ��e */, {0x8B,0x69,0x61, 0} /* ��i */,
	{0x8C,0x69,0x61, 0} /* ��i */, {0x8D,0x69,0x61, 0} /* ��i */,
	{0x84,0x61,0x61, 0} /* ��a */, {0x86,0x61,0x61, 0} /* ��a */,
	{0x82,0x65,0x61, 0} /* ��e */, {0x91,0x61,0x23, 0} /* ��a */,
	{0x91,0x61,0x23, 0} /* ��a */, {0x93,0x6F,0x61, 0} /* ��o */,
	{0x94,0x6F,0x61, 0} /* ��o */, {0x95,0x6F,0x61, 0} /* ��o */,
	{0x96,0x75,0x61, 0} /* ��u */, {0x97,0x75,0x61, 0} /* ��u */,
	{0x98,0x79,0x61, 0} /* ��y */, {0x94,0x6F,0x61, 0} /* ��o */,
	{0x81,0x75,0x61, 0} /* ��u */, {0x9B,0x63,0x23, 0} /* ��c */,
	{0x9C,0x62,0x23, 0} /* ��b */, {0x9D,0x79,0x23, 0} /* ��y */,
	{0x9E,0x70,0x23, 0} /* ��p */, {0x9F,0x66,0x23, 0} /* ��f */,
	{0xA0,0x61,0x61, 0} /* ��a */, {0xA1,0x69,0x61, 0} /* ��i */,
	{0xA2,0x6F,0x61, 0} /* ��o */, {0xA3,0x75,0x61, 0} /* ��u */,
	{0xA4,0x6E,0x61, 0} /* ��n */, {0xA4,0x6E,0x61, 0} /* ��n */,
	{0xA6,0x61,0x61, 0} /* ��a */, {0xA7,0x6F,0x61, 0} /* ��o */,
	{0xA8,0x3F,0x23, 0} /* ��? */, {0xA9,0xA9,0x23, 0} /* ��� */,
	{0xAA,0xAA,0x23, 0} /* ��� */, {0xAB,0x32,0x31, 0} /* ��2 */,
	{0xAC,0x34,0x31, 0} /* ��4 */, {0xAD,0x21,0x23, 0} /* ��! */,
	{0xAE,0x3C,0x23, 0} /* ��< */, {0xAF,0x3E,0x23, 0} /* ��> */,
	{0xB0,0xB0,0x23, 1} /* ��� */, {0xB1,0xB1,0x23, 2} /* ��� */,
	{0xB2,0xB2,0x23, 3} /* ��� */, {0xB3,0xB3,0x23, 4} /* ��� */,
	{0xB4,0xB4,0x23, 5} /* ��� */, {0xB5,0xB5,0x23, 0} /* ��� */,
	{0xB6,0xB6,0x23, 0} /* ��� */, {0xB7,0xB7,0x23, 0} /* ��� */,
	{0xB8,0xB8,0x23, 0} /* ��� */, {0xB9,0xB9,0x23, 0} /* ��� */,
	{0xBA,0xBA,0x23, 0} /* ��� */, {0xBB,0xBB,0x23, 0} /* ��� */,
	{0xBC,0xBC,0x23, 0} /* ��� */, {0xBD,0xBD,0x23, 0} /* ��� */,
	{0xBE,0xBE,0x23, 0} /* ��� */, {0xBF,0xBF,0x23, 0} /* ��� */,
	{0xC0,0xC0,0x23, 6} /* ��� */, {0xC1,0xC1,0x23, 7} /* ��� */,
	{0xC2,0xC2,0x23, 8} /* ��� */, {0xC3,0xC3,0x23, 9} /* ��� */,
	{0xC4,0xC4,0x23,10} /* ��� */, {0xC5,0xC5,0x23,11} /* ��� */,
	{0xC6,0xC6,0x23,12} /* ��� */, {0xC7,0xC7,0x23,13} /* ��� */,
	{0xC8,0xC8,0x23,14} /* ��� */, {0xC9,0xC9,0x23,15} /* ��� */,
	{0xCA,0xCA,0x23,16} /* ��� */, {0xCB,0xCB,0x23, 0} /* ��� */,
	{0xCC,0xCC,0x23, 0} /* ��� */, {0xCD,0xCD,0x23, 0} /* ��� */,
	{0xCE,0xCE,0x23, 0} /* ��� */, {0xCF,0xCF,0x23, 0} /* ��� */,
	{0xD0,0xD0,0x23, 0} /* ��� */, {0xD1,0xD1,0x23, 0} /* ��� */,
	{0xD2,0xD2,0x23, 0} /* ��� */, {0xD3,0xD3,0x23, 0} /* ��� */,
	{0xD4,0xD4,0x23, 0} /* ��� */, {0xD5,0xD5,0x23, 0} /* ��� */,
	{0xD6,0xD6,0x23, 0} /* ��� */, {0xD7,0xD7,0x23, 0} /* ��� */,
	{0xD8,0xD8,0x23, 0} /* ��� */, {0xD9,0xD9,0x23, 0} /* ��� */,
	{0xDA,0xDA,0x23, 0} /* ��� */, {0xDB,0xDB,0x23, 0} /* ��� */,
	{0xDC,0xDC,0x23, 0} /* ��� */, {0xDD,0xDD,0x23, 0} /* ��� */,
	{0xDE,0xDE,0x23, 0} /* ��� */, {0xDF,0xDF,0x23, 0} /* ��� */,
	{0xE0,0x61,0x61, 0} /* ��a */, {0xE1,0x62,0x61, 0} /* ��b */,
	{0xE2,0x67,0x61, 0} /* ��g */, {0xE3,0x70,0x61, 0} /* ��p */,
	{0xE4,0x73,0x61, 0} /* ��s */, {0xE5,0x73,0x61, 0} /* ��s */,
	{0xE6,0x6D,0x61, 0} /* ��m */, {0xE7,0x74,0x61, 0} /* ��t */,
	{0xE8,0x70,0x61, 0} /* ��p */, {0xE9,0x74,0x61, 0} /* ��t */,
	{0xEA,0x64,0x61, 0} /* ��d */, {0xEB,0x64,0x61, 0} /* ��d */,
	{0xEC,0x6F,0x61, 0} /* ��o */, {0xED,0x70,0x61, 0} /* ��p */,
	{0xEE,0x65,0x61, 0} /* ��e */, {0xEF,0xEF,0x23, 0} /* ��� */,
	{0xF0,0xF0,0x23, 0} /* ��� */, {0xF1,0x2B,0x23, 0} /* ��+ */,
	{0xF2,0x3E,0x23, 0} /* ��> */, {0xF3,0x3C,0x23, 0} /* ��< */,
	{0xF4,0xF4,0x23, 0} /* ��� */, {0xF5,0xF5,0x23, 0} /* ��� */,
	{0xF6,0xF6,0x23, 0} /* ��� */, {0xF7,0xF7,0x23, 0} /* ��� */,
	{0xF8,0xF8,0x23, 0} /* ��� */, {0xF9,0xF9,0x23, 0} /* ��� */,
	{0xFA,0xFA,0x23, 0} /* ��� */, {0xFB,0xFB,0x23, 0} /* ��� */,
	{0xFC,0xFC,0x61, 0} /* ��� */, {0xFD,0x32,0x31, 0} /* ��2 */,
	{0xFE,0xFE,0x23, 0} /* ��� */, {0xFF,0xFF,0x23, 0} /* ��� */,
	{0, 0, 0, 437}      /*** CODEPAGE IDENTIFICATION NUMBER ***/
},

{	/* CODEPAGE 850 ( intern CP[1] ) */

	/* The 128 characters above 7Fh of codepage 850 are */
	/* ���������������������������������������������������������������� */
	/* ���������������������������������������������������������������� */

  	{0x00,0x00,0x00, 0} /* ... */, {0x01,0x01,0x01, 0} /* ... */,
	{0x02,0x02,0x02, 0} /* ... */, {0x03,0x03,0x03, 0} /* ... */,
	{0x04,0x04,0x04, 0} /* ... */, {0x05,0x05,0x05, 0} /* ... */,
	{0x06,0x06,0x06, 0} /* ... */, {0x07,0x07,0x07, 0} /* ... */,
	{0x08,0x08,0x08, 0} /* ... */, {0x09,0x09,0x09, 0} /* ... */,
	{0x0A,0x0A,0x0A, 0} /* ... */, {0x0B,0x0B,0x0B, 0} /* ... */,
	{0x0C,0x0C,0x0C, 0} /* ... */, {0x0D,0x0D,0x0D, 0} /* ... */,
	{0x0E,0x0E,0x0E, 0} /* ... */, {0x0F,0x0F,0x0F, 0} /* ... */,
	{0x10,0x10,0x10, 0} /* ... */, {0x11,0x11,0x11, 0} /* ... */,
	{0x12,0x12,0x12, 0} /* ... */, {0x13,0x13,0x13, 0} /* ... */,
	{0x14,0x14,0x14, 0} /* ... */, {0x15,0x15,0x15, 0} /* ... */,
	{0x16,0x16,0x16, 0} /* ... */, {0x17,0x17,0x17, 0} /* ... */,
	{0x18,0x18,0x18, 0} /* ... */, {0x19,0x19,0x19, 0} /* ... */,
	{0x1A,0x1A,0x1A, 0} /* ... */, {0x1B,0x1B,0x1B, 0} /* ... */,
	{0x1C,0x1C,0x1C, 0} /* ... */, {0x1D,0x1D,0x1D, 0} /* ... */,
	{0x1E,0x1E,0x1E, 0} /* ... */, {0x1F,0x1F,0x1F, 0} /* ... */,
	{0x20,0x20,0x23, 0} /*     */, {0x21,0x21,0x23, 0} /* !!! */,
	{0x22,0x22,0x23, 0} /* """ */, {0x23,0x23,0x23, 0} /* ### */,
	{0x24,0x24,0x23, 0} /* $$$ */, {0x25,0x25,0x23, 0} /* %%% */,
	{0x26,0x26,0x23, 0} /* &&& */, {0x27,0x27,0x23, 0} /* ''' */,
	{0x28,0x28,0x23, 0} /* ((( */, {0x29,0x29,0x23, 0} /* ))) */,
	{0x2A,0x2A,0x23, 0} /* *** */, {0x2B,0x2B,0x23, 0} /* +++ */,
	{0x2C,0x2C,0x23, 0} /* ,,, */, {0x2D,0x2D,0x23, 0} /* --- */,
	{0x2E,0x2E,0x23, 0} /* ... */, {0x2F,0x2F,0x23, 0} /* /// */,
	{0x30,0x30,0x31, 0} /* 000 */, {0x31,0x31,0x31, 0} /* 111 */,
	{0x32,0x32,0x31, 0} /* 222 */, {0x33,0x33,0x31, 0} /* 333 */,
	{0x34,0x34,0x31, 0} /* 444 */, {0x35,0x35,0x31, 0} /* 555 */,
	{0x36,0x36,0x31, 0} /* 666 */, {0x37,0x37,0x31, 0} /* 777 */,
	{0x38,0x38,0x31, 0} /* 888 */, {0x39,0x39,0x31, 0} /* 999 */,
	{0x3A,0x3A,0x23, 0} /* ::: */, {0x3B,0x3B,0x23, 0} /* ;;; */,
	{0x3C,0x3C,0x23, 0} /* <<< */, {0x3D,0x3D,0x23, 0} /* === */,
	{0x3E,0x3E,0x23, 0} /* >>> */, {0x3F,0x3F,0x23, 0} /* ??? */,
	{0x40,0x40,0x23, 0} /* @@@ */, {0x61,0x61,0x61, 0} /* Aaa */,
	{0x62,0x62,0x61, 0} /* Bbb */, {0x63,0x63,0x61, 0} /* Ccc */,
	{0x64,0x64,0x61, 0} /* Ddd */, {0x65,0x65,0x61, 0} /* Eee */,
	{0x66,0x66,0x61, 0} /* Fff */, {0x67,0x67,0x61, 0} /* Ggg */,
	{0x68,0x68,0x61, 0} /* Hhh */, {0x69,0x69,0x61, 0} /* Iii */,
	{0x6A,0x6A,0x61, 0} /* Jjj */, {0x6B,0x6B,0x61, 0} /* Kkk */,
	{0x6C,0x6C,0x61, 0} /* Lll */, {0x6D,0x6D,0x61, 0} /* Mmm */,
	{0x6E,0x6E,0x61, 0} /* Nnn */, {0x6F,0x6F,0x61, 0} /* Ooo */,
	{0x70,0x70,0x61, 0} /* Ppp */, {0x71,0x71,0x61, 0} /* Qqq */,
	{0x72,0x72,0x61, 0} /* Rrr */, {0x73,0x73,0x61, 0} /* Sss */,
	{0x74,0x74,0x61, 0} /* Ttt */, {0x75,0x75,0x61, 0} /* Uuu */,
	{0x76,0x76,0x61, 0} /* Vvv */, {0x77,0x77,0x61, 0} /* Www */,
	{0x78,0x78,0x61, 0} /* Xxx */, {0x79,0x79,0x61, 0} /* Yyy */,
	{0x7A,0x7A,0x61, 0} /* Zzz */, {0x5B,0x5B,0x23, 0} /* [[[ */,
	{0x5C,0x5C,0x23, 0} /* \\\ */, {0x5D,0x5D,0x23, 0} /* ]]] */,
	{0x5E,0x5E,0x23, 0} /* ^^^ */, {0x5F,0x5F,0x23, 0} /* ___ */,
	{0x60,0x60,0x23, 0} /* ``` */, {0x61,0x61,0x61, 0} /* aaa */,
	{0x62,0x62,0x61, 0} /* bbb */, {0x63,0x63,0x61, 0} /* ccc */,
	{0x64,0x64,0x61, 0} /* ddd */, {0x65,0x65,0x61, 0} /* eee */,
	{0x66,0x66,0x61, 0} /* fff */, {0x67,0x67,0x61, 0} /* ggg */,
	{0x68,0x68,0x61, 0} /* hhh */, {0x69,0x69,0x61, 0} /* iii */,
	{0x6A,0x6A,0x61, 0} /* jjj */, {0x6B,0x6B,0x61, 0} /* kkk */,
	{0x6C,0x6C,0x61, 0} /* lll */, {0x6D,0x6D,0x61, 0} /* mmm */,
	{0x6E,0x6E,0x61, 0} /* nnn */, {0x6F,0x6F,0x61, 0} /* ooo */,
	{0x70,0x70,0x61, 0} /* ppp */, {0x71,0x71,0x61, 0} /* qqq */,
	{0x72,0x72,0x61, 0} /* rrr */, {0x73,0x73,0x61, 0} /* sss */,
	{0x74,0x74,0x61, 0} /* ttt */, {0x75,0x75,0x61, 0} /* uuu */,
	{0x76,0x76,0x61, 0} /* vvv */, {0x77,0x77,0x61, 0} /* www */,
	{0x78,0x78,0x61, 0} /* xxx */, {0x79,0x79,0x61, 0} /* yyy */,
	{0x7A,0x7A,0x61, 0} /* zzz */, {0x7B,0x7B,0x23, 0} /* {{{ */,
	{0x7C,0x7C,0x23, 0} /* ||| */, {0x7D,0x7D,0x23, 0} /* }}} */,
	{0x7E,0x7E,0x23, 0} /* ~~~ */, {0x7F,0x7F,0x23, 0} /*  */,
	{0x87,0x63,0x61, 0} /* ��c */, {0x81,0x75,0x61, 0} /* ��u */,
	{0x82,0x65,0x61, 0} /* ��e */, {0x83,0x61,0x61, 0} /* ��a */,
	{0x84,0x61,0x61, 0} /* ��a */, {0x85,0x61,0x61, 0} /* ��a */,
	{0x86,0x61,0x61, 0} /* ��a */, {0x87,0x63,0x61, 0} /* ��c */,
	{0x88,0x65,0x61, 0} /* ��e */, {0x89,0x65,0x61, 0} /* ��e */,
	{0x8A,0x65,0x61, 0} /* ��e */, {0x8B,0x69,0x61, 0} /* ��i */,
	{0x8C,0x69,0x61, 0} /* ��i */, {0x8D,0x69,0x61, 0} /* ��i */,
	{0x84,0x61,0x61, 0} /* ��a */, {0x86,0x61,0x61, 0} /* ��a */,
	{0x82,0x65,0x61, 0} /* ��e */, {0x91,0x61,0x61, 0} /* ��a */,
	{0x91,0x61,0x61, 0} /* ��a */, {0x93,0x6F,0x61, 0} /* ��o */,
	{0x94,0x6F,0x61, 0} /* ��o */, {0x95,0x6F,0x61, 0} /* ��o */,
	{0x96,0x75,0x61, 0} /* ��u */, {0x97,0x75,0x61, 0} /* ��u */,
	{0x98,0x79,0x61, 0} /* ��y */, {0x94,0x6F,0x61, 0} /* ��o */,
	{0x81,0x75,0x61, 0} /* ��u */, {0x9B,0x6F,0x61, 0} /* ��o */,
	{0x9C,0x62,0x23, 0} /* ��b */, {0x9D,0x6F,0x61, 0} /* ��o */,
	{0x9E,0x78,0x23, 0} /* ��x */, {0x9F,0x66,0x23, 0} /* ��f */,
	{0xA0,0x61,0x61, 0} /* ��a */, {0xA1,0x69,0x61, 0} /* ��i */,
	{0xA2,0x6F,0x61, 0} /* ��o */, {0xA3,0x75,0x61, 0} /* ��u */,
	{0xA4,0x6E,0x61, 0} /* ��n */, {0xA4,0x6E,0x61, 0} /* ��n */,
	{0xA6,0x61,0x61, 0} /* ��a */, {0xA7,0x6F,0x61, 0} /* ��o */,
	{0xA8,0x3F,0x23, 0} /* ��? */, {0xA9,0x72,0x23, 0} /* ��r */,
	{0xAA,0x23,0x23, 0} /* ��. */, {0xAB,0x32,0x31, 0} /* ��2 */,
	{0xAC,0x34,0x31, 0} /* ��4 */, {0xAD,0x21,0x23, 0} /* ��. */,
	{0xAE,0x3C,0x23, 0} /* ��< */, {0xAF,0x3E,0x23, 0} /* ��> */,
	{0xB0,0xB0,0x23, 1} /* ��. */, {0xB1,0xB1,0x23, 2} /* ��. */,
	{0xB2,0xB2,0x23, 3} /* ��. */, {0xB3,0xB3,0x23, 0} /* ��. */,
	{0xB4,0xB4,0x23, 4} /* ��. */, {0xA0,0x61,0x61, 0} /* ��a */,
	{0x83,0x61,0x61, 0} /* ��a */, {0x85,0x61,0x61, 0} /* ��a */,
	{0xB8,0x63,0x23, 0} /* ��c */, {0xB9,0xB9,0x23, 0} /* ��. */,
	{0xBA,0xBA,0x23, 0} /* ��. */, {0xBB,0xBB,0x23, 0} /* ��. */,
	{0xBC,0xBC,0x23, 0} /* ��. */, {0xBD,0xBD,0x23, 0} /* ��c */,
	{0xBE,0x79,0x23, 0} /* ��y */, {0xBF,0xBF,0x23, 0} /* ��. */,
	{0xC0,0xC0,0x23, 5} /* ��� */, {0xC1,0xC1,0x23, 6} /* ��� */,
	{0xC2,0xC2,0x23, 7} /* ��� */, {0xC3,0xC3,0x23, 8} /* ��� */,
	{0xC4,0xC4,0x23, 9} /* ��� */, {0xC5,0xC5,0x23,10} /* ��� */,
	{0xC6,0x61,0x61, 0} /* ��a */, {0xC6,0x61,0x61, 0} /* ��a */,
	{0xC8,0xC8,0x23,11} /* ��� */, {0xC9,0xC9,0x23,12} /* ��� */,
	{0xCA,0xCA,0x23,13} /* ��� */, {0xCB,0xCB,0x23,14} /* ��� */,
	{0xCC,0xCC,0x23,15} /* ��� */, {0xCD,0xCD,0x23,16} /* ��� */,
	{0xCE,0xCE,0x23, 0} /* ��� */, {0xCF,0x6F,0x23, 0} /* ��o */,
	{0xD0,0x64,0x23, 0} /* ��d */, {0xD0,0x64,0x23, 0} /* ��d */,
	{0x88,0x65,0x61, 0} /* ҈e */, {0x89,0x65,0x61, 0} /* Ӊe */,
	{0x8A,0x65,0x61, 0} /* Ԋe */, {0xD5,0x69,0x61, 0} /* ��i */,
	{0xA1,0x69,0x61, 0} /* ֡i */, {0x8C,0x69,0x61, 0} /* ׌i */,
	{0x8B,0x69,0x61, 0} /* ؋i */, {0xD9,0xD9,0x23, 0} /* ��� */,
	{0xDA,0xDA,0x23, 0} /* ��� */, {0xDB,0xDB,0x23, 0} /* ��� */,
	{0xDC,0xDC,0x23, 0} /* ��� */, {0xDD,0xDD,0x23, 0} /* ��� */,
	{0x8D,0x69,0x61, 0} /* ލi */, {0xDF,0xDF,0x23, 0} /* ��� */,
	{0xA2,0x6F,0x61, 0} /* �o */, {0xE1,0x73,0x61, 0} /* ��s */,
	{0x93,0x6F,0x61, 0} /* �o */, {0x95,0x6F,0x61, 0} /* �o */,
	{0xE4,0x6F,0x61, 0} /* ��o */, {0xE4,0x6F,0x61, 0} /* ��o */,
	{0xE6,0x6D,0x61, 0} /* ��m */, {0xE7,0x70,0x23, 0} /* ��p */,
	{0xE8,0x70,0x23, 0} /* ��p */, {0xA3,0x75,0x61, 0} /* �u */,
	{0x96,0x75,0x61, 0} /* �u */, {0x97,0x75,0x61, 0} /* �u */,
	{0xEC,0x79,0x61, 0} /* ��y */, {0xEC,0x79,0x61, 0} /* ��y */,
	{0xEE,0xEE,0x23, 0} /* ��� */, {0xEF,0xEF,0x23, 0} /* ��� */,
	{0xF0,0xF0,0x23, 0} /* ��� */, {0xF1,0xF1,0x23, 0} /* ��� */,
	{0xF2,0xF2,0x23, 0} /* ��� */, {0xF3,0x33,0x31, 0} /* ��3 */,
	{0xF4,0x70,0x23, 0} /* ��p */, {0xF5,0xF5,0x23, 0} /* ��� */,
	{0xF6,0xF6,0x23, 0} /* ��� */, {0xF7,0xF7,0x23, 0} /* ��� */,
	{0xF8,0xF8,0x23, 0} /* ��� */, {0xF9,0xF9,0x23, 0} /* ��� */,
	{0xFA,0xFA,0x23, 0} /* ��� */, {0xFB,0x31,0x31, 0} /* ��1 */,
	{0xFC,0x33,0x31, 0} /* ��3 */, {0xFD,0x32,0x31, 0} /* ��2 */,
	{0xFE,0xFE,0x23, 0} /* ��� */, {0xFF,0xFF,0x23, 0} /* ��� */,
	{0, 0, 0, 850}      /*** CODEPAGE IDENTIFICATION NUMBER ***/
},

{	/* CODEPAGE ISO 8859-1 ( intern CP[2] ) */

	/* The 128 characters above 7Fh of codepage 850 are */
	/* ���������������������������������������������������������������� */
	/* ���������������������������������������������������������������� */

  	{0x00,0x00,0x00, 0} /* ... */, {0x01,0x01,0x01, 0} /* ... */,
	{0x02,0x02,0x02, 0} /* ... */, {0x03,0x03,0x03, 0} /* ... */,
	{0x04,0x04,0x04, 0} /* ... */, {0x05,0x05,0x05, 0} /* ... */,
	{0x06,0x06,0x06, 0} /* ... */, {0x07,0x07,0x07, 0} /* ... */,
	{0x08,0x08,0x08, 0} /* ... */, {0x09,0x09,0x09, 0} /* ... */,
	{0x0A,0x0A,0x0A, 0} /* ... */, {0x0B,0x0B,0x0B, 0} /* ... */,
	{0x0C,0x0C,0x0C, 0} /* ... */, {0x0D,0x0D,0x0D, 0} /* ... */,
	{0x0E,0x0E,0x0E, 0} /* ... */, {0x0F,0x0F,0x0F, 0} /* ... */,
	{0x10,0x10,0x10, 0} /* ... */, {0x11,0x11,0x11, 0} /* ... */,
	{0x12,0x12,0x12, 0} /* ... */, {0x13,0x13,0x13, 0} /* ... */,
	{0x14,0x14,0x14, 0} /* ... */, {0x15,0x15,0x15, 0} /* ... */,
	{0x16,0x16,0x16, 0} /* ... */, {0x17,0x17,0x17, 0} /* ... */,
	{0x18,0x18,0x18, 0} /* ... */, {0x19,0x19,0x19, 0} /* ... */,
	{0x1A,0x1A,0x1A, 0} /* ... */, {0x1B,0x1B,0x1B, 0} /* ... */,
	{0x1C,0x1C,0x1C, 0} /* ... */, {0x1D,0x1D,0x1D, 0} /* ... */,
	{0x1E,0x1E,0x1E, 0} /* ... */, {0x1F,0x1F,0x1F, 0} /* ... */,
	{0x20,0x20,0x23, 0} /*   # */, {0x21,0x21,0x23, 0} /* !!# */,
	{0x22,0x22,0x23, 0} /* ""# */, {0x23,0x23,0x23, 0} /* ### */,
	{0x24,0x24,0x23, 0} /* $$# */, {0x25,0x25,0x23, 0} /* %%# */,
	{0x26,0x26,0x23, 0} /* &&# */, {0x27,0x27,0x23, 0} /* ''# */,
	{0x28,0x28,0x23, 0} /* ((# */, {0x29,0x29,0x23, 0} /* ))# */,
	{0x2A,0x2A,0x23, 0} /* **# */, {0x2B,0x2B,0x23, 0} /* ++# */,
	{0x2C,0x2C,0x23, 0} /* ,,# */, {0x2D,0x2D,0x23, 0} /* --# */,
	{0x2E,0x2E,0x23, 0} /* ..# */, {0x2F,0x2F,0x23, 0} /* //# */,
	{0x30,0x30,0x31, 0} /* 001 */, {0x31,0x31,0x31, 0} /* 111 */,
	{0x32,0x32,0x31, 0} /* 221 */, {0x33,0x33,0x31, 0} /* 331 */,
	{0x34,0x34,0x31, 0} /* 441 */, {0x35,0x35,0x31, 0} /* 551 */,
	{0x36,0x36,0x31, 0} /* 661 */, {0x37,0x37,0x31, 0} /* 771 */,
	{0x38,0x38,0x31, 0} /* 881 */, {0x39,0x39,0x31, 0} /* 991 */,
	{0x3A,0x3A,0x23, 0} /* ::# */, {0x3B,0x3B,0x23, 0} /* ;;# */,
	{0x3C,0x3C,0x23, 0} /* <<# */, {0x3D,0x3D,0x23, 0} /* ==# */,
	{0x3E,0x3E,0x23, 0} /* >># */, {0x3F,0x3F,0x23, 0} /* ??# */,
	{0x40,0x40,0x23, 0} /* @@# */, {0x61,0x61,0x61, 0} /* Aaa */,
	{0x62,0x62,0x61, 0} /* Bba */, {0x63,0x63,0x61, 0} /* Cca */,
	{0x64,0x64,0x61, 0} /* Dda */, {0x65,0x65,0x61, 0} /* Eea */,
	{0x66,0x66,0x61, 0} /* Ffa */, {0x67,0x67,0x61, 0} /* Gga */,
	{0x68,0x68,0x61, 0} /* Hha */, {0x69,0x69,0x61, 0} /* Iia */,
	{0x6A,0x6A,0x61, 0} /* Jja */, {0x6B,0x6B,0x61, 0} /* Kka */,
	{0x6C,0x6C,0x61, 0} /* Lla */, {0x6D,0x6D,0x61, 0} /* Mma */,
	{0x6E,0x6E,0x61, 0} /* Nna */, {0x6F,0x6F,0x61, 0} /* Ooa */,
	{0x70,0x70,0x61, 0} /* Ppa */, {0x71,0x71,0x61, 0} /* Qqa */,
	{0x72,0x72,0x61, 0} /* Rra */, {0x73,0x73,0x61, 0} /* Ssa */,
	{0x74,0x74,0x61, 0} /* Tta */, {0x75,0x75,0x61, 0} /* Uua */,
	{0x76,0x76,0x61, 0} /* Vva */, {0x77,0x77,0x61, 0} /* Wwa */,
	{0x78,0x78,0x61, 0} /* Xxa */, {0x79,0x79,0x61, 0} /* Yya */,
	{0x7A,0x7A,0x61, 0} /* Zza */, {0x5B,0x5B,0x23, 0} /* [[# */,
	{0x5C,0x5C,0x23, 0} /* \\# */, {0x5D,0x5D,0x23, 0} /* ]]# */,
	{0x5E,0x5E,0x23, 0} /* ^^# */, {0x5F,0x5F,0x23, 0} /* __# */,
	{0x60,0x60,0x23, 0} /* ``# */, {0x61,0x61,0x61, 0} /* aaa */,
	{0x62,0x62,0x61, 0} /* bba */, {0x63,0x63,0x61, 0} /* cca */,
	{0x64,0x64,0x61, 0} /* dda */, {0x65,0x65,0x61, 0} /* eea */,
	{0x66,0x66,0x61, 0} /* ffa */, {0x67,0x67,0x61, 0} /* gga */,
	{0x68,0x68,0x61, 0} /* hha */, {0x69,0x69,0x61, 0} /* iia */,
	{0x6A,0x6A,0x61, 0} /* jja */, {0x6B,0x6B,0x61, 0} /* kka */,
	{0x6C,0x6C,0x61, 0} /* lla */, {0x6D,0x6D,0x61, 0} /* mma */,
	{0x6E,0x6E,0x61, 0} /* nna */, {0x6F,0x6F,0x61, 0} /* ooa */,
	{0x70,0x70,0x61, 0} /* ppa */, {0x71,0x71,0x61, 0} /* qqa */,
	{0x72,0x72,0x61, 0} /* rra */, {0x73,0x73,0x61, 0} /* ssa */,
	{0x74,0x74,0x61, 0} /* tta */, {0x75,0x75,0x61, 0} /* uua */,
	{0x76,0x76,0x61, 0} /* vva */, {0x77,0x77,0x61, 0} /* wwa */,
	{0x78,0x78,0x61, 0} /* xxa */, {0x79,0x79,0x61, 0} /* yya */,
	{0x7A,0x7A,0x61, 0} /* zza */, {0x7B,0x7B,0x23, 0} /* {{# */,
	{0x7C,0x7C,0x23, 0} /* ||# */, {0x7D,0x7D,0x23, 0} /* }}# */,
	{/*0x7E*/ 0x7E,0x7E,0x23, 0} /* ~~# */, {/*0x7F*/ 0x7F,0x7F,0x23, 0} /*    #*/,
	{/*0x80*/ 0x87,0x65,0x61, 0} /* �ea */, {/*0x81*/ 0x81,0x75,0x23, 0} /* ��  */,
	{/*0x82*/ 0x82,0x65,0x23, 0} /* �   */, {/*0x83*/ 0x66,0x66,0x61, 1} /* �   */,
	{/*0x84*/ 0x84,0x61,0x23, 0} /* �   */, {/*0x85*/ 0x85,0x61,0x23, 2} /* �   */,
	{/*0x86*/ 0x86,0x2B,0x23, 0} /* �+# */, {/*0x87*/ 0x87,0x63,0x23, 3} /* �   */,
	{/*0x88*/ 0x88,0x5E,0x23, 0} /* �^# */, {/*0x89*/ 0x89,0x65,0x23, 4} /* �   */,
	{/*0x8A*/ 0x9A,0x73,0x61, 0} /* �sa */, {/*0x8B*/ 0x8B,0x69,0x23, 5} /* �   */,
	{/*0x8C*/ 0x9C,0x6F,0x61, 0} /* �oa */, {/*0x8D*/ 0x8D,0x69,0x23, 6} /* ��  */,
	{/*0x8E*/ 0x9E,0x7A,0x61, 0} /* �za */, {/*0x8F*/ 0x86,0x61,0x23, 7} /* �   */,
	{/*0x90*/ 0x82,0x82,0x00, 0} /* �   */, {/*0x91*/ 0x91,0x61,0x23, 8} /* �   */,
	{/*0x92*/ 0x91,0x27,0x23, 0} /* �'# */, {/*0x93*/ 0x93,0x6F,0x23, 9} /* �   */,
	{/*0x94*/ 0x94,0x22,0x23, 0} /* �"# */, {/*0x95*/ 0x95,0x2E,0x23,10} /* �   */,
	{/*0x96*/ 0x96,0x2D,0x23, 0} /* �-# */, {/*0x97*/ 0x97,0x2D,0x23,11} /* �-# */,
	{/*0x98*/ 0x98,0x7E,0x23, 0} /* ��# */, {/*0x99*/ 0x94,0x6D,0x61,12} /* �ma */,
	{/*0x9A*/ 0x9A,0x73,0x61, 0} /* �sa */, {/*0x9B*/ 0x9B,0x63,0x23,13} /* �   */,
	{/*0x9C*/ 0x9C,0x6F,0x61, 0} /* �oa */, {/*0x9D*/ 0x9D,0x79,0x23,14} /* ��  */,
	{/*0x9E*/ 0x9E,0x7A,0x61, 0} /* �za */, {/*0x9F*/ 0x9F,0x79,0x61, 0} /* �ya */,
	{/*0xA0*/ 0xA0,0x61,0x00, 0} /* ��  */, {/*0xA1*/ 0xA1,0x69,0x00,15} /* ��i */,
	{/*0xA2*/ 0xA2,0x63,0x61, 0} /* �ca */, {/*0xA3*/ 0xA3,0x6C,0x61, 0} /* �la */,
	{/*0xA4*/ 0xA4,0x6F,0x23, 0} /* �oa */, {/*0xA5*/ 0xA5,0x79,0x61, 0} /* �ya */,
	{/*0xA6*/ 0xA6,0x7C,0x23, 0} /* �|a */, {/*0xA7*/ 0xA7,0x6F,0x23, 0} /* ��# */,
	{/*0xA8*/ 0xA8,0x3F,0x23, 0} /* � # */, {/*0xA9*/ 0xA9,0x63,0x61, 0} /* �ca */,
	{/*0xAA*/ 0xAA,0x61,0x61, 0} /* �aa */, {/*0xAB*/ 0xAB,0x32,0x23, 0} /* ��# */,
	{/*0xAC*/ 0xAC,0x34,0x23, 0} /* ��# */, {/*0xAD*/ 0xAD,0x2D,0x23,16} /* �-# */,
	{/*0xAE*/ 0xAE,0x72,0x61, 0} /* �ra */, {/*0xAF*/ 0xAF,0x5F,0x23, 0} /* �_# */,
	{/*0xB0*/ 0xB0,0x30,0x31, 0} /* �01 */, {/*0xB1*/ 0xB1,0x23,0x23, 0} /* ��# */,
	{/*0xB2*/ 0xB2,0x32,0x31, 0} /* �21 */, {/*0xB3*/ 0xB3,0x73,0x61, 0} /* �sa */,
	{/*0xB4*/ 0xB4,0x27,0x23, 0} /* �'# */, {/*0xB5*/ 0xB5,0x6D,0x61, 0} /* ��� */,
	{/*0xB6*/ 0xB6,0x70,0x61, 0} /* �pa */, {/*0xB7*/ 0xB7,0x2E,0x23, 0} /* �.# */,
	{/*0xB8*/ 0xB8,0x2C,0x23, 0} /* �,# */, {/*0xB9*/ 0xB9,0x31,0x31, 0} /* �11 */,
	{/*0xBA*/ 0xBA,0x30,0x31, 0} /* �01 */, {/*0xBB*/ 0xBB,0xBB,0x23, 0} /* ��# */,
	{/*0xBC*/ 0xBC,0xBC,0x23, 0} /* ��# */, {/*0xBD*/ 0xBD,0xBD,0x31, 0} /* ��# */,
	{/*0xBE*/ 0xBE,0xBE,0x23, 0} /* ��# */, {/*0xBF*/ 0xBF,0xBF,0x23, 0} /* ��# */,
	{/*0xC0*/ 0xC0,0x61,0x61, 0} /* �aa */, {/*0xC1*/ 0xE1,0x61,0x61, 0} /* �aa */,
	{/*0xC2*/ 0xC2,0x61,0x61, 0} /* �aa */, {/*0xC3*/ 0xE3,0x61,0x61, 0} /* �aa */,
	{/*0xC4*/ 0xE4,0x61,0x61, 0} /* �aa */, {/*0xC5*/ 0xE5,0x61,0x61, 0} /* �aa */,
	{/*0xC6*/ 0xC6,0x61,0x61, 0} /* �aa */, {/*0xC7*/ 0xE7,0x63,0x61, 0} /* �ca */,
	{/*0xC8*/ 0xE8,0x65,0x61, 0} /* �ea */, {/*0xC9*/ 0xE9,0x65,0x61, 0} /* �ea */,
	{/*0xCA*/ 0xEA,0x65,0x61, 0} /* �ea */, {/*0xCB*/ 0xCB,0x65,0x61, 0} /* �ea */,
	{/*0xCC*/ 0xEC,0x69,0x61, 0} /* �ia */, {/*0xCD*/ 0xED,0x66,0x61, 0} /* �ia */,
	{/*0xCE*/ 0xEE,0x69,0x61, 0} /* �ia */, {/*0xCF*/ 0xEF,0x66,0x61, 0} /* �ia */,
	{/*0xD0*/ 0xD0,0x64,0x61, 0} /* �da */, {/*0xD1*/ 0xF1,0x6E,0x61, 0} /* �na */,
	{/*0xD2*/ 0xF2,0x6F,0x61, 0} /* �oa */, {/*0xD3*/ 0xF3,0x6F,0x61, 0} /* �oa */,
	{/*0xD4*/ 0xF4,0x6F,0x61, 0} /* �oa */, {/*0xD5*/ 0xF5,0x6F,0x61, 0} /* �oa */,
	{/*0xD6*/ 0xF6,0x6F,0x61, 0} /* �oa */, {/*0xD7*/ 0xD7,0x78,0x61, 0} /* �xa */,
	{/*0xD8*/ 0xF8,0x6F,0x61, 0} /* �oa */, {/*0xD9*/ 0xD9,0x75,0x61, 0} /* �ua */,
	{/*0xDA*/ 0xFA,0x75,0x61, 0} /* �ua */, {/*0xDB*/ 0xDB,0x75,0x61, 0} /* �ua */,
	{/*0xDC*/ 0xFC,0x75,0x61, 0} /* �ua */, {/*0xDD*/ 0xFD,0x79,0x61, 0} /* �ya */,
	{/*0xDE*/ 0xFE,0x70,0x61, 0} /* �pa */, {/*0xDF*/ 0xDF,0x73,0x61, 0} /* �sa */,
	{/*0xE0*/ 0xE0,0x61,0x61, 0} /* �aa */, {/*0xE1*/ 0xE1,0x61,0x61, 0} /* �aa */,
	{/*0xE2*/ 0xE2,0x61,0x61, 0} /* �aa */, {/*0xE3*/ 0xE3,0x61,0x61, 0} /* �aa */,
	{/*0xE4*/ 0xE4,0x61,0x61, 0} /* �aa */, {/*0xE5*/ 0xE5,0x61,0x61, 0} /* �aa */,
	{/*0xE6*/ 0xE6,0x61,0x61, 0} /* �aa */, {/*0xE7*/ 0xE7,0x63,0x61, 0} /* �ca */,
	{/*0xE8*/ 0xE8,0x65,0x61, 0} /* �ea */, {/*0xE9*/ 0xE9,0x65,0x61, 0} /* �ea */,
	{/*0xEA*/ 0xEA,0x65,0x61, 0} /* �ea */, {/*0xEB*/ 0xEB,0x65,0x61, 0} /* �ea */,
	{/*0xEC*/ 0xEC,0x66,0x61, 0} /* �ia */, {/*0xED*/ 0xED,0x66,0x61, 0} /* �ia */,
	{/*0xEE*/ 0xEE,0x66,0x61, 0} /* �ia */, {/*0xEF*/ 0xEF,0x66,0x61, 0} /* �ia */,
	{/*0xF0*/ 0xF0,0x64,0x61, 0} /* �da */, {/*0xF1*/ 0xF1,0x6E,0x61, 0} /* �na */,
	{/*0xF2*/ 0xF2,0x6F,0x61, 0} /* �oa */, {/*0xF3*/ 0xF3,0x6F,0x61, 0} /* �oa */,
	{/*0xF4*/ 0xF4,0x6F,0x61, 0} /* �oa */, {/*0xF5*/ 0xF5,0x6F,0x61, 0} /* �oa */,
	{/*0xF6*/ 0xF6,0x6F,0x61, 0} /* �oa */, {/*0xF7*/ 0xF7,0x3A,0x23, 0} /* �:# */,
	{/*0xF8*/ 0xF8,0x6F,0x61, 0} /* �oa */, {/*0xF9*/ 0xF9,0x75,0x61, 0} /* �ua */,
	{/*0xFA*/ 0xFA,0x75,0x61, 0} /* �ua */, {/*0xFB*/ 0xFB,0x75,0x61, 0} /* �ua */,
	{/*0xFC*/ 0xFC,0x75,0x61, 0} /* �ua */, {/*0xFD*/ 0xFD,0x79,0x61, 0} /* �ya */,
	{/*0xFE*/ 0xFE,0x70,0x61, 0} /* �pa */, {/*0xFF*/ 0xFF,0x79,0x61, 0} /* �ya */,
	{0, 0, 0, 8859 }    /*** CODEPAGE IDENTIFICATION NUMBER ***/
}

};
