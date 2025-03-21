#ifndef VOXEL_BUFFER_H
#define VOXEL_BUFFER_H

#define BUFFER_SIZE 2097152
#define MESSAGE_LEN 2097152
#define BUFFER_PERMS 0644
#define BUFFER_FILE "/voxel_buffer"
#define SEMAPHORE_NAME "voxel_semaphore"

void *getBuffer();

#endif