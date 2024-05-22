#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define INPUT_SIZE 100 //Max function string size
#define VAR_SIZE 20 // Max x String size


typedef enum {
    TOKEN_NUM, //e ,pi gibi sabitler bu kategoride
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EXPONENT, //USTEL

    TOKEN_SIN,
    TOKEN_COS,
    TOKEN_LOG,
    TOKEN_TAN,
    TOKEN_COT,
    TOKEN_ASIN,
    TOKEN_ACOS,
    TOKEN_ATAN,
    TOKEN_ACOT,



    TOKEN_VARIABLE,
    TOKEN_EOF,  // End of the expression
    TOKEN_INVALID
} TokenType;

typedef struct TokenST {
    TokenType type;
    double value; // Only used for TOKEN_NUM
} Token;

static double pi(void) {return 3.14159265358979323846;}
static double e(void) {return 2.71828182845904523536;}


double parseExpression(char **input,Token* cr,float* varX);
double parseTerm(char **input,Token* cr,float* varX);
double parseFactor(char **input,Token* cr,float* varX);



Token getNextToken(char **input,float*varX) {


    while (isspace(**input)) (*input)++; // Skip whitespace
    Token token;
    token.type = TOKEN_INVALID;
    token.value = 0;

    if (isdigit(**input) || **input == '.') { // Accept '.' as part of floating-point numbers
        token.type = TOKEN_NUM;
        token.value = strtod(*input, (char **)input);
    }else if(**input == 'x'){
        token.type = TOKEN_VARIABLE;
        token.value = *varX;
        (*input)++;
    }

    //Islem operatorleri
    else{
        switch (**input) {
            
            case '+': token.type = TOKEN_PLUS;break;
            case '-': token.type = TOKEN_MINUS;break;
            case '*': token.type = TOKEN_MUL;break;
            case '/': token.type = TOKEN_DIV;break;
            case '^': token.type = TOKEN_EXPONENT;break;
            case '(': token.type = TOKEN_LPAREN;break;
            case ')': token.type = TOKEN_RPAREN;break;
            case '\0': token.type = TOKEN_EOF;break;
            default: token.type = TOKEN_INVALID;break;
        }


    //Hangi Fonksiyon cagir ona bak
    
        //Trigo
        if (token.type == TOKEN_INVALID){ // Optimizasyon icin eger üsttekiler degilse bunlar, kontrole basla

            if(strncmp((*input),"sin(",4)==0) { 
                token.type = TOKEN_SIN;
                (*input)+=4;
            }else if(strncmp((*input),"cos(",4)==0) { 
                token.type = TOKEN_COS;
                (*input)+=4;
            }else if(strncmp((*input),"tan(",4)==0) { 
                token.type = TOKEN_TAN;
                (*input)+=4;
            }else if(strncmp((*input),"cot(",4)==0) { 
                token.type = TOKEN_COT;
                (*input)+=4;
            }
            
            //TERS TRİGO
            else if(strncmp((*input),"asin(",5)==0) { 
                token.type = TOKEN_ASIN;
                (*input)+=5;
            }else if(strncmp((*input),"acos(",5)==0) { 
                token.type = TOKEN_ACOS;
                (*input)+=5;
            }else if(strncmp((*input),"atan(",5)==0) { 
                token.type = TOKEN_ATAN;
                (*input)+=5;
            }else if(strncmp((*input),"acot(",5)==0) { 
                token.type = TOKEN_ACOT;
                (*input)+=5;
            }
            
            
            //LOG
            else if(strncmp((*input),"log_",4)==0) { 
                token.type = TOKEN_LOG;
                (*input)+=4;
            }
            
            
            //sabit sayılar
            else if(strncmp((*input),"e",1)==0) { 
                token.type = TOKEN_NUM;
                token.value = e();
                (*input)+=1;
            }else if(strncmp((*input),"pi",2)==0) { 
                token.type = TOKEN_NUM;
                token.value = pi();
                (*input)+=2;
            }
        
        }
        else if (token.type != TOKEN_EOF) (*input)++;
    }

    return token;
}



void eat(TokenType type,Token* currentToken,char **input,float* varX) {
    if ((*currentToken).type == type) {
        //printf("Onceki  %d \n",(*currentToken).type);
        (*currentToken) = getNextToken(input,varX);
        //printf("Sonraki %d \n",(*currentToken).type);
    } else {
        perror("Error: Unexpected token\n");
        printf("%d\n",currentToken->type);
        printf("char : %c\n",**input);
        exit(EXIT_FAILURE);
    }
}

