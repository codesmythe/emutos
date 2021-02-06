/* This is the config for my 'BITSY' MC68008-based SBC. */

#define CONF_ATARI_HARDWARE 0

#define CONF_STRAM_SIZE 512*1024

#define CONF_WITH_ADVANCED_CPU 0
#define CONF_WITH_APOLLO_68080 0
#define CONF_WITH_CACHE_CONTROL 0
#define CONF_WITH_BUS_ERROR 1
#define ALWAYS_SHOW_INITINFO 1

#define CONF_WITH_MFP 1
#define CONF_WITH_MFP_RS232 1
#define CONF_SERIAL_CONSOLE 1
#define RS232_DEBUG_PRINT 1
/* #define CONSOLE_DEBUG_PRINT 1 */
#define CONF_ATARI_IDE 1
#define CONF_WITH_IDE 1

#define USE_STOP_INSN_TO_FREE_HOST_CPU 0
#define DETECT_NATIVE_FEATURES 0

#define CONF_WITH_RESET 0 /* work around RESET/HALT bug on board */



