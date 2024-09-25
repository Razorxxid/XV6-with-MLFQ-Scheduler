#include "types.h"
#include "stat.h"
#include "user.h"
#include "cycles.h"


#define N 128
#define OPS_THRESHOLD 20000000

static float a[N][N];
static float b[N][N];
static float c[N][N];

static void init(void) {
  int x, y;
  for (y = 0; y < N; ++y) {
    for (x = 0; x < N; ++x) {
      a[y][x] = y - x;
      b[y][x] = x - y;
      c[y][x] = 0.0f;
    }
  }
}

static void smm(float beta) {
  int x, y, k;
  for (y = 0; y < N; ++y) {
    for (x = 0; x < N; ++x) {
      for (k = 0; k < N; ++k) {
        c[y][x] += beta * a[y][k] * b[k][x];
      }
    }
  }
}

int
main(int argc, char *argv[])
{
  int pid = getpid();
  float beta = 1.0f;
  init();
  for (;;) {
    cycles_t start = cycles_now();
    int ops;
    for (ops = 0; ops < OPS_THRESHOLD; ops += 3 * N * N * N) {
      smm(beta);
      beta = -beta;
    }
    cycles_t end = cycles_now();
    cycles_t elapsed = cycles_sub(end, start);

    int fpmc = (int)((double) ops / (cycles_to_double(elapsed) / 1000000.0));

    printf(1, "%d: %d FLOP/Mcycle (cpubench)\n", pid, fpmc);

  }
  printf(1, "%x\n", c[0][0]);
  exit();
  return 0;
}
