#include <stdlib.h>
#include "dstruct.h"

linked_list* llist_new() {
	linked_list* new_list = (linked_list*) malloc(sizeof(linked_list));
	
	if (new_list == NULL) {
		return NULL;
	}
	
	new_list->head = NULL;
	new_list->tail = NULL;
	new_list->size = 0;
	
	return new_list;
}

node* llist_insert(linked_list* list, uint32_t key, uint32_t data) {
	node* new_node = (node*) malloc(sizeof(node));
	new_node->key = key;
	new_node->data = data;
	
	if (new_node == NULL)
		return NULL;
	
	new_node->next = list->head;
	if (list->head != NULL)
		list->head->prev = new_node;
		
	list->head = new_node;
	list->size++;
	
	return new_node;
}

void llist_delete(linked_list* list, node* item) {
	if (item->prev != NULL)
		item->prev->next = item->next;
	if (item->next != NULL)
		item->next->prev = item->prev;
	if (item == list->head)
		list->head = item->next;
		
	item->next = NULL;
	item->prev = NULL;
	
	free(item);
	item = NULL;
	
	list->size--;
}

node* llist_dequeue(linked_list* list) {
	node* tmp;
	
	if (list->head->next != NULL)
		list->head->next->prev = list->head->prev;

	tmp = list->head;
	list->head = list->head->next;
	
	tmp->prev = NULL;
	tmp->next = NULL;
	
	return tmp;
}

// Fix this with tail pointer!! Not acceptable
node* llist_enqueue(linked_list* list, uint32_t key, uint32_t data) {
	node* new_node = (node*) malloc(sizeof(node));
	new_node->key = key;
	new_node->data = data;
	
	if (new_node == NULL)
		return NULL;
	
	new_node->prev = list->tail;
	if (list->tail != NULL)
		list->tail->next = new_node;
	
	list->tail = new_node;
	
	return new_node;
}

void llist_delete2(linked_list* list, uint32_t key) {
	node* p = llist_search(list, key);
	llist_delete(list, p);
}

node* llist_search(linked_list* list, uint32_t key) {
	node* p;
	
	if (list == NULL) {
		return NULL;
	} else {
		p = list->head;
	}
	
	while (p != NULL) {
		if (p->key == key)
			return p;
	}
	
	return NULL;
}

hash_table* ht_new(int size) {
	hash_table* new_ht = (hash_table*) malloc(sizeof(hash_table));
	
	new_ht->table = (linked_list**) calloc(size, sizeof(linked_list*));
	new_ht->size = size;
	
	return new_ht;
}

node* ht_insert(hash_table* ht, uint32_t key, uint32_t data) {	
	if (ht->table[key % ht->size] == NULL) {
		ht->table[key % ht->size] = llist_new();
	}
	
	return llist_insert(ht->table[key % ht->size], key, data);
}

void ht_delete(hash_table* ht, node* item) {
	llist_delete(ht->table[item->key % ht->size], item);
}

node* ht_search(hash_table* ht, uint32_t key) {
	return llist_search(ht->table[key % ht->size], key);
}



