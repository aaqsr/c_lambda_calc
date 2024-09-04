#ifndef AAQ_AST_H_
#define AAQ_AST_H_

#include "str.h"

typedef struct Exp Exp;

typedef struct
{
  const_string str;
} Variable;

typedef struct
{
  Variable var;
  Exp* exp;
} Abstraction;

typedef struct
{
  Exp* exp1;
  Exp* exp2;
} Application;

struct Exp
{
  enum {
    exp_NULL = 0,
    exp_VAR, // Variable
    exp_ABS, // Abstraction
    exp_APP, // Application
  } type;

  union
  {
    Variable toVar;
    Abstraction toAbs;
    Application toApp;
  };
};

void print_exp(Exp* exp);

Variable deep_clone_var(Arena a, Variable v);
Abstraction deep_clone_abs(Arena a, Abstraction abs);
Application deep_clone_app(Arena a, Application app);
Exp deep_clone_exp(Arena a, const Exp* e);

bool exp_equal(Exp* e1, Exp* e2);

#endif
