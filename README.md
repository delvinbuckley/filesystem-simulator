# filesystem-simulator

A **C++ in-memory filesystem simulator** that models block storage, inodes, and directories.  
Supports creating, writing, reading, deleting, and listing files via an interactive shell.  
Includes custom exceptions for error handling (e.g., `FileMissingException`, `FileAlreadyExistsException`).

---

## ✨ Features
- Block allocation and freeing through a **Block Manager**  
- **Inodes** that track file size, timestamps, and data block pointers  
- A **Directory** mapping filenames to inode indices  
- High-level **FileSystem API** for file operations  
- Interactive command-line shell (`fs>`)  
- Descriptive error handling with custom C++ exceptions  

---

## 🛠️ Build Instructions

### Linux / WSL
```bash
make
```

### Windows
```bash
mingw32-make
```

### How to Run
```bash
./filesystem
```

### Available Commands
```lua
format                        - format the filesystem
create <filename>             - create a new empty file
write <filename> <data...>    - write data to a file
read <filename>               - read and display file content
delete <filename>             - delete a file
ls                            - list all files
stats                         - show block usage stats
help                          - show help menu
exit                          - exit the shell
