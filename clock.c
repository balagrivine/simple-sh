  #include <time.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <fcntl.h>

  #define ITERATIONS 100000

  int main() {
      struct timespec start, stop;
      volatile pid_t pid;
      clock_gettime(CLOCK_MONOTONIC, &start);

      for (int i = 0; i < ITERATIONS; i++){
          pid = getpid();
      }

      clock_gettime(CLOCK_MONOTONIC, &stop);

      long elapsed = (stop.tv_sec - start.tv_sec) * 1000000000L
                   + (stop.tv_nsec - start.tv_nsec);

      printf("%d\n", pid);
      printf("avg syscall cost: %ld ns over %d iterations\n",
             elapsed / ITERATIONS, ITERATIONS);
  }
