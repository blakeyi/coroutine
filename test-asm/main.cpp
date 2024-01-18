#include <iostream>
#include <stdlib.h>
#include <cstring>
struct coctx_t
{
#if defined(__i386__)
	void *regs[ 8 ];
#else
	void *regs[ 14 ];
#endif
	size_t ss_size;
	char *ss_sp;
	
};

#define ESP 0
#define EIP 1
#define EAX 2
#define ECX 3
// -----------
#define RSP 0
#define RIP 1
#define RBX 2
#define RDI 3
#define RSI 4

#define RBP 5
#define R12 6
#define R13 7
#define R14 8
#define R15 9
#define RDX 10
#define RCX 11
#define R8 12
#define R9 13

enum {
  kRDI = 7,
  kRSI = 8,
  kRETAddr = 9,
  kRSP = 13,
};

typedef void* (*coctx_pfn_t)( void* s, void* s2 );

int coctx_make(coctx_t* ctx, coctx_pfn_t pfn, const void* s, const void* s1) {
  char* sp = ctx->ss_sp + ctx->ss_size - sizeof(void*);
  sp = (char*)((unsigned long)sp & -16LL);

  memset(ctx->regs, 0, sizeof(ctx->regs));
  void** ret_addr = (void**)(sp);
  *ret_addr = (void*)pfn;

  ctx->regs[kRSP] = sp;

  ctx->regs[kRETAddr] = (char*)pfn;

  ctx->regs[kRDI] = (char*)s;
  ctx->regs[kRSI] = (char*)s1;
  return 0;
}

int coctx_init(coctx_t* ctx) {
  memset(ctx, 0, sizeof(*ctx));
  return 0;
}

extern "C"
{
	extern void coctx_swap( coctx_t *,coctx_t* ) asm("coctx_swap");
};

int main()
{
    auto cur = coctx_t();
    cur.ss_size = 1;
    cur.ss_sp = new char[1024];
    cur.regs[sizeof(cur.regs) / sizeof(cur.regs[0]) - 1] = cur.ss_sp + 1024;

    auto prev = coctx_t();
    prev.ss_size = 1;
    prev.ss_sp = new char[1024];
    prev.regs[sizeof(prev.regs) / sizeof(prev.regs[0]) - 1] = prev.ss_sp + 1024;

    coctx_swap(&cur, &prev);
    std::cout << 111 << std::endl;

    delete[] cur.ss_sp;
    delete[] prev.ss_sp;
}