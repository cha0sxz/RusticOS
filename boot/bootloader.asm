[org 0x7c00]
[bits 16]

%include "boot/kernel_sectors.inc"
%include "boot/loader_sectors.inc"

%ifndef LOADER_SECTORS
%assign LOADER_SECTORS 1
%endif

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7000
    sti

    ; Print debug message
    mov si, msg_start
    call print_string
    call delay

    ; Save boot drive number from BIOS
    mov [boot_drive], dl

    ; Print debug message before loading
    mov si, msg_loading
    call print_string
    call delay

    ; === Load loader using simple CHS read ===
    ; First, recalibrate the drive
    mov ah, 0x00            ; INT 13h AH=0x00: recalibrate
    mov dl, 0x80            ; drive (hard drive)
    int 0x13
    
    ; Read LOADER_SECTORS sectors from sector 2 (1-indexed) into 0x7E00:0x0000
    mov ax, 0x0000
    mov es, ax              ; ES = 0x0000
    mov bx, 0x7E00          ; BX = 0x7E00 (offset into segment)
    
    mov ah, 0x02            ; INT 13h AH=0x02: read sectors (CHS)
    mov al, LOADER_SECTORS  ; number of sectors to read
    mov ch, 0               ; cylinder 0
    mov cl, 2               ; sector 2 (loader starts at sector 1, but for hard drive geometry, LBA 1 = cl=2)
    mov dh, 0               ; head 0
    mov dl, 0x80            ; drive (hard drive)
    int 0x13
    
    jc .load_error

.load_success:
    ; Print success message
    mov si, msg_success
    call print_string
    call delay

    ; Reset DS to be safe
    xor ax, ax
    mov ds, ax

    cli

    ; Push return address and jump via retf
    ; Push segment first (LIFO), then offset
    push 0x0000     ; segment
    push 0x7E00     ; offset
    retf            ; far return = far jump
    
    ; Should never reach here
    cli
    hlt

.load_error:
    cli
    hlt

print_string:
    mov ah, 0x0E
.ps_loop:
    lodsb
    test al, al
    jz .ps_done
    int 0x10
    jmp .ps_loop
.ps_done:
    ret

; Delay function - provides a visible delay for messages
; Approximately 2-3 seconds on typical systems
delay:
    push ax
    push cx
    push dx
    mov ax, 0x0040      ; Outer loop counter (64 iterations)
.delay_outer:
    mov cx, 0xFFFF      ; Middle loop counter (65535 iterations)
.delay_middle:
    mov dx, 0x00FF      ; Inner loop counter (255 iterations)
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

msg_loading:        db "[BOOT] Loading kernel loader...", 0x0D, 0x0A, 0
msg_success:        db "[BOOT] Loader loaded successfully!", 0x0D, 0x0A, 0
msg_start:          db "[BOOT] Bootloader started", 0x0D, 0x0A, 0

boot_drive: db 0

times (512 - 2 - ($-$$)) db 0
dw 0xAA55