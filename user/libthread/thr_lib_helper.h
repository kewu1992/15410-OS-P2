#ifndef _THR_LIB_HELPER_H_
#define _THR_LIB_HELPER_H_

#include <syscall.h>  // To use new_pages()
#include <simics.h>   // To use lprintf to debug for the moment
#include <stdint.h>

#include <autostack_public.h>

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


int thr_lib_helper_init(unsigned int size);

uint32_t get_new_stack_top(int count);

uint32_t get_stack_high(int index);

int get_stack_position_index();

uint32_t asm_get_esp();

#endif

