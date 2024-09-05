/*
void exp_test(void)
{
  exp x = {.type = exp_VAR, .toVar.str = str_lit("x")};
  exp abs_x = {.type = exp_ABS, .toAbs.var.str = str_lit("x"), .toAbs.exp = &x};
  exp abs_x_y = {
    .type = exp_ABS, .toAbs.var.str = str_lit("y"), .toAbs.exp = &abs_x};

  exp app = {.type = exp_APP, .toApp.exp1 = &abs_x_y, .toApp.exp2 = &abs_x};

  print_exp(&app);
}

void exp_test2(void)
{
  // Expression: `(\x.x y)z`
  exp x = {.type = exp_VAR, .toVar = {str_lit("x")}};
  exp y = {.type = exp_VAR, .toVar = {str_lit("y")}};
  exp z = {.type = exp_VAR, .toVar = {str_lit("z")}};
  exp innerExp = {
    .type = exp_APP, .toApp = {.exp1 = &x, .exp2 = &y}
  };
  exp innerAbs = {
    .type = exp_ABS, .toAbs = {.var = {str_lit("x")}, .exp = &innerExp}
  };
  exp full = {
    .type = exp_APP, .toApp = {.exp1 = &innerAbs, .exp2 = &z}
  };

  print_exp(&full);
}

void scanner_test(void)
{
  Arena input_arena = arena_init();
  const_string inp = const_str(str_readln(input_arena));

  Arena token_arena = arena_init();
  token* tokens = scan(token_arena, inp);
  arena_free(input_arena);

  print_tokens(tokens);
  arena_free(token_arena);
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
*/
