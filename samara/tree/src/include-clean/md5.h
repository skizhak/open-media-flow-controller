/*
 *
 * md5.h
 *
 *
 *
 */

#ifndef __MD5_H_
#define __MD5_H_


#ifdef __cplusplus
extern "C" {
#endif
#if 0 /* for emacs */
}
#endif

/**
 * \file src/include/md5.h Routines for computing MD5 message digests.
 * \ingroup lc
 */

/*	$NetBSD: md5.h,v 1.2 1997/04/30 00:50:10 thorpej Exp $	*/

/*
 * This file is derived from the RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm and has been modified by Jason R. Thorpe <thorpej@NetBSD.ORG>
 * for portability and formatting.
 */

/*
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 * 
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 * 
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 * 
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#include "common.h"

/* MD5 context. */
typedef struct MD5Context {
	uint32 state[4];	/* state (ABCD) */
	uint32 count[2];	/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64]; /* input buffer */
} MD5_CTX;

void	MD5Init(MD5_CTX *);
void	MD5Update(MD5_CTX *, const unsigned char *, unsigned int);
void	MD5Final(unsigned char[16], MD5_CTX *);
char	*MD5End(MD5_CTX *, char *);
char	*MD5File(const char *, char *);
char	*MD5Data(const unsigned char *, unsigned int, char *);


#ifdef __cplusplus
}
#endif

#endif /* __MD5_H_ */
