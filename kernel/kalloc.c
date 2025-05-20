=// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

extern char end[]; // first address after kernel.|
//Use fixed size: max number of pages from 0 to PHYSTOP
#define MAX_PAGES (PHYSTOP >> PGSHIFT)
static int refcount[MAX_PAGES];
#define PA2INDEX(pa) (((pa) - (uint64)end) >> PGSHIFT)

void freerange(void *pa_start, void *pa_end);

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
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  memset(pa, 1, PGSIZE); // Fill with junk
  decref((uint64)pa);
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

  if(r) {
    printf("kalloc: got page %p\n", r);
    memset((char*)r, 5, PGSIZE); // fill with junk
    refcount[PA2INDEX((uint64)r)] = 1;
  }

  return (void*)r;
}

void incref(uint64 pa) {
  if (pa < (uint64)end || pa >= PHYSTOP || pa % PGSIZE != 0)
    panic("incref: bad pa");

  int idx = PA2INDEX(pa);
  if(idx < 0 || idx >= (PHYSTOP - (uint64)end) >> PGSHIFT)
    panic("incref: idx out of bounds");

  __sync_fetch_and_add(&refcount[idx], 1);
}

void decref(uint64 pa) {
 if(pa < (uint64)end || pa >= PHYSTOP || pa % PGSIZE != 0)
    panic("decref: bad pa");

  int idx = PA2INDEX(pa);
  if(idx < 0 || idx >= (PHYSTOP - (uint64)end) >> PGSHIFT)
    panic("decref: idx out of bounds");

  if (__sync_sub_and_fetch(&refcount[idx], 1) == 0) {
    struct run *r = (struct run*)pa;
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
}