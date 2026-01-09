#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "types.h"

// PIC I/O Ports
#define PIC1_COMMAND    0x20  // Master PIC command port
#define PIC1_DATA       0x21  // Master PIC data port
#define PIC2_COMMAND    0xA0  // Slave PIC command port
#define PIC2_DATA       0xA1  // Slave PIC data port

// PIC Commands
#define PIC_EOI         0x20  // End of Interrupt command
#define PIC_ICW1_INIT   0x10  // Initialization command word 1
#define PIC_ICW1_ICW4   0x01  // ICW4 needed
#define PIC_ICW4_8086   0x01  // 8086/8088 mode

// IRQ Numbers (mapped to interrupt vectors 32-47)
#define IRQ_BASE        32
#define IRQ_TIMER       0     // Timer interrupt (IRQ0)
#define IRQ_KEYBOARD    1     // Keyboard interrupt (IRQ1)
#define IRQ_CASCADE     2     // Cascade interrupt (IRQ2)
#define IRQ_COM2        3     // COM2 (IRQ3)
#define IRQ_COM1        4     // COM1 (IRQ4)
#define IRQ_LPT2        5     // LPT2 (IRQ5)
#define IRQ_FLOPPY      6     // Floppy disk (IRQ6)
#define IRQ_LPT1        7     // LPT1 (IRQ7)
#define IRQ_CMOS        8     // CMOS real-time clock (IRQ8)
#define IRQ_FREE1       9     // Free (IRQ9)
#define IRQ_FREE2       10    // Free (IRQ10)
#define IRQ_FREE3       11    // Free (IRQ11)
#define IRQ_PS2         12    // PS/2 mouse (IRQ12)
#define IRQ_FPU         13    // FPU (IRQ13)
#define IRQ_PRIMARY_ATA 14    // Primary ATA (IRQ14)
#define IRQ_SECONDARY_ATA 15  // Secondary ATA (IRQ15)

// Function declarations
extern "C" {
    void init_idt();  // Called from assembly, needs C linkage
}

// C++ functions
void init_pic();
void enable_interrupts();
void disable_interrupts();
void send_eoi(uint8_t irq);

// IRQ handler functions (called from assembly ISR stubs)
extern "C" void irq_handler(uint8_t irq);
extern "C" void exception_handler(uint8_t vector, uint32_t error_code);

#endif // INTERRUPT_H

