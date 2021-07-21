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
#include <inttypes.h>
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

typedef enum {
    INTEGER,
    FLOATING,
    UNKNOWN
} eval_mode;

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

static const int PRECEDENCE[] = {
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

static const char TOKEN_REPRS[] = {
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

static const char NUMERIC_CHARS[] = "0123456789abcdefx.";

////// Debug

// Write list of tokens
static void dump(token_list *tl)
{
    for (int i = 0; i < tl->length; i++)
    {
        printf("[%s]", (tl->tokens + i)->string);
    }
    printf("\n");
}

////// Tokenizer

// Push a token onto the list
static void token_push(token_list *tl, token *t)
{
    ensure(tl->length < MAX_N_TOKENS, "Too many tokens");

    // Copy token memory
    int new_stack_top = tl->length;
    memcpy(tl->tokens + new_stack_top, t, sizeof(token));

    // Update stack pointer
    tl->length++;
}

// Create a token from type and name, and push it onto the list
static void token_create_and_push(token_list *tl, token_type type, const char *name, int name_length)
{
    ensure(tl->length < MAX_N_TOKENS, "Too many tokens");

    // Create token structure
    token t = {};
    t.type = type;
    strncpy(t.string, name, name_length);

    // Add it
    token_push(tl, &t);
}

// If the character is an operator, return the token type for it. Otherwise return none
static token_type operator_token_type(char c)
{
    for (int j = 0; j < ARRAY_SIZE(TOKEN_REPRS); j++)
    {
        if (c == TOKEN_REPRS[j])
        {
            return (token_type)j;
        }
    }
    return NONE;
}

// Returns whether the character is numeric
static token_type is_numeric(char c)
{
    for (int j = 0; j < ARRAY_SIZE(NUMERIC_CHARS); j++)
    {
        if (c == NUMERIC_CHARS[j])
        {
            return true;
        }
    }
    return false;
}

// Convert the input string into a list of tokens
static token_list tokenize(line_buffer expression)
{
    // Initialize token list
    token_list tl = {};

    // Keeps track of where the current number started
    int current_number_started = 0;

    // Keeps track of whether the previous character was part of a number
    bool had_number = false;

    // Iterate over the string including null terminator
    int expr_length = strlen(expression) + 1;
    for (int i = 0; i < expr_length; i++)
    {
        // Read a character
        char c = expression[i];

        // Initialize token type

        // Is the current character an operator, and if so what is it?
        token_type cur_token_type = operator_token_type(c);
        bool have_operator = cur_token_type != NONE;

        // Valid non-numeric characters are operators and terminators
        bool have_valid_non_numeric = have_operator || c == '\0' || c == ' ';

        // If not non-numeric, assume it is a number (will be validated later)
        bool have_number = !have_valid_non_numeric;

        // Handle non-numeric case
        if (have_valid_non_numeric)
        {
            // If we had a number up to now, push a token for that number
            if (had_number)
            {
                token_create_and_push(
                    &tl,
                    NUMBER,
                    expression + current_number_started,
                    i - current_number_started
                );
            }

            // If this was an operator, push a token for that operator
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

        // Validate numeric
        if (have_number && !is_numeric(c))
        {
            line_buffer error = {};
            snprintf(error, sizeof(error), "Not an operator nor a number at index %i in '%s': '%c'", i, expression, c);
            ensure(false, error);
        }

        // If this is the start of a number, note that down
        if (have_number && !had_number)
        {
            current_number_started = i;
        }

        // Update for next round
        had_number = have_number;
    }
    return tl;
}

/////// Shunting yard algorithm

// Returns the token at the tip of the list, if any (otherwise returns NULL)
static inline token *token_peek(token_list *tl)
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

// Drops a token from the list
static void token_discard(token_list *tl)
{
    ensure(tl->length > 0, "Discard from empty token list");

    // Clear the token memory (might not be necessary)
    memset(tl->tokens + tl->length - 1, 0, sizeof(token));

    tl->length--;
}

// Converts from infix to postfix
static token_list infix_to_postfix(token_list *input)
{
    // Initialize output
    token_list output = {};

    // Initialize operator stack
    token_list operators = {};

    // Iterate over each element in the input
    for (int i = 0; i < input->length; i++)
    {
        // What kind of token is it?
        token *t = input->tokens + i;

        switch (t->type)
        {
            case NUMBER:
                // Add number to queue
                token_push(&output, t);
                break;
            case LPAREN:
                // Add left paren to operator stack
                token_push(&operators, t);
                break;
            case RPAREN:
                // Locate right paren
                while (1)
                {
                    // Examine token at the top of the stack
                    token *stack_top = token_peek(&operators);
                    ensure(stack_top, "Mismatched parentheses");

                    // If it is an lparen, then it is the one that matches this rparen
                    if (stack_top->type == LPAREN)
                    {
                        token_discard(&operators);
                        break;
                    }

                    // Move the examined from the operator stack to the output
                    // These will already be in the right order
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
                // Move things around so that the stack is in the right order
                while (1)
                {
                    // Check whatever is currently at the stack
                    token *stack_top = token_peek(&operators);

                    // Nothing there -> nothing to do
                    if (!stack_top)
                    {
                        break;
                    }

                    // Lparen -> we are the first entry in a parenthesized expression -> nothing to do
                    if (stack_top->type == LPAREN)
                    {
                        break;
                    }

                    // Higher-or-equal precedence operator => needs to be evaluated before us -> push to output now
                    // NOTE: not correct for right-associative operators. We don't have any currently.
                    if (PRECEDENCE[stack_top->type] >= PRECEDENCE[t->type])
                    {
                        token_push(&output, stack_top);
                        token_discard(&operators);
                    }
                    else
                    {
                        // Lower or equal precedence => we go first
                        break;
                    }
                }

                // All done, now push the current operator to the stack
                token_push(&operators, t);
                break;
            default:
                ensure(false, "Unexpected operator");
                break;
        }
    }

    // Pop all remaining operators from the stack to the output
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

// If the token *requires* a certain evaluation mode, then return that, otherwise return unknown
static eval_mode get_eval_mode_for_token(token *t)
{
    // If we don't find anything, return unknown
    eval_mode result = UNKNOWN;

    switch (t->type)
    {
        case NUMBER:
        {
            char *string = t->string;
            if ((string[0] == '0') && ((string[1] == 'x') || (string[1] == 'b')))
            {
                // Starts with 0x or 0b prefix, so we probably want integer logic
                result = INTEGER;
            }
            else
            {
                // If there is a '.' in there, we want floating logic
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
        }
            break;
        case AND:
        case OR:
        case XOR:
            // Bit-oriented operations only make sense in integer logic
            result = INTEGER;
            break;
        case DIVIDED_BY:
            // Need floating after division to avoid silently truncating
            result = FLOATING;
            break;
        default:
            break;
    }
    return result;
}

// Returns the evaluation mode for the input
// If initial is not UNKNOWN, this validates that all tokens support that mode
static eval_mode get_eval_mode(token_list *input, eval_mode initial)
{
    eval_mode result = initial;
    for (int i = 0; i < input->length; i++)
    {
        // Does this token require a specific mode?
        eval_mode required_mode = get_eval_mode_for_token(input->tokens + i);
        if (required_mode != UNKNOWN)
        {
            // Make sure it is consistent with what we have so far
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

// Evaluate a token list with postfix operators (2 + 2 * 3 => 2 2 3 * +)
static int64_t eval_postfix(token_list *input)
{
    // Initialize number stack
    int64_t stack[MAX_N_TOKENS] = {};
    int stack_size = 0;

    for (int i = 0; i < input->length; i++)
    {
        // Look at next token
        token *t = input->tokens + i;

        if (t->type == NUMBER)
        {
            // Parse the number and add it to the stack
            int64_t n = strtol(t->string, 0, 0);
            stack[stack_size] = n;
            stack_size++;
        }
        else
        {
            // There should be 2 or more numbers on the stack when we encounter an operator
            ensure(stack_size >= 2, "Unexpected bonus operator");

            // Pop the entries; they are the operands
            int64_t a = stack[stack_size - 2];
            int64_t b = stack[stack_size - 1];
            stack_size -= 2;

            // Evaluate
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
                default:
                    ensure(false, "Unexpected operator in stack at eval time");
                    break;
            }

            // Push the result onto the stack
            stack[stack_size] = result;
            stack_size++;
        }
    }

    // At the end, the stack should just have the result of the overall expression
    ensure(stack_size == 1, "Invalid stack size at end of eval");
    return stack[0];
}

// Evaluation for doubles

static double eval_postfix_double(token_list *input)
{
    // Initialize number stack
    double stack[MAX_N_TOKENS] = {};
    int stack_size = 0;
    for (int i = 0; i < input->length; i++)
    {
        // Look at next token
        token *t = input->tokens + i;
        if (t->type == NUMBER)
        {
            // Parse the number and add it to the stack
            double n = strtod(t->string, 0);
            stack[stack_size] = n;
            stack_size++;
        }
        else
        {
            // There should be 2 or more numbers on the stack when we encounter an operator
            ensure(stack_size >= 2, "Unexpected bonus operator");

            // Pop the entries; they are the operands
            double a = stack[stack_size - 2];
            double b = stack[stack_size - 1];
            stack_size -= 2;

            // Evaluate
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
                default:
                    ensure(false, "Unexpected operator in stack at eval time");
                    break;
            }

            // Push result to the stack
            stack[stack_size] = result;
            stack_size++;
        }
    }

    // At the end, the stack should just have the result of the overall expression
    ensure(stack_size == 1, "Invalid stack size at end of eval");
    return stack[0];
}

// Main

static void help(void)
{
    printf(
        "Usage: clc <expression>\n"
        "\n"
        "Supported operators: + - * / %% & | ^ ( )\n"
        "Supported number formats: decimal, hex, binary, octal, float\n"
        ""
    );
}

void print_bin(int64_t result)
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

void print_double(double result)
{
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

int main(int argc, char **argv)
{
    // Print help if required
    if (argc == 1)
    {
        help();
        return 1;
    }

    // By default, expression starts at the first argument
    int expr_start = 1;

    // Allow one argument to specify output mode - hex or bin
    // If set, the expression starts one argument later
    bool hex = false;
    bool bin = false;
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

    // Combine all remaining args into the expression
    line_buffer expression = {};
    for (int i = expr_start; i < argc; i++)
    {
        concatenate(expression, argv[i]);
    }

    // Generate token list
    token_list tl = tokenize(expression);

    // Convert it to postfix
    tl = infix_to_postfix(&tl);

    // Determine the evaluation mode
    int eval_mode = UNKNOWN;
    if (hex || bin)
    {
        eval_mode = INTEGER;
    }
    eval_mode = get_eval_mode(&tl, eval_mode);
    if (eval_mode == UNKNOWN)
    {
        eval_mode = INTEGER;
    }

    // Evaluate and display depending on the evaluation mode
    if (eval_mode == INTEGER)
    {
        // Evaluate
        int64_t result = eval_postfix(&tl);

        // Display
        if (hex)
        {
            printf("0x%" PRId64 "\n", result);
        }
        else if (bin)
        {
            print_bin(result);
        }
        else
        {
            printf("%" PRId64 "\n", result);
        }
    }
    else
    {
        // Evaluate
        double result = eval_postfix_double(&tl);

        // Display
        print_double(result);
    }
    return 0;
}
