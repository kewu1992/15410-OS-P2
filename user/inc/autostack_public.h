#ifndef _AUTOSTACK_PUBLIC_H
#define _AUTOSTACK_PUBLIC_H

#include <stdint.h>

// Declares functions that are public to other libraries

uint32_t get_root_thread_stack_low();
uint32_t get_root_thread_stack_high();

#endif /* _AUTOSTACK_H */

