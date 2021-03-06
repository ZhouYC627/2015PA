#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)

/* Use the function to get the start address of user page directory. */
inline PDE* get_updir();
static PTE vptable[NR_PTE] align_to_page;

void create_video_mapping() {
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	//panic("please implement me");
	PDE* pdir = get_updir();
	PTE* ptable = (PTE*)va_to_pa(vptable);
	/*updir[VMEM_ADDR/PT_SIZE].val = make_pde((void*)((uint32_t)(vptable) - KOFFSET));
	uint32_t ptable_idx;
	for (ptable_idx = VMEM_ADDR/PAGE_SIZE;ptable_idx*PAGE_SIZE < VMEM_ADDR+SCR_SIZE; ptable_idx++)
	vptable[ptable_idx].val = make_pte(ptable_idx<<12);
	*/
	pdir[0].val = make_pde(ptable);
	int i = 0;
	uint32_t pframe_addr = VMEM_ADDR;
	ptable = ptable + 0xa0;
	for ( ; i<16; i++){
		ptable->val = make_pte(pframe_addr);
		pframe_addr += 4096;
		ptable++; 
	}

}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

