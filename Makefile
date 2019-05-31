# Compile memread.c as a kernel module
# Notice : add the following line after  "make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules", if your kernel has a signature
# $(LINUX_KERNEL_PATH)/scripts/sign-file sha512 $(LINUX_KERNEL_PATH)/signing_key.priv $(LINUX_KERNEL_PATH)/signing_key.x509 $(MOD_NAME) 

# Put your kernel src path here
LINUX_KERNEL_PATH := /usr/src/kernels/linux-3.18.137
MOD_NAME := kstudy

obj-m := $(MOD_NAME).o
$(MOD_NAME)-objs := main.o read_mem_linear.o read_register.o superblock.o

CURRENT_PATH := $(shell pwd)
HAVE_MODE := $(shell lsmod | grep $(MOD_NAME))

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
	gcc ktest.c -o ktest
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -f ktest
	rm -f .ksymv
install:
	@if [ -n "$(HAVE_MODE)" ]; then \
		rmmod $(MOD_NAME);\
	fi
	insmod $(MOD_NAME).ko
	@echo "module $(MOD_NAME) installed"
	@echo "device created at /dev/$(MOD_NAME)"
	chmod 440 /dev/$(MOD_NAME)
