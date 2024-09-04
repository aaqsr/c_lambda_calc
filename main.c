#include "arena.h"
#include "ast.h"
#include "colours.h"
#include "parser.h"
#include "scanner.h"
#include "str.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define TODO() (assert(0 && "TODO"), 0)

Exp deep_clone_exp(Arena a, const Exp* e);

typedef struct context_node context_node;

struct context_node
{
  Variable var;
  Exp* exp;
  context_node* nxt;
};

Exp* search_context(context_node* context, Variable var)
{
  while (context) {
    if (str_eq(context->var.str, var.str)) {
      return context->exp;
    }
    context = context->nxt;
  }
  return NULL;
}

void print_context(context_node* context)
{
  printf("Defn'\n");
  while (context) {
    printf("  ");
    str_print(context->var.str);
    printf(" = ");
    print_exp(context->exp);
    context = context->nxt;
  }
}

bool isFreeVar(Exp* e, Variable v)
{
  assert(e);

  // printf("IsFreeVar Var: ");
  // str_print(v.str);
  // printf(", Exp (Address %p): ", e);
  // print_exp(e);

  switch (e->type) {
    case exp_VAR: return str_eq(e->toVar.str, v.str);
    case exp_ABS:
      return (!str_eq(e->toAbs.var.str, v.str) && isFreeVar(e->toAbs.exp, v));
    case exp_APP:
      return (isFreeVar(e->toApp.exp1, v) || isFreeVar(e->toApp.exp2, v));
  }

  return false;
}

int counter(void)
{
  static int count = 1;
  return count++;
}

void alpha_equiv(Arena a, Abstraction* abs);

void substitute(Arena a, Exp* ast, Variable target, Exp* subs)
{
  assert(ast);
  switch (ast->type) {
    case exp_VAR:
      if (str_eq(ast->toVar.str, target.str)) {
        *ast = deep_clone_exp(a, subs);
      }
      return;
    case exp_ABS:
      // $(λy.e1)[x<-e2] = λy.e1[x<-e2]$
      // where $x != y$, and $y$ not in $Fv[e2]$

      // Variable is free in the lambda
      if (isFreeVar(ast, target)) {
        // We never want to do a substitution that makes a free variable bound
        // to prevent capture
        if (isFreeVar(subs, ast->toAbs.var)) {
          // We perform alpha_equiv to replace the bound variable and continue
          // with the substitution Add a number in front of the bound variable
          alpha_equiv(a, &(ast->toAbs));
          substitute(a, ast, target, subs);
        } else {
          substitute(a, ast->toAbs.exp, target, subs);
        }
      }
      return;
    case exp_APP:
      substitute(a, ast->toApp.exp1, target, subs);
      substitute(a, ast->toApp.exp2, target, subs);
      return;
  }
}

// $λx.e =_\alpha λy.e[x<-y]$, $y$ not in $Fv[e]$
void alpha_equiv(Arena a, Abstraction* abs)
{
  Variable old_bound = abs->var;
  Variable new_bound =
    (Variable){.str = const_str(str_cat(
                 a, abs->var.str, const_str(str_from_integer(a, counter()))))};
  Exp new_bound_exp = (Exp){.type = exp_VAR, .toVar = new_bound};

  // Make the change in the rest of the expression too
  substitute(a, abs->exp, old_bound, &new_bound_exp);
  abs->var = new_bound;
}

Exp* beta_reduce(Arena a, Exp* abs, Exp* subs)
{
  assert(abs->type == exp_ABS);
  substitute(a, abs->toAbs.exp, abs->toAbs.var, subs);
  return abs->toAbs.exp;
}

Variable deep_clone_var(Arena a, Variable v)
{
  return (Variable){.str = const_str(str_copy(a, v.str))};
}

Abstraction deep_clone_abs(Arena a, Abstraction abs)
{
  Exp* clone = arena_zalloc(a, Exp, 1);
  *clone = deep_clone_exp(a, abs.exp);
  return (Abstraction){.var = deep_clone_var(a, abs.var), .exp = clone};
}

Application deep_clone_app(Arena a, Application app)
{
  Exp* clone1 = arena_zalloc(a, Exp, 1);
  *clone1 = deep_clone_exp(a, app.exp1);
  Exp* clone2 = arena_zalloc(a, Exp, 1);
  *clone2 = deep_clone_exp(a, app.exp2);
  return (Application){.exp1 = clone1, .exp2 = clone2};
}

Exp deep_clone_exp(Arena a, const Exp* e)
{
  assert(e);

  Exp res = {0};
  res.type = e->type;

  switch (e->type) {
    case exp_VAR: res.toVar = deep_clone_var(a, e->toVar); break;
    case exp_ABS: res.toAbs = deep_clone_abs(a, e->toAbs); break;
    case exp_APP: res.toApp = deep_clone_app(a, e->toApp); break;
  }

  return res;
}

void lazy_interp(Arena a, Exp* ast, context_node* context)
{
  switch (ast->type) {
    case exp_APP:
      lazy_interp(a, ast->toApp.exp1, context);
      if (ast->toApp.exp1->type == exp_ABS) {
        *ast = *(beta_reduce(a, ast->toApp.exp1, ast->toApp.exp2));
        lazy_interp(a, ast, context);
      } else {
        lazy_interp(a, ast->toApp.exp2, context);
      }
      break;

    case exp_VAR: {
      Exp* sub = NULL;
      if ((sub = search_context(context, ast->toVar))) {
        *ast = deep_clone_exp(a, sub);
      }
    } break;
  }
}

