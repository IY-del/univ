#include "fontlib.h"

#include <stdio.h>

void printx(int flag) { printf("%s", flag ? "X" : " "); }

void printxs(int flag, const char str[]) { printf("%s", flag ? str : " "); }
