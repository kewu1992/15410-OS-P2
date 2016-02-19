#ifndef _THR_LIB_HELPER_H_
#define _THR_LIB_HELPER_H_

#include <syscall.h>  
#include <simics.h>
#include <stdint.h>

#include <lib_public.h>

/** @brief Index information of the array specifying page remove
  * information for a thread's stack space.
  */
enum page_remove_info_index_enum {
    HIGHEST_PAGE_BASE,
    HIGHEST_PAGE_CAN_REMOVE,
    MIDDLE_PAGES_BASE,
    MIDDLE_PAGES_CAN_REMOVE,
    LOWEST_PAGE_BASE,
    LOWEST_PAGE_CAN_REMOVE
};

int thr_lib_helper_init(unsigned int size);
uint32_t get_pages_to_remove(int index, int *page_remove_info);
uint32_t get_new_stack_top(int count);
uint32_t get_stack_high(int index);
int get_stack_position_index();
uint32_t asm_get_esp();
uint32_t asm_get_ebp();
void* get_last_ebp(void* ebp);
void set_rootthr_retaddr();

#endif

