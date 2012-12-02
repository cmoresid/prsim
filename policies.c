#include <stdlib.h>

#include "policies.h"
#include "mempool.h"

extern int pagefaults;
extern int flushes;
extern int totalframes;

extern mempool* inmem_pages_node_pool;

void fifo_add_page_mem_policy(page_table* pt, node* pte) {
	pt->freeframes--;	
	llist_enqueue_pte_ref(pt->inmem_pages, pte);
}

void fifo_replacement_policy(page_table* pt, node* pte) {
	node* discarded_page_pte;
	
	// Get the PTE of the page to be discarded
	discarded_page_pte = llist_dequeue(pt->inmem_pages);

	// If it was dirty, write back to memory
	if (IS_PTE_DIRTY(discarded_page_pte->pte->data)) {
		flushes++;
	}
	
	llist_enqueue_pte_ref(pt->inmem_pages, pte);
	
	// It is not valid now, discard it
	discarded_page_pte->pte->data = 0;
	free(discarded_page_pte);
}

void random_add_page_mem_policy(page_table* pt, node* pte) {
	pt->freeframes--;
	llist_enqueue_pte_ref(pt->inmem_pages, pte);
}

void random_replacement_policy(page_table* pt, node* pte) {
	node* discarded_page_pte;
	int random_page_index = rand() % totalframes;
	int i = 0;
	node* discarded_page = pt->inmem_pages->head;
	
	while (i < random_page_index) {
		discarded_page = discarded_page->next;
		i++;
	}
	
	discarded_page_pte = llist_remove(pt->inmem_pages, discarded_page);
	
	if (IS_PTE_DIRTY(discarded_page_pte->pte->data)) {
		flushes++;
	}

	llist_enqueue_pte_ref(pt->inmem_pages, pte);
	
	// It is not valid now, discard it
	discarded_page_pte->pte->data = 0;
	free(discarded_page_pte);
}

void lru_add_page_mem_policy(page_table* pt, node* pte) {
	pt->freeframes--;	
	llist_insert_pte_ref(pt->inmem_pages, pte);
}

void lru_replacement_policy(page_table* pt, node* pte) {
	node* discarded_page_pte;
	
	discarded_page_pte = llist_remove(pt->inmem_pages, pt->inmem_pages->tail);
	
	if (IS_PTE_DIRTY(discarded_page_pte->pte->data)) {
		flushes++;
	}
	
	llist_insert_pte_ref(pt->inmem_pages, pte);
	
	discarded_page_pte->pte->data = 0;
	free(discarded_page_pte);
}


