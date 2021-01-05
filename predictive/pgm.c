/*
R Midhun Suresh
<rmidhunsuresh@gmail.com>/<github.com/MidhunSureshR>

A general purpose predictive parser

See the modular version @ https://github.com/MidhunSureshR/PredictiveParser/
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

/* .h file copy*/
struct s_stack;

typedef struct s_stack *stack;

stack create_stack();

void push(stack s, char element);

char pop(stack s);

char stack_top(stack s);

void dealloc_stack(stack s);

struct set_s
{
    ssize_t length; // Length of set
    char *elements; // Array of elements in set
    int index;      // last index
};

typedef struct set_s *set;

set create_set(ssize_t);

void set_add(set, char);

set get_set_from_array(char *, ssize_t);

void set_union(set, set);

void set_remove(set, char);

int set_contains(set, char);

bool is_set_singleton(set s);

void print_set(set);

int get_set_size(set s);

void set_dealloc(set s);

struct s_stack
{
    char elements[100];
    int top;
};

typedef struct
{
    char *production;
} production_body;

typedef struct
{
    ssize_t size;
    production_body *body;
} production_body_list;

typedef struct
{
    char left;
    production_body_list *right;
} production;

void print_production(production *);

void get_production_as_string(char *, production *);

production *parse_production_from_string(const char *);

production *get_production_of_nonterminal(char non_terminal, production **grammar, ssize_t num_productions);

bool is_terminal(char c);

bool is_nonterminal(char c);

production **get_production_from_string_collection(char **grammar, int num_productions);

void copy_string(char *, const char *);

int num_occurrence(const char *, char);

// Representation of each element in our parsing table
typedef struct
{
    char non_terminal;
    char input_symbol;
    production *p; // points to NULL if this entry is empty
    int sub_production_index;
} table_entry;

typedef table_entry *table;

table create_parsing_table(production **grammar, ssize_t num_productions);

table_entry *table_get_entry(table t, char non_terminal, char input_symbol);

void print_table(table t);

set FIRST(char, production **, ssize_t);

set FIRST_string(char *string, production **grammar, ssize_t num_productions);

set FOLLOW(char symbol, production **grammar, ssize_t num_productions);

void predictive_parse(const char *string, table parsing_table, production **grammar);

/*----------------------*/

stack create_stack()
{
    stack s = malloc(sizeof(struct s_stack));
    s->top = -1;
    return s;
}

void push(stack s, const char element)
{
    if (element == '.')
        return; // Do Not Push Epsilon
    s->top++;
    s->elements[s->top] = element;
}

char pop(stack s)
{
    const char element = s->elements[s->top];
    s->top--;
    return element;
}

char stack_top(stack s)
{
    return s->elements[s->top];
}

void dealloc_stack(stack s)
{
    free(s);
}

void error(stack s)
{
    printf("Error in parsing\n");
    dealloc_stack(s);
}

void predictive_parse(const char *string, table parsing_table, production **grammar)
{
    const int buffer_length = strlen(string) + 1;
    char *input_buffer = malloc(sizeof(char) * buffer_length);
    strncpy(input_buffer, string, strlen(string));
    char *current_symbol = input_buffer;
    // Add end marker ($) to end of input buffer
    input_buffer[buffer_length - 1] = '$';

    // Create initial config of array
    stack s = create_stack();
    push(s, '$');
    const char starting_symbol = grammar[0]->left;
    push(s, starting_symbol);
    printf("Attempting follow productions:\n");
    while (stack_top(s) != '$')
    {

        const char s_top = stack_top(s);
        if (s_top == *current_symbol)
        {
            pop(s);
            current_symbol++;
        }
        else if (is_terminal(s_top))
        {
            error(s);
            return;
        }
        else if (table_get_entry(parsing_table, s_top, *current_symbol) == NULL)
        {
            error(s);
            return;
        }
        else
        {
            table_entry *t = table_get_entry(parsing_table, s_top, *current_symbol);
            production_body sub_production = t->p->right->body[t->sub_production_index];
            const int strlen_sub_production = strlen(sub_production.production);
            pop(s);
            printf("\t%c->%s\n", t->p->left, sub_production.production);
            for (int i = strlen_sub_production - 1; i >= 0; --i)
            {
                push(s, sub_production.production[i]);
            }
        }
    }
    printf("Parsing is successful!!\n");
}

void copy_string(char *destination, const char *source)
{
    strncpy(destination, source, strlen(source));
    destination[strlen(source)] = '\0';
}

int num_occurrence(const char *string, char symbol)
{
    int num = 0;
    for (int i = 0; i < strlen(string); ++i)
    {
        if (string[i] == symbol)
            ++num;
    }
    return num;
}

