#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;

int main(int argc, char *argv[])
{
        if(argc != 2)
                return 1;

        int fd = open("/proc/self/maps", O_RDONLY);
        char buf[40] = "0x";
        ssize_t rd = read(fd, buf+2, sizeof(buf)-1-2);
        if(rd != sizeof(buf)-1-2)
                return 2;
        close(fd);

        int i;
        for (i=2; i<sizeof(buf)/sizeof(buf[0]); i++){
                if(buf[i] == '-'){
                        buf[i] = 0;
                        break;
                }
        }
        u64 target_rip = strtol(buf, NULL, 16);
        target_rip += strtol(argv[1], NULL, 16);

        u32 eax = 0x12345678UL;
 
        asm volatile (
                "cpuid\n\t"
                "LOOP:\n\t"
                "mov %%edx, %%eax \n\t"
                "mov %%eax, %%ecx \n\t"
                "mov %%ecx, %%ebx \n\t"
                "mov %%ebx, %%edx \n\t"
                "mov %%edx, %%ebx \n\t"
                "mov %%ebx, %%ecx \n\t"
                "mov %%ecx, %%eax \n\t"
                "mov %%eax, %%edx \n\t"
                "mov %%edx, %%ecx \n\t"
                "mov %%ebx, %%edx \n\t"
                "mov %%edx, %%ebx \n\t"
                "mov %%ebx, %%ecx \n\t"
                "mov %%ecx, %%eax \n\t"
                "mov %%eax, %%edx \n\t"
                "mov %%ecx, %%eax \n\t"
                "jmp LOOP \n\t"
                :
                : "a"(eax), "c"(target_rip)
                : "memory", "edx"
        );
}
