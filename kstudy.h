/**
 * kstudy.h - define "syscall" functions
 *
 * 2019 Li Shaopeng
 *
 */

//module name
#define KSTUDY_DEV_NAME "kstudy"

//message buffer size
#define KSTUDY_MSG_SIZE 8192

//"syscall" number 
#define KSTUDY_FUNC_READ_MEM_LINEAR 0
#define KSTUDY_FUNC_READ_REGISTER 1
#define KSTUDY_FUNC_SUPERBLOCK 2

//get the end ptr for "a"
#define KSTUDY_PTR(a) a+strlen(a)

//*msg - buffer in kernel space
//p1,p2,p3,p4 - 8 bytes integer as parameters
void kstudy_read_mem_linear(char *msg, unsigned long p1, unsigned long p2,
			    unsigned long p3);
void kstudy_read_register(char *msg);
void kstudy_superblock(char *msg,unsigned long p1,unsigned long p2);

