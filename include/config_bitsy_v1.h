#ifdef MACHINE_BITSY_V1
# ifndef CONF_ATARI_HARDWARE
#  define CONF_ATARI_HARDWARE 0
# endif
# ifndef CONF_WITH_ADVANCED_CPU
#  define CONF_WITH_ADVANCED_CPU 0
# endif
# ifndef CONF_WITH_APOLLO_68080
#  define CONF_WITH_APOLLO_68080 0
# endif
# ifndef CONF_WITH_BUS_ERROR
#  define CONF_WITH_BUS_ERROR 1
# endif
# ifndef CONF_WITH_CACHE_CONTROL
#  define CONF_WITH_CACHE_CONTROL 0
# endif
# ifndef ALWAYS_SHOW_INITINFO
#  define ALWAYS_SHOW_INITINFO 1
# endif

# ifndef CONF_STRAM_SIZE
#  define CONF_STRAM_SIZE 1*1024*1024
# endif
# ifndef CONF_WITH_ALT_RAM
#  define CONF_WITH_ALT_RAM 0
# endif
# ifndef CONF_WITH_DUART
#  define CONF_WITH_DUART 0
# endif
# ifndef RS232_DEBUG_PRINT
#  define RS232_DEBUG_PRINT 1
# endif

# ifndef CONF_WITH_MFP
#  define CONF_WITH_MFP 1
# endif
# ifndef CONF_WITH_MFP_RS232
#  define CONF_WITH_MFP_RS232 1
# endif
/* BITSY V1 clocks the MFP at 7.387 Mhz instead of the standard 2.4576 MHz. */
# ifndef CONF_WITH_MFP_3X_CLOCK
#  define CONF_WITH_MFP_3X_CLOCK 1
# endif

# ifndef CONF_WITH_MFP_DS3231
#  define CONF_WITH_MFP_DS3231 1
# endif

# ifndef CONF_WITH_EARLY_MFP
#  define CONF_WITH_EARLY_MFP 1
# endif

# ifndef CONF_WITH_VBL_RTE
#  define CONF_WITH_VBL_RTE 1
# endif

# ifndef CONF_WITH_YM2149
#  define CONF_WITH_YM2149 1
# endif

# ifndef CONF_WITH_IDE
#  define CONF_WITH_IDE 1
# endif
# ifndef CONF_ATARI_IDE
#  define CONF_ATARI_IDE 1
# endif

# ifndef CONF_WITH_RESET
#  define CONF_WITH_RESET 0
# endif

# ifndef CONF_SERIAL_CONSOLE
#  define CONF_SERIAL_CONSOLE 1
# endif
# ifndef CONF_SERIAL_CONSOLE_ANSI
#  if CONF_SERIAL_CONSOLE
#   define CONF_SERIAL_CONSOLE_ANSI 1
#  else
#   define CONF_SERIAL_CONSOLE_ANSI 0
#  endif
# endif
# ifndef CONF_SERIAL_CONSOLE_POLLING_MODE
#  define CONF_SERIAL_CONSOLE_POLLING_MODE 1
# endif

# ifndef CONF_WITH_IKBD_ACIA
#  define CONF_WITH_IKBD_ACIA 0
#  define ACIA_IKBD_BASE (0xFFFF0031L)
# endif

# ifndef CONF_WITH_IKBD_ACE
#  define CONF_WITH_IKBD_ACE 1
#  define ACE_UART_BASE (0xFFFF0191L)
# endif

# ifndef USE_STOP_INSN_TO_FREE_HOST_CPU
#  define USE_STOP_INSN_TO_FREE_HOST_CPU 0
# endif
# ifndef DETECT_NATIVE_FEATURES
#  define DETECT_NATIVE_FEATURES 0
# endif

# ifndef DEFAULT_BAUDRATE
#  define DEFAULT_BAUDRATE B19200
# endif

#ifndef TOS_VERSION
# define TOS_VERSION 0x404 /* For 3D object support. */
#endif

# ifndef CONF_WITH_3D_OBJECT
#  define CONF_WITH_3D_OBJECTS 1
# endif

# ifndef CONF_WITH_COLOUR_ICONS
#  define CONF_WITH_COLOUR_ICONS 1
# endif

#endif
