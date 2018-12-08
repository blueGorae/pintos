#include "vm/page.h"
#include "userprog/process.h"

static unsigned page_hash_func (const struct hash_elem *e, void *aux UNUSED)
{
  struct s_pte *spte = hash_entry(e, struct s_pte, elem);
  return hash_int((int) spte->vaddr);
}

static bool page_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
  struct s_pte *spte_a = hash_entry(a, struct s_pte, elem);
  struct s_pte *spte_b = hash_entry(b, struct s_pte, elem);
  if (spte_a->vaddr < spte_b->vaddr)
      return true;

  return false;
}


void s_page_table_init(struct hash * s_page_table){
    hash_init (s_page_table, page_hash_func, page_less_func, NULL);
}

struct s_pte* s_pte_alloc(struct cur_file_info * cur_file_info, void * vaddr){
    struct s_pte * spte = (struct s_pte *) malloc(sizeof(struct s_pte));
    spte->cur_file_info =  cur_file_info;
    spte -> vaddr = vaddr;
    hash_insert(&thread_current()->s_page_table, &spte->elem);
    
    return spte;
}

static void s_page_action_func (struct hash_elem *e, void *aux UNUSED)
{
  struct s_pte *spte = hash_entry(e, struct s_pte, elem);
  if (is_loaded(spte->vaddr))
  {
      palloc_free_page(spte->vaddr);
      free(spte->cur_file_info);
  }
  free(spte);
}

void s_page_table_destroy(){
    
    hash_destroy (&thread_current()->s_page_table, s_page_action_func);
}


struct s_pte* s_pte_search_by_vaddr(void* vaddr){
    struct s_pte spte;
    spte.vaddr = pg_round_down(vaddr);

    struct hash_elem *e = hash_find(&thread_current()->s_page_table, &spte.elem);
    if (e == NULL)
        return NULL;

    return hash_entry (e, struct s_pte, elem);
}

bool load_page(void * vaddr){

    struct s_pte * spte = s_pte_search_by_vaddr(vaddr);
    enum palloc_flags flags = PAL_USER;

    if(spte == NULL)
        return false;

    if (spte->cur_file_info-> page_read_bytes == 0)
    {
        flags = PAL_ZERO | PAL_USER;
    }

    void * frame = palloc_get_page(flags);

    if(frame == NULL)
        return false;

    /* Load this page. */

    if (file_read (spte->cur_file_info->file, frame, spte->cur_file_info -> page_read_bytes) != (int) spte->cur_file_info -> page_read_bytes)
    {
        palloc_free_page (frame);
        return false; 
    }

    memset (frame + spte->cur_file_info->page_read_bytes, 0, spte->cur_file_info-> page_zero_bytes);

    /* Add the page to the process's address space. */
    if (!install_page (spte-> vaddr, frame, spte->cur_file_info->writable)) 
    {
        palloc_free_page (frame);
        return false; 
    }

    return true;

}

bool is_loaded(void * page){
    if(fte_search_by_frame(page) == NULL)
        return false;
    
    return true;
}