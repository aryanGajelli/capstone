/** Compilation: gcc -o memwriter memwriter.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>          
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "../raspi/Inc/buffer.h"
#include "../raspi/Inc/voxel.h"

#define MemContents "This is the way the world ends...\n"

pixel_t data[VOXELS_Z][VOXELS_Y][VOXELS_X] = {0};

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

void *fill_buffer() {
    int side = 50;
    int offset_x = VOXELS_X / 2 - side / 2;
    int offset_y = VOXELS_Y / 2 - side / 2;
    for (int z = 10; z < 20; z++) {
        for (int y = 0; y < 50; y++) {
            for (int x = 0; x < 50; x++) {
                data[z][y + offset_y][x + offset_x] = 0xff;
            }
        }
    }
}

void *voxel_writer_init(void **memptr_ref, sem_t **semptr_ref, int *fileptr) {
  /* Initialize file for shared memory */
  *fileptr = shm_open(BUFFER_FILE, /* File name */
    O_RDWR | O_CREAT, /* read/write, create */
    BUFFER_PERMS);    /* Access permissions */
  if (*fileptr < 0) report_and_exit("Can't open shared mem segment...");

  /* Allocate memory */
  ftruncate(*fileptr, BUFFER_SIZE);

  /* Map memory */
  *memptr_ref = mmap(NULL, /* Let system pick where to put segment */
    BUFFER_SIZE,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    *fileptr,
    0); /* No offset */
  if (*memptr_ref == NULL) report_and_exit("Can't get segment...");

  fprintf(stderr, "Shared memory address: %p [0..%d]\n", *memptr_ref, BUFFER_SIZE - 1);
  fprintf(stderr, "Backing file: /dev/shm%s\n", BUFFER_FILE );

  /* Semaphore code to lock the shared mem */
  sem_unlink(SEMAPHORE_NAME);
  *semptr_ref = sem_open(SEMAPHORE_NAME, /* name */
			   O_CREAT | O_EXCL, /* create the semaphore */
			   BUFFER_PERMS,     /* protection perms */
			   1);               /* initial value */
  if (*semptr_ref == NULL) report_and_exit("sem_open");
  printf("Semaphore created successfully\n");
}

void voxel_buffer_acquire(sem_t *semptr) {
  sem_wait(semptr);
}

void voxel_buffer_release(void *semptr) {
  if (sem_post(semptr) < 0) report_and_exit("sem_post");
}

void *voxel_writer_deinit(void *memptr, sem_t *semptr, int fd) {
  close(fd);
  munmap(memptr, BUFFER_SIZE); /* unmap the storage */
  shm_unlink(BUFFER_FILE); /* unlink from the backing file */
  sem_close(semptr);
  sem_unlink(SEMAPHORE_NAME);
}

int main() {
  void *memptr;
  sem_t *semptr;
  int fd;
  voxel_writer_init(&memptr, &semptr, &fd);
  
  voxel_buffer_acquire(semptr);

  memcpy(memptr, data, MESSAGE_LEN*sizeof(pixel_t));
  
  voxel_buffer_release(semptr);

  sleep(60);

  voxel_writer_deinit(memptr, semptr, fd);
  
  return 0;
}


