### Kstudy

* Kstudy implements "system call" functions in a simple kernel module
* The channel between user space and kernel space is "/dev/kstudy", a virtaul charactor device 
* This program is used only to the beginners of the kernel, for studying purpose only 
* This program is tested in kernel v3.18.137
* Kernel source and symbols in: https://elixir.bootlin.com/linux/v3.18.137/source

### Make and install 

* Step1: Open ./Makefile, change LINUX_KERNEL_PATH := /usr/src/kernels/linux-3.18.137 to your kernel src
* Step2: make - make kernel module and ktest 
* Step3: make install - install kernel module
* Clean: make clean - if you want to clean your kstudy directory before rebuild

### Usage

* Usage1: ./ktest reg - show registers
* Usage2: ./ktest superblock - show super_block info
* Usage3: ./ktest mem {kernel addr in hex} {line num in hex} - inspect kernel mem

### Add a new syscall in module

* Step1: Define {your_call} in kstudy.h
* Step2: Add entrance in main.c, static ssize_t device_read()
* Step3: Create {your_call}.c to implement the function
* Step4: Add caller in ktest.c 
