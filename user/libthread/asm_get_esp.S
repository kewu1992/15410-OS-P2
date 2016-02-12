
# uint32_t asm_get_esp();

.globl asm_get_esp

asm_get_esp:
movl    %esp, %eax      # Get current %esp value
ret                     # Return current %esp value