void scanner_parser_print_test(void)
{
  Arena input_arena = arena_init();
  Arena token_arena = arena_init();
  Arena exp_arena = arena_init();

  const_string main_inp = const_str(str_readln(input_arena));
  token* main_tokens = scan(token_arena, main_inp);
  Exp* main_expr = parse(exp_arena, main_tokens);

  print_exp(main_expr);

  // `printf("%d\n", isFreeVar(expressions, (Variable){.str = inp}));`
  Variable target = (Variable){.str = const_str(str_readln(input_arena))};

  const_string sub_inp = const_str(str_readln(input_arena));
  token* sub_tokens = scan(token_arena, sub_inp);
  Exp* substitution = parse(exp_arena, sub_tokens);

  substitute(exp_arena, main_expr, target, substitution);
  print_exp(main_expr);

  arena_free(input_arena);
  arena_free(exp_arena);
  arena_free(token_arena);
}

bool isReducible(Exp* e, context_node* ctx)
{
  switch (e->type) {
    case exp_VAR: return (search_context(ctx, e->toVar) != NULL);
    case exp_ABS: return (isReducible(e->toAbs.exp, ctx));
    case exp_APP:
      return (e->toApp.exp1->type == exp_ABS ||
              isReducible(e->toApp.exp1, ctx) ||
              isReducible(e->toApp.exp2, ctx));
  }
  return false;
}

bool exp_equal(Exp* e1, Exp* e2)
{
  assert(e1 && e2);

  if (e1->type != e2->type) {
    return false;
  }

  switch (e1->type) {
    case exp_VAR: return str_eq(e1->toVar.str, e2->toVar.str);
    case exp_ABS:
      return (str_eq(e1->toAbs.var.str, e2->toAbs.var.str) &&
              exp_equal(e1->toAbs.exp, e2->toAbs.exp));
    case exp_APP:
      return (exp_equal(e1->toApp.exp1, e2->toApp.exp1) &&
              exp_equal(e1->toApp.exp2, e2->toApp.exp2));
  }

  return false;
}

// clean up,
// tests
// better errors
// better seg fault tolerance
int main(int argc, char* argv[])
{
  Arena input_arena = arena_init();
  Arena token_arena = arena_init();
  Arena exp_arena = arena_init();
  Arena context_arena = arena_init();
  Arena prev_exprs = arena_init();
  Arena scratch = arena_init();

  context_node* context = NULL;
  context_node* context_cur = NULL;

  if (argc == 2) {
    FILE* f = fopen(argv[1], "r");
    const_string defn_inp;
    while (!str_eq(defn_inp = const_str(str_readln_from_file(input_arena, f)),
                   const_str(EMPTY_STR)))
    {
      token* tokens = scan(token_arena, defn_inp);

      if (tokens && tokens->type == tok_TERM && tokens->nxt &&
          tokens->nxt->type == tok_EQ)
      {
        if (context) {
          context_cur->nxt = arena_zalloc(context_arena, context_node, 1);
          context_cur = context_cur->nxt;
        } else {
          context = arena_zalloc(context_arena, context_node, 1);
          context_cur = context;
        }
        context_cur->exp = parse(context_arena, tokens->nxt->nxt);
        context_cur->var = (Variable){
          .str = const_str(str_copy(context_arena, tokens->term_str))};
      }
    }
  }

  arena_reset(input_arena);
  arena_reset(token_arena);

  while (1) {
    printf(GRN "> " RST);
    const_string main_inp = const_str(str_readln(input_arena));
    // TODO: Make this into command interpreter with commands beginning with '?'
    if (str_eq(main_inp, str_lit("?def"))) {
      arena_reset(input_arena);
      print_context(context);
      continue;
    }

    token* main_tokens = scan(token_arena, main_inp);

    // It's a new assignment
    if (main_tokens && main_tokens->type == tok_TERM && main_tokens->nxt &&
        main_tokens->nxt->type == tok_EQ)
    {
      if (context) {
        context_cur->nxt = arena_zalloc(context_arena, context_node, 1);
        context_cur = context_cur->nxt;
      } else {
        context = arena_zalloc(context_arena, context_node, 1);
        context_cur = context;
      }
      context_cur->exp = parse(context_arena, main_tokens->nxt->nxt);
      context_cur->var = (Variable){
        .str = const_str(str_copy(context_arena, main_tokens->term_str))};
    } else {
      Exp* main_expr = parse(exp_arena, main_tokens);
      lazy_interp(exp_arena, main_expr, context);
      // If it can be reduced, reduce it
      // TODO: Add a history
      while (isReducible(main_expr, context)) {
        *arena_zalloc(prev_exprs, Exp, 1) = deep_clone_exp(scratch, main_expr);
        lazy_interp(exp_arena, main_expr, context);
      }
      arena_reset(scratch);
      arena_reset(prev_exprs);
      printf(YEL "  -> " RST);
      print_exp(main_expr);
    }

    arena_reset(input_arena);
    arena_reset(exp_arena);
    arena_reset(token_arena);
  }

  arena_free(scratch);
  arena_free(prev_exprs);
  arena_free(context_arena);
  arena_free(input_arena);
  arena_free(exp_arena);
  arena_free(token_arena);
}
