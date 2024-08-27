#include "str.h"
#include "arena.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: add short string optimisation (SSO)
// TODO: Think about const strings!

// Make these macros?
const_string const_str(const string str)
{
  return (const_string){._str = str._str, .size = str.size};
}

const_string str_lit(const char* str_literal)
{
  return (const_string){._str = str_literal, .size = strlen(str_literal)};
}

string str_alloc(Arena a, size_t sz)
{
  return (string){._str = arena_zalloc(a, char, sz), .size = sz};
}

const_string str_from_char(const char* c)
{
  return (const_string){._str = c, .size = 1};
}

string str_from_integer(Arena a, int x)
{
  int strSize = (x == 0) ? 1 : floor(log10(abs(x)) + 1);
  if (x < 0) {
    ++strSize;
  }

  char* str = arena_zalloc(a, char, strSize);

  if (x < 0) {
    str[0] = '-';
  }

  for (int i = 0; x != 0; (x /= 10, ++i)) {
    str[strSize - (1 + i)] = (abs(x) % 10) + '0';
  }

  return (string){._str = str, .size = strSize};
}

string str_copy(Arena dest, const const_string src)
{
  char* res = arena_zalloc(dest, char, src.size);
  memcpy(res, src._str, src.size);
  return (string){._str = res, .size = src.size};
}

string str_cat(Arena dest, const const_string src1, const const_string src2)
{
  // If the string was the last thing allocated in the arena
  if (arena_get_head_ptr(dest) == (uintptr_t)(src1._str + src1.size)) {
    // Copy over src2
    str_copy(dest, src2);
    return (string){._str = src1._str, .size = src1.size + src2.size};
  }

  // Else copy both
  string res = str_copy(dest, src1);
  str_copy(dest, src2);
  res.size = src1.size + src2.size;
  return res;
}

// Kinda terrible right now because no buffered writes
size_t str_print(const const_string str)
{
  size_t count = 0;
  flockfile(stdout);
  for (size_t i = 0; i < str.size; ++i) {
    // Returns char written
    if (putchar_unlocked(str._str[i]) != EOF) {
      ++count;
    }
  }
  funlockfile(stdout);
  return count;
}

size_t str_println(const const_string str)
{
  size_t res = str_print(str);
  res += printf("\n");
  return res;
}

string str_readln_from_file(Arena a, FILE* f)
{
  string res = EMPTY_STR;

  int c;
  for (c = getc(f); c != EOF && c != '\n'; c = getc(f)) {
    res = str_cat(a, const_str(res), str_from_char((char*)&c));
  }

  return res;
}

bool str_eq(const_string str1, const_string str2)
{
  if (str1.size != str2.size) {
    return false;
  }

  for (size_t i = 0; i < str1.size; ++i) {
    if (str1._str[i] != str2._str[i]) {
      return false;
    }
  }
  return true;
}

// Debugging main
// int main()
// {
//   Arena a = arena_init_cap(40);
//   string str = str_copy(a, str_lit("hello there"));
//   printf("%lu %lu\n", arena_get_head_ptr(a), (uintptr_t)&str._str[str.size]);
//   str = str_cat(a, const_str(str), str_lit(", general kenobi"));
//   arena_debug_print(a);
//
//   str_print(const_str(str));
//   printf("\n");
// }
