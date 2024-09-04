#include "scanner.h"

bool isTerm(char c)
{
  return c != '\\' && c != '(' && c != ')' && c != ' ' && c != '.';
}

size_t parseTerm(Arena a, const_string parse_str, token* tk, size_t pos,
                 size_t max_pos)
{
  tk->type = tok_TERM;
  const_string term = {0};
  while (isTerm(parse_str._str[pos - 1]) && pos <= max_pos) {
    term = const_str(str_cat(a, term, str_from_char(&parse_str._str[pos - 1])));
    ++pos;
  }
  tk->term_str = term;
  return pos - 1;
}

token* scan(Arena a, const_string str)
{
  if (str.size == 0) {
    return NULL;
  }

  size_t pos = 0;
  token* res = arena_zalloc(a, token, 1);
  token* cur = res;

  while (1) {
    cur->pos = pos;
    ++pos;

    if (pos <= str.size) {
      switch (str._str[pos - 1]) {
        case '\\': cur->type = tok_LAMBDA; break;
        case '(': cur->type = tok_LPAREN; break;
        case ')': cur->type = tok_RPAREN; break;
        case '.': cur->type = tok_DOT; break;
        case '=': cur->type = tok_EQ; break;

        // Ignore
        case ' ': continue;

        default: // It's a term
          pos = parseTerm(a, str, cur, pos, str.size);
          break;
      }
    }

    // Don't alloc if we are at the end
    if (pos >= str.size) {
      break;
    }

    cur->nxt = arena_zalloc(a, token, 1);
    cur->nxt->prv = cur;
    cur = cur->nxt;
  }

  return res;
}

void print_tokens(token* t)
{
  printf("Begin\n");
  while (t) {
    switch (t->type) {
      case tok_NULL: printf("NULL "); break;
      case tok_DOT: printf("DOT "); break;
      case tok_LPAREN: printf("LPAREN "); break;
      case tok_RPAREN: printf("RPAREN "); break;
      case tok_LAMBDA: printf("LAMBDA "); break;
      case tok_TERM:
        printf("TERM(");
        str_print(t->term_str);
        printf(") ");
        break;
      default: printf("  FAILFAILFAIL  ");
    }
    t = t->nxt;
  }

  printf("\nEnd\n");
}
