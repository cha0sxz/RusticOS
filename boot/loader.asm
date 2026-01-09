[org 0x7E00]
[bits 16]

%include "boot/kernel_sectors.inc"

loader_start:
    cli
    cld
    
    ; Set up segment registers for loader
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000      ; Stack at 0x00000:0x9000 = 0x9000 linear
    
    ; Skip over protected mode code
    jmp real_mode_start
    
    ; ---------------------------
    ; Protected-mode entry stub (32-bit)
    ; - Reached after far jump with CS=0x08 (code selector)
    ; - Initializes data selectors and ESP, then jumps to kernel
    ; ---------------------------
    align 4
pm_entry_32:
    bits 32
    mov ax, 0x10                ; data selector (linear addressing)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; Set a safe 32-bit stack
    mov esp, 0x00089000
    
    
    ; Far jump to kernel at 0x00001000
    jmp 0x08:0x00001000
    ; Unreachable, but return to 16-bit mode for NASM segment tracking
    bits 16

real_mode_start:
    sti

    mov [boot_drive], dl

    ; Print loader start message
    mov si, msg_loader_start
    call print_string
    call delay

    ; Print kernel loading message
    mov si, msg_kernel_loading
    call print_string
    call delay

    ; Load kernel with LBA
    mov si, msg_chs
    call print_string
    call delay
    
    ; Hide cursor during loading
    mov ah, 0x01
    mov ch, 0x20  ; Start scanline > end scanline to hide cursor
    mov cl, 0x00
    int 0x10
    
    ; Initialize
    mov ax, 0x0000
    mov es, ax
    xor bx, bx
    
    ; Set geometry (hard disk: heads=16, sectors=63)
    mov byte [sectors_per_track], 63
    mov byte [heads], 16
    
    ; Small delay
    mov cx, 0xFFFF
.delay_loop:
    loop .delay_loop
    
    ; Try LBA read of entire kernel
    ; Set DAP LBA to 3
    mov word [dap + 8], 3
    mov word [dap + 10], 0
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, dap
    int 0x13
    jc .read_error
    
    ; Success
    jmp .kernel_loaded
    
.kernel_loaded:
    ; Success
    mov si, msg_kernel_loaded
    call print_string
    call delay

    ; Success - proceed to PM
    jmp .kernel_valid
    
.read_error:
    ; Print error message and halt
    mov si, msg_kernel_err
    call print_string
    hlt
    
.kernel_valid:
    ; About to enter PM
    mov si, msg_entering_pm
    call print_string
    call delay

    ; Prepare to enter protected mode and jump to kernel at 0x00000000

    ; Load GDT
    lgdt [gdt_ptr]

    ; Disable interrupts during mode switch
    cli

    ; Set CR0.PE to enter protected mode
    mov eax, cr0
    or eax, 0x00000001     ; Set PE bit
    mov cr0, eax

    ; CRITICAL: Far jump immediately after PE bit set
    ; This jumps to protected mode code and reloads CS
    jmp 0x08:pm_entry_32

; ========== MESSAGES ==========
msg_loader_here:    db "[LOADER] Loader executing", 0
msg_loader_start:   db "[LOADER] Loader started", 0x0D, 0x0A, 0
msg_kernel_loading: db "[LOADER] Loading kernel from disk...", 0x0D, 0x0A, 0
msg_kernel_loaded:  db "[LOADER] Kernel loaded successfully!", 0x0D, 0x0A, 0
msg_entering_pm:    db "[LOADER] Entering protected mode...", 0x0D, 0x0A, 0
msg_kernel_err:     db "[LOADER] Failed to read kernel from disk", 0x0D, 0x0A, 0
msg_lba:            db "[LOADER] Trying LBA read...", 0x0D, 0x0A, 0
msg_chs:            db "[LOADER] Loading kernel with LBA...", 0x0D, 0x0A, 0
msg_debug_sector:   db "[LOADER] Kernel starts at sector: ", 0
msg_debug_lba:      db "[LOADER] LBA address: ", 0
msg_newline:        db 0x0D, 0x0A, 0

; ========== GDT (Global Descriptor Table) =========
; Print string using INT 10h (teletype mode) - same as bootloader
; This ensures consistent color and automatic cursor handling

; ========== HELPERS ==========
; Print string using INT 10h (teletype mode)
print_string:
    mov ah, 0x0E            ; INT 10h AH=0x0E: teletype output
    xor bx, bx              ; BH=0 (page 0)
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

