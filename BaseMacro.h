#ifndef _BASE_MACRO_H_
#define _BASE_MACRO_H_

#include <windows.h>



#ifndef NULL
#define NULL 0
#endif


/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
     If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
     returns an empty list. */


/* Constants.

     Note: MAX_NN_DIGITS is long enough to hold any RSA modulus, plus
   one more digit as required by R_GeneratePEMKeys (for n and phiN,
   whose lengths must be even). All natural numbers have at most
   MAX_NN_DIGITS digits, except for double-length intermediate values
     in NN_Mult (t), NN_ModMult (t), and NN_Div (c).
*/

/* RSA key lengths. */
#define MAX_RSA_MODULUS_BITS 2048    // RSA算法支持的最大位数为2048位，即256字节

#define MAX_NN_LEN ((MAX_RSA_MODULUS_BITS+31)/32)

#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

/* Length of digit in bits */
#define NN_DIGIT_BITS 32
#define NN_HALF_DIGIT_BITS 16

/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS / 8)


/* Maximum length in digits */
#define MAX_NN_DIGITS \
  ((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN - 1) / NN_DIGIT_LEN + 1)

/* Maximum digits */
#define MAX_NN_DIGIT 0xffffffff
#define MAX_NN_HALF_DIGIT 0xffff

/* Macros. */
#define LOW_HALF(x) ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x) (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((DWORD)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_2MSB(x) (DWORD)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

/*Assigns a = b. */
void NN_Assign(DWORD *a, DWORD *b, DWORD digits);

/* Assigns a = 0. */
void NN_AssignZero(DWORD *a, DWORD digits);

/* Computes a = b - c. */
DWORD NN_Sub(DWORD *a, DWORD *b, DWORD *c, DWORD digits);

/* Computes a = b * c. */
void NN_Mult(DWORD *a, DWORD *b, DWORD *c, DWORD digits);

/* Computes a = c div d and b = c mod d. */
void NN_Div(DWORD *a, DWORD *b, DWORD *c, DWORD cDigits, DWORD *d, DWORD dDigits);

/* Computes a = b * 2^c. */
DWORD NN_LShift(DWORD *a, DWORD *b, DWORD c, DWORD digits);

/* Computes a = b / 2^c. */
DWORD NN_RShift(DWORD *a, DWORD *b, DWORD c, DWORD digits);


/* Computes a = b mod c. */
void NN_Mod(DWORD *a, DWORD *b, DWORD bDigits, DWORD *c, DWORD cDigits);

/* Computes a = b^c mod d. */
void NN_ModExp(DWORD *a, DWORD *b, DWORD *c, DWORD cDigits, DWORD *d, DWORD dDigits);

/*  Returns sign of a - b. */
int NN_Cmp(DWORD *a, DWORD *b, DWORD digits);

/* Returns significant length of a in digits. */
DWORD NN_Digits(DWORD *a, DWORD digits);


/*  Assigns a = b, where b is a digit. */
#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZero(a, digits); a[0] = b;}



//#include "global.h"

#define SHA_VERSION 1

//in PC or C51 it is 1234
#define SHA_BYTE_ORDER 1234
//in MC68Kvz328 it is 4321
//#define SHA_BYTE_ORDER 4321

/* NIST Secure Hash Algorithm */
/* heavily modified by Uwe Hollerbach <uh@alumni.caltech edu> */
/* from Peter C. Gutmann's implementation as found in */
/* Applied Cryptography by Bruce Schneier */

/* This code is in the public domain */

/* Useful defines & typedefs */
//typedef unsigned char BYTE;    /* 8-bit quantity */

#define SHA_BLOCKSIZE        64
#define SHA_DIGESTSIZE        20


typedef struct {
    unsigned long digest[5];        /* message digest */
    unsigned long count_lo, count_hi;    /* 64-bit bit count */
    BYTE data[SHA_BLOCKSIZE];    /* SHA data buffer */
    int local;            /* unprocessed amount in data */
} SHA_INFO;

void sha_init(SHA_INFO *);
void sha_update(SHA_INFO *, BYTE *, int);
void sha_final(BYTE [20], SHA_INFO *);

#endif

