#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define INPUT_SIZE 100
#define VAR_SIZE 20
#ifndef NAN
#define NAN (0.0/0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

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

typedef struct MT 
{
    double** data;
    int row;
    int column;
}Matris;

// Fonksiyonlar deger dondurmedigi icin int olmaları sikinti degil
int Bisection(),Trapez(),Gregory_Newton(); 
int Regula_Falsi(),Newton_Raphson(),Inverse_Matrix();
int Gauss_Elimination(),Gauss_Seidal(),Numeric_Deriv(),Simpson();
int PrintMatris(Matris p);
Matris createIdentityMatrix(int n);

//Hesap yapiyor bundan dolayi double
double centralDerivative(char* input,double x,double h);
double backwardDerivative(char* input,double x,double h);
double forwardDerivative(char* input,double x,double h);



static double pi(void) {return 3.14159265358979323846;}
static double e(void) {return 2.71828182845904523536;}
static double fac(double a) {/* simplest version of fac */
    if (a < 0.0)
        return NAN;
    if (a > UINT_MAX)
        return INFINITY;
    unsigned int ua = (unsigned int)(a);
    unsigned long int result = 1, i;
    for (i = 1; i <= ua; i++) {
        if (i > ULONG_MAX / result)
            return INFINITY;
        result *= i;
    }
    return (double)result;
}




double parseDegree3(char **input,Token* cr,float* varX);
double parseDegree2(char **input,Token* cr,float* varX);
double parseDegree1(char **input,Token* cr,float* varX);
double funcEvaluate(char *input,float varX);



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
        printf("Error: Unexpected token:");
        printf("%d\n",currentToken->type);
        printf("Current char : %c\n",**input);
        exit(EXIT_FAILURE);
    }
}

