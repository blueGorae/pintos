
#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <list.h>
#include <stdbool.h>
#include "threads/thread.h"
#include "threads/palloc.h"

struct list frame_table;
struct fte{
    void * frame;

    struct thread * thread;
    bool allocatable;
    
    struct list_elem elem;
};

void frame_table_init();
struct fte* fte_alloc(void * page);
void fte_free(void * frame);
struct fte* fte_search_by_frame(void * frame);
void frame_evict();





#endif /* vm/frame.h */