#ifndef _AUTOSTACK_H
#define _AUTOSTACK_H

#include <stdint.h>
#include <simics.h> // To use lprintf to debug

#include <syscall.h>
#include <stddef.h>
#include <stdlib.h> 

#include <lib_public.h>

/** @brief Exception stack size 
  * The size is set to be sufficient enough
  */
#define EXCEPTION_STACK_SIZE (PAGE_SIZE/16)

int allocate_pages(uint32_t range_high, uint32_t range_low);
void swexn_handler(void *arg, ureg_t *ureg);
uint32_t get_swexn_stack_high();
void install_autostack(void *stack_high, void *stack_low);

#endif /* _AUTOSTACK_H */

