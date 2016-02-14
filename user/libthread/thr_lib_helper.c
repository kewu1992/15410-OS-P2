#include <stdint.h>
#include <thr_lib_helper.h>
#include <arraytcb.h>


/**
 * @brief Root thread stack low
 */
static uint32_t root_thread_stack_low;

/**
 * @brief Root thread stack high
 */
static uint32_t root_thread_stack_high;

/** @brief The amount of stack space available for each thread */
static unsigned int stack_size;

int thr_lib_helper_init(unsigned int size) {

    stack_size = size;

    // root thread stack high is deterministic
    root_thread_stack_high = get_root_thread_stack_high();

    return 0;
}

/** @brief Get stack top for a new thread
 *  
 *  Compute the stack region for a new thread, allocate new pages
 *  if necessary, and get a stack top that meets alignment requirement 
 *
 *  @param count The number of new threads has ever been created
 *  (master thread is #0)
 *
 *  @return Stack top for a new thread
 *
 *  @bug 1) Don't compare root thread stack low with stack_size, will
 *  address it when doing autostack 2) Don't reuse stack space after 
 *  thread being reaped, will address it when doing thread garbage
 *  collection.
 *
 */
uint32_t get_new_stack_top(int count) {

    // Once we create a new thread, the root thread's stack region is
    // fixed
    if(count == 1) {
        // There's a race condition here, before the root_thread_stack_low
        // is acquired, the other thread creation should wait
        root_thread_stack_low = get_root_thread_stack_low();
        if(root_thread_stack_low % ALIGNMENT != 0) {
            lprintf("get_root_thread_stack_low failed");
            return 3;
        }
    }

    // Assume pages on the stack grow down continuouly
    uint32_t new_page_base = (root_thread_stack_low - 
            count * stack_size) & PAGE_ALIGN_MASK;

    uint32_t old_page_base = (root_thread_stack_low - 
            (count - 1) * stack_size) & PAGE_ALIGN_MASK;

    //lprintf("old_page_base: %x", (unsigned)old_page_base);
    //lprintf("new_page_base: %x", (unsigned)new_page_base);

    // Depending on how kernel schedules, a thread that's created later may
    // call this function earlier than one that's created earlier
    int num_pages = (old_page_base - new_page_base)/PAGE_SIZE;

    //lprintf("num_pages: %d", num_pages);

    // Allocate highest page of this stack region, fail is normal since this 
    // page may have been already been allocated 
    if(new_pages((void *)old_page_base, PAGE_SIZE) != 0) {
            // Should check return value
            // New pages failed, the page may have been already allocated
    } 

    if(num_pages > 1) {
        // Allocate middle pages, shouldn't fail since middle pages of this
        // stack region don't overlap with other threads' stack regions
        if(new_pages((void *)(new_page_base + PAGE_SIZE), 
                    (num_pages - 1) * PAGE_SIZE) != 0) {
            lprintf("New page failed, something's wrong");
            return 3;
        }    
    }

    if(old_page_base != new_page_base) {
        // Allocate lowest page, fail is normal since the page may have
        // already been allocated
        if(new_pages((void *)new_page_base, PAGE_SIZE) != 0) {
                // Should check return value
                // New pages failed, the page may have been already allocated
        } 
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

/** @brief Get stack high given stack position index 
 *  
 *  @param index Stack position index
 *
 *  @return Stack high of a stack position index
 *  @bug Should check if root_thread_stack_low has been initialized
 *  Will address it shortly
 */
uint32_t get_stack_high(int index) {

    if(index == 0) {
         return root_thread_stack_high;
    }

    uint32_t cur_stack_high = root_thread_stack_low - 
        (index - 1) * stack_size - 1;
    while(cur_stack_high % ALIGNMENT != 0) {
        cur_stack_high--;
    }

    return cur_stack_high;
}

/** @brief Get stack position index of the current thread 
 *  
 *
 *  @return Stack position index of the current thread
 */
int get_stack_position_index() {
    
    uint32_t esp = asm_get_esp();

    if(esp <= root_thread_stack_high && esp >= root_thread_stack_low) {
        return 0;
    } else {
        // (root_thread_stack_low - esp - 1) so that all esp within a stack 
        // region maps to the same number
        return 1 + (root_thread_stack_low - esp - 1) / stack_size;
    }

}

