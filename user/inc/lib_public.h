/** @file lib_public.c
 *  @brief Declares functions that are public among libraries
 *
 *  @bug Better use spin lock for mutex_thread_count
 */
#ifndef _LIB_PUBLIC_H
#define _LIB_PUBLIC_H

#include <stdint.h>

// libautostack
uint32_t get_root_thread_stack_low();
uint32_t get_root_thread_stack_high();

// libthread
int malloc_init();

#endif /* _AUTOSTACK_H */

