/*================================================================================
 * Copyright 2001- by HUMAX Co., Ltd.
 * 
 * R&D Center 
 * Humax Venture Tower 271-2
 * Seohyeon-Dong, Bundang-Gu, Seongnam-Si,
 * Gyeonggi-Do, Korea(ROK)
 *
 * This source code is confidential, proprietary information of HUMAX and 
 * is an unpublished work of authorship protected by the copyright laws of 
 * the Korea(ROK). Unauthorized copying or use of this document or the program
 * contained herein, in original or modified form, is a violation of Korea law.
 *--------------------------------------------------------------------------------
 * {PROJECT}
 *--------------------------------------------------------------------------------
 * RG100, HMS
 *--------------------------------------------------------------------------------
 * {FILENAME}
 *--------------------------------------------------------------------------------
 * SMS.H
 *
 * $Id: sms.h,v 1.1.1.1 2004/08/31 01:54:44 ktpark Exp $
 *--------------------------------------------------------------------------------
 * {DESCRIPTION}
 *--------------------------------------------------------------------------------
 * This file contains public interfaces of the SMS
 *--------------------------------------------------------------------------------
 * {INITIAL AUTHOR}
 *--------------------------------------------------------------------------------
 * Kyoung Taek, Park, Humax. Contacts at ktpark@humaxdigital.com
 *--------------------------------------------------------------------------------
 * {ANKNOWLEDGMENTS}
 *--------------------------------------------------------------------------------
 * 
 *--------------------------------------------------------------------------------
 * {REVISION HISTORY}
 *--------------------------------------------------------------------------------
 * $Log: sms.h,v $
 * Revision 1.1.1.1  2004/08/31 01:54:44  ktpark
 * (STB) 초기작업
 *
 *================================================================================
 */

#ifndef __SMS_H__
#define __SMS_H__

#ifdef WIN32		 
#ifndef UINT32
typedef unsigned int UINT32;
#endif
#endif
#ifndef INT32
typedef int				INT32;
#endif
#ifndef UINT16
typedef unsigned short	UINT16;
#endif
#ifndef INT16
typedef short				INT16;
#endif
#ifndef UINT8
typedef unsigned char	UINT8;
#endif
#ifndef INT8
typedef char				INT8;
#endif

/* ----------------------------------------------------------------------------------
 * SMS_KEY_INFO
 * ----------------------------------------------------------------------------------
 * SMS_MAX_KEY 	: max num of keys that sms can handle
 * SMS_MAX_CODE	: max num of char code that one key can handle
 */
#define  SMS_MAX_KEY		12
#define  SMS_MAX_CODE	10

typedef struct
{
	UINT8   	key;
	UINT8		type;
	UINT8    codes[SMS_MAX_CODE];
} SMS_KEY_INFO;


/* ----------------------------------------------------------------------------------
 * SMS_CALLBACK
 * ----------------------------------------------------------------------------------
 */
typedef enum
{
	CALLBACK_TIMEOUT,
	CALLBACK_DIFFERENT_KEY
} CALLBACK_REASON;

typedef void (*SMS_CALLBACK)( CALLBACK_REASON );


/* ----------------------------------------------------------------------------------
 * SMS_STATUS
 * ----------------------------------------------------------------------------------
 */
#define  SMS_ERR_BASE	100

typedef enum
{
	SMS_STAT_OK			=	1,
	SMS_STAT_ERR		= (SMS_ERR_BASE+0),
	SMS_STAT_ERR_ARG	= (SMS_ERR_BASE+1)
} SMS_STATUS;


/* ----------------------------------------------------------------------------------
 * WATCH LEVELS
 * ----------------------------------------------------------------------------------
 */
#define SMS_L1   (WATCH_SMS|WATCH_LEVEL_1) 
#define SMS_L2   (WATCH_SMS|WATCH_LEVEL_2) 
#define SMS_L3   (WATCH_SMS|WATCH_LEVEL_3) 
#define SMS_L4   (WATCH_SMS|WATCH_LEVEL_4) 
#define SMS_L5   (WATCH_SMS|WATCH_LEVEL_5) 
#define SMS_L6   (WATCH_SMS|WATCH_LEVEL_6) 
#define SMS_ERR  (WATCH_SMS|WATCH_LEVEL_ALWAYS) 


/* =================================================================================
 * I N T E R F A C E S
 * =================================================================================
 */  
#if defined(__cplusplus)
extern "C"
{
#endif
 
extern SMS_STATUS	sms_open( SMS_KEY_INFO*, SMS_CALLBACK );
extern SMS_STATUS	sms_close();
extern SMS_STATUS sms_get_code( UINT8 key, UINT8* pcode );

#if defined(__cplusplus)
}
#endif

#endif /*__IMC_MUX_H__*/
