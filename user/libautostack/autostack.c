/** @file autostack.c
 *
 *  @brief This file contains implementation of autostack library
 *
 *  @bug None known
 */
#include <autostack.h>
#include <lib_public.h>
#include <thr_lib_helper.h>

/** @brief Exception stack high */
static uint32_t exn_stack_high;

/** @brief Exception stack low */
static uint32_t exn_stack_low;

/** @brief Root thread stack low */
static uint32_t root_thread_stack_low;

/** @brief Root thread stack high */
static uint32_t root_thread_stack_high;

void *ebp__main;

/** @brief Get current root thread stack low
 *  
 *  Root thread stack low has the chance to grow down before a new thread is 
 *  created. This function is called when the thread creation library is 
 *  trying to create a new thread, we should fixate root thread low now and 
 *  disable autostack, freeing exception stack. 
 *
 *  @return Current root thread stack low; 1 if failure
 */
uint32_t get_root_thread_stack_low() {

    // Autostack is not used in multi-threaded mode, de-register exception
    // handler and free exception stack space
    if(swexn(NULL, NULL, NULL, NULL) < 0) {
        return ERROR_SWEXN_REGIS;
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
        return ERROR_NEW_PAGES_GENERAL;
    }    

    return lowest_page_base;
}

/** @brief Exception handler for autostack 
 *
 *  Only handles stack growth for root thread before a new thread is created. 
 *  Expands the root thread's stack region down to the page where a memory 
 *  address which results in fault page is in.

 *  @param arg The exception handler argument, ignored.
 *  @param ureg The saved execution environment at the moment the exception 
 *  happened.
 *
 *  @return void
 */
void swexn_handler(void *arg, ureg_t *ureg) {

    // Only handle page fault for root stack growth, ignore other exceptions
    if(ureg->cause == SWEXN_CAUSE_PAGEFAULT) {
        // Check if the faulting address is within the current stack frame,
        // and if so, try allocating new pages for it; else, that's an
        // invalid memory reference, let kernel handle

        // ureg->cr2 is the memory address that resulted in the fault
        // The faulting address is an invalid memory reference
        if(ureg->cr2 > ureg->ebp || ureg->cr2 < ureg->esp) {
            return;
        }
        
        // Try allocating new pages for it
        // After this call, memory region down to faulting address
        // will become valid if pages are successfully allocated
        uint32_t new_root_thread_stack_low =
            allocate_pages(root_thread_stack_low - 1, ureg->cr2); 
        if(new_root_thread_stack_low == ERROR_NEW_PAGES_GENERAL) {
            return;
        }
        // Update root thread's valid stack region
        root_thread_stack_low = new_root_thread_stack_low;

        // Re-register exception handler and re-execute faulting instruction
        // Get exception stack high
        uint32_t esp3 = exn_stack_high;
        if(swexn((void *)esp3, swexn_handler, NULL, ureg) < 0) {
            // Registration failed
            return;
        }
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
    // first get ebp of _main()
    void* ebp = (void*)asm_get_ebp();
    ebp__main = get_last_ebp(ebp);

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
        return;
    }
}


