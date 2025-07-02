#include <stdio.h>

int main(){
    printf("hey");
    
    printf("Running GCC version: %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
}