double parseFactor(char **input,Token* currentToken,float* varX) {
    double result = 0;
    double tempLogBase;

    if ((*currentToken).type == TOKEN_NUM ) {
        result = (*currentToken).value;
        eat(TOKEN_NUM,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_VARIABLE ) {
        result = (*currentToken).value;
        eat(TOKEN_VARIABLE,currentToken,input,varX);
    } else if ((*currentToken).type == TOKEN_LPAREN) {
        eat(TOKEN_LPAREN,currentToken,input,varX);
        result = parseExpression(input,currentToken,varX);
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }


    //Fonksiyonlar
    

    //TRİGO
    else if ((*currentToken).type == TOKEN_SIN) {
        eat(TOKEN_SIN,currentToken,input,varX);
        result = sin(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_COS) {
        eat(TOKEN_COS,currentToken,input,varX);
        result = cos(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_TAN) {
        eat(TOKEN_TAN,currentToken,input,varX);
        result = tan(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_COT) {
        eat(TOKEN_COT,currentToken,input,varX);
        result = 1/tan(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }


    //Ters trigo
    else if ((*currentToken).type == TOKEN_ASIN) {
        eat(TOKEN_ASIN,currentToken,input,varX);
        result = asin(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_ACOS) {
        eat(TOKEN_ACOS,currentToken,input,varX);
        result = acos(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_ATAN) {
        eat(TOKEN_ATAN,currentToken,input,varX);
        result = atan(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_ACOT) {
        eat(TOKEN_ACOT,currentToken,input,varX);
        result = 1/atan(parseExpression(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }

    //Log
    else if ((*currentToken).type == TOKEN_LOG) {
        eat(TOKEN_LOG,currentToken,input,varX);
        tempLogBase= currentToken->value;
        eat(TOKEN_NUM,currentToken,input,varX);
        eat(TOKEN_LPAREN,currentToken,input,varX);
        result = log10(parseExpression(input,currentToken,varX))/log10(tempLogBase);
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }


    //üstelde sadecd 1. adıma dön digerlerinde 3. adıma dön
    while ((*currentToken).type == TOKEN_EXPONENT )
    {
        eat(TOKEN_EXPONENT,currentToken,input,varX);
        result = pow(result,parseFactor(input,currentToken,varX));
    }
    
    return result;
}


double parseTerm(char **input,Token* currentToken,float* varX) {
    double result = parseFactor(input,currentToken,varX);


    while ((*currentToken).type == TOKEN_MUL || (*currentToken).type == TOKEN_DIV) {

        if ((*currentToken).type == TOKEN_MUL) {
            eat(TOKEN_MUL,currentToken,input,varX);
            result *= parseFactor(input,currentToken,varX);
        }else if ((*currentToken).type == TOKEN_DIV) {
            eat(TOKEN_DIV,currentToken,input,varX);
            double divisor = parseFactor(input,currentToken, varX);
            if (divisor != 0.0) {
                result /= divisor;
            } else {
                perror("Error: Division by zero\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    return result;
}


double parseExpression(char **input,Token* currentToken,float* varX) {
    double result = parseTerm(input,currentToken,varX);
    
    while ((*currentToken).type == TOKEN_PLUS || (*currentToken).type == TOKEN_MINUS) {
        if ((*currentToken).type == TOKEN_PLUS) {
            eat(TOKEN_PLUS,currentToken,input,varX);
            result += parseTerm(input,currentToken,varX);
        } else if ((*currentToken).type == TOKEN_MINUS) {
            eat(TOKEN_MINUS,currentToken,input,varX);
            result -= parseTerm(input,currentToken,varX);
        }
    }
    return result;
}


double funcEvaluate(char *input,float varX) {
    Token currentToken;
    currentToken = getNextToken(&input,&varX);
    return parseExpression(&input,&currentToken,&varX); 

}

int main() {

    // char input[30];
    char func[100];
    float varx;

    printf("Enter Function:");  // sin(pi*x) , log_(e)(e^(2*x)) first paranthesis is base second one is the inside of logarithm 
                                // 4^(x*3) - 2*e + asin(1) 
    
    gets(func);

    printf("Enter Value of x:");
    scanf("%f",&varx);
    
    char *input = func;
    double result = funcEvaluate(input,varx);
    
    printf("Result: %lf \n",result);
    return 0;
}
