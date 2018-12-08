#include "frame.h"

void frame_table_init(){
    list_init(&frame_table);
}

struct fte* fte_alloc(void * page){
    
    struct fte* fte = (struct fte *)malloc(sizeof(struct fte));
    fte -> frame = page;
    fte -> thread = thread_current();
    fte -> allocatable = true;
    list_push_back(&frame_table, &fte->elem);
    return fte;
}
void fte_free(void * frame){

    struct fte * fte = fte_search_by_frame(frame);
    list_remove(&fte->elem);
    free(fte);
}

struct fte* fte_search_by_frame(void * frame){
    struct list_elem *e;

    for(e = list_begin(&frame_table) ; e != list_end(&frame_table) ; e = list_next(e)){
	struct fte * fte = list_entry(e, struct fte, elem);
	if(fte-> frame == frame)
	    return fte;
    }
    return NULL;
}

void frame_evict(){
    //use clock algorithm
    //now FIFO
    
    struct list_elem * e;

    e = list_begin(&frame_table);

    struct fte* fte = list_entry(e, struct fte, elem);
    void * frame = fte-> frame;
    
    palloc_free_page(frame);
}