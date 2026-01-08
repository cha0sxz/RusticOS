# RusticOS Command-Line Interface and Filesystem

## Overview
RusticOS now includes a fully functional command-line interface with a basic filesystem. Users can type commands at the kernel prompt and interact with a root filesystem mounted at "/".

## Features Implemented

### Command-Line Interface
- **Interactive prompt**: The kernel displays a "> " prompt where users can type commands
- **Keyboard input**: Full keyboard support with character input, backspace, and enter
- **Command execution**: Commands are parsed and executed in real-time
- **Error handling**: Invalid commands show appropriate error messages

### Filesystem
- **Root directory**: A filesystem mounted at "/" (root)
- **Directory operations**: Create and navigate directories
- **File operations**: Create, read, and write files
- **Memory allocation**: Uses dynamic allocation with `new`/`delete` for filesystem nodes
- **Hierarchical structure**: Supports parent-child directory relationships

### Available Commands

#### `help`
- **Usage**: `help`
- **Description**: Lists all available commands with brief descriptions
- **Example**: 
  ```
  > help
  Available commands: help, clear, echo, mkdir, cd, ls, pwd, touch, cat, write
  ```

#### `mkdir`
- **Usage**: `mkdir <directory_name>`
- **Description**: Creates a new directory in the current location
- **Example**:
  ```
  > mkdir testdir
  Directory 'testdir' created successfully
  ```

#### `cd`
- **Usage**: `cd <path>`
- **Description**: Changes the current directory
- **Special paths**:
  - `cd /` - Go to root directory
  - `cd ..` - Go to parent directory
- **Example**:
  ```
  > cd testdir
  > cd ..
  > cd /
  ```

#### `pwd`
- **Usage**: `pwd`
- **Description**: Prints the current working directory
- **Note**: Currently always displays "/" regardless of current directory (full path resolution not yet implemented)
- **Example**:
  ```
  > pwd
  /
  ```

#### `ls`
- **Usage**: `ls`
- **Description**: Lists contents of the current directory. Directories are shown with a trailing "/"
- **Example**:
  ```
  > ls
  testdir/
  myfile.txt
  ```

#### `clear`
- **Usage**: `clear`
- **Description**: Clears the screen and redraws the header
- **Example**:
  ```
  > clear
  [Screen clears and shows fresh prompt]
  ```

#### `echo`
- **Usage**: `echo <text>`
- **Description**: Prints the specified text
- **Example**:
  ```
  > echo Hello, RusticOS!
  Hello, RusticOS!
  ```

#### `touch`
- **Usage**: `touch <filename>`
- **Description**: Creates an empty file with the specified name
- **Example**:
  ```
  > touch myfile.txt
  File created: myfile.txt
  ```

#### `cat`
- **Usage**: `cat <filename>`
- **Description**: Displays the contents of a file
- **Example**:
  ```
  > cat myfile.txt
  Hello, World!
  ```

#### `write`
- **Usage**: `write <filename> <content>`
- **Description**: Writes content to an existing file
- **Note**: The file must already exist (created with `touch` first)
- **Example**:
  ```
  > touch myfile.txt
  > write myfile.txt Hello, World!
  File written: myfile.txt
  > cat myfile.txt
  Hello, World!
  ```

## Technical Implementation

### Architecture
- **Kernel**: Main kernel loop with keyboard polling
- **Terminal**: VGA text-mode display with cursor management
- **Keyboard**: PS/2 keyboard input handling
- **Filesystem**: In-memory filesystem with static allocation
- **Command System**: Command parser and executor

### Memory Management
- **Dynamic allocation**: Filesystem nodes are allocated with `new`/`delete`
- **File data**: File contents are dynamically allocated and can be resized
- **Limitations**: Maximum of 64 entries per directory, 32 character name limit, 256 character path limit

### File Structure
```
src/
├── kernel.cpp      # Main kernel with command loop
├── terminal.h/cpp  # VGA terminal interface
├── keyboard.h/cpp  # Keyboard input handling
├── filesystem.h/cpp # Filesystem implementation
└── command.h/cpp   # Command parsing and execution
```

## Usage Examples

### Basic Navigation
```
> help
> mkdir projects
> cd projects
> pwd
> mkdir myproject
> cd myproject
> pwd
> cd ../..
> pwd
```

### File Operations
```
> touch example.txt
> write example.txt Hello, World!
> cat example.txt
Hello, World!
> echo "Testing echo command"
Testing echo command
> clear
> help
```

## Building and Running

### Build
```bash
make clean
make all
```

### Run
```bash
# Headless mode (no display)
make run-headless

# With VNC display
make run

# With curses display
make run-curses
```

## Future Enhancements

### Planned Features
- **File deletion**: `rm` command to delete files
- **File moving/copying**: `mv` and `cp` commands
- **Path resolution**: Full path resolution for `pwd` and relative paths in `cd`
- **Persistent storage**: Save filesystem to disk and load on boot
- **Directory deletion**: Enhanced `rmdir` with recursive deletion
- **More commands**: Additional utilities and system commands

### Technical Improvements
- **Interrupt-driven keyboard**: Replace polling with proper interrupts
- **Memory management**: Dynamic allocation with heap management
- **Persistent storage**: Save filesystem to disk
- **Command history**: Up/down arrow support
- **Tab completion**: Auto-complete commands and paths

## Notes
- The filesystem is currently in-memory only and resets on reboot
- Keyboard input uses polling (not interrupt-driven)
- File content storage is not yet implemented
- The system is designed for educational purposes and demonstrates basic OS concepts
