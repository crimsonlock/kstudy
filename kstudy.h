
#define KSTUDY_DEV_NAME "kstudy"
#define KSTUDY_MSG_SIZE 8192

#define KSTUDY_FUNC_READ_MEM_LINEAR 0
#define KSTUDY_FUNC_READ_REGISTER 1
#define KSTUDY_FUNC_SUPERBLOCK 2

#define KSTUDY_PTR(a) a+strlen(a)

void kstudy_read_mem_linear(char *msg, unsigned long p1, unsigned long p2,
			    unsigned long p3);
void kstudy_read_register(char *msg);
void kstudy_superblock(char *msg,unsigned long p1,unsigned long p2);

