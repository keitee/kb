#ifndef __WATCH_H__
#define __WATCH_H__


#ifdef WIN32
#define WOUTPUT   printf
#else
extern int DDI_UART_Print(const char *format, ...);
#define WOUTPUT   DDI_UART_Print
#endif

#ifndef UINT32
typedef unsigned int UINT32;
#endif


/* ----------------------------------------------------------------------------------
 ** CONSTANT :: WATCH_LEVEL_X
 * ----------------------------------------------------------------------------------
 ** DESCRIPTION 
 *    it defines watch levels and catagories
 *    level(4bits) | cat(28bits)
 *
 * ----------------------------------------------------------------------------------
 */      
#define WATCH_MODULE_MASK  0x0FFFFFFF
#define WATCH_LEVEL_MASK   0x70000000

#define WATCH_LEVEL_NEVER  0x00000000
#define WATCH_LEVEL_1      0x10000000
#define WATCH_LEVEL_2      0x20000000
#define WATCH_LEVEL_3      0x30000000
#define WATCH_LEVEL_4      0x40000000
#define WATCH_LEVEL_5      0x50000000
#define WATCH_LEVEL_6      0x60000000
#define WATCH_LEVEL_ALWAYS 0x70000000

#define WATCH_NULL		   WATCH_LEVEL_NEVER
#define WATCH_ALL          0x0FFFFFFF

#define WATCH_SMS      0x00000001
#define WATCH_IMC_MUX      0x00000002
#define WATCH_IMC_ADT	   0x00000004	
#define WATCH_IMC_APP	   0x00000008

#define WATCH_USER         0x00000010
#define WATCH_RTC          0x00000020
#define WATCH_RST          0x00000040
#define WATCH_AUD          0x00000080
#define WATCH_MPG          0x00000100
#define WATCH_MOV          0x00000200
#define WATCH_IRD          0x00000400
#define WATCH_I2C          0x00000800
#define WATCH_GCP          0x00001000
#define WATCH_ETH          0x00002000


/* ----------------------------------------------------------------------------------
 ** CONSTANT :: WATCH_COLOR
 * ----------------------------------------------------------------------------------
 ** DESCRIPTION 
 *    it defines watch colors
 *
 * ----------------------------------------------------------------------------------
 */      
#if WATCH_USE_COLOR
#define WATCH_FG_LIGHT_BLUE            "\033[1;34m"
#define WATCH_FG_LIGHT_GREEN           "\033[1;32m"
#define WATCH_FG_LIGHT_CYAN            "\033[1;36m"
#define WATCH_FG_LIGHT_RED             "\033[1;31m"
#define WATCH_FG_WHITE                 "\033[1;37m"
#define WATCH_FG_NORMAL                WATCH_FG_WHITE
#define WATCH_BG_GRAY                  "\033[0;47m"
#define WATCH_BG_BLACK                 "\033[0;40m"
#else
#define WATCH_FG_LIGHT_BLUE            ""
#define WATCH_FG_LIGHT_GREEN           ""
#define WATCH_FG_LIGHT_CYAN            ""
#define WATCH_FG_WHITE                 ""
#define WATCH_FG_NORMAL                WATCH_FG_WHITE
#define WATCH_BG_GRAY                  ""
#define WATCH_BG_BLACK                 ""
#endif /*WATCH_USE_COLOR*/


/* ----------------------------------------------------------------------------------
 ** CONSTANT :: WATCH_PORT_XXX
 * ----------------------------------------------------------------------------------
 ** DESCRIPTION 
 *    it defines watch ports
 *
 * ----------------------------------------------------------------------------------
 */      
#define WATCH_PORT_ENT				0
#define WATCH_PORT_LEV				1
#define WATCH_PORT_ERR				2
#define WATCH_PORT_WAR				3
#define WATCH_PORT_MSG				4


/* ----------------------------------------------------------------------------------
 ** INTERFACES
 * ----------------------------------------------------------------------------------
 */
#if defined(__cplusplus)
extern "C"
{
#endif
/*
extern void WATCH_INIT();
extern void WATCH_UNINIT();
extern void WATCH_SET( UINT32 flags );
extern void WATCH_SET_LEVEL( UINT32 level);
extern void WATCH_MESG( UINT32 flags, UINT32 port, char *string, ...);
extern void WATCH( UINT32 flags, char *string, ...);
extern void WATCH_N( UINT32 flags, char *format, ...);
*/
#define WATCH_INIT               if (0) ((void (*)(void)) 0)
#define WATCH_UNINIT             if (0) ((void (*)(void)) 0)
#define WATCH_SET                if (0) ((void (*)(unsigned int)) 0)
#define WATCH_SET_LEVEL          if (0) ((void (*)(unsigned int)) 0)
#define WATCH_MESG					if (0) ((void (*)(unsigned int, unsigned int, char*, ...)) 0)
#define WATCH							if (0) ((void (*)(unsigned int, char*, ...)) 0)
#define WATCH_N						if (0) ((void (*)(unsigned int, char*, ...)) 0)

#if defined(__cplusplus)
}
#endif


#define WATCH_GET(var)  (*(var) = guWatchFlags)

#define WATCH_ENT(flags, fname)			WATCH_MESG(flags|WATCH_LEVEL_2, WATCH_PORT_ENT, "[%s] ::::::::::::::::::::::::::::", fname)
#define WATCH_LEV(flags, fname)			WATCH_MESG(flags|WATCH_LEVEL_2, WATCH_PORT_LEV, "[%s] ::::::::::::::::::::::::::::", fname)
#define WATCH_ERR(flags, message)		WATCH_MESG(WATCH_ALL|WATCH_LEVEL_ALWAYS, WATCH_PORT_ERR, "%s in %s(%d)", message, __FILE__, __LINE__)
#define WATCH_WAR(flags, message)		WATCH_MESG(flags|WATCH_LEVEL_ALWAYS, WATCH_PORT_WAR, "%s", message)

#endif /*__WATCH_H__*/