double parseDegree1(char **input,Token* currentToken,float* varX) {
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
        result = parseDegree3(input,currentToken,varX);
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }


    //Fonksiyonlar
    

    //TRİGO
    else if ((*currentToken).type == TOKEN_SIN) {
        eat(TOKEN_SIN,currentToken,input,varX);
        result = sin(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_COS) {
        eat(TOKEN_COS,currentToken,input,varX);
        result = cos(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_TAN) {
        eat(TOKEN_TAN,currentToken,input,varX);
        result = tan(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_COT) {
        eat(TOKEN_COT,currentToken,input,varX);
        result = 1/tan(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }


    //Ters trigo
    else if ((*currentToken).type == TOKEN_ASIN) {
        eat(TOKEN_ASIN,currentToken,input,varX);
        result = asin(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_ACOS) {
        eat(TOKEN_ACOS,currentToken,input,varX);
        result = acos(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_ATAN) {
        eat(TOKEN_ATAN,currentToken,input,varX);
        result = atan(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }else if ((*currentToken).type == TOKEN_ACOT) {
        eat(TOKEN_ACOT,currentToken,input,varX);
        result = 1/atan(parseDegree3(input,currentToken,varX));
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }

    //Log
    else if ((*currentToken).type == TOKEN_LOG) {
        eat(TOKEN_LOG,currentToken,input,varX);
        tempLogBase= currentToken->value;
        eat(TOKEN_NUM,currentToken,input,varX);
        eat(TOKEN_LPAREN,currentToken,input,varX);
        result = log10(parseDegree3(input,currentToken,varX))/log10(tempLogBase);
        eat(TOKEN_RPAREN,currentToken,input,varX);
    }


    //üstelde sadecd 1. adıma dön digerlerinde 3. adıma dön
    while ((*currentToken).type == TOKEN_EXPONENT )
    {
        eat(TOKEN_EXPONENT,currentToken,input,varX);
        result = pow(result,parseDegree1(input,currentToken,varX));
    }
    
    return result;
}


double parseDegree2(char **input,Token* currentToken,float* varX) {
    double result = parseDegree1(input,currentToken,varX);


    while ((*currentToken).type == TOKEN_MUL || (*currentToken).type == TOKEN_DIV) {

        if ((*currentToken).type == TOKEN_MUL) {
            eat(TOKEN_MUL,currentToken,input,varX);
            result *= parseDegree1(input,currentToken,varX);
        }else if ((*currentToken).type == TOKEN_DIV) {
            eat(TOKEN_DIV,currentToken,input,varX);
            double divisor = parseDegree1(input,currentToken, varX);
            if (divisor != 0.0) {
                result /= divisor;
            } else {
                printf("Error: Sifirla bölme\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    return result;
}


double parseDegree3(char **input,Token* currentToken,float* varX) {
    double result = parseDegree2(input,currentToken,varX);
    
    while ((*currentToken).type == TOKEN_PLUS || (*currentToken).type == TOKEN_MINUS) {
        if ((*currentToken).type == TOKEN_PLUS) {
            eat(TOKEN_PLUS,currentToken,input,varX);
            result += parseDegree2(input,currentToken,varX);
        } else if ((*currentToken).type == TOKEN_MINUS) {
            eat(TOKEN_MINUS,currentToken,input,varX);
            result -= parseDegree2(input,currentToken,varX);
        }
    }
    return result;
}


double funcEvaluate(char *input,float varX) {
    Token currentToken;
    currentToken = getNextToken(&input,&varX);
    return parseDegree3(&input,&currentToken,&varX); 

}


int main() {

   
    
    int program = 1;
    int choice = 0;

    system("Cls");

    while (program==1){
        
        //menu
        //system("Cls");
        printf("-------------------\n");
        printf("Program Secenekleri\n");
        printf("-------------------\n");
        printf("1  + Bisection Yontemi \n");
        printf("2  + Regula-Falsi Yontemi\n");
        printf("3  + Newton Raphson Yontemi\n");
        printf("4  + NxN'lik bir matrisin ters\n");
        printf("5  + Gauss Eliminasyon\n");
        printf("6  + Gauss Seidal Yontemi\n");
        printf("7  + Sayisal Turev \n");
        printf("8  + Simpson Yontemi\n");
        printf("9  + Trapez Yontemi\n");
        printf("10 + Gregory newton Enterpolasyonu\n");
        printf("0  - Programdan Cik\n");
        printf("-------------------\n");

        //İşlem seçimi
        printf("Yapilacak islem secin :");

        scanf(" %d",&choice);
        
        if (choice!=0){
            switch (choice){

                case 1:
                    Bisection();
                    break;
                case 2:
                    Regula_Falsi();
                    break;
                case 3:
                    Newton_Raphson();
                    break;
                case 4:
                    Inverse_Matrix();
                    break;
                case 5:
                    Gauss_Elimination();
                    break;
                case 6:
                    Gauss_Seidal();
                    break;
                case 7:
                    Numeric_Deriv();
                    break;
                case 8:
                    Simpson();
                    break;
                case 9:
                    Trapez();
                    break;
                case 10:
                    Gregory_Newton();
                    break;
                
                default:
                    break;
            }


        }else if(choice==0){program=0;}
        printf("\n");
    
    
    
    }

    return 0;
}

int Bisection(){

    char inputFunc[100];
    char inputNumber[20];
    char* ptr1;
    char* tmp;
    

    double bisectionValLeft; //Val fonksiyon degerlerini tutan degiskenler
    double bisectionValRight;
    double bisectionValcenter;
    double errorVar;
    double currentError;
   
    int maxIter = 1000;
    int iterCount=0;

    double leftEdge; // x degerlerini tutan degiskenler
    double rightEdge;
    double central;

    printf("-----------\n");
    
    printf("Fonksiyonu giriniz:");
    scanf(" ");
    fgets(inputFunc,INPUT_SIZE,stdin);
    printf("Kok aranacak ilk araligi girin.\n");

    printf("Sol sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    leftEdge = strtod(inputNumber,&ptr1);//ptr1 kullanılmıyor sadece strtod kullanımı icin
    
    printf("Sag sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    rightEdge = strtod(inputNumber,&ptr1);

    printf("Epsilon degeri girin:");
    fgets(inputNumber,VAR_SIZE,stdin);
    errorVar = strtod(inputNumber,&ptr1);
    


    printf("%s\n",inputFunc);
    bisectionValLeft = funcEvaluate(inputFunc,leftEdge);

    bisectionValRight = funcEvaluate(inputFunc,rightEdge);


    if (bisectionValLeft*bisectionValRight<=0)
    {   
        currentError = bisectionValRight-bisectionValLeft;

        while ((iterCount<maxIter)&&(fabs(currentError)>=errorVar))
        {
            central = (leftEdge + rightEdge)/2;
            bisectionValcenter =funcEvaluate(inputFunc,central);

            if(bisectionValcenter == 0){break;} // Kok bulundu
            if (bisectionValcenter * bisectionValLeft <0){
                bisectionValRight = bisectionValcenter;
                rightEdge=central;

            }else{
                bisectionValLeft= bisectionValcenter;
                leftEdge=central;
            }

            iterCount++;
            currentError = bisectionValRight-bisectionValLeft;

        }

        printf("Kok Bulundu: %.4f Degeri:%.4f\n",central,bisectionValcenter);
        printf("Iterasyon Sayisi : %d\n",iterCount);
        system("Pause");
    }
    
    else{
        printf("Degerlerde hata var. f(Sol) = %.4lf , f(Sag) = %.4lf\n",bisectionValLeft,bisectionValRight);
        system("Pause");
    }
  
    return 0;
}
int Regula_Falsi(){
    
    char inputFunc[100];
    char inputNumber[20];
    char* ptr1;
    char* tmp;
    

    double leftX; //x degerlerini tutan degiskenler
    double rightX;
    double nextX;
    double errorVar;
    double currentError;
   
    int maxIter = 1000;
    int iterCount=0;

    double leftValue; // fonksiyon degerlerini tutan degiskenler 
    double rightValue;
    double nextValue;

    printf("-----------\n");
    
    printf("Fonksiyonu giriniz:");
    scanf(" ");
    fgets(inputFunc,INPUT_SIZE,stdin);
    printf("Kok aranacak ilk araligi girin.\n");

    printf("Sol sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    leftX = strtod(inputNumber,&ptr1);//ptr1 kullanılmıyor sadece strtod kullanımı icin
    
    printf("Sag sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    rightX = strtod(inputNumber,&ptr1);

    printf("Epsilon degeri girin:");
    fgets(inputNumber,VAR_SIZE,stdin);
    errorVar = strtod(inputNumber,&ptr1);
    

    leftValue = funcEvaluate(inputFunc,leftX);
    rightValue = funcEvaluate(inputFunc,rightX);


    if (leftValue*rightValue < 0)
    {   
        currentError = rightValue-leftValue;

        while ((iterCount<maxIter)&&(fabs(currentError)>=errorVar))
        {
            nextX = (rightX*leftValue-leftX*rightValue)/(leftValue-rightValue);
            nextValue =funcEvaluate(inputFunc,nextX);

            if(nextValue == 0){break;} // Kok bulundu
            if (nextValue * leftValue <0){
                rightX=nextX;
                rightValue = nextValue;

            }else{
                leftX=nextX;
                leftValue= nextValue;
            }

            iterCount++;
            currentError = rightValue-leftValue;

        }

        printf("Kok Bulundu: %.4f Degeri:%.4f\n",nextX,nextValue);
        printf("Iterasyon Sayisi : %d\n",iterCount);
        system("Pause");
    }
    else if (leftValue == 0){
        printf("Kok Sol sinirda: %.4f Degeri:%.4f\n",leftX,leftValue);
        system("Pause");
    }else if (rightValue == 0){
        printf("Kok Sag sinirda: %.4f Degeri:%.4f\n",rightX,rightValue);
        system("Pause");
    }
    
    else{
        printf("Degerlerde hata var. f(Sol) = %.4f ,f(Sag) = %.4f\n",leftValue,rightValue);
        system("Pause");
    }


    return 0;
}
int Newton_Raphson(){

    
    char inputFunc[100];
    char inputNumber[20];
    char* ptr1;
    char* tmp;
    

    double leftX; 
    double nextX;

    double errorVar;
    double currentError;
   
    int maxIter = 1000;
    int iterCount=0;

    double leftValue; // fonksiyon degerlerini tutan degiskenler 
    double nextValue;
    double leftValueDeriv;
    

    printf("-----------\n");
    
    printf("Fonksiyonu giriniz:");
    scanf(" ");
    fgets(inputFunc,INPUT_SIZE,stdin);

    printf("Arama icin baslangic noktasi girin:");
    fgets(inputNumber,VAR_SIZE,stdin);
    leftX = strtod(inputNumber,&ptr1);//ptr1 kullanılmıyor sadece strtod kullanımı icin

    printf("Epsilon degeri girin:");
    fgets(inputNumber,VAR_SIZE,stdin);
    errorVar = strtod(inputNumber,&ptr1);
    


    currentError = errorVar+5; //Basta error bilmiyoruz ,donguye girmesi icin deger verdik


    while ((iterCount<maxIter)&&(fabs(currentError)>=errorVar))
    {
        leftValue = funcEvaluate(inputFunc,leftX);
        leftValueDeriv = centralDerivative(inputFunc,leftX,0.01);

        nextX = leftX-(leftValue/leftValueDeriv);
        nextValue =funcEvaluate(inputFunc,nextX);

        iterCount++;
        currentError = nextValue-leftValue;
        
        leftX=nextX;
        leftValue= nextValue;

        if(nextValue == 0){break;} // Kok bulundu
    }
    
    if (iterCount==maxIter){
        printf("Iterasyon sinirina ulasildi. Bulunan nokta : %.4f Degeri:%.4f\n",nextX,nextValue);
        system("Pause");
    }else{        
        printf("\nKok Bulundu: %.4f Degeri:%.4f\n",nextX,nextValue);
        printf("Iterasyon Sayisi : %d\n",iterCount);
        system("Pause");
    }

    return 0;
}
int Inverse_Matrix(){

    int N,i,j,k;
    double divider,subtract,temp;
    Matris matris;
    
    printf("Matrisi Boyutu girin:");
    scanf("%d",&N);
    matris.column = N;
    matris.row = N;


    Matris I =createIdentityMatrix(N);
    I.row=N;
    I.column=N;
    matris.data=(double**) malloc(N*sizeof(double*));
    
    
    for (i = 0; i < N; i++)
    {
        matris.data[i]=(double*) malloc(N*sizeof(double));
    }


    //Matrisi al

    printf("\nMatrisi girin\n");
    for (i = 0; i < matris.row; i++)
    {
        for (j = 0; j < matris.column; j++)
        {
            printf("Matris[%d][%d]:",i,j);
            scanf("%lf",&matris.data[i][j]);
        }
    }
    printf("\nMatris[\n");
    PrintMatris(matris);
    printf("]");

    for (i = 0; i < N; i++) {
        if (matris.data[i][i] == 0) {  // Pivot sıfırsa, sıfır olmayan bir pivot bul
            int swap = 0;
            for (k = i + 1; k < N; k++) {
                if (matris.data[k][i] != 0) {
                    swap = 1;
                    for (j = 0; j < N; j++) {
                        temp = matris.data[i][j];
                        matris.data[i][j] = matris.data[k][j];
                        matris.data[k][j] = temp;

                        temp = I.data[i][j];
                        I.data[i][j] = I.data[k][j];
                        I.data[k][j] = temp;
                    }
                    break;
                }
            }
            if (!swap) {
                for (k = 0; k < N; k++) free(I.data[k]);
                free(I.data);

                printf("Matrisin tersi yok.\n");
                return 0;  // Matris tekil, tersi yok
            }
        }

        temp = matris.data[i][i];
        // printf("Temp: %lf \n",temp);
        for (j = 0; j < N; j++) {
            matris.data[i][j] /= temp;
            I.data[i][j] /= temp;
        }



        for (k = 0; k < N; k++) {
            if (k != i) {

                
                temp = matris.data[k][i];
                // printf("Temp For icinde : %lf\n",temp);
                for ( j = 0; j < N; j++) {
                    matris.data[k][j] -= matris.data[i][j] * temp;
                    I.data[k][j] -= I.data[i][j] * temp;
                }
            }
        }
    }
    
    printf("\n\n");
    printf("Matrisin Tersi[\n");
    PrintMatris(I);
    printf("]");

    for (i = 0; i < N; i++)
    {
        free(matris.data[i]);
    }
    free(matris.data);
    return 0;
}

Matris createIdentityMatrix(int n) {
    int i,j;
    Matris I;
    I.data = (double **)malloc(n * sizeof(double *));
    for (i = 0; i < n; i++) {
        I.data[i] = (double *)malloc(n * sizeof(double));
        for (j = 0; j < n; j++) {
            if (i == j) I.data[i][j] = 1.0;
            else I.data[i][j] = 0.0;
        }
    }
    return I;
}

int PrintMatris(Matris m1){

    int i,j;
    for (i = 0; i < m1.row; i++)
    {   
        printf("    ");
        for (j = 0; j< m1.column; j++)
        {
            printf("%lf ",m1.data[i][j]);
        }
        printf("\n");   
    }
    return 0;
}
int Gauss_Elimination(){

    int N,i,j,k;
    double divider,subtract,ratio,sum,temp;
    Matris matris;

    printf("Denklem sayisi:");
    scanf("%d",&N);

    matris.column = N+1;
    matris.row = N;
    double solutions[N];

    //Matirisi oluşturma
    matris.data=(double**) malloc(N*sizeof(double*));    
    for (i = 0; i < N; i++)
    {
        matris.data[i]=(double*) malloc((N+1)*sizeof(double));
    }



    //Matrisi al
    printf("Denklem sistemini girin.\n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("Denklem[%d] Bilinmeyen[%d] Katsayisi:",i+1,j+1);
            scanf("%lf",&matris.data[i][j]);
        }
        printf("Denklem[%d] Sonuc:",i+1);
        scanf("%lf",&matris.data[i][j]);
    }

    //sistemi yazdır
    
    printf("\n");
    printf("Denklem Sistemi:\n",i+1);
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N-1; j++)
        {
            printf("%.2lf*x_%d + ",matris.data[i][j],j+1);
        }
        printf("%.2lf*x_%d = %lf\n",matris.data[i][j],j+1,matris.data[i][j+1]);
        
    }
    printf("\n");
    

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++){
            if(j>i){
                ratio = matris.data[j][i]/ matris.data[i][i];
                for (k = 0; k < N+1; k++) {
                    matris.data[j][k]-= ratio*matris.data[i][k];
                }
            }
        }
    }
    
    //Terimleri sıfırla
    for ( i = 0; i < N; i++)
    {
        solutions[i]=0;
    }

    //Son terimi gir
    solutions[N-1]=matris.data[N-1][N]/matris.data[N-1][N-1];

    //Terimleri yerine koyarak çöz
    for ( i = N-1; i >= 0; i--){
        sum=0;
        for (j = N-1; j > i; j--){
            sum+=solutions[j]*matris.data[i][j];
        }
        solutions[j]=(matris.data[j][N]-sum)/matris.data[j][j];
    }
    
    printf("Cozumler:\n",i+1);
    for ( i = 0; i < N; i++)
    {
        printf("x_%d : %.4lf \n",i+1,solutions[i]);
    }


    // PrintMatris(matris);

    for (i = 0; i < N; i++)
    {
        free(matris.data[i]);
    }
    free(matris.data);
    return 0;

}
int Gauss_Seidal(){

    int N,i,j,k,max,iter,maxIter;
    double num,temp,error,currenError;
    Matris matris;

    maxIter=100;
    error=0.01;
    currenError=5;//Büyük bir deger olsun diye


    printf("Denklem sayisi:");
    scanf("%d",&N);

    matris.column = N+1;
    matris.row = N;
    double solutions[N];

    //Matirisi oluşturma
    matris.data=(double**) malloc(N*sizeof(double*));    
    for (i = 0; i < N; i++)
    {
        matris.data[i]=(double*) malloc((N+1)*sizeof(double));
    }



    //Matrisi al
    printf("Denklem sistemini girin\n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("Denklem[%d] Bilinmeyen[%d] Katsayisi:",i+1,j+1);
            scanf("%lf",&matris.data[i][j]);
        }
        printf("Denklem[%d] Sonucu:",i+1);
        scanf("%lf",&matris.data[i][j]);
    }
    

    //Degiskenleri gecici olarak tutacak
    printf("Epsilon degeri girin\n");
    scanf(" %lf",&error);
    printf("Baslangic degelerini girin\n");

    for ( i = 0; i < N; i++)
    {
        printf("Bilinmeyen[%d]:",i+1);
        scanf("%lf",&num);
        solutions[i]=num;
    }

    //Kosegen max yap

    for (i = 0; i < N; i++) {
        //i şuanki satır sütun
        max=i;
        for (j = 0; j < N; j++){
            if (matris.data[max][i]<matris.data[j][i])
            {
                max=j;  
            }
        }

        if (max!=i){
            //Degistirmek mantıklıysa degistir
            if(matris.data[i][i]*matris.data[max][max]<matris.data[max][i]*matris.data[i][max]){ 
                //Swap
                for (k = 0; k < N+1; k++)//Max satır ile i inci satırı swaple
                {
                    temp = matris.data[i][k];
                    matris.data[i][k]=matris.data[max][k];
                    matris.data[max][k]=temp;

                }
            }
        }
        
    }
    printf("\nGenisletilmis Matris:\n");
    PrintMatris(matris);

    iter=0;
    while ((currenError > error)&&(iter<maxIter))
    {
        currenError = 0;
        // Denklemi cöz
        for (i = 0; i < N; i++)
        {

            temp = matris.data[i][N]; // Denklemin sonucu
            for (j = 0; j < N; j++)
            {   
                if(j!=i){
                    temp -= solutions[j] * matris.data[i][j];
                }
            }
            currenError+=fabs((temp / matris.data[i][i])-solutions[i]);
            solutions[i] = temp / matris.data[i][i];
        ;
        
        iter++;
        }

    }

    printf("\n%d iterasyonda bulundu. \nSonuclar:\n",iter);
    for (i = 0; i < N; i++)
    {
        printf("Bilinmeyen[%d] : %.4lf \n",i+1,solutions[i]);
    }

    for (i = 0; i < N; i++)
    {
        free(matris.data[i]);

    }
    free(matris.data);
    return 0;
}
int Numeric_Deriv(){

    char inputFunc[100];
    char inputNumber[20];
    char* ptr1;
    char* tmp;
    double x_coord;

    double result;
    double step;
    int choice;

    printf("-----------\n");
    
    printf("1=Merkezi Fark 2=Ileri Fark 3=Geri fark\nTurevin turunu secin: ");
    scanf(" %d",&choice);
      
    printf("Adim boyutunu girin:");
    scanf(" %lf",&step);
    
    printf("Fonksiyonu giriniz:");
    scanf(" ");
    fgets(inputFunc,INPUT_SIZE,stdin);
    printf("Turev alinacak noktayi girin:");

    fgets(inputNumber,VAR_SIZE,stdin);
    x_coord = strtod(inputNumber,&ptr1);


    switch (choice)
    {
    case 1:
        result = centralDerivative(inputFunc,x_coord,step);
        printf("\nSonuc:%lf\n",result);
        break;
    case 2:
        result = forwardDerivative(inputFunc,x_coord,step);
        printf("\nSonuc:%lf\n",result);
        break;
    case 3:
        result = backwardDerivative(inputFunc,x_coord,step);
        printf("\nSonuc:%lf\n",result);
        break;
    
    default:
        break;
    }


    system("Pause");
    
    return 0;
}
double centralDerivative(char* input,double x,double h){
    return (funcEvaluate(input,x+h/2)-funcEvaluate(input,x-h/2))/(h);
}
double forwardDerivative(char* input,double x,double h){
    return (funcEvaluate(input,x+h)-funcEvaluate(input,x))/(h);
}
double backwardDerivative(char* input,double x,double h){ 
    return (funcEvaluate(input,x)-funcEvaluate(input,x-h))/(h);
}
int  Simpson(){

    char inputFunc[100];
    char inputNumber[20]; //Sayilari aldigimiz degisken
    char* ptr1;
    char* tmp;
    

    double leftX; //x degerlerini tutan degiskenler
    double rightX;

    int stepNumber;
    int currentStep;
    double stepSize;
   

    double leftValue; // fonksiyon degerlerini tutan degiskenler 
    double rightValue;

    double y0;
    double y1;
    double y2;

    double sum=0;

    printf("-----------\n");
    
    printf("Fonksiyonu giriniz:");
    scanf(" ");
    fgets(inputFunc,INPUT_SIZE,stdin);
    printf("Integral alinacak araligi girin.\n");

    printf("Sol sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    leftX = strtod(inputNumber,&ptr1);//ptr1 kullanılmıyor sadece strtod kullanımı icin
    
    printf("Sag sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    rightX = strtod(inputNumber,&ptr1);

    printf("Adim sayisi:");
    fgets(inputNumber,VAR_SIZE,stdin);
    stepNumber = strtod(inputNumber,&ptr1);

    while (stepNumber % 2 !=0)
    {
        printf("Adim sayisi Simpson da cift olmalidir. Tekrar girin.\n");
        printf("Adim sayisi:");
        fgets(inputNumber,VAR_SIZE,stdin);
        stepNumber = strtod(inputNumber,&ptr1);
    }

    rightValue = funcEvaluate(inputFunc,rightX);


    if (leftX<rightX)
    {   
        currentStep=0;
        stepSize = (rightX-leftX)/stepNumber;

        while (currentStep<stepNumber)
        {
            y0 = funcEvaluate(inputFunc,leftX);
            y1 = funcEvaluate(inputFunc,leftX+stepSize);
            y2 = funcEvaluate(inputFunc,leftX+stepSize*2);

            sum += stepSize/3*(y0+4*y1+y2);

            currentStep+=2;
            leftX+=2*stepSize;
        }
        
        printf("Sonuc: %.4f\n",sum);
        system("Pause");    
    }
    
    else{
        printf("Degerlerde hata var. Sol = %.4f , Sag = %.4f\n",leftValue,rightValue);
        system("Pause");
    }


    return 0;
}
int Trapez(){

    char inputFunc[100];
    char inputNumber[20];
    char* ptr1;
    char* tmp;
    

    double leftX; //x degerlerini tutan degiskenler
    double rightX;

    double stepNumber;
    double stepSize;
    int currentStep;
   

    double leftValue; // fonksiyon degerlerini tutan degiskenler 
    double rightValue;

    double sum=0;

    printf("-----------\n");
    
    printf("Fonksiyonu giriniz:");
    scanf(" ");
    fgets(inputFunc,INPUT_SIZE,stdin);
    printf("Integral alinacak araligi girin.\n");

    printf("Sol sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    leftX = strtod(inputNumber,&ptr1);//ptr1 kullanılmıyor sadece strtod kullanımı icin
    
    printf("Sag sinir:");
    fgets(inputNumber,VAR_SIZE,stdin);
    rightX = strtod(inputNumber,&ptr1);

    printf("Adim sayisi:");
    fgets(inputNumber,VAR_SIZE,stdin);
    stepNumber = strtod(inputNumber,&ptr1);

    
    rightValue = funcEvaluate(inputFunc,rightX);


    if (leftX<rightX)
    {   
        currentStep=0;
        stepSize = (rightX-leftX)/stepNumber;

        //Donguye girmeden once bir kere hesapla
        leftValue = funcEvaluate(inputFunc,leftX);

        while (currentStep<stepNumber)
        {
            rightX = leftX+stepSize;
            rightValue =funcEvaluate(inputFunc,rightX);

            sum += (leftValue+rightValue)/2 * stepSize;

            leftX = rightX;
            leftValue = rightValue;

            currentStep++;
        }
        
        printf("Sonuc: %.4f\n",sum);
        system("Pause");    
    }
    
    else{
        printf("Degerlerde hata var. Sol = %.4lf , Sag = %.4lf\n",leftValue,rightValue);
        system("Pause");
    }


    return 0;
}
int Gregory_Newton(){

    double varX,varX2,result;
    double** table;
    int N,i,j,flag,degree,memoryNum;
    memoryNum=2;

    printf("-----------\n");
    printf("Kac adet deger girilecek :");
    scanf(" %d",&N);
   


    table = calloc(N,sizeof(double*));

    if (table == NULL) {
        printf("Bellek tahsisi basarisiz.\n");
        return 1;
    }

    for ( i = 0; i < N; i++)
    {
        table[i] = calloc(memoryNum,sizeof(double));

        if (table[i] == NULL) {
            printf("%d. satir icin bellek tahsisi basarisiz.\n", i);
            // Burada önceden ayrılmış belleği temizle
            while (--i >= 0) free(table[i]);
            free(table);
            return 1;
        }
    }
    

    for ( i = 0; i < N; i++)
    {
        printf("%d. degeri girin:" , i+1);
        scanf("%lf",&table[i][0]);
    }

    flag = 1;
    degree = 0;
    while (flag)
    {

        for (i = 1; i < memoryNum; i++)//Hepsi aynı mı kontorl et
        {    
            if(table[i][degree]==table[i-1][degree]){
                flag = 0;
            }
        }

        if (flag){

            degree++;

            if (degree==memoryNum){
                memoryNum=memoryNum*2;
                table=realloc(table,memoryNum*sizeof(double*));
    
                for ( i = memoryNum/2; i < memoryNum; i++)
                {
                    table[i]=(double*) calloc(N,sizeof(double));
                }
            }

            for (j = 0; j < N-degree; j++)
            {
                table[j][degree]= table[j+1][degree-1]-table[j][degree-1];
            }
        }
        if (degree+1==N){flag=0;}

    }


    printf("Fonksiyonda istenen noktayi girin:");
    scanf("%lf",&varX);



    //Degeri hesapla
    result = 0;
    result += table[0][0];
    varX2=varX;

    for (i = 1; i < degree+1; i++)
    {
        result += varX*table[0][i]/fac(i);
        varX = varX* (varX2-(1*i));
    }

    printf("\nx = %.4lf icin y = %.4lf \n",varX2,result);

    for (i = 0; i < degree; i++) {
        free(table[i]);
    }
    free(table);
    
    printf("\n");
    system("Pause");

    return 0;
}

