// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

//----Project 3----



#define STUDENT_NUMBER 220012134  
#define BLOCK_SIZE 768             
#define MAGIC_NUMBER 34          
#define ALIGNMENT 2              
#define FREE_LIST_SIZE 13
#define STRATEGY 0               // 0=First-fit, 1=Best-fit, 2=Worst-fit

// Block header structure
struct student_block {
    uint size;                     // Size of this block
    uint magic;                    // Your magic number for verification
    uint allocated;                // 1 if in use, 0 if free
    struct student_block *next;    // Next block in list
};

// Global allocator state
struct {
    struct spinlock lock;
    struct student_block *free_list;  // List of free blocks
    uint total_allocated;             // Statistics
    uint num_blocks;
} student_mem;

// Initialize the allocator
void student_init(void) {
    initlock(&student_mem.lock, "student_mem");
    student_mem.free_list = 0;
    student_mem.total_allocated = 0;
    student_mem.num_blocks = 0;
    
    printf("Student Allocator Initialized\n");
    printf("  Student Number: %d\n", STUDENT_NUMBER);
    printf("  Block Size: %d bytes\n", BLOCK_SIZE);
    printf("  Strategy: %d\n", STRATEGY);
    printf("  Magic Number: %d\n", MAGIC_NUMBER);
}

// Align size to required alignment
static uint align_size(uint size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

// Allocate memory using your strategy
void* student_malloc(uint size) {
    if(size == 0)
        return 0;
    
    // Align the requested size
    size = align_size(size);
    
    acquire(&student_mem.lock);
    
    // Need space for header + data
    //uint total_size = size + sizeof(struct student_block);
    
    // For now, use kalloc to get a page
    // Later you'll manage this yourself
    void *mem = kalloc();
    if(mem == 0) {
        release(&student_mem.lock);
        return 0;
    }
    
    // Set up the block header
    struct student_block *block = (struct student_block*)mem;
    block->size = size;
    block->magic = MAGIC_NUMBER;
    block->allocated = 1;
    block->next = 0;
    
    // Update statistics
    student_mem.total_allocated += size;
    student_mem.num_blocks++;
    
    release(&student_mem.lock);
    
    // Return pointer to data (after header)
    return (void*)((char*)mem + sizeof(struct student_block));
}

// Free allocated memory
void student_free(void *ptr) {
    if(ptr == 0)
        return;
    
    // Get block header (it's right before the data)
    struct student_block *block = 
        (struct student_block*)((char*)ptr - sizeof(struct student_block));
    
    acquire(&student_mem.lock);
    
    // Verify magic number
    if(block->magic != MAGIC_NUMBER) {
        printf("ERROR: Invalid magic number in free!\n");
        release(&student_mem.lock);
        return;
    }
    
    // Verify it was allocated
    if(block->allocated == 0) {
        printf("ERROR: Double free detected!\n");
        release(&student_mem.lock);
        return;
    }
    
    // Mark as free
    block->allocated = 0;
    
    // Update statistics
    student_mem.total_allocated -= block->size;
    student_mem.num_blocks--;
    
    // For now, return the whole page to kalloc
    // Later you'll manage a free list
    kfree((void*)block);
    
    release(&student_mem.lock);
}

// Get statistics
uint student_stats(void) {
    acquire(&student_mem.lock);
    uint blocks = student_mem.num_blocks;
    release(&student_mem.lock);
    return blocks;
}