/*
 *  read_register.c - read registers in kernel mod, for study x86_64/Linux 
 *  2019 Li Shaopeng
 */

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/string.h>
#include "kstudy.h"

//read cpuid info
//fc : func number of cpuid instruction , see page 292, Intel developer's manual, volume2(abcd)
//you can read other cpuid info according to the following samples
static void read_cpuid(char *msg, int fc)
{

	unsigned long long reg[4] = { 0, 0, 0, 0 };
	__asm__ __volatile__("movq %[fc],%%rax;\n"
			     "movq $0x0,%%rbx;\n"
			     "movq $0x0,%%rcx;\n"
			     "movq $0x0,%%rdx;\n"
			     "cpuid":"=a"(reg[0]),
			     "=b"(reg[1]), "=c"(reg[2]), "=d"(reg[3])
			     :[fc] "m"(fc)
	    );

	//get the apicid of the processor(logical) that is processing the current instruction
	if (fc == 1) {
		sprintf(KSTUDY_PTR(msg), "dump_reg: CPU-APICID = %llx\n",
			(reg[1] & 0xFF000000) >> 24);
	}
}

//read general regs : rbp,rsp,rip,rflags
//read segment regs : cs ds ss es gs fs ldtr tr gdtr(gdt) idtr(idt)
//read control regs : cr0 cr2 cr3 cr4
//read msr : IA32_EFER IA32_LSTAR
static void read_regs(char *msg)
{

	//init local storage
	unsigned long long reg = 0;
	unsigned int eregax = 0;
	unsigned int eregdx = 0;
	unsigned char mg[10] = { 0 };

	//clear interrupt flag
	__asm__ __volatile__("cli");

	//print rbp     
	__asm__ __volatile__("movq %%rbp,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: RBP = 0x%016llx\n", reg);

	//print rsp     
	__asm__ __volatile__("movq %%rsp,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: RSP = 0x%016llx\n", reg);

	//print rip     
	__asm__ __volatile__("call .+5;\n" "pop %%rax":"=a"(reg)
	    );
	sprintf(KSTUDY_PTR(msg), "dump_reg: RIP = 0x%016llx\n", reg);

	//print rflags
	__asm__ __volatile__("pushfq;\n" "pop %%rax":"=a"(reg)
	    );
	sprintf(KSTUDY_PTR(msg), "dump_reg: RFLAGS = 0x%016llx\n", reg);

	//print cs, user mode
	__asm__ __volatile__("movq %%cs,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: CS = 0x%04llx\n", reg);

	//print ss, user mode
	__asm__ __volatile__("movq %%ss,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: SS = 0x%04llx\n", reg);

	//print ds, user mode
	__asm__ __volatile__("movq %%ds,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: DS = 0x%04llx\n", reg);

	//print es, user mode
	__asm__ __volatile__("movq %%es,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: ES = 0x%04llx\n", reg);

	//print fs, user mode
	__asm__ __volatile__("movq %%fs,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: FS = 0x%04llx\n", reg);

	//print gs, user mode
	__asm__ __volatile__("movq %%gs,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: GS = 0x%04llx\n", reg);

	//print ldtr
	__asm__ __volatile__("sldt %%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: LDTR = 0x%04llx\n", reg);

	//print tr
	__asm__ __volatile__("str %%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: TR = 0x%04llx\n", reg);

	//print gdt, user mode
	__asm__ __volatile__("sgdt %[mm]":[mm] "=m"(mg[0]));
	sprintf(KSTUDY_PTR(msg),
		"dump_reg: GDTR = BASE-0x%02x%02x%02x%02x%02x%02x%02x%02x LIMIT-%02x%02x\n",
		mg[9], mg[8], mg[7], mg[6], mg[5], mg[4], mg[3], mg[2], mg[1],
		mg[0]);

	//print idt, user mode
	__asm__ __volatile__("sidt %[mm]":[mm] "=m"(mg[0]));
	sprintf(KSTUDY_PTR(msg),
		"dump_reg: IDTR = BASE-0x%02x%02x%02x%02x%02x%02x%02x%02x LIMIT-%02x%02x\n",
		mg[9], mg[8], mg[7], mg[6], mg[5], mg[4], mg[3], mg[2], mg[1],
		mg[0]);

	//print cr0, user mode
	//__asm__ __volatile__ ("smsw %[ax]":[ax] "=r" (reg));
	//sprintf(KSTUDY_PTR(msg),"dump_reg: CR0 = 0x%016llx\n",reg);

	//print cr0
	__asm__ __volatile__("movq %%cr0,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg),
		"dump_reg: CR0 = 0x%016llx (PE-%llx MP-%llx EM-%llx TS-%llx ET-%llx NE-%llx WP-%llx AM-%llx NW-%llx CD-%llx PG-%llx)\n",
		reg, reg & 0x1, (reg & 0x2) >> 1, (reg & 0x4) >> 2,
		(reg & 0x8) >> 3, (reg & 0x10) >> 4, (reg & 0x20) >> 5,
		(reg & 0x10000) >> 16, (reg & 0x40000) >> 18,
		(reg & 0x20000000) >> 29, (reg & 0x40000000) >> 30,
		(reg & 0x80000000) >> 31);

	//print cr2
	__asm__ __volatile__("movq %%cr2,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: CR2 = 0x%016llx\n", reg);

	//print cr3
	__asm__ __volatile__("movq %%cr3,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg),
		"dump_reg: CR3 = 0x%016llx (PWT-%llx PCD-%llx PDB-0x%016llx)\n",
		reg, (reg & 0x8) >> 3, (reg & 0x10) >> 4, reg >> 12);

	//print cr4
	__asm__ __volatile__("movq %%cr4,%%rax":"=a"(reg));
	sprintf(KSTUDY_PTR(msg), "dump_reg: CR4 = 0x%016llx\n", reg);

	//read MSR registers with instruction "rdmsr"
	//refs : page 1186, Intel software developer's manual, volume2(abcd)
	//refs : page 60, Intel software developer's manual, volume4(model-specific registers)
	//you can read other MSR registers according to the following samples
	//print IA32_EFER
	__asm__ __volatile__("movq $0xc0000080,%%rcx;\n" "rdmsr":"=a"(reg)
	    );
	sprintf(KSTUDY_PTR(msg),
		"dump_reg: IA32_EFER = 0x%016llx (SCE-%llx LME-%llx LMA-%llx NXE-%llx)\n",
		reg, reg & 0x1, (reg & 0x100) >> 8, (reg & 0x400) >> 10,
		(reg & 0x800) >> 11);

	//print IA32_LSTAR : syscall entry address in IA-32e
	__asm__ __volatile__("movq $0xc0000082,%%rcx;\n"
			     "rdmsr;\n":"=a"(eregax), "=d"(eregdx)
	    );
	sprintf(KSTUDY_PTR(msg), "dump_reg: IA32_LSTAR = 0x%08x%08x\n", eregdx,
		eregax);

	//set interrupt flag
	__asm__ __volatile__("sti");

}

void kstudy_read_register(char *msg)
{
	read_cpuid(msg, 1);
	read_regs(msg);
	return;
}
