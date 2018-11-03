#include "linkedlist.h"
#include <stdlib.h>

void addToLinkedList( void* element, LinkedList* list ){
	Container* c = (Container*) malloc (sizeof(Container));
	c->element = element;
	if (list->start == NULL) {
		list->start = c;
		list->end = c;
	} else {
		list->end->next = c;
		list->end = c;
	}
}
