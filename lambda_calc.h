#ifndef AAQ_LAMBDA_CALC_H_
#define AAQ_LAMBDA_CALC_H_

#include "ast.h"
#include "context.h"
#include <stdbool.h>

bool isFreeVar(Exp* e, Variable v);

void substitute(Arena a, Exp* ast, Variable target, Exp* subs);
// $λx.e =_\alpha λy.e[x<-y]$, $y$ not in $Fv[e]$
void alpha_equiv(Arena a, Abstraction* abs);

Exp* beta_reduce(Arena a, Exp* abs, Exp* subs);

void lazy_interp(Arena a, Exp* ast, context_list context);

bool isReducible(Exp* e, context_list ctx);

#endif
