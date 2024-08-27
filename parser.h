#ifndef AAQ_PARSER_H_
#define AAQ_PARSER_H_

#include "ast.h"
#include "scanner.h"

// Grammar
// Applications are left associative! a b c is ((a b) c), not (a (b c))!
// `exp -> abstraction | application | term | "(" expr ")"`
// `application -> exp exp exp*`
// `term -> [A-Za-z]+`
// `abstraction -> "\" term term* . exp`

// TODO: Add better errors
typedef struct
{
  size_t pos; // Position in the string
  enum {
    pErr_NULL = 0,
    pErr_UnclosedParen,
    pErr_UnopenedParen,
    pErr_MissingLambdaVar,
    pErr_MissingLambdaBody,
    pErr_EmptyExprList
  } type;
} ParserError;

typedef struct
{
  exp* res;
  token* leftover;
} parsedTree;


parsedTree parse_exp(Arena a, token* tks);
Variable parse_var(Arena a, token* tks);
parsedTree parse_term(Arena a, token* tks);
parsedTree parse_app(Arena a, token* tks);
parsedTree parse_abs(Arena a, token* tks);

exp* parse(Arena a, token* tks);

#endif
