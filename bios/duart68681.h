
#ifndef DUART68681_H
#define DUART68681_H

#define DUART_PORT_A 0
#define DUART_PORT_B 1

/* Some Motorola 68681 DUART support borrowed from
 * FUZIX: https://github.com/EtchedPixels/FUZIX/blob/master/Kernel/platform-tiny68k/devtty.c
 * FUZIX is licensed under GPL v2, like EmuTOS.
 */

#define DUART_MRA        0x01
#define DUART_SRA        0x03
#define DUART_CSRA       0x03
#define DUART_CRA        0x05
#define DUART_RHRA       0x07
#define DUART_THRA       0x07
#define DUART_IPCR       0x09
#define DUART_ACR        0x09
#define DUART_ISR        0x0B
#define DUART_IMR        0x0B
#define DUART_CTU        0x0D
#define DUART_CTUR       0x0D
#define DUART_CTL        0x0F
#define DUART_CTLR       0x0F
#define DUART_MRB        0x11
#define DUART_SRB        0x13
#define DUART_CSRB       0x13
#define DUART_CRB        0x15
#define DUART_RHRB       0x17
#define DUART_THRB       0x17
#define DUART_IVR        0x19
#define DUART_OPCR       0x1B
#define DUART_STARTCTR   0x1D
#define DUART_SETOPR     0x1D
#define DUART_STOPCTR    0x1F
#define DUART_CLROPR     0x1F

/* The bit definitions here copied with slight renaming from
 * coldpriv.h
 */
/* Bit definitions and macros for DUART_MR */
#define DUART_MR_BC(x)              (((x)&0x03)<<0)
#define DUART_MR_PT                 (0x04)
#define DUART_MR_PM(x)              (((x)&0x03)<<3)
#define DUART_MR_ERR                (0x20)
#define DUART_MR_RXIRQ              (0x40)
#define DUART_MR_RXRTS              (0x80)
#define DUART_MR_SB(x)              (((x)&0x0F)<<0)
#define DUART_MR_TXCTS              (0x10)
#define DUART_MR_TXRTS              (0x20)
#define DUART_MR_CM(x)              (((x)&0x03)<<6)
#define DUART_MR_PM_MULTI_ADDR      (0x1C)
#define DUART_MR_PM_MULTI_DATA      (0x18)
#define DUART_MR_PM_NONE            (0x10)
#define DUART_MR_PM_FORCE_HI        (0x0C)
#define DUART_MR_PM_FORCE_LO        (0x08)
#define DUART_MR_PM_ODD             (0x04)
#define DUART_MR_PM_EVEN            (0x00)
#define DUART_MR_BC_5               (0x00)
#define DUART_MR_BC_6               (0x01)
#define DUART_MR_BC_7               (0x02)
#define DUART_MR_BC_8               (0x03)
#define DUART_MR_CM_NORMAL          (0x00)
#define DUART_MR_CM_ECHO            (0x40)
#define DUART_MR_CM_LOCAL_LOOP      (0x80)
#define DUART_MR_CM_REMOTE_LOOP     (0xC0)
#define DUART_MR_SB_STOP_BITS_1     (0x07)
#define DUART_MR_SB_STOP_BITS_15    (0x08)
#define DUART_MR_SB_STOP_BITS_2     (0x0F)

/* Bit definitions and macros for DUART_SR */
#define DUART_SR_RXRDY              (0x01)
#define DUART_SR_FFULL              (0x02)
#define DUART_SR_TXRDY              (0x04)
#define DUART_SR_TXEMP              (0x08)
#define DUART_SR_OE                 (0x10)
#define DUART_SR_PE                 (0x20)
#define DUART_SR_FE                 (0x40)
#define DUART_SR_RB                 (0x80)

/* Bit definitions and macros for DUART_CSR */
#define DUART_CSR_TCS(x)            (((x)&0x0F)<<0)
#define DUART_CSR_RCS(x)            (((x)&0x0F)<<4)
#define DUART_CSR_RCS_SYS_CLK       (0xD0)
#define DUART_CSR_RCS_CTM16         (0xE0)
#define DUART_CSR_RCS_CTM           (0xF0)
#define DUART_CSR_TCS_SYS_CLK       (0x0D)
#define DUART_CSR_TCS_CTM16         (0x0E)
#define DUART_CSR_TCS_CTM           (0x0F)

/* Bit definitions and macros for DUART_CR */
#define DUART_CR_RXC(x)             (((x)&0x03)<<0)
#define DUART_CR_TXC(x)             (((x)&0x03)<<2)
#define DUART_CR_MISC(x)            (((x)&0x07)<<4)
#define DUART_CR_NONE               (0x00)
#define DUART_CR_STOP_BREAK         (0x70)
#define DUART_CR_START_BREAK        (0x60)
#define DUART_CR_BKCHGINT           (0x50)
#define DUART_CR_RESET_ERROR        (0x40)
#define DUART_CR_RESET_TX           (0x30)
#define DUART_CR_RESET_RX           (0x20)
#define DUART_CR_RESET_MR           (0x10)
#define DUART_CR_TX_DISABLED        (0x08)
#define DUART_CR_TX_ENABLED         (0x04)
#define DUART_CR_RX_DISABLED        (0x02)
#define DUART_CR_RX_ENABLED         (0x01)

/* These are extended commands supported by the XR68C681 */
#define DUART_CR_SET_RX_BRG_EXTEND_BIT (0x80)
#define DUART_CR_CLR_RX_BRG_EXTEND_BIT (0x90)
#define DUART_CR_SET_TX_BRG_EXTEND_BIT (0xA0)
#define DUART_CR_CLR_TX_BRG_EXTEND_BIT (0xB0)
#define DUART_CR_SET_STANDBY_MODE      (0xC0)
#define DUART_CR_SET_ACTIVE_MODE       (0xD0)

/* Bit definitions and macros for DUART_IPCR */
#define DUART_IPCR_CTS              (0x01)
#define DUART_IPCR_COS              (0x10)

/* Bit definitions and macros for DUART_ACR */
#define DUART_ACR_IEC               (0x01)

/* Bit definitions and macros for DUART_ISR */
#define DUART_ISR_TXRDY             (0x01)
#define DUART_ISR_RXRDY_FU          (0x02)
#define DUART_ISR_DB                (0x04)
#define DUART_ISR_RXFTO             (0x08)
#define DUART_ISR_TXFIFO            (0x10)
#define DUART_ISR_RXFIFO            (0x20)
#define DUART_ISR_COS               (0x80)

/* Bit definitions and macros for DUART_IMR */
#define DUART_IMR_TXRDY_A           (0x01)
#define DUART_IMR_RXRDY_A           (0x02)
#define DUART_IMR_DB_A              (0x04)
#define DUART_IMR_COUNTER_READY     (0x08)
#define DUART_IMR_TXRDY_B           (0x10)
#define DUART_IMR_RXRDY_B           (0x20)
#define DUART_IMR_DB_B              (0x40)
#define DUART_IMR_INPUT_CHG         (0x80)

/* Bit definitions and macros for DUART_IP */
#define DUART_IP_CTS                (0x01)

/* Bit definitions and macros for DUART_OP1, bit 1 */
#define DUART_OP1_RTS               (0x02)

/* Bit definitions and macros for DUART_OP0, bit 0 */
#define DUART_OP0_RTS               (0x01)

#endif
