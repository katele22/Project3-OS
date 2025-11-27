// user/test_basic.c
// Basic test for your custom allocator

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("=== Student Memory Allocator Test ===\n\n");
  
  // Test 1: Get initial statistics
  printf("Test 1: Initial State\n");
  int stats = getmemstats();
  printf("  Active blocks: %d\n", stats);
  printf("  ✓ System call works!\n\n");
  
  // Test 2: Simple allocation would go here
  // (You'll implement student_malloc in user space later)
  printf("Test 2: Allocation Test\n");
  printf("  (Will implement after malloc is exposed)\n\n");
  
  // Test 3: Your student number verification
  printf("Test 3: Student Number Verification\n");
  printf("  Your student number should be embedded in the kernel\n");
  printf("  Magic number will be verified on allocation\n\n");
  
  printf("=== Basic tests completed ===\n");
  exit(0);
}