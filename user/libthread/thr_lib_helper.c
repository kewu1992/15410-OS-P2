#include <stdint.h>
#include <thr_lib_helper.h>

/**
 * @brief Root thread stack low
 */
static uint32_t root_thread_stack_low;

/** @brief Get current root thread stack low
 *  
 *  @return Current root thread stack low
 *  @bug Will eventually put this call in libautostack
 */
uint32_t get_root_thread_stack_low() {

    return 0xffffe000;
}

/** @brief Get stack top for a new thread
 *  
 *  Compute the stack region for a new thread, allocate new pages
 *  if necessary, and get a stack top that meets alignment requirement 
 *
 *  @param count The number of new threads has ever been created
 *  (master thread is #0)
 *
 *  @param stack_size The max stack size for each new thread
 *
 *  @return Stack top for a new thread
 *
 *  @bug 1) Don't compare root thread stack low with stack_size, will
 *  address it when doing autostack 2) Don't reuse stack space after 
 *  thread being reaped, will address it when doing thread garbage
 *  collection.
 *
 */
uint32_t get_new_stack_top(int count, 
        unsigned int stack_size) {

    // Once we create a new thread, the root thread's stack region is
    // fixed
    if(count == 1) {
        root_thread_stack_low = get_root_thread_stack_low();
    }

    // Assume pages on the stack grow down continuouly
    uint32_t new_page_base = (root_thread_stack_low - 
            count * stack_size) & PAGE_ALIGN_MASK;

    uint32_t old_page_base = (root_thread_stack_low - 
            (count - 1) * stack_size) & PAGE_ALIGN_MASK;

    // Allocate if previously allocated pages are not enough
    if(new_page_base != old_page_base) {
        lprintf("new_page_base: %x", (unsigned int)new_page_base);
        if(new_pages((void *)new_page_base, 
                    old_page_base - new_page_base) != 0) {
            lprintf("new_pages fail");
            // Should return other error code though
            return -1;
        } else {
            lprintf("new_pages succeed");
        }
    } else {
        lprintf("No new page required");
    }

    // The 1st available new stack position is last thread's stack low - 1
    // Keep decrementing until it aligns with 4
    uint32_t new_stack_top = root_thread_stack_low - 
        (count - 1) * stack_size - 1;
    while(new_stack_top % ALIGNMENT != 0) {
        new_stack_top--;
    }

    return new_stack_top;
}