void print_production(production *p)
{
    printf("%c->", p->left);
    int i;
    for (i = 0; i < p->right->size - 1; ++i)
    {
        printf("%s|", p->right->body[i].production);
    }
    printf("%s\n", p->right->body[i].production);
}

void get_production_as_string(char *string, production *p)
{
    sprintf(string, "%c->", p->left);
    int i;
    for (i = 0; i < p->right->size - 1; ++i)
    {
        sprintf(strchr(string, '\0'), "%s|", p->right->body[i].production);
    }
    sprintf(strchr(string, '\0'), "%s", p->right->body[i].production);
}

production_body_list *parse_production_body_from_string(const char *start_of_production_body)
{
    char backup[100];
    copy_string(backup, start_of_production_body);

    production_body_list *p = malloc((sizeof(production_body_list)));
    p->size = num_occurrence(start_of_production_body, '|') + 1;
    p->body = malloc(sizeof(production_body) * p->size);

    for (int i = 0; i < p->size; ++i)
    {
        p->body[i].production = malloc(sizeof(char) * 10);
    }

    char *token;

    token = strtok(backup, "|");
    copy_string(p->body[0].production, token);
    for (int i = 1; token && i < p->size; ++i)
    {
        token = strtok(NULL, "|");
        copy_string(p->body[i].production, token);
    }
    return p;
}

production *parse_production_from_string(const char *string)
{
    // Assume our production has the string form A->bB|cC|d;
    production *p = malloc(sizeof(production));
    // Lets first find our left side non-terminal ( A in our example)
    p->left = string[0];
    production_body_list *b = parse_production_body_from_string(string + 3);
    p->right = b;
    return p;
}

bool is_terminal(char c)
{
    return !isupper(c);
}

bool is_nonterminal(char c)
{
    return isupper(c);
}

production *get_production_of_nonterminal(char non_terminal, production **grammar, ssize_t num_productions)
{
    for (ssize_t i = 0; i < num_productions; ++i)
    {
        if (grammar[i]->left == non_terminal)
            return grammar[i];
    }
    return NULL;
}

production **get_production_from_string_collection(char **grammar, int num_productions)
{
    // Create array of pointers to production
    production **p = malloc(sizeof(production) * num_productions);
    // Parse each production
    for (int i = 0; i < num_productions; ++i)
    {
        p[i] = parse_production_from_string(grammar[i]);
    }
    return p;
}

// Find union of two sets and store in destination
void set_union(set destination, set source)
{
    for (ssize_t i = 0; i < source->index; ++i)
    {
        set_add(destination, source->elements[i]);
    }
}

// Create a set with length number of elements
set create_set(ssize_t length)
{
    set s = malloc(sizeof(struct set_s));
    s->elements = malloc(sizeof(char) * length);
    s->length = length;
    s->index = 0;
    return s;
}

// Add element to set
void set_add(set s, char element)
{
    if (set_contains(s, element) != -1)
        return;
    s->elements[s->index++] = element;
}

// Get a set from array
set get_set_from_array(char *array, ssize_t len)
{
    set s = malloc(sizeof(set));
    s->length = len;
    s->elements = malloc(sizeof(char) * len);
    for (int i = 0; i < s->length; ++i)
        s->elements[i] = array[i];
    return s;
}

int set_contains(set s, char element)
{
    for (ssize_t i = 0; i < s->index; ++i)
    {
        if (s->elements[i] == element)
            return i;
    }
    return -1;
}

bool is_set_singleton(set s)
{
    return s->index == 1 ? true : false;
}

void set_remove(set s, char element)
{
    int pos = set_contains(s, element);
    if (pos == -1)
        return;
    for (ssize_t i = pos; i < s->index - 1; ++i)
    {
        s->elements[i] = s->elements[i + 1];
    }
    s->length = s->length - 1;
    s->index = s->index - 1;
}

void print_set(set s)
{
    printf("{");
    for (ssize_t i = 0; i < s->index - 1; ++i)
    {
        printf("%c, ", s->elements[i]);
    }
    printf("%c}\n", s->elements[s->index - 1]);
}

int get_set_size(set s)
{
    return s->index;
}

void set_dealloc(set s)
{
    free(s->elements);
    free(s);
}

bool is_epsilon_present(production *p)
{
    ssize_t len = p->right->size;
    for (ssize_t i = 0; i < len; ++i)
    {
        if (strcmp(p->right->body[i].production, ".") == 0)
            return true;
    }
    return false;
}

set FIRST(char symbol, production **grammar, ssize_t num_productions)
{
    set first = create_set(10);
    if (is_terminal(symbol))
    {
        set_add(first, symbol);
        return first;
    }
    else
    {
        // Rule 2
        // Find production of symbol
        production *p = get_production_of_nonterminal(symbol, grammar, num_productions);
        // Is symbol->. present ? then add . to set
        if (is_epsilon_present(p))
        {
            set_add(first, '.');
        }

        // Rule 3
        for (ssize_t i = 0; i < p->right->size; ++i)
        {
            bool add_epsilon = true;
            const production_body sub_production = p->right->body[i];
            for (int j = 0; j < strlen(sub_production.production); ++j)
            {
                set _first = FIRST(sub_production.production[j], grammar, num_productions);
                if (set_contains(_first, '.') == -1)
                {
                    add_epsilon = false;
                    break;
                }
            }
            if (add_epsilon)
                set_add(first, '.');
        }

        // For each  X->Y1 Y2 Y3… Yn
        for (ssize_t i = 0; i < p->right->size; ++i)
        {
            const production_body sub_production = p->right->body[i];
            const char first_char = sub_production.production[0];
            set _first = FIRST(first_char, grammar, num_productions);
            if (set_contains(_first, '.') != -1 && strlen(sub_production.production) >= 2)
            {
                set_remove(_first, '.'); // Remove epsilon
                set_union(_first, FIRST(sub_production.production[1], grammar, num_productions));
                set_union(first, _first);
            }
            else
            {
                // FIRST(X) = FIRST(Y1)
                set_union(first, _first);
            }
        }
    }
    return first;
}

set FIRST_string(char *string, production **grammar, ssize_t num_productions)
{
    const int len = strlen(string);
    set first = create_set(20);
    bool add_epsilon = true;
    for (int i = 0; i < len; ++i)
    {
        const char symbol = string[i];
        set _first = FIRST(symbol, grammar, num_productions);
        bool cond = set_contains(_first, '.') == -1;
        set_remove(_first, '.');
        set_union(first, _first);
        if (cond)
        {
            // first does not contain epsilon
            add_epsilon = false;
            break;
        }
    }
    if (add_epsilon)
        set_add(first, '.');
    return first;
}

set FOLLOW(char symbol, production **grammar, ssize_t num_productions)
{
    // FOLLOW is defined only for non-terminals
    /*
    FOLLOW(S) = { $ } where S is the starting Non-Terminal
    */
    set follow = create_set(10);
    char S = grammar[0]->left;
    if (symbol == S)
    {
        set_add(follow, '$');
    }
    for (ssize_t i = 0; i < num_productions; ++i)
    {
        production *p = grammar[i];
        for (ssize_t j = 0; j < p->right->size; ++j)
        {
            production_body sub_production = p->right->body[j];
            const int strlen_production = strlen(sub_production.production);
            for (int k = 0; k < strlen_production; ++k)
            {
                char check_char = sub_production.production[k];
                if (sub_production.production[k] == symbol)
                {
                    if (k == strlen_production - 1)
                    {
                        // symbol is last character in the production
                        if (p->left != symbol)
                        { // Eliminate nasty infinite recursion
                            set _follow = FOLLOW(p->left, grammar, num_productions);
                            set_union(follow, _follow);
                        }
                    }
                    else
                    {
                        // there are characters to the right of symbol
                        bool set_contains_epsilon = true;
                        int start = k + 1;
                        while (set_contains_epsilon && start < strlen_production)
                        {
                            set _first = FIRST(sub_production.production[start], grammar, num_productions);
                            set_contains_epsilon = set_contains(_first, '.') != -1;
                            set_remove(_first, '.');
                            set_union(follow, _first);
                            if (set_contains_epsilon /*&& k+1 == strlen_production-1*/)
                            {
                                // FIRST(next_char) contains epsilon
                                set _follow = FOLLOW(p->left, grammar, num_productions);
                                set_union(follow, _follow);
                                start++;
                            }
                        }
                    }
                }
            }
        }
    }
    return follow;
}

int table_size = 0;

void print_table(table t)
{
    int c = 0;
    for (int i = 0; i < table_size; ++i)
    {
        if (t[i].p != NULL)
        {
            c++;
            printf("TABLE(%c,%c) =", t[i].non_terminal, t[i].input_symbol);
            const int sub_index = t[i].sub_production_index;
            printf("%c->%s\n", t[i].p->left, t[i].p->right->body[sub_index].production);
        }
    }
    printf("Total table size = %d\n", c);
}

void empty(table t)
{
    for (int i = 0; i < table_size; ++i)
    {
        t[i].p = NULL;
    }
}

table_entry *table_get_entry(table t, char non_terminal, char input_symbol)
{
    for (int i = 0; i < table_size; ++i)
    {
        if (t[i].input_symbol == input_symbol && t[i].non_terminal == non_terminal)
        {
            return &t[i];
        }
    }
    return NULL;
}

