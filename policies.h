
#ifndef POLICIES_H
#define POLICIES_H

#include "prsim.h"

void fifo_add_page_mem_policy(page_table* pt, node* pte);
void fifo_replacement_policy(page_table* pt, node* pte);
void random_add_page_mem_policy(page_table* pt, node* pte);
void random_replacement_policy(page_table* pt, node* pte);
void lru_add_page_mem_policy(page_table* pt, node* pte);
void lru_replacement_policy(page_table* pt, node* pte);

#endif // POLICIES_H
