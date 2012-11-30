/*
 * =====================================================================================
 *
 *       Filename:  prsim.h
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

#ifndef PRSIM_H
#define PRSIM_H

#include "dstruct.h"

#define ADDRESS_WIDTH 32
#define PAGE_ROUND_UP(pagesize,x) ( (((int)(x)) + pagesize-1)  & (~(pagesize-1)) )

#define IS_PTE_VALID(pte) ( ((uint32_t)(1 << ((ADDRESS_WIDTH)-1))) & pte )
#define IS_PTE_DIRTY(pte) ( ((uint32_t)(1 << ((ADDRESS_WIDTH)-2))) & pte )
#define IS_WRITE_REF(ref) ( ((uint32_t)1) & ref )

#define TOGGLE_PTE_VALID(pte) ( pte ^= ((uint32_t)(1 << ((ADDRESS_WIDTH)-1))) )
#define TOGGLE_PTE_DIRTY(pte) ( pte ^= ((uint32_t)(1 << ((ADDRESS_WIDTH)-2))) )

typedef struct page_table {
	linked_list* free_frames;
	linked_list* inmem_pages;
	hash_table* pt_ht;
	void (*add_page_mem_policy)(struct page_table* pt, uint32_t pagenum);
	void (*replacement_policy) (struct page_table* pt, uint32_t pagenum);
} page_table;

typedef void (*add_page_mem_policy)(page_table* pt, uint32_t pagenum);
typedef void (*replacement_policy) (page_table* pt, uint32_t pagenum);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  print_usage()
 *  Description:  Prints parameters that should be passed to prsim
 *	  Arguments:  None
 *      Returns:  (void)
 * =====================================================================================
 */
void print_usage();

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  start_simulation(char strategy, int pagesize, int memsize)
 *  Description:  
 *	  Arguments:  
 *      Returns:  0 if successful, 1 if something went wrong
 * =====================================================================================
 */
int start_simulation(char strategy, int pagesize, int memsize);

void create_address_masks(int pagesize, int memsize);

page_table* pt_new(int pagetable_size, int totalframes, add_page_mem_policy add_func, replacement_policy replace_func);
inline int pt_page_exists(page_table* pt, uint32_t pagenum);
void pt_load_page(page_table* pt, uint32_t memref, uint32_t pagenum);
inline node* pt_get_pte(page_table* pt, uint32_t pagenum);

#endif // PRSIM_H

