# RusticOS - Simple x86_64 Operating System

A minimal operating system development environment built with NASM and QEMU for learning OS development concepts.

## Features

- **Bootloader**: Simple BIOS bootloader that loads the kernel
- **Kernel**: C++ kernel with text output capabilities, keyboard input, and command-line interface
- **Command-Line Interface**: Interactive shell with commands for filesystem operations
- **Filesystem**: In-memory hierarchical filesystem with directory and file operations
- **Build System**: Makefile for easy development
- **Emulation**: QEMU integration for testing without real hardware

## Prerequisites

### Required Software

1. **NASM** (Netwide Assembler) - for compiling assembly code
2. **QEMU** - for emulating the x86_64 system
3. **Make** - for building the project
4. **dd** - for creating disk images (usually pre-installed on Linux)

### Installation

#### Arch Linux
```bash
sudo pacman -S nasm qemu make
```

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install nasm qemu-system-x86 make
```

#### macOS
```bash
brew install nasm qemu make
```

## Project structure

- `boot/`: bootloader and loader assembly sources and binaries
- `src/`: kernel sources (C++), startup code, and system components
  - `kernel.cpp`: Main kernel entry point and command loop
  - `terminal.cpp`: VGA text-mode display interface
  - `keyboard.cpp`: PS/2 keyboard input handling
  - `command.cpp`: Command parsing and execution system
  - `filesystem.cpp`: In-memory filesystem implementation
- `build/`: intermediate object files and build artifacts (generated)
- `scripts/`: helper scripts for building and running

## Common commands

```bash
# Build everything
make all

# Run with VNC display
make run

# Run in debug mode (no reboot on halt)
make run-debug

# Run test with serial output
make run-test

# Clean build artifacts
make clean

# Clean everything (including source object files)
make distclean
```

## How It Works

### 1. Bootloader (bootloader.asm)
- Loaded by BIOS at memory address `0x7c00`
- Runs in 16-bit real mode
- Loads the second-stage loader from disk

### 2. Loader (loader.asm)
- Loaded by the bootloader
- Switches to 32-bit protected mode
- Loads the kernel from disk into memory
- Jumps to the kernel entry point

### 3. Kernel (kernel.cpp)
- Written in C++ with assembly startup code (crt0.s)
- Provides interactive command-line interface
- Handles keyboard input via polling
- Manages an in-memory filesystem
- Supports commands: help, mkdir, cd, ls, pwd, touch, cat, write, echo, clear

### 4. Build Process
1. NASM compiles bootloader and loader assembly files to binary format
2. GCC/G++ compiles C++ kernel sources to object files
3. Linker creates the kernel ELF binary, then converts to flat binary
4. `dd` creates a disk image
5. Bootloader, loader, and kernel are written to the image in sequence
6. QEMU boots from the image

## Development

### Adding Features

#### Extending the Kernel
- Modify `kernel.cpp` to add new kernel functionality
- Add new C++ source files in `src/`
- Update the Makefile `KERNEL_SOURCES` variable to include new files
- Add new commands in `command.cpp`

#### Adding New Commands
1. Implement command handler in `command.cpp` (e.g., `cmd_newcommand`)
2. Add command dispatch in `CommandSystem::execute_command()`
3. Update `cmd_help()` to include the new command
4. Optionally update `interface_and_filesystem.md` documentation

### Debugging

#### QEMU Debug Mode
```bash
make run-debug
```
This starts QEMU in debug mode where the system won't reboot on halt. For full GDB integration, you can manually start QEMU with:
```bash
qemu-system-x86_64 -drive file=build/disk.img,format=raw,if=ide -boot c -m 512M -serial stdio -s -S
```
Then connect GDB with:
```bash
gdb build/kernel.elf
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
```

#### Common Issues
- **"Permission denied"**: Make sure `build.sh` is executable (`chmod +x build.sh`)
- **"Command not found"**: Install required packages (NASM, QEMU)
- **Build errors**: Check assembly syntax and Makefile dependencies

## Learning Resources

- [NASM Documentation](https://www.nasm.us/doc/)
- [QEMU Documentation](https://qemu.readthedocs.io/)
- [OSDev Wiki](https://wiki.osdev.org/)
- [x86 Assembly Guide](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html)

## Documentation

For detailed information about the command-line interface and filesystem, see [interface_and_filesystem.md](interface_and_filesystem.md).

## Next Steps

This is a basic foundation. Consider adding:
- Memory management (heap allocator)
- Process scheduling
- Persistent file storage (save filesystem to disk)
- Interrupt-driven keyboard (replace polling)
- More commands (rm, mv, cp, etc.)
- Command history and tab completion
- Networking stack
- Device drivers for additional hardware

## License

This project is open source and available under the MIT License.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this OS development environment. 