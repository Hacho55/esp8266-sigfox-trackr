/* ======================================================================
    This file is part of disk91_tools.

    disk91_tools is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
  =======================================================================
 */
/* ======================================================================
 *  ESP8266 Misc tooling functions
 * ----------------------------------------------------------------------
 * (c) Disk91 - 2018
 * Author : Paul Pinault aka disk91.com
 * ----------------------------------------------------------------------
 */

#include <Arduino.h>

/**
 * Calculate a CRC32 on a memory zone
 */
uint32_t calculateCRC32(const uint8_t *data, size_t length) {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }
      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

// =======================================================================================
// Converters
// =======================================================================================

/* -----------------------------------------------------------
 * Convert a 0-16 value to a upper/lower Char
 */
char dsk_convertHalfInt2HexChar(uint8_t v,bool upper) {
  if ( v >= 0 && v <= 9 ) return '0'+v;
  if ( v >= 10 && v <= 15 ) return (upper)?'A'+(v-10):'a'+(v+10);
  return 0;
}

/* -----------------------------------------------------------
 * Convert a 0-F char to a 0-16 value
 */
uint8_t dsk_convertHexChar2HalfInt(char c) {
  if ( c >= '0' && c <= '9' ) return c-'0';
  if ( c >= 'a' && c <= 'f' ) return 10+c-'a';
  if ( c >= 'A' && c <= 'F' ) return 10+c-'A';
  return 0;
}


/* -----------------------------------------------------------
 * Convert a 0-256 value to a 2 byte upper/lower case string
 */
void dsk_convertInt2HexChar(uint8_t v, char * dest, bool upper) {
  uint8_t q0 = (v & 0xF0) >> 4;
  uint8_t q1 = (v & 0x0F);
  dest[0] = dsk_convertHalfInt2HexChar(q0,upper);
  dest[1] = dsk_convertHalfInt2HexChar(q1,upper);
}

/* -----------------------------------------------------------
 * Convert a "0"-"FF" value to 0-255 uint8_t value
 */
uint8_t dsk_convertHexChar2Int(char * v) {
  uint8_t q0 = dsk_convertHexChar2HalfInt(v[0]);
  uint8_t q1 = dsk_convertHexChar2HalfInt(v[1]);
  return (q0*16)+q1;
}

/* -----------------------------------------------------------
 *  Convert a 32bit hex string value into uint32_t value
 */
uint32_t dsk_convertHexChar8Int(char * v) {
  uint32_t ret = dsk_convertHexChar2Int(&v[0]);
  ret <<= 8;
  ret += dsk_convertHexChar2Int(&v[2]);
  ret <<= 8;
  ret += dsk_convertHexChar2Int(&v[4]);
  ret <<= 8;
  ret += dsk_convertHexChar2Int(&v[6]);
  return ret;
}

/* -----------------------------------------------------------
 *  Convert a 4 char decimal (+ sign) string value into uint16_t value
 */
int16_t dsk_convertDecChar4Int(char * v) {
  int sign = 1;
  if ( *v == '-' ) {
     sign = -1;
     v++;
  }
  uint16_t ret = dsk_convertHexChar2HalfInt(*v);v++;
  ret *= 10;
  ret += dsk_convertHexChar2HalfInt(*v);v++;
  ret *= 10;
  ret += dsk_convertHexChar2HalfInt(*v);v++;
  ret *= 10;
  ret += dsk_convertHexChar2HalfInt(*v);v++;
  ret *=sign;
  return ret;
}

/* -----------------------------------------------------------
 * Convert a 8bytes table to Upper Hex upper/lower string
 */
void dsk_convertIntTab2Hex(char * dest, uint8_t * tab, int len, bool upper) {
  int i;
  for ( i = 0; i < len ; i++ ) {
    dsk_convertInt2HexChar(tab[i],&dest[2*i],upper);
  }
  dest[2*len]='\0';
}

/* ----------------------------------------------------------
 * Convert a Char String to a hex value tab entries
 */
void dsk_convertHexStr2IntTab(char * hexstr,uint8_t * tab, int len) {

  int i;
  for ( i = 0; i < len ; i++ ) {
    tab[i] = dsk_convertHexChar2Int(&hexstr[2*i]);
  }

}

/* ----------------------------------------------------------
 * Verify a char is an Hex Char
 */
bool dsk_isHexChar(char c, bool upper) {
  if (    (c >= '0' && c <= '9' )
       || (c >= 'A' && c <= 'F' )
     || (!upper   && c >= 'a' && c <= 'f')
    ) {
    return true;
  }
  return false;
}

/* ----------------------------------------------------------
 * Verify a string is a valid Hex string with given size
 */
bool dsk_isHexString(char * str,int n,bool upper) {
  int i = 0;
  while ( i < n && str[i] != 0 ) {
    if (    (str[i] >= '0' && str[i] <= '9' )
       || (str[i] >= 'A' && str[i] <= 'F' )
       || (!upper && str[i] >= 'a' && str[i] <= 'f')
       ) {
      i++;
    } else {
      return false;
    }
  }
  return ( i == n )?true:false;
}

/* ----------------------------------------------------
 *  Convert a 6 x uint8_t mac data into a String
 *  XX:XX:XX:XX:XX:XX into the str buffer. The str
 *  buffer size must be 18 bytes long or more
 */
void dsk_macToString(char * str, uint8_t * mac) {
  for ( int j = 0 ; j < 6 ; j++ ) {
     dsk_convertInt2HexChar(mac[j],&str[3*j],true);
     if ( j < 5 ) str[3*j+2]=':';
  }
  str[17]='\0';
}

