#pragma once
#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include "cpu.h"

#define TEST_COUNT 70
#define ASSERT_EQUAL(exp, got, argName) \
   do { \
      if ((exp) != (got)) { \
         printf_s("Assertion failed at line %d, in %s. ", __LINE__, __FILE__); \
         printf_s("%s - Expected 0x%X but got 0x%X\n", (argName), (exp), (got)); \
      } else { \
         printf_s("ASSERT\t| %s: E:[0x%X], G:[0x%X] - OK\n", (argName), (exp), (got)); \
      } \
   } while (0)

#define PRINT_TEST_NAME() \
   printf_s("\n### %s ###\n", __func__)

void run_tests();

#endif // TEST_H
