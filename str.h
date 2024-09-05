#ifndef AAQ_STR_H_
#define AAQ_STR_H_

#include "arena.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct
{
  char* _str;
  size_t size;
} string;

typedef struct
{
  const char* _str;
  size_t size;
} const_string;

#define EMPTY_STR                                                              \
  (string)                                                                     \
  {                                                                            \
    ._str = NULL, .size = 0                                                    \
  }

const_string const_str(string str);
const_string str_lit(const char* str_literal);

// Does not produce a string that is owned
// Produce a string of size 1 with a character from an owned string
const_string str_from_char(const char* c);
string str_from_integer(Arena a, int x);

string str_alloc(Arena a, size_t sz);
string str_copy(Arena dest, const_string src);

// If src1 is the last thing in the dest arena then it copies src2 to it. Else
// it copies both src1 and src2 to dest. Then creates a new string object to
// return.
//
// OK look this is const poisoning: it kills the const qualifier.
// Here's the reason why this is OK for now.
//
// If we consider the implementation where both strings are copied into the
// arena, they are now both in non-const memory. If we consider the first
// implementation, src1 was already in the non-const arena and src2 is copied in
// either way.
string str_cat(Arena dest, const_string src1, const_string src2);

// Returns number of characters printed
size_t str_print(const_string str);
size_t str_println(const_string str);

typedef struct
{
  string res;
  // Whether file has ended, or we still have more to read
  enum { srr_EOF = 0, srr_MORE } status;
} str_readln_return;

// Reads line from std in into an arena.
// Unsafe if arena does not have enough space.
str_readln_return str_readln_from_file(Arena a, FILE* f);

bool str_eq(const_string str1, const_string str2);

#define str_readln(arena) str_readln_from_file(arena, stdin)

#endif