; Delay function - provides a visible delay for messages
; Approximately 2-3 seconds on typical systems
delay:
    push ax
    push cx
    push dx
    mov ax, 0x0040          ; Outer loop counter (64 iterations)
.delay_outer:
    mov cx, 0xFFFF          ; Middle loop counter (65535 iterations)
.delay_middle:
    mov dx, 0x00FF          ; Inner loop counter (255 iterations)
.delay_inner:
    dec dx
    jnz .delay_inner
    dec cx
    jnz .delay_middle
    dec ax
    jnz .delay_outer
    pop dx
    pop cx
    pop ax
    ret

; Print 32-bit hex number
print_hex_dword:
    push eax
    push ecx
    mov ecx, 8  ; 8 hex digits
.hex_loop:
    rol eax, 4
    mov dl, al
    and dl, 0x0F
    add dl, '0'
    cmp dl, '9'
    jle .print_digit
    add dl, 7   ; 'A' - '9' - 1
.print_digit:
    mov ah, 0x0E
    int 0x10
    loop .hex_loop
    pop ecx
    pop eax
    ret

; ========== SERIAL DEBUG HELPERS ==========
; Not currently used - kept for reference
; serial_puts:
;     push ax
;     push bx
;     push si
;     mov dx, 0x3F8
; .s_loop:
;     lodsb
;     test al, al
;     jz .s_done
;     mov bl, al
; .s_wait:
;     in al, 0x3FD
;     test al, 0x20
;     jz .s_wait
;     mov al, bl
;     out dx, al
;     jmp .s_loop
; .s_done:
;     pop si
;     pop bx
;     pop ax
;     ret

; LBA to CHS conversion
; Input: AX = LBA
; Output: CH = cylinder, CL = sector (1-based), DH = head
lba_to_chs:
    push bx
    push dx
    xor bx, bx
    mov bl, [sectors_per_track]
    xor dx, dx
    div bx      ; AX = temp = LBA / SECTORS_PER_TRACK, DX = LBA % SECTORS_PER_TRACK
    mov cl, dl
    inc cl      ; CL = sector (1-based)
    xor bx, bx
    mov bl, [heads]
    xor dx, dx
    div bx      ; AX = cylinder, DX = head
    mov ch, al  ; CH = cylinder
    mov dh, dl  ; DH = head
    pop dx
    pop bx
    ret

; ========== GDT (Global Descriptor Table) ==========
gdt:
    ; GDT Entry 0: Null descriptor
    dq 0x0000000000000000
    
    ; GDT Entry 1 (offset 0x08): 32-bit Code segment (flat memory, base=0x0000)
    ; Base=0x00000000, Limit=0xFFFFF, Type=Code, P=1, S=1, DPL=0, DB=1, G=1
    dw 0xFFFF           ; Limit 15:0
    dw 0x0000           ; Base 15:0 (low 16 bits of 0x00000000)
    db 0x00             ; Base 23:16
    db 0x9A             ; P=1, DPL=0, S=1, Type=1010 (code, conforming)
    db 0xCF             ; G=1, DB=1, L=0, AVL=0, Limit 19:16=1111
    db 0x00             ; Base 31:24
    
    ; GDT Entry 2 (offset 0x10): 32-bit Data segment (flat memory, base=0)
    ; Base=0x00000000, Limit=0xFFFFF, Type=Data, P=1, S=1, DPL=0, DB=1, G=1
    dw 0xFFFF           ; Limit 15:0
    dw 0x0000           ; Base 15:0 (low 16 bits of 0x00000000)
    db 0x00             ; Base 23:16
    db 0x92             ; P=1, DPL=0, S=1, Type=0010 (data)
    db 0xCF             ; G=1, DB=1, L=0, AVL=0, Limit 19:16=1111
    db 0x00             ; Base 31:24

; Variables for kernel loading
kernel_start_sector: dq 0x1122334455667788  ; Will be patched by patch_loader_dap.py
boot_drive: db 0
current_lba: dw 0
remaining: dw 0
sectors_per_track: db 63
heads: db 255

; DAP (Disk Address Packet) for LBA reading
dap:
    db 0x10      ; Size of DAP
    db 0         ; Unused
    dw KERNEL_SECTORS  ; Number of sectors to read
    dw 0x1000    ; Destination offset
    dw 0x0000    ; Destination segment
    dq 0x0000000000000000  ; LBA address (will be set at runtime)

; The GDT is at address gdt
gdt_ptr:
    dw 0x0017           ; GDT limit (23 bytes = 3 entries - 1)
    dd gdt              ; GDT base
gdt_end:
