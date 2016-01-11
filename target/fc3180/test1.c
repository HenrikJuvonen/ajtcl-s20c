#include "alljoyn.h"
#include <stdarg.h>

#define va_copy(x, y) (x) = (y)

void va2(int *x, char *y, va_list *argpp) {
  va_list argp;
  va_copy(argp, *argpp);
  int i = 0;
  while (i++ < *x) {
    int z = va_arg(argp, int);
    printf("%d %d\n", x, z);
  }
  va_copy(*argpp, argp);
}

void va1(int *x, char *y, ...) {
  va_list argp;
  va_start(argp, y);
  va2(x, y, &argp);
  va_end(argp);
}

void va_test() {
  int x = 4;
  char y[6] = "hello";
  va1(&x, y, 1, 2, 3, 4);
}

void tm_test() {
  AJ_Time t;
  AJ_InitTimer(&t);
  int x = 0;
  while (x++ < 10) {
    uint32_t msec = AJ_GetElapsedTime(&t, TRUE);
    AJ_Printf("%d\n", msec);
  }
}

void en_test() {
  union {
    uint32_t i;
    char c[4];
  } e = {0x01000000};
  volatile uint32_t i = 0x01234567;
  if ((*((uint8_t *)(&i))) != 0x67) {
    printf("big endian 1\n");
  }
  if (e.c[0]) {
    printf("big endian 2\n");
  }
}
