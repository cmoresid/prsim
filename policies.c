#include <stdlib.h>

#include "policies.h"

extern int pagefaults;
extern int flushes;

void fifo_add_page_mem_policy(page_table* pt, node* pte) {
	node* free_frame = llist_dequeue(pt->free_frames);
	
	pte->data = ((pte->data & 0xC0000000) | (0x3FFFFFFF & free_frame->data));
	free(free_frame);
	
	llist_enqueue(pt->inmem_pages, pte->key, pte->data);
}

void fifo_replacement_policy(page_table* pt, node* pte) {
	node* discarded_page_pte;
	node* discarded_page;
	
	// Get page # to be removed from memory
	discarded_page = llist_dequeue(pt->inmem_pages);
	// Get the PTE of the page to be discarded
	discarded_page_pte = pt_get_pte(pt, discarded_page->key);

	// If it was dirty, write back to memory
	if (IS_PTE_DIRTY(discarded_page_pte->data)) {
		flushes++;
	}
	
	// Now place pte in the frame where discarded_page_pte
	// was
	pte->data &= ((pte->data & 0xC0000000) | (0x3FFFFFFF & discarded_page_pte->data));
	llist_enqueue(pt->inmem_pages, pte->key, pte->data);
	
	// It is not valid now, discard it
	discarded_page_pte->data = 0;
	free(discarded_page);
	discarded_page = NULL;
}

void random_add_page_mem_policy(page_table* pt, node* pte) {

}

void random_replacement_policy(page_table* pt, node* pte) {

}

void lru_add_page_mem_policy(page_table* pt, node* pte) {

}

void lru_replacement_policy(page_table* pt, node* pte) {

}


