#include "parser.h"
#include <assert.h>

// `term -> [A-Za-z]+`
Variable parse_var(Arena a, token* tks)
{
  assert(tks);
  assert(tks->type == tok_TERM);

  return (Variable){.str = const_str(str_copy(a, tks->term_str))};
}

parsedTree parse_term(Arena a, token* tks)
{
  assert(tks);
  Variable var = parse_var(a, tks);
  Exp* res = arena_zalloc(a, Exp, 1);
  *res = (Exp){.type = exp_VAR, .toVar = var};
  return (parsedTree){.res = res, .leftover = tks->nxt};
}

// `abstraction -> lambda term . exp`
// parsedTree parse_abs(Arena a, token* tks)
// {
//   assert(tks); // Lambda
//
//   exp* res = arena_zalloc(a, exp, 1);
//
//   res->type = exp_ABS;
//   res->toAbs.var = parse_var(a, tks->nxt);
//   parsedTree exp_res = parse_app(a, tks->nxt->nxt->nxt);
//   res->toAbs.exp = exp_res.res;
//
//   return (parsedTree){.res = res, .leftover = exp_res.leftover};
// }

// `abstraction -> lambda term term* . exp`
// `\x y . x  --> \x . \y . z`

// assume first token is term after lambda
parsedTree parse_abs(Arena a, token* tks)
{
  assert(tks);

  Exp* res = arena_zalloc(a, Exp, 1);
  res->type = exp_ABS;
  res->toAbs.var = parse_var(a, tks);

  parsedTree exp_res = {0};

  if (tks->nxt->type == tok_DOT) {
    exp_res = parse_app(a, tks->nxt->nxt);
  } else {
    // More terms
    exp_res = parse_abs(a, tks->nxt);
  }

  res->toAbs.exp = exp_res.res;
  return (parsedTree){.res = res, .leftover = exp_res.leftover};
}

// Parse as much as we can and then return the leftover tokens
parsedTree parse_exp(Arena a, token* tks)
{
  parsedTree res = {0};
  assert(tks);
  switch (tks->type) {
    case tok_LAMBDA: res = parse_abs(a, tks->nxt); break;
    case tok_TERM: {
      // Parse the term
      res = parse_term(a, tks);
    } break;
    case tok_LPAREN: {
      res = parse_app(a, tks->nxt);
      // skip over the right paren
      if (res.leftover)
        res.leftover = res.leftover->nxt;
    } break;
  }
  return res;
}

parsedTree parse_app(Arena a, token* tks)
{
  parsedTree parsed_exp1 = parse_exp(a, tks);

  if (parsed_exp1.leftover == NULL || parsed_exp1.leftover->type == tok_RPAREN)
  {
    return parsed_exp1;
  }

  Exp* res = arena_zalloc(a, Exp, 1);
  res->type = exp_APP;

  parsedTree parsed_exp2 = parse_exp(a, parsed_exp1.leftover);

  res->toApp = (Application){.exp1 = parsed_exp1.res, .exp2 = parsed_exp2.res};

  while (parsed_exp2.leftover && parsed_exp2.leftover->type != tok_RPAREN) {
    Exp* tmp = res;
    res = arena_zalloc(a, Exp, 1);
    res->type = exp_APP;
    res->toApp.exp1 = tmp;
    parsed_exp2 = parse_exp(a, parsed_exp2.leftover);
    res->toApp.exp2 = parsed_exp2.res;
  }

  return (parsedTree){.res = res, .leftover = parsed_exp2.leftover};
}

Exp* parse(Arena a, token* tks)
{
  parsedTree res = parse_app(a, tks);
  assert(res.leftover == NULL);
  return res.res;
}
