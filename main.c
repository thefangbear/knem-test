#include <stdio.h>
#include <stdlib.h>
#include <knem_io.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <sys/mman.h>
#include <errno.h>
int main()
{
    puts("starting knem...");
    int k_fd = open(KNEM_DEVICE_FILENAME, O_RDWR);
    struct knem_cmd_create_region create;
    struct knem_cmd_param_iovec data;
    const char * str = "hello world!";
    data.base = (uint64_t) str;
    data.len = strlen(str) + 1;
    create.iovec_array = (uintptr_t) &data;
    create.iovec_nr = 1;
    create.flags = KNEM_FLAG_SINGLEUSE;
    create.protection =PROT_READ;
    int err = ioctl(k_fd, KNEM_CMD_CREATE_REGION, &create);
    if (err < 0) {
        puts("Err: cannot create knem region. ");
        puts(strerror(errno));
        return 1;
    }
    puts("successfully created knem region.");
    printf("Cookie is: %ld\n", create.cookie);
    puts("attempting to access created region...");


    struct knem_cmd_inline_copy copied;
    struct knem_cmd_param_iovec cpy_iov;
    char * cpy_str = malloc(30);
    cpy_iov.base = (uint64_t) cpy_str;
    cpy_iov.len = 30;
    copied.local_iovec_array = (uintptr_t) &cpy_iov;
    copied.local_iovec_nr = 1;
    copied.remote_cookie = create.cookie;
    copied.remote_offset = 0;
    copied.write = 0;
    copied.flags = 0;
    err = ioctl(k_fd, KNEM_CMD_INLINE_COPY, &copied);
    if (err < 0) {
        puts("Err: cannot copy knem region.");
        puts(strerror(errno));
        return 1;
    }
    puts("opened knem copy region.");
    if (copied.current_status != KNEM_STATUS_SUCCESS) {
        puts("Err: KNEM copy failed.");
        return 1;
    }
    printf("received string: %s\n", cpy_str);
    free(cpy_str);
    return 0;
}