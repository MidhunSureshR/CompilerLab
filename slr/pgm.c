/*
 * SLR Parser
 * <rmidhunsuresh@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define USE_PREDEFINED_GRAMMAR true
#define TAKE_USER_INPUT false

struct
{
    int num_states;
    int num_terminals;
    int num_nonterminals;
    int num_productions;
} grammar_data;

typedef struct
{
    /*
    Type    |   Symbol
    ------------------
    s       |   shift
    r       |   reduce
    a       |   accept
    x   |   Error
    ------------------
    */
    char type;
    int index;
} action_element;

typedef char terminal;
typedef char non_terminal;

typedef struct
{
    char left;
    char right[10];
} production;

/* Data Structures for parsing algorithm */
action_element **action_table;
int **goto_table;
terminal *t_list;
non_terminal *n_list;
production *productions;

/* Memory Allocation */
void alloc_action_table()
{
    action_table = malloc(sizeof(action_element *) * grammar_data.num_states);
    for (int i = 0; i < grammar_data.num_states; ++i)
        action_table[i] = malloc(sizeof(action_element) * grammar_data.num_terminals);
}

void alloc_goto_table()
{
    goto_table = malloc(sizeof(int *) * grammar_data.num_states);
    for (int i = 0; i < grammar_data.num_states; ++i)
        goto_table[i] = malloc(sizeof(int) * grammar_data.num_nonterminals);
}

/* Stack Implementation */
int stack[100];
int s_top = 0;

void push(int n)
{
    if (s_top + 1 < 100)
    {
        stack[++s_top] = n;
    }
    else
        printf("Buffer Overflow\n");
}

int pop()
{
    if (s_top - 1 >= 0)
    {
        int t = stack[s_top];
        s_top--;
        return t;
    }
    else
    {
        printf("Buffer Underflow\n");
        return -1;
    }
}

/* Convert char to array index */
int get_index(const char *list, const int size, char t)
{
    for (int i = 0; i < size; ++i)
    {
        if (list[i] == t)
            return i;
    }
    return -1;
}

int t_to_i(char t)
{
    return get_index(t_list, grammar_data.num_terminals, t);
}

int n_to_i(char t)
{
    return get_index(n_list, grammar_data.num_nonterminals, t);
}

/* Input & Output Helper*/
void int_input(const char *message, int *var)
{
    printf("%s", message);
    scanf("%d", var);
}

void action_input(const int state, char terminal)
{
    char inp[3];
    scanf("%s", inp);
    action_table[state][t_to_i(terminal)].type = inp[0];
    action_table[state][t_to_i(terminal)].index = inp[1] - '0';
}

void goto_input(const int state, char nonterminal)
{
    int inp;
    scanf("%d", &inp);
    goto_table[state][n_to_i(nonterminal)] = inp;
}

void print_production(production p)
{
    printf("\t%c->%s\n", p.left, p.right);
}

void ask_for_repeat(bool *toggle)
{
    printf("Do you want to repeat <1/0>?:");
    int t;
    scanf("%d", &t);
    *toggle = t;
}

void get_input_string(char *str)
{
    printf("Enter the string:\n");
    scanf("%s", str);
    strcat(str, "$");
}

/* SLR Parsing algorithm*/
void parse(char *string)
{
    s_top = -1;
    push(0);
    char *current = string;

    while (true)
    {
        int s = stack[s_top];
        action_element action_value = action_table[s][t_to_i(current[0])];
        if (action_value.type == 's')
        {
            // Shift
            push(action_value.index);
            current++;
        }
        else if (action_value.type == 'r')
        {
            // Reduce

            production p = productions[action_value.index];
            const int len = strlen(p.right);
            for (int i = 0; i < len; ++i)
            {
                pop();
            }
            s = stack[s_top];
            int goto_val = goto_table[s][n_to_i(p.left)];
            push(goto_val);
            print_production(p);
        }
        else if (action_value.type == 'a')
        {
            printf("String Accepted\n");
            break;
        }
        else
        {
            printf("String not accepted!\n");
            break;
        }
    }
}

void string_to_action_element(action_element *a, char *str)
{
    a->type = str[0];
    a->index = str[1] - '0';
}

void test()
{
    grammar_data.num_states = 6;
    grammar_data.num_nonterminals = 3;
    grammar_data.num_terminals = 3;
    grammar_data.num_productions = 4;

    alloc_action_table();
    alloc_goto_table();
    t_list = malloc(sizeof(char) * grammar_data.num_terminals);
    n_list = malloc(sizeof(char) * grammar_data.num_nonterminals);
    productions = malloc(sizeof(production) * grammar_data.num_productions);

    t_list[0] = '+';
    t_list[1] = 'i';
    t_list[2] = '$';

    n_list[0] = 'S';
    n_list[1] = 'E';
    n_list[2] = 'T';

    string_to_action_element(&action_table[0][0], "x0");
    string_to_action_element(&action_table[0][1], "s3");
    string_to_action_element(&action_table[0][2], "x0");

    string_to_action_element(&action_table[1][0], "s4");
    string_to_action_element(&action_table[1][1], "x0");
    string_to_action_element(&action_table[1][2], "a0");

    string_to_action_element(&action_table[2][0], "r2");
    string_to_action_element(&action_table[2][1], "x0");
    string_to_action_element(&action_table[2][2], "r2");

    string_to_action_element(&action_table[3][0], "r3");
    string_to_action_element(&action_table[3][1], "x0");
    string_to_action_element(&action_table[3][2], "r3");

    string_to_action_element(&action_table[4][0], "x0");
    string_to_action_element(&action_table[4][1], "s3");
    string_to_action_element(&action_table[4][2], "x0");

    string_to_action_element(&action_table[5][0], "r1");
    string_to_action_element(&action_table[5][1], "x0");
    string_to_action_element(&action_table[5][2], "r1");

    goto_table[0][0] = -1;
    goto_table[0][1] = 1;
    goto_table[0][2] = 2;

    goto_table[1][0] = -1;
    goto_table[1][1] = -1;
    goto_table[1][2] = -1;

    goto_table[2][0] = -1;
    goto_table[2][1] = -1;
    goto_table[2][2] = -1;

    goto_table[3][0] = -1;
    goto_table[3][1] = -1;
    goto_table[3][2] = -1;

    goto_table[4][0] = -1;
    goto_table[4][1] = -1;
    goto_table[4][2] = 5;

    goto_table[5][0] = -1;
    goto_table[5][1] = -1;
    goto_table[5][2] = -1;

    productions[0].left = 'S';
    strcpy(productions[0].right, "E");

    productions[1].left = 'E';
    strcpy(productions[1].right, "E+T");

    productions[2].left = 'E';
    strcpy(productions[2].right, "T");

    productions[3].left = 'T';
    strcpy(productions[3].right, "i");
}

void input()
{
    int i, j;

    int_input("Enter the number of states:", &grammar_data.num_states);
    int_input("Enter the number of terminals:", &grammar_data.num_terminals);
    int_input("Enter the number of non-terminals:", &grammar_data.num_nonterminals);
    int_input("Enter the number of productions:", &grammar_data.num_productions);

    alloc_action_table();
    alloc_goto_table();
    t_list = malloc(sizeof(char) * grammar_data.num_terminals);
    n_list = malloc(sizeof(char) * grammar_data.num_nonterminals);
    productions = malloc(sizeof(production) * grammar_data.num_productions);

    printf("Enter the terminals:\n");
    for (i = 0; i < grammar_data.num_terminals; ++i)
    {
        scanf(" %c", &t_list[i]);
    }

    printf("Enter the non-terminals:\n");
    for (i = 0; i < grammar_data.num_nonterminals; ++i)
    {
        scanf(" %c", &n_list[i]);
    }

    // Take action table as input
    for (i = 0; i < grammar_data.num_states; ++i)
    {
        for (j = 0; j < grammar_data.num_terminals; ++j)
        {
            printf("Enter ACTION(%d,%c):", i, t_list[j]);
            action_input(i, t_list[j]);
        }
        printf("\n");
    }
    printf("\n\n");

    // Take goto table as input
    for (i = 0; i < grammar_data.num_states; ++i)
    {
        for (j = 0; j < grammar_data.num_nonterminals; ++j)
        {
            printf("Enter GOTO(%d,%c):", i, n_list[j]);
            goto_input(i, n_list[j]);
        }
        printf("\n");
    }
    printf("\n\n");

    printf("Enter the productions:\n");
    for (i = 0; i < grammar_data.num_productions; ++i)
    {
        char p[20];
        scanf("%s", p);
        productions[i].left = p[0];
        strcpy(productions[i].right, p + 3);
    }
    printf("\n\n");
}

void SET_PROGRAM_MODE(bool mode)
{
    mode ? test() : input();
}

int main()
{
    // Pass TAKE_USER_INPUT to use custom grammar and table
    SET_PROGRAM_MODE(USE_PREDEFINED_GRAMMAR);

    bool repeat = true;
    do
    {
        char str[100];
        get_input_string(str);
        parse(str);
        ask_for_repeat(&repeat);
    } while (repeat);
    return 0;
}