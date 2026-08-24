![C Language](https://img.shields.io/badge/Language-C-%2300599C?style=for-the-badge&logo=c&logoColor=white)
![Assembly](https://img.shields.io/badge/Arch-x86_Assembly-%23E34F26?style=for-the-badge&logo=assemblyscript&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Bare_Metal_%2F_QEMU-%2341B883?style=for-the-badge&logo=linux&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)
# VG OS 🚀

A custom x86 operating system built from scratch for an academic presentation, featuring a custom bootloader, custom kernel, and basic keyboard input handling.

## 📋 Features
* **Custom Bootloader:** Configured using GRUB.
* **Custom Kernel:** Written in C and Assembly, running in protected mode.
* **Video Memory Text Output:** Direct VGA buffer rendering (`0xB8000`) for logos and text prompts.
* **Keyboard Support:** Basic interrupt/scancode mapping for keyboard interaction.

## 🛠️ Technologies Used
* **C & Assembly (x86)**
* **GCC & GNU Assembler**
* **QEMU Emulator**
* **WSL (Ubuntu on Windows)**

## 🚀 How to Run
1. Download the latest `.iso` file from the **Releases** section.
2. Run it using QEMU:
   ```bash
   qemu-system-x86_64 -cdrom vgos_final.iso
## 📚 Acknowledgments & References
* **[OSDev Wiki](https://wiki.osdev.org/)** - For providing invaluable tutorials, documentation, and the foundational "Bare Bones" guide used to build this operating system.
