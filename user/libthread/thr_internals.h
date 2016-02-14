/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

/** @brief C wrapper for xchg(lock_available, val)
 *  
 *  In the inside, it will atomically exchange *lock_available with val
 *
 *  @param lock_available The address of variable to be exchanged
 *  @param val The value to replace *lock_available
 * 
 *  @return The old value of (*lock_availble)
 */
int asm_xchg(int *lock_available, int val);

/** @brief Creates a new thread to run func(args) on a given stack
 *  
 *  This function is writtrn in assembly. It will create a thread 
 *  (a register set) to run func(args) on a given stack.
 *
 *  To be more specific, this function will first save its two parameters to
 *  registers. Then it will invoke thread_fork which is a trap. After that, two
 *  threads will run the same code. The original thread will just return. The 
 *  new thread will set its esp to new_stack, and call func.
 *
 *  Note that the two paramters (func, new_stack) must be saved in registers
 *  before calling thread_fork. Because at first they are saved at the original
 *  thread's stack. After thread_fork, the new thread can't rely on the original 
 *  threads's stack to fetch these two paramters. Because at that time, the 
 *  stack maybe destroied by the original thread. 
 *  
 *
 *  @param func The address of function for new thread to run
 *  @param new_stack The address of new stack (esp) for new thread to run on.
 *                   Note that the argument for new thread to run func(args) has 
 *                   been pushed to stack conforming to the calling conventions.
 * 
 *  @return On success the thread ID of the new thread is returned, on error a 
 *          negative number is returned
 */
int thr_create_kernel(void *(*func)(void *), void *new_stack);


int thr_getktid();

#endif /* THR_INTERNALS_H */
