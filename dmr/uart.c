/*
 *  "RS-232 Interrupts The C Way"
 */
#define RBR 0               /* Receive buffer register */
#define THR 0               /* Transmit holding reg.   */
#define IER 1               /* Interrupt Enable reg.   */
#define IER_RX_DATA 1       /* Enable RX interrupt bit */
#define IER_THRE 2          /* Enable TX interrupt bit */
#define IIR 2               /* Interrupt ID register   */
#define IIR_MODEM_STATUS 0  /* Modem stat. interrupt ID*/
#define IIR_TRANSMIT 2      /* Transmit interrupt ID   */
#define IIR_RECEIVE 4       /* Receive interrupt ID    */
#define IIR_LINE_STATUS 6   /* Line stat. interrupt ID */
#define LCR 3               /* Line control register   */
#define LCR_DLAB 0x80       /* Divisor access bit      */
#define LCR_EVEN_PARITY 0x8 /* Set parity 'E' bits     */
#define LCR_ODD_PARITY 0x18 /* Set parity 'O' bits     */
#define LCR_NO_PARITY 0     /* Set parity 'N' bits     */
#define LCR_1_STOP_BIT 0    /* Bits to set 1 stop bit  */
#define LCR_2_STOP_BITS 4   /* Bits to set 2 stop bits */
#define LCR_5_DATA_BITS 0   /* Bits to set 5 data bits */
#define LCR_6_DATA_BITS 1   /* Bits to set 6 data bits */
#define LCR_7_DATA_BITS 2   /* Bits to set 7 data bits */
#define LCR_8_DATA_BITS 3   /* Bits to set 8 data bits */
#define MCR 4               /* Modem control register  */
#define MCR_DTR 1           /* Bit to turn on DTR      */
#define MCR_RTS 2           /* Bit to turn on RTS      */
#define MCR_OUT1 4          /* Bit to turn on OUT1     */
#define MCR_OUT2 8          /* Bit to turn on OUT2     */
#define MCR_LOOPBACK 16     /* Bit to turn on LOOKBACK */
#define LSR 5               /* Line Status register    */
#define MSR 6               /* Modem Status register   */
#define DLL 0               /* Divisor latch LSB       */
#define DLM 1               /* Divisor latch MSB       */

#define INT_CONTROLLER 0x20 /* The address of the 8259*/
#define EOI 0x20            /* The end of int command */

#define COM1_IO_BASE 0x3f8
#define COM1_INTERRUPT 12
#define COM1_IRQ 4

#ifndef outportb
void outport(unsigned port, unsigned val);
void outportb(unsigned port, unsigned char val);
unsigned inport(unsigned port);
unsigned char inportb(unsigned port);
#endif

void kltxintr(void);
void klrxintr(char c);

/* 115200L, 'N', 8, 1 */
void uart_init(void)
{
    unsigned char lcr_out, mcr_out, low_divisor, high_divisor, irq_mask;
    /*
     * First disable all interrupts from the port.  I also read
     * RBR just in case their is a char sitting there ready to
     * generate an interupt.
     */
    outportb(COM1_IO_BASE + IER, 0);
    inportb(COM1_IO_BASE);
    /*
     * Writing the baud rate means first enabling the divisor
     * latch registers, then writing the 16 bit divisor int
     * two steps, then disabling the divisor latch so the other
     * registers can be accessed normally.
     */
    low_divisor = 1;    /* 115200 */
    high_divisor = 0;
    outportb(COM1_IO_BASE + LCR, LCR_DLAB);
    outportb(COM1_IO_BASE + DLL, low_divisor);
    outportb(COM1_IO_BASE + DLM, high_divisor);
    outportb(COM1_IO_BASE + LCR, 0);
    /*
     * Setting up the line control register establishes the
     * parity, number of bits, and number of stop bits.
     */
    lcr_out = LCR_NO_PARITY;
    lcr_out |= LCR_8_DATA_BITS;
    outportb(COM1_IO_BASE + LCR, lcr_out);
    /*
     * I turn on RTS and DTR, as well as OUT2.  OUT2 is needed
     * to allow interrupts on PC compatible cards.
     */
    mcr_out = MCR_RTS | MCR_DTR | MCR_OUT2;
    outportb(COM1_IO_BASE + MCR, mcr_out);
    /*
     * Finally, enable tx|rx interrupts, and exit.
     */
    outportb(COM1_IO_BASE + IER, IER_THRE | IER_RX_DATA);
    irq_mask = inportb(INT_CONTROLLER + 1);
    irq_mask &= ~(1 << COM1_IRQ);
    outportb(INT_CONTROLLER + 1, irq_mask);
}

void uartintr(void)
{
    for (;;)
    {
        switch (inportb(COM1_IO_BASE + IIR))
        {
        case IIR_MODEM_STATUS:
            inportb(COM1_IO_BASE + MSR);
            break;
        case IIR_TRANSMIT:
            kltxintr();
            break;
        case IIR_RECEIVE:
            klrxintr(inportb(COM1_IO_BASE + RBR));
            break;
        case IIR_LINE_STATUS:
            inportb(COM1_IO_BASE + LSR);
            break;
        default:
            return;
        }
    }
}

void uart_putc(char c)
{
    outportb( COM1_IO_BASE + THR, c );
}

int uart_getc(void)
{
    return inportb( COM1_IO_BASE + RBR );
}
