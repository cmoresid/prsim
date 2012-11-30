#include <stdlib.h>

#include "policies.h"

extern int pagefaults;
extern int flushes;

void fifo_add_page_mem_policy(page_table* pt, uint32_t pagenum) {
	llist_enqueue(pt->free_frames, pagenum, pagenum);
}

void fifo_replacement_policy(page_table* pt, uint32_t pagenum) {
	node* discarded_page_pte;
	node* discarded_page;
	
	discarded_page = llist_dequeue(pt->inmem_pages);
	if (IS_PTE_DIRTY(discarded_page->data)) {
		flushes++;
	}
	
	discarded_page_pte = pt_get_pte(pt, discarded_page->key);
	TOGGLE_PTE_VALID(discarded_page_pte->data);
	
	free(discarded_page);
	discarded_page = NULL;
	
	//llist_enqueue(
}

void random_add_page_mem_policy(page_table* pt, uint32_t pagenum) {

}

void random_replacement_policy(page_table* pt, uint32_t pagenum) {

}

void lru_add_page_mem_policy(page_table* pt, uint32_t pagenum) {

}

void lru_replacement_policy(page_table* pt, uint32_t pagenum) {

}


