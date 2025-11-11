#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <ulimit.h>

//Bytes To GigaBytes
static inline unsigned long btogb(unsigned long bytes) {
    return bytes / (1024 * 1024 * 1024);
}

//Bytes To ExaBytes
static inline double btoeb(double bytes) {
    return bytes / (1024.00 * 1024.00 * 1024.00 * 1024.00 * 1024.00 * 1024.00);
}

int main() {

    printf("\n");

    struct rlimit rlim_addr_space;
    rlim_t addr_space;

    long what = ulimit(3);
    printf("what is %lu\n", what);
    /*
    * Here we call to getrlimit(), with RLIMIT_AS (Address Space) and
    * a pointer to our instance of rlimit struct.
    */
    int retval = getrlimit(RLIMIT_AS, &rlim_addr_space);
    // Get limit returns 0 if succeded, let's check that.
    if(!retval) {
        addr_space = rlim_addr_space.rlim_cur;
        fprintf(stdout, "Current address_space: %lu Bytes, or %lu GB, or %f EB\n", addr_space, btogb(addr_space), btoeb((double)addr_space));
    } else {
        fprintf(stderr, "Coundn\'t get address space current limit.");
        return 1;
    }

    return 0;
}
