#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern int __ikos_nondet_int(void);
extern unsigned int __ikos_nondet_uint(void);
extern uint64_t f(void);

int main() {
  int c[40];
  uint64_t a = ULONG_MAX / 4 + __ikos_nondet_int();
  c[a] = 5;
}
