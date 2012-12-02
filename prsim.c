/*
 * =====================================================================================
 *
 *       Filename:  prsim.c
 *
 *    Description:  Simulates a virtual memory environment for a single process.
 *
 *        Version:  1.0
 *        Created:  28/11/2012 21:16:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Connor Moreside (conman720), cmoresid@ualberta.ca
 *   Organization:  CMPUT379
 *
 * =====================================================================================
 */

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "prsim.h"
#include "policies.h"
#include "dstruct.h"

const int pt_size = 256;

int totalframes = 0;
int memrefs = 0;
int pagefaults = 0;
int flushes = 0;

// Counting from left to right
// bit 1, bit 2, bit 3, etc...
uint32_t create_bitmask(uint32_t firstbit, uint32_t lastbit) {
	uint32_t r = 0;
	uint32_t i;
	
	for (i = 32-firstbit; i >= (32-lastbit); i--) {
		r |= 1 << i;
		if (i == 0)
			break;
	}
	
	return r;
}

int start_simulation(char strategy, int pagesize, int memsize) {
	uint32_t buff[SIZE];
	size_t n;
	int i;
	uint32_t offsetbits;
	uint32_t pagenumbits;
	uint32_t pagenum_mask;
	page_table* pt;
	
	// Set total number of physical frames
	if ( (totalframes = memsize / pagesize) == 1 ) {
		printf("** Need at least 2 frames to be interesting.\n");
		exit(1);
	}
	
	switch (strategy) {
		case 'f':
			pt = pt_new(pt_size, totalframes, fifo_add_page_mem_policy, fifo_replacement_policy);
			break;
		case 'r':
			srand(time(NULL));
			pt = pt_new(pt_size, totalframes, random_add_page_mem_policy, random_replacement_policy);
			break;
		case 'l':
			pt = pt_new(pt_size, totalframes, lru_add_page_mem_policy, lru_replacement_policy);
			break;
	}
	
	offsetbits = log2(pagesize);
	pagenumbits = ADDRESS_WIDTH - offsetbits;
	pagenum_mask = create_bitmask(1, pagenumbits);
	
	while ( (n = read(0, &buff, sizeof(buff))) == sizeof(buff) ) {
		for (i = 0; i < (n/sizeof(uint32_t)); i++) {
			pt_load_page(pt, buff[i], pagenum_mask & buff[i]);
			memrefs++;
		}
	}
	
	if (n < sizeof(buff)) {
		for (i = 0; i < (n/sizeof(uint32_t)); i++) {
			pt_load_page(pt, buff[i], pagenum_mask & buff[i]);
			memrefs++;
		}
	}
	
	// Done, now print statistics
	print_statistics(pagesize, memsize);
	
	return 0;
}

inline void print_statistics(int pagesize, int memsize) {
	printf("\n********************\n");
	printf("*      PRSIM       *\n");
	printf("********************\n");
	printf("(physical memory set to %d frames @ %d bytes = %d bytes)\n", 
		totalframes, pagesize, memsize);
	printf("References  : %d\n", memrefs);
	printf("Page faults : %d\n", pagefaults);
	printf("Flushes     : %d\n\n", flushes);
}

page_table* pt_new(int pagetable_size, int totalframes, add_page_mem_policy add_func, replacement_policy replace_func) {
	page_table* new_pagetable;
	
	new_pagetable = (page_table*) malloc(sizeof(page_table));
	new_pagetable->add_page_mem_policy = add_func;
	new_pagetable->replacement_policy = replace_func;
	new_pagetable->pt_ht = ht_new(pagetable_size);
	new_pagetable->inmem_pages = llist_new();
	new_pagetable->freeframes = totalframes;
		
	return new_pagetable;
}

inline node* pt_get_pte(page_table* pt, uint32_t pagenum) {
	return ht_search(pt->pt_ht, pagenum);
}

void pt_load_page(page_table* pt, uint32_t memref, uint32_t pagenum) {
	node* pte;
	
	if ( (pte = pt_get_pte(pt, pagenum)) == NULL ) {
		pte = ht_insert(pt->pt_ht, pagenum, 0);
	}
	
	if (IS_WRITE_REF(memref)) {
		SET_PTE_DIRTY(pte->data);
	}
	
	if (!IS_PTE_VALID(pte->data)) {
		pagefaults++;
		
		if (pt->freeframes > 0) {
			pt->add_page_mem_policy(pt, pte);
		} else {
			pt->replacement_policy(pt, pte);
		}
		
		SET_PTE_VALID(pte->data);
	} else {
		if (pt->replacement_policy == &lru_replacement_policy) {
			linked_list *inmem_pages = pt->inmem_pages;
			
			node* mru_page = llist_search(inmem_pages, pagenum);
			if (mru_page != pt->inmem_pages->head) {
				mru_page = llist_remove(inmem_pages, mru_page);
				llist_insert2(inmem_pages, mru_page);
			}
		}
	}
}

