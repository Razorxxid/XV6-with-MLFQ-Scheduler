#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "cycles.h"

#define N 4096
#define TIMES 8

static char path[] = "NNiobench";
static char data[N];

int
main(int argc, char *argv[])
{
  int rfd, wfd;
  int pid = getpid();
  int i;
  path[0] = '0' + (pid / 10);
  path[1] = '0' + (pid % 10);

  memset(data, 'a', sizeof(data));

  int bytes = 2 * N * TIMES;
  for(;;) {
    cycles_t start = cycles_now();
    for(i = 0; i < TIMES; ++i) {
      // Seek syscall missing!
      wfd = open(path, O_CREATE | O_WRONLY);
      rfd = open(path, O_RDONLY);

      write(wfd, data, N);
      read(rfd, data, N);

      close(wfd);
      close(rfd);
    }
    cycles_t end = cycles_now();

    cycles_t elapsed = cycles_sub(end, start);

    int bpm = (int)((double) bytes / (cycles_to_double(elapsed) / 1000000.0));
    printf(1, "\t\t\t\t\t%d: %d bytes/Mcycle (iobench)\n", pid, bpm);
  }

  exit();
  return 0;
}
