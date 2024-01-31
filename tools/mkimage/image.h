/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 ********************************************************************
 * NOTE: This header file defines an interface to U-Boot. Including
 * this (unmodified) header file in another file is considered normal
 * use of U-Boot, and does *not* fall under the heading of "derived
 * work".
 ********************************************************************
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <compiler.h>
#include <asm/byteorder.h>
#include <stdbool.h>

#include <sys/types.h>


enum ih_category {
	IH_ARCH,
	IH_COMP,
	IH_OS,
	IH_TYPE,

	IH_COUNT,
};

enum {
	IH_OS_INVALID		= 0,	/* Invalid OS	*/
	IH_OS_LINUX,			/* Linux	*/

	IH_OS_COUNT,
};

enum {
	IH_ARCH_INVALID		= 0,	/* Invalid CPU	*/
	IH_ARCH_ARM,			/* ARM		*/

	IH_ARCH_COUNT,
};

enum {
	IH_TYPE_INVALID		= 0,	/* Invalid Image		*/
	IH_TYPE_S32CCIMAGE,		/* NXP S32 Common Chassis Boot Image */

	IH_TYPE_COUNT,			/* Number of image types */
};

enum {
	IH_COMP_NONE		= 0,	/*  No	 Compression Used	*/

	IH_COMP_COUNT,
};

/* Reused from common.h */
#define ROUND(a, b)		(((a) + (b) - 1) & ~((b) - 1))

#endif	/* __IMAGE_H__ */
