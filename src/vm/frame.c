#include "frame.h"

void frame_table_init(){
    list_init(&frame_table);
}

void* fte_alloc(enum palloc_flags flag, struct s_pte* entry){
    if(flag != PAL_USER) return NULL;
    void* frame = palloc_get_page(flag);
    if(frame)
    {
      struct fte* fte = malloc(sizeof(struct fte));
      fte->frame = frame;
      fte->s_pte = entry;
      fte->thread = thread_current();
      fte->allocatable= true;
      list_push_back(&frame_table, &fte->elem);
    }
    else
    {
      while(true)
      {
	 frame = frame_evict(flag);
	 if(frame) break;
      }
      if(!frame) PANIC("frame full");
      struct fte* fte = malloc(sizeof(struct fte));
      fte->frame = frame;
      fte->s_pte = entry;
      fte->thread = thread_current();
      fte->allocatable = true;
      list_push_back(&frame_table, &fte->elem);
    }
    return frame;
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

void* frame_evict(enum palloc_flags flag){
    //use clock algorithm
    //now FIFO
    
    struct list_elem * e;

    e = list_begin(&frame_table);

    struct fte* fte = list_entry(e, struct fte, elem);
    void * frame = fte-> frame;
    list_remove(&fte->elem);
    pagedir_clear_page(fte->thread->pagedir, fte->s_pte->vaddr);
    palloc_free_page(frame);
    free(fte);

    return palloc_get_page(flag);
}
