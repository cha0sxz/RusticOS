#include "interrupt.h"
#include "keyboard.h"
#include "terminal.h"

extern Terminal terminal;
extern KeyboardDriver keyboard;

// External references to ISR and IRQ stubs (defined in crt0.s)
extern "C" {
    extern void isr0();
    extern void isr1();
    extern void isr2();
    extern void isr3();
    extern void isr4();
    extern void isr5();
    extern void isr6();
    extern void isr7();
    extern void isr8();
    extern void isr9();
    extern void isr10();
    extern void isr11();
    extern void isr12();
    extern void isr13();
    extern void isr14();
    extern void isr15();
    extern void isr16();
    extern void isr17();
    extern void isr18();
    extern void isr19();
    extern void isr20();
    extern void isr21();
    extern void isr22();
    extern void isr23();
    extern void isr24();
    extern void isr25();
    extern void isr26();
    extern void isr27();
    extern void isr28();
    extern void isr29();
    extern void isr30();
    extern void isr31();
    extern void irq32();
    extern void irq33();
    extern void irq34();
    extern void irq35();
    extern void irq36();
    extern void irq37();
    extern void irq38();
    extern void irq39();
    extern void irq40();
    extern void irq41();
    extern void irq42();
    extern void irq43();
    extern void irq44();
    extern void irq45();
    extern void irq46();
    extern void irq47();
}

// IDT Entry structure (must match x86 IDT format)
struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed));

// External IDT (defined in crt0.s)
extern "C" {
    extern IDTEntry idt[256];
}

/**
 * Set an IDT entry
 */
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].offset_low  = (base & 0xFFFF);
    idt[num].offset_high = (base >> 16) & 0xFFFF;
    idt[num].selector    = selector;
    idt[num].zero        = 0;
    idt[num].type_attr   = flags;
}

/**
 * Initialize the Interrupt Descriptor Table
 */
extern "C" void init_idt() {
    // Set up exception handlers (0-31)
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);  // Divide by Zero
    idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);  // Debug
    idt_set_gate(2,  (uint32_t)isr2,  0x08, 0x8E);  // NMI
    idt_set_gate(3,  (uint32_t)isr3,  0x08, 0x8E);  // Breakpoint
    idt_set_gate(4,  (uint32_t)isr4,  0x08, 0x8E);  // Overflow
    idt_set_gate(5,  (uint32_t)isr5,  0x08, 0x8E);  // Bound Range
    idt_set_gate(6,  (uint32_t)isr6,  0x08, 0x8E);  // Invalid Opcode
    idt_set_gate(7,  (uint32_t)isr7,  0x08, 0x8E);  // Device Not Available
    idt_set_gate(8,  (uint32_t)isr8,  0x08, 0x8E);  // Double Fault
    idt_set_gate(9,  (uint32_t)isr9,  0x08, 0x8E);  // Coprocessor Segment
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);  // Invalid TSS
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);  // Segment Not Present
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);  // Stack Fault
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);  // General Protection Fault
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);  // Page Fault
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);  // Reserved
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);  // x87 FPU Error
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);  // Alignment Check
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);  // Machine Check
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);  // SIMD Exception
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);  // Virtualization
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);  // Control Protection
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);  // Reserved
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);  // Reserved
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);  // Reserved
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);  // Reserved
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);  // Reserved
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);  // Reserved
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);  // Hypervisor
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);  // VMM Communication
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);  // Security
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);  // Reserved
    
    // Set up IRQ handlers (32-47)
    idt_set_gate(32, (uint32_t)irq32, 0x08, 0x8E);  // Timer (IRQ0)
    idt_set_gate(33, (uint32_t)irq33, 0x08, 0x8E);  // Keyboard (IRQ1)
    idt_set_gate(34, (uint32_t)irq34, 0x08, 0x8E);  // Cascade (IRQ2)
    idt_set_gate(35, (uint32_t)irq35, 0x08, 0x8E);  // COM2 (IRQ3)
    idt_set_gate(36, (uint32_t)irq36, 0x08, 0x8E);  // COM1 (IRQ4)
    idt_set_gate(37, (uint32_t)irq37, 0x08, 0x8E);  // LPT2 (IRQ5)
    idt_set_gate(38, (uint32_t)irq38, 0x08, 0x8E);  // Floppy (IRQ6)
    idt_set_gate(39, (uint32_t)irq39, 0x08, 0x8E);  // LPT1 (IRQ7)
    idt_set_gate(40, (uint32_t)irq40, 0x08, 0x8E);  // CMOS (IRQ8)
    idt_set_gate(41, (uint32_t)irq41, 0x08, 0x8E);  // Free (IRQ9)
    idt_set_gate(42, (uint32_t)irq42, 0x08, 0x8E);  // Free (IRQ10)
    idt_set_gate(43, (uint32_t)irq43, 0x08, 0x8E);  // Free (IRQ11)
    idt_set_gate(44, (uint32_t)irq44, 0x08, 0x8E);  // PS/2 Mouse (IRQ12)
    idt_set_gate(45, (uint32_t)irq45, 0x08, 0x8E);  // FPU (IRQ13)
    idt_set_gate(46, (uint32_t)irq46, 0x08, 0x8E);  // Primary ATA (IRQ14)
    idt_set_gate(47, (uint32_t)irq47, 0x08, 0x8E);  // Secondary ATA (IRQ15)
}