void table_add(table t, char non_terminal, char input_symbol, production *p, int sub_production_index)
{
    if (input_symbol == '.')
        return; // Do Not Add Epsilon to table
    static int i = 0;
    t[i].non_terminal = non_terminal;
    t[i].input_symbol = input_symbol;
    t[i].p = p;
    t[i].sub_production_index = sub_production_index;
    i = i + 1;
}

set get_terminals(production **grammar, ssize_t num_productions)
{
    set terminals = create_set(10);
    for (ssize_t i = 0; i < num_productions; ++i)
    {
        production *p = grammar[i];
        for (ssize_t j = 0; j < p->right->size; ++j)
        {
            char *sub_production = p->right->body[j].production;
            for (int k = 0; k < strlen(sub_production); ++k)
            {
                if (is_terminal(sub_production[k]))
                    set_add(terminals, sub_production[k]);
            }
        }
    }
    return terminals;
}

int get_number_of_terminals(production **grammar, ssize_t num_productions)
{
    set terminals = get_terminals(grammar, num_productions);
    const int num_terminals = get_set_size(terminals);
    set_dealloc(terminals);
    return num_terminals;
}

table create_parsing_table(production **grammar, ssize_t num_productions)
{
    const int num_non_terminals = num_productions;
    const int num_terminals = get_number_of_terminals(grammar, num_productions);
    const int pt_size = num_non_terminals * num_terminals;
    table_size = pt_size;
    table entries = malloc(sizeof(table_entry) * pt_size);
    empty(entries);
    for (ssize_t i = 0; i < num_productions; ++i)
    {
        production *p = grammar[i];
        const int num_sub_productions = p->right->size;
        for (ssize_t k = 0; k < num_sub_productions; ++k)
        {
            production_body sub_production = p->right->body[k];
            // Currently we are examining p.left -> sub_production
            set first = FIRST_string(sub_production.production, grammar, num_productions);

            // Rule 1
            for (ssize_t j = 0; j < first->index; ++j)
            {
                const char element = first->elements[j];
                if (is_terminal(element))
                {
                    table_add(entries, p->left, element, p, k);
                }
            }

            // Rule 2
            if (set_contains(first, '.') != -1)
            {
                // first(alpha) contains epsilon then ...
                set follow = FOLLOW(p->left, grammar, num_productions);
                for (ssize_t j = 0; j < follow->index; ++j)
                {
                    const char element = follow->elements[j];
                    if (is_terminal(element))
                    {
                        table_add(entries, p->left, element, p, k);
                    }
                }
            }
        }
    }
    return entries;
}

char **get_grammar_from_user(int num_productions)
{
    char **grammar = malloc(sizeof(int *) * num_productions);
    for (int i = 0; i < num_productions; ++i)
    {
        grammar[i] = malloc(sizeof(char) * 100);
        scanf("%s", grammar[i]);
    }
    return grammar;
}

int get_productions_count()
{
    int num_productions;
    scanf("%d", &num_productions);
    return num_productions;
}

void run_tests()
{
    char grammar_string[5][50] = {"E->TA", "A->+TA|.", "T->FB", "B->*FB|.", "F->(E)|i"};
    // char grammar_string[6][50] = {"S->aBDh", "B->cC", "C->bC|.", "D->EF", "E->g|.", "F->f|."};
    //char grammar_string[4][50] = {"S->ACB|Cbb|Ba", "A->da|BC", "B->g|.", "C->h|."};
    //    test_first(grammar_string, 5);
    //    printf("----------------------------------------\n");
    //    test_follow(grammar_string, 5);
    production **p = malloc(sizeof(production) * 5);
    for (int i = 0; i < 5; ++i)
    {
        p[i] = parse_production_from_string(grammar_string[i]);
    }
    table t = create_parsing_table(p, 5);
    predictive_parse("i+i*i", t, p);
    predictive_parse("(i)", t, p);
    predictive_parse("(i++)", t, p); // Fail case
    //print_table(t);
}

int main()
{
    printf("Predictive Parser\n");
    run_tests();

    /*  
    Uncomment the following for user input ...
    printf("Enter number of productions:");
    const int num_productions = get_productions_count();
    char **grammar = get_grammar_from_user(num_productions);
    production **p = get_production_from_string_collection(grammar, num_productions);
    table t = create_parsing_table(p, 5);
    while (true)
    {
        printf("Enter string:");
        char str[100];
        scanf("%s", str);
        predictive_parse(str, t, p);
        char c;
        scanf("%c", &c);
        if (c == 'n')
            break;
    }*/
    return 0;
}