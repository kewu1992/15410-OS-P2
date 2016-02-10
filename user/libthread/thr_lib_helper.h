#ifndef _THR_LIB_HELPER_H_
#define _THR_LIB_HELPER_H_

#include <syscall.h>  // To use new_pages()
#include <simics.h>   // To use lprintf to debug for the moment

/**
 * @brief Page size alignment mask 
 *
 * If PAGE_SIZE is 4096, i.e., 0x00001000, then PAGE_ALIGN_MASK 
 * will be 0xfffff000 
 */
#define PAGE_ALIGN_MASK ((unsigned int) ~((unsigned int) (PAGE_SIZE-1)))

/**
 * @brief Alignment requirement of stack top addr
 */
#define ALIGNMENT 4

unsigned int get_new_stack_top(int count, 
        unsigned int stack_size);

#endif

