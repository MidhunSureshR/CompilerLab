#include <stdio.h>
#include <string.h>

/*
Grammar 
E -> E+T |T
T -> T*F | F
F -> (E) |id

Accepting input:
(i+i)$
Rejecting input:
(i+)$
*/

void main()
{
    int i, j, a, k, count = 0;
    int top, col, row, m = 0, n;
    char stack[20], ip[20], T[6] = {"+*()i$"}, push[10];
    char arr[6][6] = {
        "><<><>",
        ">><><>",
        "<<<=<-",
        ">>->->",
        ">>->->",
        "<<<-<-",
    };
    printf("\nOperator Precedence Parser Table\n");
    printf("\n----------------------------------\n");
    printf("\t+\t*\t(\t)\ti\t$\n");
    for (i = 0; i < 6; i++)
    {
        printf("%c\t", T[i]);
        for (j = 0; j < 6; j++)
        {
            printf("%c\t", arr[i][j]);
        }
        printf("\n");
    }
    scanf("%s", ip);
    stack[top] = '$';
    printf("\nSTACK\t\t\tINPUT\t\t\tACTION\n");
    printf("\n%s\t\t\t%s\t\t\t", stack, ip);
    i = 0;
    while (i <= strlen(ip))
    {
        for (n = 0; n < 6; n++)
        {
            if (stack[top] == T[n])
                col = n;
            if (ip[i] == T[n])
                row = n;
        }
        if ((stack[top] == '$') && (ip[i] == '$'))
        {
            printf("\nThe string is a accepted.\n");
            break;
        }
        if ((arr[col][row] == '<') || (arr[col][row] == '='))
        {
            stack[++top] = arr[col][row];
            stack[++top] = ip[i];
            printf("Push %c", ip[i]);
            i++;
        }
        else
        {

            if (arr[col][row] == '>')
            {
                if (stack[top] == 'i')
                {
                    printf("Pop %c", stack[top]);
                    push[m] = stack[top];
                    m++;
                    while (stack[top] != '<')
                    {
                        --top;
                    }
                    --top;
                }
                else if (stack[top] != 'i')
                {
                    for (a = 0; a < m; a++)
                    {
                        if (push[a] == 'i')
                        {
                            count++;
                        }
                    }
                    if (count > 1)
                    {
                        printf("Pop %c", stack[top]);
                        while (stack[top] != '<')
                        {
                            --top;
                        }
                        --top;
                        ;
                    }
                    else
                    {
                        printf("\nThe string is rejected.\n");
                        break;
                    }
                }
            }
            else
            {
                printf("\nThe string is rejected.\n");
                break;
            }
        }
        printf("\n");
        for (k = 0; k <= top; k++)
        {
            printf("%c", stack[k]);
        }
        printf("\t\t\t\t");
        for (k = i; k < strlen(ip); k++)
        {
            printf("%c", ip[k]);
        }
        printf("\t\t\t\t");
    }
}