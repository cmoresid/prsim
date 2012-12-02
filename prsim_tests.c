#include <stdio.h>
#include <stdlib.h>

#include "dstruct.h"
#include "minunit.h"

int tests_run;

static char* test_newlist() {
	linked_list* list = llist_new();
	mu_assert("** test_newlist: linked_list creation failed.", list != NULL);
	mu_assert("** test_newlist: head not null.", list->head == NULL);
	mu_assert("** test_newlist: tail not null.", list->tail == NULL);
	mu_assert("** test_newlist: size not 0.", list->size == 0);
	
	return 0;
}

static char* test_insert1() {
	linked_list* list = llist_new();
	node* n = llist_insert(list, 1, 1);
	
	mu_assert("** test_insert1: n->key != 1", n->key == 1);
	mu_assert("** test_insert1: n->data != 1", n->data == 1);
	mu_assert("** test_insert1: unable to insert node.", list->head != NULL);
	mu_assert("** test_insert1: head and tail not equal.", list->head == list->tail);
	mu_assert("** test_insert1: size != 1", list->size == 1);
	mu_assert("** test_insert1: head->next != null.", list->head->next == NULL);
	mu_assert("** test_insert1: head->prev != null.", list->head->prev == NULL);
	
	return 0;
}

static char* test_insert2() {
	linked_list* list = llist_new();
	llist_insert(list, 1, 1);
	llist_insert(list, 2, 2);
	
	mu_assert("** test_insert2: head == tail.", list->head != list->tail);
	mu_assert("** test_insert2: head->next != tail.", list->head->next == list->tail);
	mu_assert("** test_insert2: tail->prev != head.", list->tail->prev == list->head);
	mu_assert("** test_insert2: size != 2.", list->size == 2);
	
	return 0;
}

static char* test_remove1() {
	linked_list* list = llist_new();
	node* n = llist_insert(list, 1, 1);
	free(llist_remove(list, n));
	
	mu_assert("** test_removehead: size != 0.", list->size == 0);
	mu_assert("** test_removehead: head != tail.", list->head == list->tail);
	mu_assert("** test_removehead: head != NULL.", list->head == NULL);
	
	return 0;
}

static char* test_remove2() {
	linked_list* list = llist_new();
	node* n = llist_insert(list, 1, 1);
	node* n2 = llist_insert(list, 2, 2);
	
	free(llist_remove(list, n));
	free(llist_remove(list, n2));
	
	mu_assert("** test_remove2: size != 0.", list->size == 0);
	mu_assert("** test_remove2: head != tail.", list->head == list->tail);
	mu_assert("** test_remove2: head != NULL.", list->head == NULL);
	
	return 0;
}

static char* test_remove3() {
	linked_list* list = llist_new();
	llist_insert(list, 1, 1);
	node* n = llist_insert(list, 2, 2);
	
	llist_remove(list, n);
	
	mu_assert("** test_remove3: head != tail.", list->head == list->tail);
	mu_assert("** test_remove3: head->next != NULL.", list->head->next == NULL);
	mu_assert("** test_remove3: head->prev != NULL.", list->head->prev == NULL);
	mu_assert("** test_remove3: removed node != n.", n->key == 2);
	
	free(n);
	
	return 0;
}

static char* test_remove4() {
	linked_list* list = llist_new();
	llist_insert(list, 1, 1);
	node* n = llist_insert(list, 2, 2);
	llist_insert(list, 3, 3);
	
	llist_remove(list, n);
	
	mu_assert("** test_remove4: head == tail.", list->head != list->tail);
	mu_assert("** test_remove4: head->next != tail.", list->head->next == list->tail);
	mu_assert("** test_remove4: tail->prev != head.", list->tail->prev == list->head);
	mu_assert("** test_remove4: head->prev != NULL.", list->head->prev == NULL);
	mu_assert("** test_remove4: tail->next != NULL.", list->tail->next == NULL);
	mu_assert("** test_remove4: head != 1.", list->head->key == 3);
	mu_assert("** test_remove4: tail != 3.", list->tail->key == 1);
	
	free(n);
	
	return 0; 
}

static char* all_tests() {
	mu_run_test(test_newlist);
	mu_run_test(test_insert1);
	mu_run_test(test_insert2);
	mu_run_test(test_remove1);
	mu_run_test(test_remove2);
	mu_run_test(test_remove3);
	mu_run_test(test_remove4);
	
	return 0;
}

int main(int argc, char* argv[]) {
	char* result = all_tests();
	
	printf("\n====================\n");
	printf("    RUNNING TESTS\n");
	printf("====================\n");
	if (result != 0) {
		printf("\nTEST FAILED:\n");
		printf("%s\n", result);
	} else {
		printf("\nALL TESTS PASSED\n");
	}
	printf("\n****************\n");
	printf("Tests Run: %d\n", tests_run);
	printf("****************\n");
	return 0;
}

