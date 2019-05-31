/*
 *  superblock.c - read super_block info in kernel mod, for study x86_64/Linux 
 *  
 *  2019 Li Shaopeng
 */
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include "kstudy.h"


//msg : return msg
//p1 : addr for sym "super_blocks"
//p2 : addr for sym "inode_sb_list_lock"
void kstudy_superblock(char *msg,unsigned long p1, unsigned long p2){

	//superblock count
	unsigned long sbnum = 0;

	//inode count for each superblock
	unsigned long innum = 0; 

	//spinlock for inode->i_sb_list, defined in /fs/inode.c
	spinlock_t * inode_sb_list_lock = (spinlock_t *)p2;

	//super_blocks, superblock list head define in /fs/super.c
	struct list_head * head = (struct list_head *)p1;

	//init tmp var
	struct list_head * next = head->next;
	struct list_head * inodes_head;
	struct list_head * inodes_next;
	struct super_block * sb;

	//column head
	sprintf(KSTUDY_PTR(msg),"       dev     %-16s%-16s %-16s %-16s %-16s%-10s\n","fs_type","super_block_name","dentry_name","parent_dentry","block_size","inodes");

	//print each super block info
	while( next != head ){
		sb = (struct super_block *)next;
		sprintf(KSTUDY_PTR(msg), "%04ld : %03d-%03d %-16s",sbnum,MAJOR(sb->s_dev),MINOR(sb->s_dev),sb->s_type->name);
		sprintf(KSTUDY_PTR(msg), "%-16s %-16s %-16p %-16lu",sb->s_id,sb->s_root->d_iname,sb->s_root->d_inode,sb->s_blocksize);

		//count inodes in this superblock
		spin_lock(inode_sb_list_lock);
		inodes_head = &(sb->s_inodes);
		inodes_next = inodes_head->next;
		while ( inodes_next != inodes_head ){
			innum++;
			inodes_next = inodes_next->next;
		}		
		spin_unlock(inode_sb_list_lock);
		sprintf(KSTUDY_PTR(msg) , "%-10ld\n", innum);

		innum = 0;
		sbnum++;
		next = next->next;
	}
	
	
	sprintf(KSTUDY_PTR(msg), "struct super_block size = %ld\n", sizeof(struct super_block));
}


