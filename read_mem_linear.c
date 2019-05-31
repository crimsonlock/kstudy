/*
 *  read_mem_linear.c - read memory in kernel mod, for study x86_64/Linux 
 *  
 *  2019 Li Shaopeng
 */

#include<linux/kernel.h>
#include<linux/init.h>
#include "kstudy.h"

//msg : read mem to msg[]
//p1 : base address in unsigned long
//p2 : start line
//p3 : end line
void kstudy_read_mem_linear(char *msg, unsigned long p1, unsigned long p2,
			    unsigned long p3)
{

	int i = 0;
	unsigned long *ptr = (unsigned long *)p1;
	ptr += p2;

	for (; p2 <= p3; p2++) {
		sprintf(msg + i * 41, "%04ld [%p] %016lx\n", p2, ptr, *ptr);
		ptr++;
		i++;
	}

}
