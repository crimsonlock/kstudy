#include<stdio.h>
#include<string.h>

#define BUF_SIZE 8192

char block[BUF_SIZE] = { 0 };

FILE *fp;


int tolower(int c)
{
	if (c >= 'A' && c <= 'Z') {
		return c + 'a' - 'A';
	} else {
		return c;
	}
}

unsigned long hex2dec(char *s)
{
	int i = 0;
	unsigned long n = 0;
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
		i = 2;
	} else {
		i = 0;
	}

	for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z')
	     || (s[i] >= 'A' && s[i] <= 'Z'); ++i) {
		if (tolower(s[i]) > '9') {
			n = 16 * n + (10 + tolower(s[i]) - 'a');
		} else {
			n = 16 * n + (tolower(s[i]) - '0');
		}
	}
	return n;
}

unsigned long get_symbol_addr(char *sym){
	char cmd[50] = {0};
	char addr[20] = {0};

	sprintf(cmd , "cat /proc/kallsyms | grep \\ %s$ | cut -c1-16 > .ksymv",sym);
	system(cmd);
	
	fp = fopen(".ksymv","r");
	fread(addr,1,16,fp);
	fclose(fp);
		
	return hex2dec(addr);
}

void init_param(unsigned long p0, unsigned long p1, unsigned long p2,
		unsigned long p3)
{

	unsigned long *cb;
	cb = (unsigned long *)block;
	memset(block, 0, BUF_SIZE);
	cb[0] = p0;
	cb[1] = p1;
	cb[2] = p2;
	cb[3] = p3;

}

void read_mem_linear(unsigned long addr, unsigned long cnum)
{
	int i;
	fp = fopen("/dev/kstudy", "r");

	for (i = 0; i < cnum; i += 20) {
		init_param(0, addr + i * 8, i,
			   (i + 19) < cnum ? (i + 19) : (cnum - 1));
		fread(block, 1 , BUF_SIZE, fp);
		printf("%s", block);
	}

	fclose(fp);

}

void read_register()
{

	init_param(1, 0, 0, 0);

	fp = fopen("/dev/kstudy", "r");
	fread(block, 1 , BUF_SIZE, fp);
	fclose(fp);

	printf("%s", block);

}


void superblock()
{

	init_param(2, get_symbol_addr("super_blocks"), get_symbol_addr("inode_sb_list_lock"), 0);

	fp = fopen("/dev/kstudy", "r");
	fread(block, 1 , BUF_SIZE, fp);
	fclose(fp);

	printf("%s", block);

}

void main(int argc, char *argv[], char *env[])
{

	if (argc < 2) {
		printf("Need parameter\n");
		return;
	}

	if (!strcmp(argv[1], "mem")) {
		if (argc < 4) {
			printf("Bad parameter\n");
			return;
		}
		read_mem_linear(hex2dec(argv[2]), hex2dec(argv[3]));
		return;
	}

	if (!strcmp(argv[1], "reg")) {
		read_register();
		return;
	}

	if (!strcmp(argv[1], "superblock")) {
		superblock();
		return;
	}
}
