#include <stdint.h>
#include <thr_lib_helper.h>
#include <arraytcb.h>
#include <string.h>
#include <thr_internals.h>


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

extern void *ebp__main;

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
 *  @param index The index of thread stacks
 *  (The highest stack is #0)
 *
 *  @return Stack top for a new thread
 *
 */
uint32_t get_new_stack_top(int index) {

    // When the first new thread is to be created, fixate the root thread 
    // stack low
    if(index == 1) {
        if(root_thread_stack_low == 0) {
            root_thread_stack_low = get_root_thread_stack_low();
            if(root_thread_stack_low % ALIGNMENT) {
                return ERROR_MISALIGNMENT;
            }
        }
    }

    // Allocate stack space for new threads

    // Stack space allocated for root thread will be preserved until task 
    // vanishes, since it's allocated by kernel and it may be the result
    // of one or more new_pages() call, so that without this information
    // it would be hard to call remove_pages() correctly. Future thread
    // that is put on the highest stack already has space allocated.
    if(index == 0) {
        return root_thread_stack_low;
    }

    uint32_t new_thread_stack_low = root_thread_stack_low - 
        index * stack_size;
    uint32_t new_thread_stack_high = new_thread_stack_low + 
        stack_size - 1;

    //uint32_t upper_stack_low = new_thread_stack_high + 1; 

    // uint32_t lower_stack_high = new_thread_stack_low - 1;


    // # of pages between
    int num_pages = ((new_thread_stack_high & PAGE_ALIGN_MASK) -
        (new_thread_stack_low & PAGE_ALIGN_MASK))/PAGE_SIZE;

    // Allocate highest page of this stack region, fail is normal since this 
    // page may have been already been allocated 
    int ret = new_pages((void *)(new_thread_stack_high & PAGE_ALIGN_MASK), 
                PAGE_SIZE);
    if(ret && ret != ERROR_NEW_PAGES_OVERLAP_EXISTING_REGION) {
        return ret;
    } 

    if(num_pages > 1) {
        // Allocate middle pages, shouldn't fail since middle pages of this
        // stack region don't overlap with other threads' stack regions
        ret = new_pages((void *)((new_thread_stack_low & PAGE_ALIGN_MASK)
                    + PAGE_SIZE),  (num_pages - 1) * PAGE_SIZE);
        if(ret) {
            return ret;
        }    
    }

    if((new_thread_stack_low & PAGE_ALIGN_MASK) != 
            (new_thread_stack_high & PAGE_ALIGN_MASK)) {
        // Allocate lowest page, fail is normal since the page may have
        // already been allocated
        ret = new_pages((void *)(new_thread_stack_low & PAGE_ALIGN_MASK),
                    PAGE_SIZE);
        if(ret && ret != ERROR_NEW_PAGES_OVERLAP_EXISTING_REGION) {
            return ret;
        } 
    }

/*
    uint32_t new_page_base = (root_thread_stack_low - 
            index * stack_size) & PAGE_ALIGN_MASK;

    uint32_t old_page_base = (root_thread_stack_low - 
            (index - 1) * stack_size) & PAGE_ALIGN_MASK;

    // Depending on how kernel schedules, a thread that's created later may
    // call this function earlier than one that's created earlier
    int num_pages = (old_page_base - new_page_base)/PAGE_SIZE;
*/
    // Allocate highest page of this stack region, fail is normal since this 
    // page may have been already been allocated 
    /*int ret = new_pages((void *)old_page_base, PAGE_SIZE);
    if(ret && ret != ERROR_NEW_PAGES_OVERLAP_EXISTING_REGION) {
        return ret;
    } 

    if(num_pages > 1) {
        // Allocate middle pages, shouldn't fail since middle pages of this
        // stack region don't overlap with other threads' stack regions
        ret = new_pages((void *)(new_page_base + PAGE_SIZE), 
                (num_pages - 1) * PAGE_SIZE);
        if(ret) {
            return ret;
        }    
    }

    if(old_page_base != new_page_base) {
        // Allocate lowest page, fail is normal since the page may have
        // already been allocated
        ret = new_pages((void *)new_page_base, PAGE_SIZE);
        if(ret && ret != ERROR_NEW_PAGES_OVERLAP_EXISTING_REGION) {
            return ret;
        } 
    }
*/
    // The 1st available new stack position is last thread's stack low - 1
    // Keep decrementing until it aligns with 4
    uint32_t new_stack_top = root_thread_stack_low - 
        (index - 1) * stack_size - 1;
    while(new_stack_top % ALIGNMENT != 0) {
        new_stack_top--;
    }

        

    return new_stack_top;
}

// page_remove_indo[0] = base1, [1] = is_remove, [2]
uint32_t get_pages_to_remove(int index, int *page_remove_info) {

    if(index == 0) {
        page_remove_info[1] = 0;
        page_remove_info[3] = 0;
        page_remove_info[5] = 0;
        return 0;
    }

    uint32_t new_thread_stack_low = root_thread_stack_low - 
        index * stack_size;
    uint32_t new_thread_stack_high = new_thread_stack_low + 
        stack_size - 1;

    tcb_t *thr;

    // The page address where new_thread_stack_high is in
    uint32_t new_thread_stack_high_page = new_thread_stack_high & PAGE_ALIGN_MASK;
    uint32_t upper_stack_low = new_thread_stack_high + 1; 
    int can_remove = 1;
    int i = 0;
    if(index - 1 != 0) {
        while((upper_stack_low & PAGE_ALIGN_MASK) == new_thread_stack_high_page) {
            if(!arraytcb_is_valid(index - i)) {
                break;
            }

            thr = arraytcb_get_thread(index - i);
            if(thr) {
                // There's thread alive
                can_remove = 0;
                break;
            } 

            upper_stack_low += stack_size; 
            i--;
        } 
    }

    if(can_remove) {
        page_remove_info[0] = new_thread_stack_high & PAGE_ALIGN_MASK;
        page_remove_info[1] = 1;
    } else {
        page_remove_info[0] = 0;
    }

    // # of pages between
    int num_pages = ((new_thread_stack_high & PAGE_ALIGN_MASK) -
        (new_thread_stack_low & PAGE_ALIGN_MASK))/PAGE_SIZE;

    // Middle pages
    if(num_pages > 1) {
        page_remove_info[2] = (new_thread_stack_low & PAGE_ALIGN_MASK) + PAGE_SIZE;
        page_remove_info[3] = 1;
    } else {
        page_remove_info[3] = 0;
    }

    // If lowest page overlaps with lower stack, remove page
    // if there's no thread alive in lower stack


    // If stack size is smaller than a page, must ensure all threads inside the
    // page are not alive

    uint32_t new_thread_stack_low_page = new_thread_stack_low & PAGE_ALIGN_MASK;
    uint32_t lower_stack_high = new_thread_stack_low - 1;
    can_remove = 1;
    i = 0;
    while((lower_stack_high & PAGE_ALIGN_MASK) == new_thread_stack_low_page) {
        if(!arraytcb_is_valid(index + i)) {
            break;
        }

        thr = arraytcb_get_thread(index + i);
        if(thr) {
            // There's thread alive
            can_remove = 0;
            break;
        } 

        lower_stack_high -= stack_size; 
        i++;
    } 

    if(can_remove) {
        page_remove_info[4] = new_thread_stack_low & PAGE_ALIGN_MASK;
        page_remove_info[5] = 1;
    } else {
        page_remove_info[5] = 0;
    }

    return 0;
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

/** @brief get old %ebp value based on current %ebp
 *
 *  @param ebp Value of current %ebp
 *
 *  @return Value of old %ebp
 */
void* get_last_ebp(void* ebp) {
    unsigned long last_ebp = *((unsigned long*) ebp);
    return (void*) last_ebp;
}

void set_rootthr_retaddr(){
    // trace back, until find main() and __main()
    void* ebp = (void*)asm_get_ebp();
    void* last_ebp = get_last_ebp(ebp);
    while (last_ebp != ebp__main) {
        ebp = last_ebp;
        last_ebp = get_last_ebp(ebp);
    }
    void *thr_ret2exit_addr = thr_ret2exit;
    memcpy((void*)((uint32_t)ebp + 4), &thr_ret2exit_addr, 4);
}

