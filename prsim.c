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
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "prsim.h"
#include "policies.h"
#include "dstruct.h"

uint32_t offset_mask;
uint32_t pagenum_mask;
int totalframes = 0;

int memrefs = 0;
int pagefaults = 0;
int flushes = 0;

int main(int argc, char* argv[]) {
	/* Contains the raw string from getopt */
	char* strategy_full = NULL;
	/* After strategy_full sanitized, contains replacement policy */
	char strategy = '0';
	/* Size of the pages */
	int pagesize = 0;
	/* Size of the real memory */
	int memsize = 0;
	/* Scaled size of real memory, if needed */
	int new_memsize = 0;
	/* Used by getopt to store state */
	int c;

	// Make sure there are the proper # of arguments
	if (argc != 7) {
		print_usage();
		exit(1);
	}

	// Parse parameters
	opterr = 0;	
	while ( (c = getopt(argc, argv, "s:p:m:h::")) != -1) {
		switch (c) {
			case 's':
				strategy_full = optarg;
				break;
			case 'p':
				pagesize = atoi(optarg);
				break;
			case 'm':
				memsize = atoi(optarg);
				break;
			case 'h':
				print_usage();
				exit(0);
				break;
			case '?':
				if (optopt == 's' || optopt == 'p' || optopt == 'm')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option '-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
			default:
				print_usage();
				exit(1);
		}
	}
	
	// Verify strategy parameter
	if (strlen(strategy_full) > 1 || (strategy_full[0] != 'r' && strategy_full[0] != 'f' && strategy_full[0] != 'l')) {
		fprintf(stderr, "Strategy must be either 'r', 'f', or 'l'\n");
		exit(1);
	} else {
		strategy = strategy_full[0];
	}
	
	// Verify page size parameter
	if (pagesize < 32 || pagesize > 8192) {
		fprintf(stderr, "Page size must be between 32 and 8192 bytes, inclusive\n");
		exit(1);
	}
	if (!((pagesize & (~pagesize + 1)) == pagesize)) {
		fprintf(stderr, "Page size must be a power of 2.\n");
		exit(1);
	}
	
	// Verify memory size parameter
	if (memsize <= 0) {
		fprintf(stderr, "Memory size must be greater than zero.\n");
		exit(1);
	}
	
	// Round memory size up to nearest multiple of page size if necessary
	new_memsize = PAGE_ROUND_UP(pagesize, memsize);
	if (new_memsize != memsize) {
		printf("Note: Memory size scaled to %d bytes.\n", new_memsize);
	}
	memsize = new_memsize;
	
	// Create bit masks
	create_address_masks(pagesize, memsize);
	
	// Set total number of physical frames
	totalframes = memsize / pagesize;
	
	// Now start simulation!
	return start_simulation(strategy, pagesize, memsize);
}

void print_usage() {
	fprintf(stderr, "Usage: prsim -s strategy -p pagesize -m memsize\n");
}

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

void create_address_masks(int pagesize, int memsize) {
	uint32_t offsetbits = log2(pagesize);
	uint32_t pagenumbits = ADDRESS_WIDTH - offsetbits;
	
	pagenum_mask = create_bitmask(1, pagenumbits);
	offset_mask = create_bitmask(pagenumbits+1, ADDRESS_WIDTH);
}

int start_simulation(char strategy, int pagesize, int memsize) {
	uint32_t buff[1];
	uint32_t pagenum;
	page_table* pt;
	
	switch (strategy) {
		case 'f':
			pt = pt_new(32, totalframes, fifo_add_page_mem_policy, fifo_replacement_policy);
			break;
		case 'r':
			pt = pt_new(32, totalframes, random_add_page_mem_policy, random_replacement_policy);
			break;
		case 'l':
			pt = pt_new(32, totalframes, lru_add_page_mem_policy, lru_replacement_policy);
			break;
	}
	
	while (read(0, &buff, sizeof(uint32_t)) != 0) {
		pagenum = pagenum_mask & buff[0];
		pt_load_page(pt, buff[0], pagenum);
		memrefs++;
	}
	
	printf("\n********************\n");
	printf("*      PRSIM      *\n");
	printf("********************\n");
	printf("(physical memory set to %d frames @ %d bytes = %d)\n", totalframes, pagesize, memsize);
	printf("References  : %d\n", memrefs);
	printf("Page faults : %d\n", pagefaults);
	printf("Flushes     : %d\n\n", flushes);
	
	return 0;
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
	
	if (!IS_PTE_VALID(pte->data)) {
		pagefaults++;
		
		if (IS_WRITE_REF(memref)) {
			TOGGLE_PTE_DIRTY(pte->data);
		}
		
		if (pt->free_frames->size > 0) {
			pt->add_page_mem_policy(pt, pte);
		} else {
			pt->replacement_policy(pt, pte);
		}
		
		TOGGLE_PTE_VALID(pte->data);
	} else {
		// if page replacement policy is LRU, do something
	}
}

