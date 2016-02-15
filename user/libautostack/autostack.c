/** @file autostack.c
 *
 *  @brief This file contains implementation of autostack library
 *
 *  @bug 1) Should we set a limit to the memory reachable by the root thread
 */
#include <autostack.h>
#include <lib_public.h>

/** @brief Exception stack high */
static uint32_t exn_stack_high;

/** @brief Exception stack low */
static uint32_t exn_stack_low;

/** @brief Root thread stack low */
static uint32_t root_thread_stack_low;

/** @brief Root thread stack high */
static uint32_t root_thread_stack_high;

/** @brief Get current root thread stack low
 *  
 *  Root thread stack low has the chance to grow down
 *  before a new thread is created. This function is 
 *  called when the thread creation library is trying
 *  to create a new thread, we should fixate root thread
 *  low now and disable autostack, freeing exception stack
 *
 *  @return Current root thread stack low; 1 if failure
 */
uint32_t get_root_thread_stack_low() {

    // De-register exception handler to disable autostack
    if(swexn(NULL, NULL, NULL, NULL) < 0) {
        lprintf("Should never reach here, de-register failed");
        return 1;
    }

    // Free exception stack space if we have allocated before
    if(exn_stack_low != 0) {
        free((void *)exn_stack_low);
    }

    return root_thread_stack_low;
}

/** @brief Get root thread stack high
 *  
 *  Root thread stack high is fixed after install_autostack is called
 *
 *  @return Root thread stack high
 */
uint32_t get_root_thread_stack_high() {
    return root_thread_stack_high;
}

/** @brief Allocate pages for a memory region
 *
 *  If this call succeeds, memory region from range_high to range_low
 *  (inclusive) will become valid
 *
 *  @param range_high The memory region high
 *  @param range_low The memory region low 
 *
 *  @return The page base where range_low is in; 1 if failure
 *
 */
int allocate_pages(uint32_t range_high, uint32_t range_low) {

    uint32_t highest_page_base = range_high & PAGE_ALIGN_MASK;

    uint32_t lowest_page_base = range_low & PAGE_ALIGN_MASK;

    // The highest page may overlap with the page
    // where root thread stack low is in
    int num_pages = 1 + (highest_page_base - lowest_page_base) / PAGE_SIZE;
    if(highest_page_base == (root_thread_stack_low & PAGE_ALIGN_MASK)) {
        num_pages--;
    }

    if(num_pages > 0 && 
            new_pages((void *)lowest_page_base, num_pages * PAGE_SIZE) != 0) {

        lprintf("New page failed, something's wrong");
        return 1;
    }    

    return lowest_page_base;
}

/** @brief Exception handler for autostack 
 *
 *  Only handles stack growth for root thread before a new thread
 *  is created. Expands the root thread's stack region down to the 
 *  page where a memory address which results in fault page is in.
 *
 *  @param arg The exception handler argument, ignored
 *  @param ureg The saved execution environment at the moment 
 *  exception happened
 *
 *  @return void
 */
void swexn_handler(void *arg, ureg_t *ureg) {

    // Only handle page fault for root stack growth, ignore other exceptions
    if(ureg->cause == SWEXN_CAUSE_PAGEFAULT) {

        // ureg->cr2 is the memory address that resulted in the fault
        // After this call, memory region from param1 to param2 
        // (inclusive) will become valid
        uint32_t new_root_thread_stack_low =
            allocate_pages(root_thread_stack_low - 1, ureg->cr2); 
        if(new_root_thread_stack_low == 1) {
            lprintf("allocate_pages failed");
            return;
        }
        // Update root thread's valid stack region
        root_thread_stack_low = new_root_thread_stack_low;

        // Re-register exception handler and re-execute faulting instruction
        // Get exception stack high
        uint32_t esp3 = exn_stack_high;
        if(swexn((void *)esp3, swexn_handler, NULL, ureg) < 0) {
            lprintf("Should never reach here, re-register failed");
            return;
        }
    } else {
        lprintf("Ignore other exception: 0x%x", ureg->cause);
    }

}

/** @brief Install exception handler and record initial root thread 
 *  stack position
 *
 *  @param stack_high The inital stack high of root thread
 *  @param stack_low The initial stack low of root thread
 *
 *  @return void
 */
void install_autostack(void *stack_high, void *stack_low) {
    // The original root thread stack region
    root_thread_stack_high = (uint32_t)stack_high;
    root_thread_stack_low = (uint32_t)stack_low;

    // Initialize malloc library and allocate an exception stack
    if(malloc_init() < 0) {
        lprintf("malloc_init failed");
        return;
    }

    void *new_base = malloc(EXCEPTION_STACK_SIZE);
    if(new_base == NULL) {
        lprintf("malloc failed");
        return;
    }

    exn_stack_low = (uint32_t)new_base;
    exn_stack_high = (uint32_t)exn_stack_low + EXCEPTION_STACK_SIZE - 1;
    while(exn_stack_high % ALIGNMENT != 0) {
        exn_stack_high--;
    }

    uint32_t esp3 = exn_stack_high;
    // Register exception handler
    if(swexn((void *)esp3, swexn_handler, NULL, NULL) < 0) {
        lprintf("Register exception hanlder failed");
        return;
    }
}


