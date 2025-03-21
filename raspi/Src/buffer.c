/** Compilation: gcc -o memreader memreader.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>          
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#include "voxel.h"
#include "buffer.h"

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}
void *voxel_reader_init(void **memptr_ref, sem_t **semptr_ref, int *fileptr) {
  /* Initialize file for shared memory */
  *fileptr = shm_open(BUFFER_FILE, /* File name */
    O_RDWR,        /* Read only */
    BUFFER_PERMS); /* Access permissions */
  if (*fileptr < 0) report_and_exit("Can't open shared mem segment...");

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

  /* Semaphore code to access the shared mem */
  *semptr_ref = sem_open(SEMAPHORE_NAME,
			   0, /* Fail if the semaphore does not exist */
			   BUFFER_PERMS,
			   0); /* Initial value ignored if the semaphore already exists */
  if (*semptr_ref == NULL) report_and_exit("sem_open");
  printf("Semaphore opened successfully\n");
}

void voxel_buffer_acquire(sem_t *semptr) {
  sem_wait(semptr);
}

void voxel_buffer_release(void *semptr) {
  if (sem_post(semptr) < 0) report_and_exit("sem_post");
}

void voxel_reader_deinit(void *memptr, sem_t *semptr, int fd) {
  close(fd);
  munmap(memptr, BUFFER_SIZE); /* unmap the storage */
  sem_close(semptr);
}

void *getBuffer() {
    void *memptr;
    sem_t *semptr;
    int fd;
    voxel_reader_init(&memptr, &semptr, &fd);

    return memptr;
}