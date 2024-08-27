#ifndef AAQ_SCANNER_H_
#define AAQ_SCANNER_H_

#include "arena.h"
#include "str.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct token token;

struct token
{
  const_string term_str; // Only used for terms
  size_t pos;

  token* nxt;
  token* prv;

  enum {
    tok_NULL = 0,
    tok_LPAREN,
    tok_RPAREN,
    tok_LAMBDA,
    tok_TERM,
    tok_DOT,
  } type;
};

bool isTerm(char c);

size_t parseTerm(Arena a, const_string parse_str, token* tk, size_t pos,
                 size_t max_pos);

token* scan(Arena a, const_string str);

void print_tokens(token* t);

#endif
