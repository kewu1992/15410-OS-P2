#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <simics.h>

int main(int argc, char **argv)
{   
    lprintf("I am in");
    
    char buf[] = "Hello, world\n";
    int rv = print(13, buf);
    lprintf("return value: %d", rv);
    
    lprintf("I am out");
    
    return 0;
}
