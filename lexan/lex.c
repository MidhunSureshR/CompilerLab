#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

/* List of Keywords */
const char *keyword[] = {
                        "auto", "break", "case", "char",
                        "const", "continue", "default",
                        "do", "double", "else", "enum",
                        "extern", "float", "for", "goto",
                        "if", "int", "long", "register",
                        "return", "short", "signed", "sizeof",
                        "static", "struct", "switch", "typedef",
                        "union", "unsigned", "void", "volatile",
                        "while"
                        };
const int keyword_size = 32;

/* List of Operators */
const char *operator[] = {
                        "+", "-", "*", "/", "%", "++", "--", "==",
                        "!=", ">", "<", "<=", ">=", "&&", "||", "!",
                        "&", "|", "^", "~", "<<", ">>", "=", "+=",
                        "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", 
                        "^=", "|=" , "{", "}", "(", ")", " ", ";",
                        "{", "}"
                        };
const int operator_size = 41;


static inline bool compareString(const char* string1, const char* string2){
    return strcmp(string1, string2) == 0 ? true : false;
}

/* String Detection */
bool isKeyword(const char* word){
    for(int i=0; i<keyword_size; ++i){
        if(compareString(keyword[i], word)) return true;
    }
    return false;
}

bool isOperator(const char* word){
    for(int i=0; i<operator_size; ++i){
        if(compareString(operator[i], word)) return true;
    }
    return false;
}

bool isConstant(const char* word){
    const int len = strlen(word);
    for(int i=0; i<len; ++i){
        if(!isdigit(word[i])){
            return false;
        }
    }
    return true;
}

bool isString(const char* word){
    const int len = strlen(word);
    return (word[0] == '"' && word[len-1] == '"') ||
           (word[0] == '\'' && word[len-1] == '\'')
           ? true : false;
}

bool isIdentifier(const char* word){
    return (isalpha(word[0]) || word[0] == '_') ? true : false;
}

void handleToken(const char* token){
    if(isKeyword(token))
        printf("%s - %s\n",token, "Keyword");
    else if(isConstant(token))
        printf("%s - %s\n",token, "Constant");
    else if(isString(token))
        printf("%s - %s\n",token, "String");
    else if(isIdentifier(token))
        printf("%s - %s\n",token, "Identifier");
    else
        printf("%s - %s\n",token, "Unidentified");
}

char* charToString(char character){
    char *str = malloc(sizeof(char) * 2);
    str[0] = character;
    str[1] = '\0';
    return str;
}

char* getToken(char *start, char *end){
    const int len = end - start;
    char* token = malloc(sizeof(char)*len);
    strncpy(token, start, len);
    return token;
}


void tokenize(char* string){
    const int len = strlen(string);
    char *lexemeBegin=string, *forward = string;
    while(*forward != '\0'){
        if(isOperator(charToString(forward[0]))){
            if(lexemeBegin != forward){
                const char* token = getToken(lexemeBegin, forward);
                handleToken(token);
            }
            printf("\'%c\' - %s\n",forward[0], "Operator");
            lexemeBegin = ++forward;
        }
        else ++forward;
    }
}

int main(int argc, char **argv){
    char input[100];   
    printf("Enter the C code:\n");
    fgets(input, 100, stdin); 
    tokenize(input);
    return 0;
}
