#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  printf("=== Student Memory Allocator Test ===\n\n");

  // Allocate blocks of different sizes
  void *a = student_malloc(32);
  void *b = student_malloc(128);
  void *c = student_malloc(256);
  void *d = student_malloc(512);

  printf("Allocated blocks: %p %p %p %p\n", a, b, c, d);

  // Free some blocks
  student_free(b);
  student_free(d);

  printf("Freed blocks b and d\n");
  // Allocate again to test reuse
  void *e = student_malloc(64);
  void *f = student_malloc(200);
  printf("Allocated block e: %p\n", e);
  printf("Allocated block f: %p\n", f);
  //should reuse b's space
  int stats = getmemstats();
  printf("Current allocated blocks: %d\n", stats);


  exit(0);
}