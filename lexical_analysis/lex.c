/*
 * Lexical Analyzer for the C programming language
 * Author R Midhun Suresh <rmidhunsuresh@gmail.com>
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

static inline bool compareString(const char*, const char*);

typedef struct node{
        char symbol[30];
        char type[10];
        int id;
        struct node* next;
} tabNode;

tabNode symTabHead;

char lastToken[20];

static inline void initSymtab(){
        symTabHead.next = NULL;
}

void addSymtabEntry(const char* symbol, const char* type, int id){
   tabNode* ptr = symTabHead.next;
   tabNode* prev_ptr = &symTabHead;
   while(ptr != NULL){ 
       if(compareString(symbol, ptr->symbol)) return;
       prev_ptr = ptr;
       ptr = ptr-> next;
   }

   // Create a new Symbol Table Node
   tabNode* newEntry = malloc(sizeof(tabNode));
   strcpy(newEntry->symbol, symbol); 
   strcpy(newEntry->type, type); 
   newEntry->id = id;

   // Add to end of list
   prev_ptr->next = newEntry;
}

void prettyPrintSymtab(){
    printf("--------------------------------------------\n");
    printf("|%10s\t|%10s\t|%10s|\n", "Symbol", "Type", "Attribute");
    printf("--------------------------------------------\n");
    tabNode* ptr = symTabHead.next;
    while(ptr != NULL){
            char id[10];
            sprintf(id, "id%d", ptr->id);
            printf("|%10s\t|%10s\t|%10s|\n", ptr->symbol, ptr->type, id);
            ptr = ptr->next;
    }
    printf("--------------------------------------------\n");
}

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

/* Token Detection Helper Functions */
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
    else if(isIdentifier(token)){
        printf("%s - %s\n",token, "Identifier");
        static int id = 0;
        addSymtabEntry(token, lastToken, id++);
    }
    else
        printf("%s - %s\n",token, "Unidentified");
}

/* Generic Helper Functions */
char* charToString(char character){
    char *str = malloc(sizeof(char) * 2);
    str[0] = character;
    str[1] = '\0';
    return str;
}

void copyString(char *destination, char *source, const int len){
        strncpy(destination, source, len);
        // Null terminate string
        *(destination + len) = '\0';
}

/* Tokenization Main */
char* getToken(char *start, char *end){
    const int len = end - start;
    char* token = malloc(sizeof(char)*len);
    strncpy(token, start, len);
    return token;
}

/*
 * forward: char* to start of operator
 * returns: length of operator
 */
int getOperatorSize(char *forward){
    char _twoOp[3], _threeOp[4];
    int skipAhead = 1;

    copyString(_twoOp, forward, 2);
    copyString(_threeOp, forward, 3);

    if(isOperator(_threeOp)){
           skipAhead = 3; 
    }
    else if(isOperator(_twoOp)){
           skipAhead = 2; 
    }
    return skipAhead;
}

void tokenize(char* string){
    char *lexemeBegin=string, *forward = string;
    while(*forward != '\0'){
        if(isOperator(charToString(forward[0]))){
            int skipAhead = getOperatorSize(forward);
            if(lexemeBegin != forward){
                char* token = getToken(lexemeBegin, forward);
                handleToken(token);
                copyString(lastToken, token, (int)(forward-lexemeBegin)); 
            }
            char *operator;
            if(skipAhead == 1) {
                operator = charToString(forward[0]);
            }
            else strncpy(operator, forward, skipAhead);
            if(operator[0] != ' ') printf("\'%s\' - %s\n", operator, "Operator");
            forward += skipAhead;
            lexemeBegin = forward;
        }
        else ++forward;
    }
}

int main(int argc, char **argv){
    char input[100];   
    printf("Enter the C code:\n");
    fgets(input, 100, stdin); 
    initSymtab();
    printf("\nTokens found are:\n");
    tokenize(input);
    printf("\n\tSymbol Table Generated is:\n");
    prettyPrintSymtab();
    return 0;
}
