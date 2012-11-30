
#ifndef POLICIES_H
#define POLICIES_H

#include "prsim.h"

void fifo_add_page_mem_policy(page_table* pt, uint32_t pagenum);
void fifo_replacement_policy(page_table* pt, uint32_t pagenum);
void random_add_page_mem_policy(page_table* pt, uint32_t pagenum);
void random_replacement_policy(page_table* pt, uint32_t pagenum);
void lru_add_page_mem_policy(page_table* pt, uint32_t pagenum);
void lru_replacement_policy(page_table* pt, uint32_t pagenum);

#endif // POLICIES_H
