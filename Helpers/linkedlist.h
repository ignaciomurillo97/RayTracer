#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct Container{
	void* element;
	struct Container* next;
	struct Container* previus;
} Container;

typedef struct LinkedList {
	Container* start;
	Container* end;
} LinkedList;

#endif
