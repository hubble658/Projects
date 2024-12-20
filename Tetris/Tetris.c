#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define SYMBOLSIZE 3  //Sekillerin oldugu array in boyutu 3x3
#define CURSOR_TO(X,Y) printf("\033[%d;%df",(Y),(X))


void newShape(int arr[SYMBOLSIZE][SYMBOLSIZE]){

    int i;

    int shape1[SYMBOLSIZE][SYMBOLSIZE]={
        {0,2,0},
        {2,2,2},
        {0,0,0}};                
    int shape2[SYMBOLSIZE][SYMBOLSIZE]={
        {2,0,0},
        {2,0,0},
        {2,2,0}};
    int shape3[SYMBOLSIZE][SYMBOLSIZE]={
        {0,0,0},
        {2,2,2},
        {0,0,0}};
    int shape4[SYMBOLSIZE][SYMBOLSIZE]={
        {0,0,0},
        {0,2,2},
        {2,2,0}};
    int shape5[SYMBOLSIZE][SYMBOLSIZE]={
        {0,0,0},
        {0,2,2},
        {0,0,0}};
    int shape6[SYMBOLSIZE][SYMBOLSIZE]={
        {0,0,0},
        {0,2,0},
        {0,0,0}};
    int shape7[SYMBOLSIZE][SYMBOLSIZE]={
        {2,2,0},
        {2,2,0},
        {0,0,0}};               

    //Rastgele sekiller arasından birini seçer
    i = rand()%7;

    switch (i)
    {
    case 0:
        memcpy(arr,shape1,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
        break;
    case 1:
        memcpy(arr,shape2,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
        break;
    case 2:
        memcpy(arr,shape3,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
        break;
    case 3:
        memcpy(arr,shape4,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
        break;
    case 4:
        memcpy(arr,shape5,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
        break;
    case 5:
        memcpy(arr,shape6,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
        break;
    case 6:
        memcpy(arr,shape7,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
        break;
    default:
        break;
    }
}

void rotateLeft(int arr[SYMBOLSIZE][SYMBOLSIZE]){
    int i = 0;
    int j = 0;
    int rotatedArr[SYMBOLSIZE][SYMBOLSIZE];

    for(i=0;i<SYMBOLSIZE;i++){
        for(j=0;j<SYMBOLSIZE;j++){
            rotatedArr[2-j][i]=arr[i][j];
        }
    }
    memcpy(arr,rotatedArr,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
}

void rotateRight(int arr[SYMBOLSIZE][SYMBOLSIZE]){
    int i = 0;
    int j = 0;
    int rotatedArr[SYMBOLSIZE][SYMBOLSIZE];

    for(i=0;i<SYMBOLSIZE;i++){
        for(j=0;j<SYMBOLSIZE;j++){
            rotatedArr[j][2-i]=arr[i][j];
        }
    }
    memcpy(arr,rotatedArr,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
}

void placeShape(int symbol[3][3], int x_cord,int** array, int fRows, int fColumns){
    int farLeft;
    int top;
    int control=1;
    int xLenght=0;
    int yLenght=0;

    int i,j,k;

    
    i=0;
    while((i<3)&&(control==1))//Sembolun ilk dolu olan sutunu
    {
        for (j = 0; j < 3; j++)
        {
            if(symbol[j][i]==2){
                farLeft=i;
                control=0;
            }
        }
        i++;
    }
    i=0;
    control=1;
    while((i<3)&&(control==1))//Sembolun ilk dolu olan satiri
    {
        for (j = 0; j < 3; j++)
        {
            if(symbol[i][j]==2){
                top=i;
                control=0;
            }
        }
        i++;
    }
    //uzunluk bulma
    for (i = 0; i < 3; i++)//Sembolun yatay uzunulugu   
    {
        k=0;
        for (j = 0; j < 3; j++){
            if (symbol[j][i]==2)
            {
                k=1;
            }
            
        }
        if(k==1){
            xLenght++;
        }
    }

    for (i = 0; i < 3; i++)//Sembolun dikey uzunulugu   
    {   
        k=0;
        for (j = 0; j < 3; j++){
            if (symbol[i][j]==2)
            {
                k=1;
            }    
        }
        if(k==1){
            yLenght++;
        }
    }
    


    while ((x_cord+xLenght)>fColumns)    //yerlestirilecek yer sınırlar içinde mi
    {
        x_cord--;
    }


    for (i = top; i < top+yLenght; i++)//Sembolu yerlestir
    {
        for (j = farLeft; j < farLeft+xLenght; j++)
        {   
            if (symbol[i][j]==2)
            {
               array[i-top][x_cord+j-farLeft]=symbol[i][j];
            }
            
            
        }               
    }      
}

int moveDown(int** array, int fRows, int fColumns){   // Fonksiyonun donderdigi degere göre en asagıda olup olmadigi anlasiliyor
    int i,j;
    int control=1;
    fRows--;//indeks dizilerde 0 dan baslıyor  


    for(i=fRows;i>-1;i--){
        for (j = 0; j < fColumns; j++)
        {   
            if(array[i][j]==2){   
                if (i==fRows)
                {
                    return 1;// Blok en asagı dustu 1 degeri döndur
                }else if (array[i+1][j]==1)
                {
                    return 1;//Blokun altı dolu 1 degeri döndur
                }
            }  
        }
    }
    for(i=fRows;i>-1;i--){
        for (j = 0; j < fColumns; j++)//Bir birim asagı ötele
        {   
            if(array[i][j]==2)
            {
                array[i][j]=0;
                array[i+1][j]=2;
            } 
        }

    }
    return 0;
}

void drawTable(int** array, int fRows, int fColumns,int currentS[SYMBOLSIZE][SYMBOLSIZE],int nextS[SYMBOLSIZE][SYMBOLSIZE]){

    int i = 0;
    int j = 0;
    
    system("cls");   
        
    //ustte yerlestirilecek olan sekili yazdır 
    printf("\033[1;32m");
    for (i = 0; i < SYMBOLSIZE; i++)
    {
        
        for (j = 0; j < SYMBOLSIZE; j++)
        {
            CURSOR_TO(fColumns+2*j-1,i+1);
            if(currentS[i][j]==2){
                printf("*");
            }else{
                printf(" ");
            }
            
        }
    }
    printf("\033[1;0m");
    printf("\n\n");
    
    //Tabloyu yazdır
    for (i=0; i<fRows; i++){
        printf("|");
        for (j=0; j<fColumns; j++){
            if (array[i][j]==0){
                printf(" |");            
            }else{
                printf("\033[1;32m");
                printf("*");
                printf("\033[1;0m");
                printf("|");
                }
            }
        printf("\n");
    }
    for (i = 0; i < fColumns; i++){
        printf("--");
    }
    printf("\n"); 
    printf("|");
    if (fColumns<10) // Eger yatay boyut 10 dan kucukse tek satir boyunca numaralandırır
    {
        for (j=0; j<fColumns; j++){
            printf("%d|",(j+1));
        }
    }else{        // Eger yatay boyut 10 dan buyukse iki satir ust uste numaralari yazdırır
        for (j=0; j<fColumns; j++){
    
            printf("%d|",(j+1)/10);
        }
        printf("\n"); 
        printf("|");
        for (j=0; j<fColumns; j++){
    
            printf("%d|",(j+1)%10);
        }
    }

    //Sonraki blogu yanda yazdırır yazdır
    printf("\033[1;34m");
    CURSOR_TO((fColumns*2+5),(5+fRows/2));
    printf("Next Shape");
    
    for (i = 0; i < SYMBOLSIZE; i++)
    {
        for (j = 0; j < SYMBOLSIZE; j++)
        {
            CURSOR_TO((fColumns*2+6+2*j),(6+fRows/2+i)); //Tablo boyutuna göre yeri belirleniyor
            if(nextS[i][j]==2){
                printf("*");
            }
        }
    }
    printf("\033[1;0m");
    printf("\n\n");
    CURSOR_TO(0,fColumns+8);
}

void updateTable(int** array, int fRows, int fColumns,int* fScore){
    int i,j,k,l;
    int isFull;

    //Yerlesen blokları 1 e cevirir
    for(i=0;i<fRows;i++){
        for (j = 0; j < fColumns; j++)
        {  
            if(array[i][j]==2)
            {   
                array[i][j]=1;
            }
        }           
    }

    //satır yok etme
    for (i = 0; i < fRows; i++)
    {
        isFull=1;
        j=0;
        while ((j<fColumns)&&isFull==1)
        {
            if (array[i][j]==0)
            {
                isFull=0;
            }
            j++;
        }
        if (isFull==1)
        {
            for (k = i; k > 0; k--)//bloklari asagı dusur              
            {
                for (l = 0; l < fColumns; l++)
                {
                    array[k][l]=array[k-1][l];
                }
            }
        Sleep(100);
            for (k = 0; k < fColumns; k++)//ilk blogu sifirla
            {
                array[0][k]=0;
            }
            *fScore = *fScore +fColumns; //scoru arttir
        }
            
    }
}


int main(){

    int i,j;
    int rows;
    int columns;
    int isAlive=1;
    int gameOn=1;
    int score=0;
    int highscore=0;

    char userInput[1];;
    int userPlace;

    int **gameTable;
    int emptyShape[SYMBOLSIZE][SYMBOLSIZE]={{0,0,0},{0,0,0},{0,0,0}};
    int currentShape[SYMBOLSIZE][SYMBOLSIZE];
    int nextShape[SYMBOLSIZE][SYMBOLSIZE];
    
    time_t t1;
    srand((unsigned)time(&t1));

    //Oyun tablosunu olustur
    printf("----------TETRIS----------\n");
    printf("\\\\\\\\\\\\\\\\ WELLCOME ////////\n\n");
    printf("--Rules--\n");
    printf("Rule 1: To start the game you must enter the dimensions\n");
    printf("Rule 2: Dimension of the must be between 3-50.\n");
    printf("Rule 3: You can rotate the shape as many times as you want before placing it.\n");
    printf("Rule 4: To place the shape, you must press the 'S' key, then the game will ask you where to place it.\n");
    printf("Rule 5: Shape placement reference is the leftmost side of the shape\n");
    printf("Rule 6: If the block goes outside the game boundaries, it is shifted to the left\n\n");
    printf("Enter board dimensions to start Game.\n\n");
    printf("Number of Rows :");
    scanf(" %d",&rows);
    printf("Number of Colums :");
    scanf(" %d",&columns);

    while ((rows<3)||(columns<3)||(rows>50)||(columns>50))
    {
        printf("\nThe row and column length must be between 3-50.\nPlease enter again.\n");
        printf("Number of Rows :");
        scanf(" %d",&rows);
        printf("Number of Colums :");
        scanf(" %d",&columns);
    }
    
    while (gameOn==1)
    {
        
        gameTable = malloc(sizeof(int *) * rows);       //Tablo sıfırla
        for (i=0;i<rows;i++){
            gameTable[i]= malloc(sizeof(int) * columns);    
        }
        for (i=0;i<rows;i++){
            for(j=0;j<columns;j++){
                gameTable[i][j]=0;
            }
        }
        isAlive=1;
        score=0;

        newShape(nextShape);//oyuna başlamadan 1 kere tanımla
        while (isAlive)
        {

            memcpy(currentShape,nextShape,sizeof(int)*SYMBOLSIZE*SYMBOLSIZE);
            newShape(nextShape);
            drawTable(gameTable,rows,columns,currentShape,nextShape);
            printf("\nRotate -> 'A'-'D' \nPlace  ->   'S'\nMenu   ->   'X'\nScore  ->    %d\n",score);
            
            //Oyun devam ediyor mu
            for (i = 0; i < columns; i++)
            {
                if(gameTable[0][i]==1){
                    userInput[0]='x';
                }
            }

            if(userInput[0]!='x'){      //devam ediyorsa input al
                scanf(" %c",&userInput);
            }
            while (userInput[0]!='S'&&userInput[0]!='s'&&isAlive){

                if ((userInput[0]=='X')||(userInput[0]=='x'))
                {
                    printf("\n");
                    printf("------------------\n");
                    printf("Game Over\n");
                    printf("------------------\n");
                    printf("Score :%d\n",score);
                    printf("Highest Score:%d\n",highscore);
                    printf("Do you want to play again? (Y)es/(N)o :");
                    scanf(" %c",&userInput);

                    if (userInput[0]=='Y'||userInput[0]=='y')//Tekrar oynamak istiyor
                   {
                    system("cls");
                    isAlive=0;
                   }else{ //Oyunu bitir
                    isAlive=0;
                    gameOn=0;
                    
                  }
                
                }

                if (userInput[0]=='A'||userInput[0]=='a')
                {
                    rotateLeft(currentShape);
                    drawTable(gameTable,rows,columns,currentShape,nextShape);
                }
                else if (userInput[0]=='D'||userInput[0]=='d')
                {
                    rotateRight(currentShape);
                    drawTable(gameTable,rows,columns,currentShape,nextShape);
                }
                
                if(userInput[0]!='Y'&&userInput[0]!='y'&&userInput[0]!='N'&&userInput[0]!='n'){
                    printf("\nRotate -> 'A'-'D' \nPlace  ->   'S'\nMenu   ->   'X'\nScore  ->    %d\n",score);
                    scanf(" %c",&userInput);
                }
            }
            

            if(userInput[0]!='Y'&&userInput[0]!='y'&&userInput[0]!='N'&&userInput[0]!='n'){//Oyun devam etmiyorsa buraları atlayacak
                do
                {
                    printf("\nEnter the column number ->");
                    scanf("%d",&userPlace);

                } while (userPlace>columns);

                userPlace--;//arraylerde indeks 0 dan basladıgı için 1 azalttık 
                placeShape(currentShape,userPlace,gameTable,rows,columns);
                drawTable(gameTable,rows,columns,emptyShape,nextShape);
                Sleep(250); 

                while (moveDown(gameTable,rows,columns)==0)//EN asagı dusene kadar devam et
                {
                    moveDown(gameTable,rows,columns);//Bir birim asagı dusur ve ekrana yaz
                    drawTable(gameTable,rows,columns,emptyShape,nextShape);
                    Sleep(150);
                }
                
                score++;  //1 tane sekil yerleştirdik puan arttır.
                updateTable(gameTable,rows,columns,&score);   // Ta  bloyu guncelle
                Sleep(300);

                if (score>highscore)
                {
                    highscore=score;
                }
            }
        }
               
    }
    
    for(i = 0; i < rows; i++){
        free(gameTable[i]);
    }     
    free(gameTable);

    return 0;
}
    
