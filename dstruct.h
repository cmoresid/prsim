#ifndef DSTRUCT_H
#define DSTRUCT_H

#include <stdint.h>

typedef struct node {
	uint32_t key;
	uint32_t data;
	struct node* next;
	struct node* prev;
} node;

typedef struct linked_list {
	node* head;
	node* tail;
	int size;
} linked_list;

typedef struct hash_table {
	linked_list** table;
	int size;
} hash_table;

linked_list* llist_new();
node* llist_insert(linked_list* list, uint32_t key, uint32_t data);
void  llist_insert2(linked_list* list, node* new_node);
node* llist_remove(linked_list* list, node* item);
node* llist_remove2(linked_list* list, uint32_t key);
node* llist_search(linked_list* list, uint32_t key); 

node* llist_dequeue(linked_list* list);
node* llist_enqueue(linked_list* list, uint32_t key, uint32_t data);

hash_table* ht_new(int size);
node* ht_insert(hash_table* htable, uint32_t key, uint32_t data);
void ht_delete(hash_table* ht, node* item);
node* ht_search(hash_table* ht, uint32_t key);

#endif // DSTRUCT_H

