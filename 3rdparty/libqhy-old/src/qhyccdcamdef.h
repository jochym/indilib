/*
 QHYCCD SDK
 
 Copyright (c) 2014 QHYCCD.
 All Rights Reserved.
 
 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the Free
 Software Foundation; either version 2 of the License, or (at your option)
 any later version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details.
 
 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59
 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 
 The full GNU General Public License is included in this distribution in the
 file called LICENSE.
 */

/*! 
   @file qhyccdcamdef.h
   @brief QHYCCD SDK error define
  */

#ifndef __QHYCCDCAMDEF_H__
#define __QHYCCDCAMDEF_H__



/**
 * Type define for IMG0S */
#define DEVICETYPE_IMG0S        00

/**
 * Type define for IMG0H */
#define DEVICETYPE_IMG0H        01

/**
 * Type define for IMG0L */
#define DEVICETYPE_IMG0L        02

/**
 * Type define for IMG0X */
#define DEVICETYPE_IMG0X        03

/**
 * Type define for IMG1S */
#define DEVICETYPE_IMG1S        10

/**
 * Type define for IMG2S */
#define DEVICETYPE_IMG2S        20

/**
 * Type define for IMG1E */
#define DEVICETYPE_IMG1E        110

/**
 * Type define for IMG2P */
#define DEVICETYPE_IMG2P        220

/**
 * Type define for QHY8 */
#define DEVICETYPE_QHY8         400

/**
 * Type define for QHY8L */
#define DEVICETYPE_QHY8L        413

/**
 * Type define for QHY8PRO */
#define DEVICETYPE_QHY8PRO      453

/**
 * Type define for QHY10 */
#define DEVICETYPE_QHY10        493

/**
 * Type define for QHY12 */
#define DEVICETYPE_QHY12        613

/**
 * Type define for QHY9S */
#define DEVICETYPE_QHY9S        390

/**
 * Type define for QHY11 */
#define DEVICETYPE_QHY11        1100

/**
 * Type define for IC8300 */
#define DEVICETYPE_IC8300       391

/**
 * Type define for QHY16 */
#define DEVICETYPE_QHY16        16

/**
 * Type define for QHY5 */
#define DEVICETYPE_QHY5         51

/**
 * Type define for QHY5II */
#define DEVICETYPE_QHY5II       52

/**
 * Type define for QHY5LII_M */
#define DEVICETYPE_QHY5LII_M    53

/**
 * Type define for QHY5LII_C */
#define DEVICETYPE_QHY5LII_C    531

/**
 * Type define for QHY5TII */
#define DEVICETYPE_QHY5TII      54

/**
 * Type define for QHY5RII */
#define DEVICETYPE_QHY5RII      55

/**
 * Type define for QHY5PII */
#define DEVICETYPE_QHY5PII      56

/**
 * Type define for QHY5VII */
#define DEVICETYPE_QHY5VII      57

/**
 * Type define for QHY5LII_COOL */
#define DEVICETYPE_QHY5LII_COOL 58

/**
 * Type define for QHYXXX */
#define DEVICETYPE_QHYXXX       59

/**
 * Type define for QHY16000 */
#define DEVICETYPE_Q16000       160

/**
 * Type define for QHY21 */
#define DEVICETYPE_QHY21        674

/**
 * Type define for QHY22 */
#define DEVICETYPE_QHY22        694

/**
 * Type define for QHY23 */
#define DEVICETYPE_QHY23        814

/**
 * Type define for QHY6 */
#define DEVICETYPE_QHY6         60

/**
 * Type define for QHY15GIGAE */
#define DEVICETYPE_QHY15GIGAE   9000

/**
 * Type define for SOLAR800G */
#define DEVICETYPE_SOLAR800G    9001

/**
 * Type define for SIMULOTOR */
#define DEVICETYPE_SIMULATOR    0xffff

/**
 * Type define for UNKNOW */
#define DEVICETYPE_UNKNOW       -1

/**
 * debayer mode for mono to color */
enum BAYER_ID
{
    BAYER_GB = 1,
    BAYER_GR,
    BAYER_BG,
    BAYER_RG
};

#endif
