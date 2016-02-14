#ifndef _AUTOSTACK_H
#define _AUTOSTACK_H

#include <stdint.h>
#include <simics.h> // To use lprintf to debug

#include <syscall.h>

#define ALIGNMENT 4

/**
 * @brief Page size alignment mask 
 *
 * If PAGE_SIZE is 4096, i.e., 0x00001000, then PAGE_ALIGN_MASK 
 * will be 0xfffff000 
 */
#define PAGE_ALIGN_MASK ((unsigned int) ~((unsigned int) (PAGE_SIZE-1)))


int allocate_pages(uint32_t range_high, uint32_t range_low);
void swexn_handler(void *arg, ureg_t *ureg);
uint32_t get_swexn_stack_high();
void install_autostack(void *stack_high, void *stack_low);

#endif /* _AUTOSTACK_H */

