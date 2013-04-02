 /************************************************************
 * Filename : binary.cpp                                     *
 *			  All rights reserved                            *
 * Started  : 30-12-2004                                     *
 *			: $Date: 2005/06/02 19:14:40 $	$Revision: 1.3 $ *
 ************************************************************/

#include <windows.h>

#include "binary.h"

#define MAX_BINARY_STRING 1024

extern char aNumeric[] ; // ={"0123456789*U -)("};	// contains numeric paging data format

static char EightBits[MAX_BINARY_STRING] ;

static char *Convert7BitsTo8Bits(unsigned char *SevenBits, int len, unsigned char crypt, int bin)
{
	int num, i, tmp ;

	for(i = num = 0; i < len; i++, num++) {
		SevenBits[i] ^= crypt ;
		if(bin) {
			tmp = SevenBits[i/2] ;
			if(!(i % 2)) {
				tmp >>= 4 ;
			}
			tmp &=0x0F ;
			EightBits[i] = aNumeric[tmp] ; 
		}
		else {
			switch(i % 7) {
				case 0 : 
					EightBits[num] = (SevenBits[i] >> 1 & 0x7F) ;
					break ;
				case 1 : 
					EightBits[num] = (((SevenBits[i-1] & 0x01) << 6) & 0x40) + (SevenBits[i] >> 2 & 0x3F) ;
					break ;
				case 2 : 
					EightBits[num] = (((SevenBits[i-1] & 0x03) << 5) & 0x60) + (SevenBits[i] >> 3 & 0x1F) ;
					break ;
				case 3 : 
					EightBits[num] = (((SevenBits[i-1] & 0x07) << 4) & 0x70) + (SevenBits[i] >> 4 & 0x0F) ;
					break ;
				case 4 : 
					EightBits[num] = (((SevenBits[i-1] & 0x0F) << 3) & 0x78) + (SevenBits[i] >> 5 & 0x07) ;
					break ;
				case 5 : 
					EightBits[num] = (((SevenBits[i-1] & 0x1F) << 2) & 0x7C) + (SevenBits[i] >> 6 & 0x03) ;
					break ;
				case 6 : 
					EightBits[num] = (((SevenBits[i-1] & 0x3F) << 1) & 0x7E) + (SevenBits[i] >> 7 & 0x01) ;
					if(i < len-1) {
						num++ ;
						EightBits[num] = SevenBits[i] & 0x7F ;
					}
					break ;
			}
		}
	}
	EightBits[num] = 0 ;
	return(EightBits) ;
}


static int AsciiToHex(unsigned char *byDest, char *szStr, int nLen, int bSwap)
{
	int ch, b = 0, i = 0; 

	for(i = 0; i < nLen; i++) {
		switch(ch = szStr[i]) {
			case '0' : 
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
				b += ch - '0' ;
				break ;
			case 'a'  :
			case 'b'  :
			case 'c'  :
			case 'd'  :
			case 'e'  :
			case 'f'  :
				b += ch - 'W' ;
				break ;

			case 'A'  :
			case 'B'  :
			case 'C'  :
			case 'D'  :
			case 'E'  :
			case 'F'  :
				b += ch - '7' ;
				break ;
			default	  :
				return (0);
		}
		if(!(i % 2)) {
			b <<= 4 ;
		}
		else {
			if(bSwap) {
				b = ((b >> 4) & 0x0F) + ((b << 4) & 0xF0) ;
			}
			byDest[i / 2] = (unsigned char) b ;
			b = 0 ;
		}
	}
	if(b) {
		if(bSwap) {
			b = ((b >> 4) & 0x0F) + ((b << 4) & 0xF0) ;
		}
		byDest[i / 2] = (unsigned char) b ;
		b = 0 ;
	}
	return(i / 2 ) ;
}

char *DeCrlf(char *szStr)
{
	char *szTmp = szStr ;
	if(szTmp) {
		while(*szTmp) {
			if(*szTmp == '\n') {
				*szTmp = '»' ;
			}
			if(*szTmp == '\r') {
				*szTmp = '»' ;
			}
			szTmp++ ;
		}
	}
	return(szStr) ;
}

unsigned char array[MAX_BINARY_STRING] ;

// This function return the decrypted string or a NULL when wrong data is found
char *DeKroket(char *szData, char *szReturn)
{
    unsigned char by, by1, cal = 16, bin ;
    char *pData ;
    int len ;

	bin = (szData[11] == '0') ;
	if(szData[0] == '0' && szData[1] == 'F') {
		for(len = strlen(szData); len > 0, szData[len-1] == 'F';len--) szData[len-1] = '\0';
		if(AsciiToHex(&by, &szData[8], 2, 1)) {
			if(AsciiToHex(&by1, &szData[14], 2, 1)) {
				if(by1 >= 0x80) cal = 18 ;
	            if(len = AsciiToHex(array, szData + cal, min(MAX_BINARY_STRING, strlen(szData) - cal), 1)) {
	                pData = DeCrlf(Convert7BitsTo8Bits(array, len, by, bin)) ;
	                strcpy(szReturn, pData) ;
	                return(szReturn) ;		
				}
			}
		}
	}
	return(NULL) ;
}
