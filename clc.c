/**
 * Calculator
 *
 * Limitations:
 * - no functions (sqrt, sin, ...)
 * - no unary operators (-, ~)
 * - no multi-char operators (**)
 * - no support for right-associativity (**)
 * - lots of buffer overflow opportunities
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "shell_utils.h"

#define MAX_N_TOKENS 32
#define MAX_TOKEN_SIZE 32
#define FRACTIONAL_BITS 8

typedef enum {
    NONE = 0,
    NUMBER,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    TIMES,
    DIVIDED_BY,
    MODULO,
    AND,
    XOR,
    OR,
    EOE // Sets max size of array
} token_type;

int PRECEDENCE[] = {
    [LPAREN] = 50,
    [RPAREN] = 50,
    [MODULO] = 40,
    [DIVIDED_BY] = 40,
    [TIMES] = 40,
    [MINUS] = 30,
    [PLUS] = 30,
    [AND] = 20,
    [XOR] = 10,
    [OR] = 0,
    [EOE] = 0
};

char TOKEN_REPRS[] = {
    [NONE] = '?',
    [NUMBER] = 'N',
    [LPAREN] = '(',
    [RPAREN] = ')',
    [PLUS] = '+',
    [MINUS] = '-',
    [TIMES] = '*',
    [DIVIDED_BY] = '/',
    [MODULO] = '%',
    [AND] = '&',
    [XOR] = '^',
    [OR] = '|',
    [EOE] = '!'
};

typedef enum {
    INTEGER,
    FLOATING,
    UNKNOWN
} eval_mode;

char NUMERIC_CHARS[] = "0123456789abcdefx.";

typedef struct {
    token_type type;
    char string[MAX_TOKEN_SIZE];
} token;

typedef struct token_list {
    token tokens[MAX_N_TOKENS];
    int length;
} token_list;

typedef struct result_t {
    double real;
    int64_t integer;
    int64_t fixedpoint;
} result_t;

////// Debug

void dump(token_list *tl)
{
    for (int i = 0; i < tl->length; i++)
    {
        printf("[%s]", (tl->tokens + i)->string);
    }
    printf("\n");
}

////// Tokenizer

void token_push(token_list *tl, token *t)
{
    ensure(tl->length < MAX_N_TOKENS, "Too many tokens");
    int prev_length = tl->length;
    memcpy(tl->tokens + prev_length, t, sizeof(token));
    tl->length++;
}

void token_create_and_push(token_list *tl, token_type type, const char *name, int name_length)
{
    ensure(tl->length < MAX_N_TOKENS, "Too many tokens");
    token t = {};
    t.type = type;
    strncpy(t.string, name, name_length);
    token_push(tl, &t);
}

token_list tokenize(line_buffer expression)
{
    token_list tl = {};
    int expr_length = strlen(expression) + 1;
    int number_start = 0;
    bool had_number = false;
    for (int i = 0; i < expr_length; i++)
    {
        char c = expression[i];
        token_type cur_token_type = NONE;
        bool have_number = false;
        bool have_operator = false;
        for (int j = 0; j < ARRAY_SIZE(TOKEN_REPRS); j++)
        {
            if (c == TOKEN_REPRS[j])
            {
                cur_token_type = (token_type)j;
                have_operator = true;
                break;
            }
        }
        if (have_operator || c == '\0' || c == ' ')
        {
            if (had_number)
            {
                token_create_and_push(
                    &tl,
                    NUMBER,
                    expression + number_start,
                    i - number_start
                );
            }
            if (have_operator)
            {
                token_create_and_push(
                    &tl,
                    cur_token_type,
                    expression + i,
                    1
                );
            }
        }
        else
        {
            for (int j = 0; j < ARRAY_SIZE(NUMERIC_CHARS); j++)
            {
                if (c == NUMERIC_CHARS[j])
                {
                    cur_token_type = NUMBER;
                    have_number = true;
                    break;
                }
            }
            if (!have_number)
            {
                line_buffer error = {};
                snprintf(error, sizeof(error), "Not an operator nor a number at index %i in '%s': '%c'", i, expression, c);
                ensure(false, error);
            }
        }
        if (have_number && !had_number)
        {
            number_start = i;
        }
        had_number = have_number;
    }
    return tl;
}

/////// Shunting yard algorithm

token *token_peek(token_list *tl)
{
    if (tl->length > 0)
    {
        return tl->tokens + tl->length - 1;
    }
    else
    {
        return NULL;
    }
}

void token_discard(token_list *tl)
{
    ensure(tl->length > 0, "Discard from empty token list");
    memset(tl->tokens + tl->length - 1, 0, sizeof(token));
    tl->length--;
}

token token_pop(token_list *tl)
{
    token *out_p = token_peek(tl);
    ensure(out_p, "Pop from empty token list");
    token out = *out_p;
    token_discard(tl);
    return out;
}

token_list infix_to_postfix(token_list *input)
{
    token_list output = {};
    token_list operators = {};
    for (int i = 0; i < input->length; i++)
    {
        token *t = input->tokens + i;
        switch (t->type)
        {
            case NUMBER:
                token_push(&output, t);
                break;
            case LPAREN:
                token_push(&operators, t);
                break;
            case RPAREN:
                while (1)
                {
                    token *stack_top = token_peek(&operators);
                    ensure(stack_top, "Mismatched parentheses");
                    if (stack_top->type == LPAREN)
                    {
                        token_discard(&operators);
                        break;
                    }
                    token_push(&output, stack_top);
                    token_discard(&operators);
                }
                break;
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDED_BY:
            case MODULO:
            case AND:
            case OR:
            case XOR:
                while (1)
                {
                    token *stack_top = token_peek(&operators);
                    if (!stack_top)
                    {
                        break;
                    }
                    if (stack_top->type == LPAREN)
                    {
                        break;
                    }
                    if (PRECEDENCE[stack_top->type] >= PRECEDENCE[t->type])
                    {
                        token_push(&output, stack_top);
                        token_discard(&operators);
                    }
                    else
                    {
                        break;
                    }
                }
                token_push(&operators, t);
                break;
            default:
                break;
        }
    }
    int remaining_operators = operators.length;
    for (int i = 0; i < remaining_operators; i++)
    {
        token *stack_top = token_peek(&operators);
        ensure(stack_top, "Null operator in stack");
        ensure(stack_top->type != LPAREN, "Mismatched parentheses");
        token_push(&output, stack_top);
        token_discard(&operators);
    }

    return output;
}

////// Evaluation mode

eval_mode get_eval_mode_for_token(token *t)
{
    eval_mode result = UNKNOWN;
    switch (t->type)
    {
        case NUMBER:
            char *string = t->string;
            if ((string[0] == '0') && ((string[1] == 'x') || (string[1] == 'b')))
            {
                result = INTEGER;
            }
            else
            {
                while (*string)
                {
                    if (*string == '.')
                    {
                        result = FLOATING;
                        break;
                    }
                    string++;
                }
            }
            break;
        case AND:
        case OR:
        case XOR:
            result = INTEGER;
            break;
        case DIVIDED_BY:
            result = FLOATING;
            break;
        default:
            break;
    }
    return result;
}

eval_mode get_eval_mode(token_list *input, eval_mode initial)
{
    eval_mode result = initial;
    for (int i = 0; i < input->length; i++)
    {
        eval_mode required_mode = get_eval_mode_for_token(input->tokens + i);
        if (required_mode != UNKNOWN)
        {
            ensure(
                (result == UNKNOWN) || (required_mode == result),
                "both integer and floating point constructions in expression"
            );
            result = required_mode;
        }
    }
    return result;
}

// Evaluation for integers

int64_t eval_postfix(token_list *input)
{
    int64_t stack[MAX_N_TOKENS] = {};
    int stack_size = 0;
    for (int i = 0; i < input->length; i++)
    {
        token *t = input->tokens + i;
        if (t->type == NUMBER)
        {
            int64_t n = strtol(t->string, 0, 0);
            stack[stack_size] = n;
            stack_size++;
        }
        else
        {
            ensure(stack_size >= 2, "Unexpected bonus operator");
            int64_t a = stack[stack_size - 2];
            int64_t b = stack[stack_size - 1];
            stack_size -= 2;
            int64_t result = 0;
            switch (t->type)
            {
                case PLUS:
                    result = a + b;
                    break;
                case MINUS:
                    result = a - b;
                    break;
                case TIMES:
                    result = a * b;
                    break;
                case DIVIDED_BY:
                    result = a / b;
                    break;
                case MODULO:
                    result = a % b;
                    break;
                case AND:
                    result = a & b;
                    break;
                case OR:
                    result = a | b;
                    break;
                case XOR:
                    result = a ^ b;
                    break;
                case NONE:
                case LPAREN:
                case RPAREN:
                case EOE:
                case NUMBER:
                    ensure(false, "Unexpected operator in stack at eval time");
                    break;
            }
            stack[stack_size] = result;
            stack_size++;
        }
    }
    ensure(stack_size == 1, "Invalid stack size at end of eval");
    return stack[0];
}

// Evaluation for doubles

double eval_postfix_double(token_list *input)
{
    double stack[MAX_N_TOKENS] = {};
    int stack_size = 0;
    for (int i = 0; i < input->length; i++)
    {
        token *t = input->tokens + i;
        if (t->type == NUMBER)
        {
            double n = strtod(t->string, 0);
            stack[stack_size] = n;
            stack_size++;
        }
        else
        {
            ensure(stack_size >= 2, "Unexpected bonus operator");
            double a = stack[stack_size - 2];
            double b = stack[stack_size - 1];
            stack_size -= 2;
            double result = 0;
            switch (t->type)
            {
                case PLUS:
                    result = a + b;
                    break;
                case MINUS:
                    result = a - b;
                    break;
                case TIMES:
                    result = a * b;
                    break;
                case DIVIDED_BY:
                    result = a / b;
                    break;
                case MODULO:
                    result = fmod(a, b);
                    break;
                case AND:
                case OR:
                case XOR:
                    ensure(false, "Invalid operator in floating mode");
                    break;
                case NONE:
                case LPAREN:
                case RPAREN:
                case EOE:
                case NUMBER:
                    ensure(false, "Unexpected operator in stack at eval time");
                    break;
            }
            stack[stack_size] = result;
            stack_size++;
        }
    }
    ensure(stack_size == 1, "Invalid stack size at end of eval");
    return stack[0];
}

// Main

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf(
"Usage: clc <expression>\n"
"\n"
"Supported operators: + - * / %% & | ^ ( )\n"
"Supported number formats: decimal, hex, binary, octal, float\n"
""
);
        return 1;
    }

    bool hex = false;
    bool bin = false;
    int expr_start = 1;
    if (strcmp(argv[1], "hex") == 0)
    {
        hex = true;
        expr_start = 2;
    }
    if (strcmp(argv[1], "bin") == 0)
    {
        bin = true;
        expr_start = 2;
    }
    line_buffer expression = {};
    for (int i = expr_start; i < argc; i++)
    {
        concatenate(expression, argv[i]);
    }
    token_list tl = tokenize(expression);
    tl = infix_to_postfix(&tl);
    int eval_mode = UNKNOWN;
    if (hex || bin)
    {
        eval_mode = INTEGER;
    }
    eval_mode = get_eval_mode(&tl, eval_mode);
    if ((eval_mode == UNKNOWN) || (eval_mode == INTEGER))
    {
        int64_t result = eval_postfix(&tl);
        if (hex)
        {
            printf("0x%llx\n", result);
        }
        else if (bin)
        {
            printf("0b");
            bool firstone = false;
            for (int i = 63; i >= 0; i--)
            {
                int64_t mask = 1LL << i;
                if (result & mask)
                {
                    printf("1");
                    firstone = true;
                }
                else
                {
                    if (firstone || i == 0)
                    {
                        printf("0");
                    }
                }
            }
            printf("\n");
        }
        else
        {
            printf("%lld\n", result);
        }
    }
    else
    {
        double result = eval_postfix_double(&tl);
        line_buffer result_txt = {};
        snprintf(result_txt, sizeof(result_txt), "%f", result);
        int result_len = strlen(result_txt);
        for (int i = result_len-1; i >= 0; i--)
        {
            if (result_txt[i] == '0')
            {
                result_txt[i] = '\0';
            }
            else if (result_txt[i] == '.')
            {
                result_txt[i] = '\0';
                break;
            }
            else
            {
                break;
            }
        }
        printf(result_txt);
    }
    return 0;
}