/**
 * Initialize the Programmable Interrupt Controller (PIC)
 * 
 * Remaps IRQ 0-15 to interrupt vectors 32-47 to avoid conflicts
 * with CPU exceptions (0-31).
 */
void init_pic() {
    // Save masks
    uint8_t a1, a2;
    
    // Get current interrupt masks
    asm volatile("inb %1, %0" : "=a"(a1) : "Nd"(PIC1_DATA));
    asm volatile("inb %1, %0" : "=a"(a2) : "Nd"(PIC2_DATA));
    
    // Initialize PICs (cascade mode)
    // ICW1: Start initialization sequence
    asm volatile("outb %0, %1" : : "a"((uint8_t)(PIC_ICW1_INIT | PIC_ICW1_ICW4)), "Nd"(PIC1_COMMAND));
    asm volatile("outb %0, %1" : : "a"((uint8_t)(PIC_ICW1_INIT | PIC_ICW1_ICW4)), "Nd"(PIC2_COMMAND));
    
    // ICW2: Set interrupt vector offset (master: 32, slave: 40)
    asm volatile("outb %0, %1" : : "a"((uint8_t)IRQ_BASE), "Nd"(PIC1_DATA));
    asm volatile("outb %0, %1" : : "a"((uint8_t)(IRQ_BASE + 8)), "Nd"(PIC2_DATA));
    
    // ICW3: Tell master PIC that slave PIC is at IRQ2
    asm volatile("outb %0, %1" : : "a"((uint8_t)0x04), "Nd"(PIC1_DATA));
    // Tell slave PIC its cascade identity (bit 2)
    asm volatile("outb %0, %1" : : "a"((uint8_t)0x02), "Nd"(PIC2_DATA));
    
    // ICW4: Set 8086 mode
    asm volatile("outb %0, %1" : : "a"((uint8_t)PIC_ICW4_8086), "Nd"(PIC1_DATA));
    asm volatile("outb %0, %1" : : "a"((uint8_t)PIC_ICW4_8086), "Nd"(PIC2_DATA));
    
    // Restore masks (enable only keyboard and timer initially)
    // Mask all interrupts except timer (0) and keyboard (1)
    asm volatile("outb %0, %1" : : "a"((uint8_t)0xFC), "Nd"(PIC1_DATA)); // Enable timer and keyboard on master
    asm volatile("outb %0, %1" : : "a"((uint8_t)0xFF), "Nd"(PIC2_DATA)); // Disable all on slave for now
}

/**
 * Send End of Interrupt signal to PIC
 * This must be called at the end of each IRQ handler
 */
void send_eoi(uint8_t irq) {
    if (irq >= 8) {
        // If IRQ came from slave PIC, send EOI to both
        asm volatile("outb %0, %1" : : "a"((uint8_t)PIC_EOI), "Nd"(PIC2_COMMAND));
    }
    // Always send EOI to master PIC
    asm volatile("outb %0, %1" : : "a"((uint8_t)PIC_EOI), "Nd"(PIC1_COMMAND));
}


/**
 * IRQ Handler - called from assembly ISR stubs
 * Routes IRQs to appropriate device drivers
 */
extern "C" void irq_handler(uint8_t irq) {
    switch (irq) {
        case IRQ_TIMER:
            // Timer interrupt - can be used for scheduling later
            // For now, just acknowledge
            break;
            
        case IRQ_KEYBOARD: {
            // Keyboard interrupt - read scan code and process
            uint8_t scan_code;
            asm volatile("inb %1, %0" : "=a"(scan_code) : "Nd"((uint16_t)0x60));
            keyboard.handle_interrupt(scan_code);
            break;
        }
        
        default:
            // Unknown IRQ - acknowledge anyway
            break;
    }
    
    // Send End of Interrupt to PIC
    send_eoi(irq);
}

/**
 * Exception Handler - called from assembly ISR stubs for CPU exceptions
 * Currently just displays error information
 */
extern "C" void exception_handler(uint8_t vector, uint32_t error_code) {
    // Simple error handling - just display the exception number
    // In a real OS, this would handle page faults, divide by zero, etc.
    terminal.write("Exception ");
    char buf[4];
    buf[0] = '0' + (vector / 10);
    buf[1] = '0' + (vector % 10);
    buf[2] = '\n';
    buf[3] = '\0';
    terminal.write(buf);
    
    // Halt for exceptions (except page fault, double fault which we might want to handle)
    if (vector != 14) {  // Don't halt on page fault (not implemented yet)
        asm volatile("cli; hlt");  // Disable interrupts and halt
    }
}

/**
 * Enable interrupts (STI)
 */
void enable_interrupts() {
    asm volatile("sti");
}

/**
 * Disable interrupts (CLI)
 */
void disable_interrupts() {
    asm volatile("cli");
}

