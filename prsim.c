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

#include "prsim.h"
#include "policies.h"
#include "dstruct.h"

const int pt_size = 32;

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
	uint32_t buff[1];
	uint32_t pagenum;
	uint32_t offsetbits;
	uint32_t pagenumbits;
	uint32_t pagenum_mask;
	page_table* pt;
	
	// Set total number of physical frames
	totalframes = memsize / pagesize;
	
	switch (strategy) {
		case 'f':
			pt = pt_new(pt_size, totalframes, fifo_add_page_mem_policy, fifo_replacement_policy);
			break;
		case 'r':
			pt = pt_new(pt_size, totalframes, random_add_page_mem_policy, random_replacement_policy);
			break;
		case 'l':
			pt = pt_new(pt_size, totalframes, lru_add_page_mem_policy, lru_replacement_policy);
			break;
	}
	
	offsetbits = log2(pagesize);
	pagenumbits = ADDRESS_WIDTH - offsetbits;
	pagenum_mask = create_bitmask(1, pagenumbits);
	
	while (read(0, &buff, sizeof(uint32_t)) != 0) {
		pagenum = pagenum_mask & buff[0];
		pt_load_page(pt, buff[0], pagenum);
		memrefs++;
	}
	
	// Done, now print statistics
	print_statistics(pagesize, memsize);
	
	return 0;
}

void print_statistics(int pagesize, int memsize) {
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
	uint32_t i;
	
	new_pagetable = (page_table*) malloc(sizeof(page_table));
	new_pagetable->add_page_mem_policy = add_func;
	new_pagetable->replacement_policy = replace_func;
	new_pagetable->pt_ht = ht_new(pagetable_size);
	new_pagetable->inmem_pages = llist_new();
	new_pagetable->free_frames = llist_new();
	for (i = 0; i < totalframes; i++)
		llist_insert(new_pagetable->free_frames, i, i);
		
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
		
		if (pt->free_frames->size > 0) {
			pt->add_page_mem_policy(pt, pte);
		} else {
			pt->replacement_policy(pt, pte);
		}
		
		SET_PTE_VALID(pte->data);
	} else {
		if (pt->replacement_policy == &lru_replacement_policy) {
			// update_lru
		}
	}
}

