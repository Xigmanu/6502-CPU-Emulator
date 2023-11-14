#pragma once
#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include "cpu.h"

#define ASSERT_EQUAL(exp, got, argName) \
   do { \
      if ((exp) != (got)) { \
         printf("Assertion failed at line %d, in %s. ", __LINE__, __FILE__); \
         printf("%s - Expected 0x%X but got 0x%X\n", (argName), (exp), (got)); \
      } else { \
         printf("ASSERT\t| %s: E:[0x%X], G:[0x%X] - OK\n", (argName), (exp), (got)); \
      } \
   } while (0)

#define PRINT_TEST_NAME() \
   printf("\n### %s ###\n", __func__)

void testResetCPU();
void testLDAImmediate();
void testLDAZeroPage();
void testLDAZeroPageX();
void testLDAAbsolute();
void testLDAAbsoluteX();
void testLDAAbsoluteY();
void testLDAIndirectX();
void testLDAIndirectY();
void testJSR();
void testRTS();

#endif // !TEST_H
