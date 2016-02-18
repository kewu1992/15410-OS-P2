#ifndef _THR_LIB_HELPER_H_
#define _THR_LIB_HELPER_H_

#include <syscall.h>  // To use new_pages()
#include <simics.h>   // To use lprintf to debug for the moment
#include <stdint.h>

#include <lib_public.h>



int thr_lib_helper_init(unsigned int size);

uint32_t get_new_stack_top(int count);

uint32_t get_stack_high(int index);

int get_stack_position_index();

uint32_t asm_get_esp();

#endif

