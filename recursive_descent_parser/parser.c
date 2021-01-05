/* R Midhun Suresh
 * <rmidhunsuresh@gmail.com>
 *
 * Recursive Descent Parser for the following grammar:
 * E := E+T | T
 * T := a | a[] | a[X]
 * X := E,E | E
 * -------
 * Accepting inputs:
 * a+a
 * a+a+a+...
 * a[a]
 * a[a,a]
 * a+a[a]
 * a+a[a,a]
 * Non-accepting inputs:
 * a+
 * a[,a]
 * a[a,]
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define match(terminal) _match(terminal, false)
#define match_optional(terminal) _match(terminal, true)

char *lookahead = NULL;
size_t input_length;

bool _match(const char terminal, bool suppressError)
{
    if (*lookahead == terminal)
    {
        lookahead++;
        return true;
    }
    else if (!suppressError)
    {
        printf("Failed at character %c at position %d\n", *lookahead, (int)(input_length - strlen(lookahead)));
        fputs("Error: Cannot Parse Input String\n", stdout);
        exit(EXIT_FAILURE);
    }
    return false;
}

// Terminal Functions ahoy
void X();
void E();
void T2();
void T1();
void T();
void E1();

void T2()
{
    if (match_optional(']'))
        return;
    else
    {
        X();
        match(']');
    }
}

void T1()
{
    if (match_optional('['))
    {
        T2();
    }
}

void T()
{
    match('a');
    T1();
}

void E1()
{
    if (match_optional('+'))
    {
        T();
        E1();
    }
}

void E()
{
    T();
    E1();
}

void X()
{
    E();
    if (match_optional(','))
        E();
}

int main()
{
    char input[100];
    printf("Enter the input string:");
    scanf("%s", input);
    lookahead = input;
    input_length = strlen(input);
    E();
    if (*lookahead == '\0')
    {
        printf("Parsing is successful.\n");
    }
    else
    {
        printf("Parsing is unsuccessful.\n");
    }
    return 0;
}
