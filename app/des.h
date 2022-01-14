#ifndef _DES_H
#define _DES_H


void des(unsigned char*, unsigned char*, unsigned char, unsigned char*);
//void FLASH_Read_KEYS(unsigned char key_index);
static void transpose (unsigned char*, unsigned char*, const unsigned char*, unsigned char);
static void rotate_l (unsigned char*);
static void compute_subkeys (unsigned char*);
static void f (unsigned char*, unsigned char*, unsigned char*);


#endif

