#include <stdio.h>
#include <math.h>

void printd(int x) { printf("%d\n", x); }

int readInt32() {
  int x;
  scanf("%d", &x);
  return x;
}

void printb(int x) {
  if (x == 0)
    printf("False\n");
  else
    printf("True\n");
}

