#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <math.h>

#define _CRTDBG_MAP_ALLOC //Memoryleak test etmek icindir. Memory Leak bulunmamaktadır.
#include <crtdbg.h>


//LİNK :   https://www.youtube.com/watch?v=EAT5o7RRsgk

//Video da süre yetmedigi icin bazı kısımları hızlandırdım ve kestim. Youtube dan siz de videoyu hızlandırabilirisiniz.
//Mikrofon bazen sesi almadigi icin kesik kesik gelmis kusura bakmayin.



#define CURSOR_TO(X, Y) printf("\033[%d;%df", (Y), (X))
#define COLOR_BLACK "\033[1;30m"
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[1;34m"
#define COLOR_PURPLE "\033[1;35m"
#define COLOR_CYAN "\033[1;36m"
#define COLOR_WHITE "\033[1;37m"
#define COLOR_RESET "\033[1;0m"

#define MAX_DEPTH 4 // Hamle aramasi yaparken kac hamle ilerisine bakabilir


typedef struct P
{
    unsigned short score;
    signed char piece[5]; // 0-A 1-B 2-C 3-D 4-E sayilara karsılık gelen harfler , signed char yeterli oluyor.
} Player;

typedef struct T // Tablo struct ı
{

    int pcScore; // Bot da kullanmak için
    unsigned short turn;
    unsigned char gameType;
    unsigned char N;
    signed char** data; // int 4 byte char 1 byte. Hafizadan tasarruf etmek icin bu sekilde kullandim

    Player p[2];
} Table;

typedef struct node // Stack structı
{
    Table* data;
    struct node* next;
} stackNode;

stackNode* push(stackNode* head, Table* table);

void freeTable(Table* a);
void createTable(Table* a);
Table* copyTable(Table* src);

void printTable(Table* t);
void printPlayer(Table* gt);
int isMoveable(Table* t, int x, int y);
int isMoveOK(Table* t, int x, int y, int direction);
void highlightPiece(Table* t, int x, int y);
int doMove(Table* t, int* x, int* y, int direction);
void undoMove(Table* t, int* x, int* y, int direction, int eaten);
int convertToDirection(int a);
int isGameOver(Table* t);

void saveTable(const char* filename, Table* t);
Table* loadTable(const char* filename);

void calculateScore(Table* t);
void calculateScoreReal(Table* t);
void nextPositionsList(Table* a, stackNode** b, int pos_x, int pos_y, int end_x, int end_y);
int minimax(Table* position, int depht, int maximizingPlayer, int alpha, int beta);
Table* findBestMove(Table* position, int alpha , int beta);

stackNode* push(stackNode* head, Table* table)
{ 
    // Stack e veri kaydetme
    // Listenin yeni head i return ediliyor

    stackNode* newNode = malloc(sizeof(stackNode));
    if (newNode == NULL)
    {
        printf("Stack Overflow\n");
        exit(1);
    }
    newNode->data = table;
    newNode->next = head;
    return newNode;
}

void freeStack(stackNode* top) {
    //Stack free leme kodu

    stackNode* temp;
    while (top->next != NULL) {

        temp = top;
        top = top->next;
        
        if (temp->data != NULL) {
            freeTable(temp->data);
            temp->data=NULL;
        }
        free(temp);
        temp==NULL;
    }
    freeTable(top->data);
    free(top);
}

void createTable(Table* gameTable)//Tablo oluştur ve tabloyu rastgele taslarla doldur.
{

    int i, j, N;
    N = gameTable->N;

    gameTable->data = malloc(sizeof(signed char*) * N);
    for (i = 0; i < N; i++)
    {
        gameTable->data[i] = malloc(sizeof(signed char) * N);
    }
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (((j == N / 2) || (j == N / 2 - 1)) && ((i == N / 2) || (i == N / 2 - 1)))
            {
                gameTable->data[i][j] = -1;
            }
            else
            {
                gameTable->data[i][j] = rand() % 5;
            }
        }
    }
}

void freeTable(Table* table) //Tabloyu free le
{ 
    int i;
    if (table!=NULL) {
        if (table->data != NULL) {

            for (i = 0; i < table->N; ++i) {
                free(table->data[i]);
                table->data[i] = NULL;
            }
            free(table->data);
            table->data = NULL;
        }
    }
    free(table);
    table = NULL;
}

Table* copyTable(Table* src) //Deep copy yapar
{
    if (src == NULL) return NULL;

    Table* newTable = malloc(sizeof(Table));

    int i, j, N;
    N = src->N;

    newTable->N = src->N;
    newTable->turn = src->turn;
    newTable->pcScore = src->pcScore;
    newTable->gameType = src->gameType;

    //Oyuncuları kopyala
    for (i = 0; i < 2; i++)
    {
        newTable->p[i].score = src->p[i].score;
        for (j = 0; j < 5; j++)
        {
            newTable->p[i].piece[j] = src->p[i].piece[j];
        }
    }

    //Tabloyu kopyala
    newTable->data = malloc(sizeof(signed char*) * N);
    for (i = 0; i < N; i++)
    {
        newTable->data[i] = malloc(sizeof(signed char) * N);
        for (j = 0; j < N; j++)
        {
            newTable->data[i][j] = src->data[i][j];
        }
    }
    return newTable;
}

void printPlayer(Table* gt)//Oyuncuları yazdırır
{

    int i, j;
    calculateScoreReal(gt);

    for (j = 0; j < 2; j++)
    {
        // Sıra kimdeyse onun İsmi beyaz yazıyor
        if (j == gt->turn % 2){printf(COLOR_WHITE "P%d " COLOR_RESET, j + 1);}       
        else{printf("P%d ", j + 1);}


        //Taşları renkleriyle beraber kaç tane olduklarını yazdır
        
        for (i = 0; i < 5; i++)
        {
            if (gt->p[j].piece[i] == 0)
            {
                printf("\033[1;%dm_ ", 31 + i); // Taş yoksa alt cizgi koy
            }
            else
            {
                printf("\033[1;%dm%hhu ", 31 + i, gt->p[j].piece[i]); //Signed char %hhu
            }
        }

        // Önde olan kişiyi gösteriyor
        if ((gt->pcScore < 0) && (j == 0)) {printf(COLOR_WHITE "Lead " COLOR_RESET);}
        if ((gt->pcScore > 0) && (j == 1)){printf(COLOR_WHITE "Lead " COLOR_RESET);}
        printf("\n");
        printf(COLOR_RESET);

    }
}

void printTable(Table* table)//Oyun tablosunu yazıdır
{

    system("cls"); 
    int i, j;
    if (table->N < 10)
    {
        for (i = 0; i < table->N; i++)
        {
            printf(COLOR_WHITE);
            printf("|%d|", i + 1);
            printf(COLOR_RESET);
            for (j = 0; j < table->N; j++)
            {
                for (j = 0; j < table->N; j++)
                {
                    switch (table->data[i][j])
                    {
                    case 0:
                        // printf("%d%d",i,j);
                        printf(COLOR_RED "A " COLOR_RESET);
                        break;
                    case 1:
                        printf(COLOR_GREEN "B " COLOR_RESET);
                        break;
                    case 2:
                        printf(COLOR_YELLOW "C " COLOR_RESET);
                        break;
                    case 3:
                        printf(COLOR_BLUE "D " COLOR_RESET);
                        break;
                    case 4:
                        printf(COLOR_PURPLE "E " COLOR_RESET);
                        break;
                    case -1:
                        printf("_ ");
                        break;

                    default:
                        break;
                    }
                }
            }
            printf("\n");
        }
        printf(COLOR_WHITE);
        printf("  "); // Hizalamak için 3 bosluk
        for (i = 0; i < table->N; i++)
        {
            printf("|%d", i + 1);
        }
        printf("|\n\n");
        printf(COLOR_RESET);
    }
    else
    {
        for (i = 0; i < table->N; i++)
        {
            printf(COLOR_WHITE);
            // printf("%d", (table->N - i) / 10);
            // printf("%d|", (table->N - i) % 10);
            printf("%2.d|", i + 1);
            // printf("%d|", i+1 % 10);
            printf(COLOR_RESET);
            for (j = 0; j < table->N; j++)
            {
                switch (table->data[i][j])
                {
                case 0:
                    // printf("%d%d",i,j);
                    printf(COLOR_RED "A " COLOR_RESET);
                    break;
                case 1:
                    printf(COLOR_GREEN "B " COLOR_RESET);
                    break;
                case 2:
                    printf(COLOR_YELLOW "C " COLOR_RESET);
                    break;
                case 3:
                    printf(COLOR_BLUE "D " COLOR_RESET);
                    break;
                case 4:
                    printf(COLOR_PURPLE "E " COLOR_RESET);
                    break;
                case -1:
                    printf("_ ");
                    break;

                default:
                    break;
                }
            }
            printf("\n");
        }
        printf(COLOR_WHITE);
        printf("  "); // Hizalamak için 2 bosluk
        for (i = 0; i < table->N; i++)
        { // İlk Basamak
            if (((i + 1) / 10) > 0)
            {
                printf("|%d", ((i + 1) / 10));
            }
            else
            {
                printf("|_");
            }
        }
        printf("|\n");
        printf("  "); // Hizalamak için 3 bosluk
        for (i = 0; i < table->N; i++)
        { // İkinci Basamak
            printf("|%d", ((i + 1) % 10));
        }
        printf("|\n\n");
        printf(COLOR_RESET);
    }
}

int isMoveable(Table* t, int x, int y)// Secilen nokta hareket ettirilebilir mi
{

    int i, j, N;
    N = t->N;
    i = y;
    j = x;

    if (t->data[i][j] != -1)
    {
        // Dikey i
        // Yatay j
        if ((i >= 2) && (t->data[i - 1][j] != -1) && (t->data[i - 2][j] == -1)) { return 1; }         // Yukarı
        if ((j >= 2) && (t->data[i][j - 1] != -1) && (t->data[i][j - 2] == -1)) { return 1; }         //Sol
        if ((N - i - 1 >= 2) && (t->data[i + 1][j] != -1) && (t->data[i + 2][j] == -1)) { return 1; } // Asagı
        if ((N - j - 1 >= 2) && (t->data[i][j + 1] != -1) && (t->data[i][j + 2] == -1)) { return 1; } // Sag
    }

    return 0;
}

int isMoveOK(Table* t, int x, int y, int direction) // Hareket yönü uygun mu
{

    int i, j, N;
    N = t->N;
    i = y;
    j = x;

    switch (direction)
    {
    case 1:if ((i >= 2) && (t->data[i - 1][j] != -1) && (t->data[i - 2][j] == -1)) { return 1; }break; // Yukarı
    case 2:if ((j >= 2) && (t->data[i][j - 1] != -1) && (t->data[i][j - 2] == -1)) { return 1; }break; // Sol
    case 3:if ((N - i - 1 >= 2) && (t->data[i + 1][j] != -1) && (t->data[i + 2][j] == -1)) { return 1; }break; // Asagı
    case 4:if ((N - j - 1 >= 2) && (t->data[i][j + 1] != -1) && (t->data[i][j + 2] == -1)) { return 1; }break; // Sag

    default:
        break;
    }

    return 0;
}

void highlightPiece(Table* t, int x, int y)// verilen konumdaki parça beyaza boyanır
{
    int i, j, N;
    N = t->N;
    i = y;
    j = x;
    int ch = t->data[i][j] + 'A';
    if (N < 9)
    {
        x = 2 * x + 4;
        CURSOR_TO(x, y + 1);
        printf(COLOR_WHITE "%c" COLOR_RESET, ch);
    }
    else
    {
        x = 2 * x + 4;
        CURSOR_TO(x, y + 1);
        printf(COLOR_WHITE "%c" COLOR_RESET, ch);
    }
    CURSOR_TO(0, N + 6); // N+ artabilir
}

void highlightDifference(Table* before,Table* after)
{

    int i, j,x,y;
    int N = before->N;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if ((after->data[i][j] ==-1)&& (before->data[i][j]!=-1))
            {
                x = j;
                y = i;
                int ch = before->data[i][j] + 'A';
                if (N < 9)
                {
                    x = 2 * x + 4;
                    CURSOR_TO(x, y + 1);
                    printf(COLOR_WHITE "%c" COLOR_RESET, ch);
                }
                else
                {
                    x = 2 * x + 4;
                    CURSOR_TO(x, y + 1);
                    printf(COLOR_WHITE "%c" COLOR_RESET, ch);
                }
                CURSOR_TO(0, N + 6); // N+ artabilir Cursorun gideceği konum

            }
        }
    }

    

}

int doMove(Table* t, int* x, int* y, int direction)//Yon ve konum ver hamleyi yapsın
{
    int i, j, eaten;

    i = *y;
    j = *x;

    switch (direction)
    {
    case 1: // Yukarı
        t->data[i - 2][j] = t->data[i][j]; // Önce Taşı hareket ettiriyoruz
        t->data[i][j] = (signed char)-1;  //Hareket ettirdigimiz yeri boşaltıyoruz
        *y = *y - 2;

        eaten = t->data[i - 1][j];
        t->data[i - 1][j] = (signed char)-1; // Yedigimiz taşı siliyoruz
        break;
    case 3: // Asagı
        t->data[i + 2][j] = t->data[i][j];
        t->data[i][j] = (signed char)-1;
        *y = *y + 2;

        eaten = t->data[i + 1][j];
        t->data[i + 1][j] = (signed char)-1;
        break;
    case 2: // Sol
        t->data[i][j - 2] = t->data[i][j];
        t->data[i][j] = (signed char)-1;
        *x = *x - 2;

        eaten = t->data[i][j - 1];
        t->data[i][j - 1] = (signed char)-1;
        break;
    case 4: // Sag
        t->data[i][j + 2] = t->data[i][j];
        t->data[i][j] = (signed char)-1;
        *x = *x + 2;

        eaten = t->data[i][j + 1];
        t->data[i][j + 1] = (signed char)-1;
        break;

    default:
        break;
    }

    //Skorları olması gereken kişiye ekliyoruz
    if (t->turn % 2 == 0)
    {
        t->p[0].piece[eaten]++;
    }
    else
    {
        t->p[1].piece[eaten]++;
    }

    return eaten;
}

void undoMove(Table* t, int* x, int* y, int direction, int eaten)//Konum yön ve yedigin taşı ver, Tahtayı eski haline getirsin
{
    int i, j;

    i = *y;
    j = *x;

    switch (direction)
    {
    case 3: // Yukarı aslı 1
        t->data[i - 2][j] = t->data[i][j];
        t->data[i][j] = (signed char)-1;
        *y = *y - 2;
        t->data[i - 1][j] = (signed char)eaten;
        break;
    case 1: // Asagı aslı 3
        t->data[i + 2][j] = t->data[i][j];
        t->data[i][j] = (signed char)-1;
        *y = *y + 2;

        t->data[i + 1][j] = (signed char)eaten;
        break;

    case 4: // Sol aslı 2
        t->data[i][j - 2] = t->data[i][j];
        t->data[i][j] = (signed char)-1;
        *x = *x - 2;

        t->data[i][j - 1] = (signed char)eaten;
        break;
    case 2: // Sag aslı 4
        t->data[i][j + 2] = t->data[i][j];
        t->data[i][j] = (signed char)-1;
        *x = *x + 2;

        t->data[i][j + 1] = (signed char)eaten;
        break;

    default:
        break;
    }
}

int convertToDirection(int a) // verilen char ı 1,2,3,4 gibi sayiya çevirir
{

    if (a >= 'a')
    {
        a -= 'a' - 'A';
    }
    switch (a) {
    case 'W':a = 1;break;
    case 'A':a = 2;break;
    case 'S':a = 3;break;
    case 'D':a = 4;break;
    default:break;
    }

    return a;
}

int isGameOver(Table* t)
{
    //Oyun bitti mi? Bittiyse 1, bitmediyse 0

    int i, j;
    int N = t->N;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (t->data[i][j] != -1)
            {
                // yatay
                if ((i >= 2) && (t->data[i - 1][j] != -1) && (t->data[i - 2][j] == -1)) { return 0; }          // Yukarı
                if ((N - i - 1 >= 2) && (t->data[i + 1][j] != -1) && (t->data[i + 2][j] == -1)) { return 0; }  // Asagı
                // Dikey
                if ((j >= 2) && (t->data[i][j - 1] != -1) && (t->data[i][j - 2] == -1)) { return 0; }          // Sol
                if ((N - j - 1 >= 2) && (t->data[i][j + 1] != -1) && (t->data[i][j + 2] == -1)) { return 0; }  // Sag
            }
        }
    }

    return 1; // Hamle kalmadı
}

void saveTable(const char* filename, Table* t)//Dosyaya yaz
{
    int i, j;
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        perror("Can t open the file");
        return;
    }

    // Temel alanları kaydet
    fprintf(file, " %d %hu %hhu %hhu ", t->pcScore, t->turn, t->gameType, t->N); // pcScore  , turn ,type ,N 0 1 2 3 4, 0 1 2 3 4

    for (i = 0; i < 2; i++)
    {
        fprintf(file, "\n");
        fprintf(file, " %hu", t->p[i].score); // Player Kaydet
        for (j = 0; j < 5; j++)
        {
            fprintf(file, " %hhu", t->p[i].piece[j]); // Player Kaydet
        }
    }
    for (i = 0; i < t->N; i++)
    {
        fprintf(file, "\n");
        for (j = 0; j < t->N; j++)
        {
            fprintf(file, " %hhu", t->data[i][j]); // oyun datası
        }
    }

    fclose(file);
}

Table* loadTable(const char* filename)//Dosyadan oku
{
    int i, j;

    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        perror("Can't open the file");
        return NULL;
    }

    Table* t = malloc(sizeof(Table));
    if (!t)
    {
        perror("Memory error");
        fclose(file);
        return NULL;
    }

    // Temel alanları oku
    fscanf_s(file, " %d %hu %hhu %hhu ", &(t->pcScore), &(t->turn), &(t->gameType), &(t->N)); // pcScore , N , gametype , turn ,int, ushort, uchar uchar

    for (i = 0; i < 2; i++)
    {
        fscanf_s(file, "\n");
        fscanf_s(file, " %hu", &(t->p[i].score)); // Player Score
        for (j = 0; j < 5; j++)
        {
            fscanf_s(file, " %hhu", &(t->p[i].piece[j])); // Player Kaydet
        }
    }

    // datayı aç
    t->data = malloc(sizeof(signed char*) * t->N);
    for (i = 0; i < t->N; i++)
    {
        t->data[i] = malloc(sizeof(signed char) * t->N);

        fscanf_s(file, "\n");
        for (j = 0; j < t->N; j++)
        {
            fscanf_s(file, " %hhu", &(t->data[i][j]));
        }
    }

    fclose(file);
    return t;
}

void calculateScore(Table* t)//Bilgisayarın gözünden scoreları hesapla
{

    int i, j;
    int set, skipper;
    int score;
    int flag;
    for (j = 0; j < 2; j++)
    {
        score = 0;
        set = INT_MAX;

        for (i = 0; i < 5; i++) // Minimun alırsan seti hesaplamış olursun
        {
            set = (t->p[j].piece[i] < set) ? t->p[j].piece[i] : set; // Kaç set oldugunu hesaplıyor
        }

        score = set * 40;

        flag = 1;
        while (flag==1)
        {
            flag=0;
            skipper = 0;
            for ( i = 0; i < 5; i++)
            {
                if (t->p[j].piece[i] - set >0)
                {
                    flag = 1;
                    skipper++;
                }
            }
            
            switch (skipper){
            case 1:score += 1;break;  // Tek taş varsa
            case 2:score += 4;break;  // Iki taş varsa
            case 3:score += 9;break;  // Uc taş varsa
            case 4:score += 19;break; // Dort taş varsa
            default:break;}
            set++;
        
        }
        

        t->p[j].score = score;
    }
    t->pcScore = t->p[1].score - t->p[0].score;
}

void calculateScoreReal(Table* t) //Gerçek skoru hesaplar
{

    int i, j;
    int set, skipper;
    int score;

    for (j = 0; j < 2; j++)
    {
        score = 0;
        set = INT_MAX;

        for (i = 0; i < 5; i++) // Minimun alırsan set sayisini hesaplamış olursun
        {
            set = (t->p[j].piece[i] < set) ? t->p[j].piece[i] : set; // Kaç set oldugunu hesaplıyor
        }

        score = set * 1000;

        for (i = 0; i < 5; i++) // Kalan Skipperları ekle
        {
            skipper = t->p[j].piece[i] - set;
            score += skipper;
        }

        t->p[j].score = score;
    }
    t->pcScore = t->p[1].score - t->p[0].score;
}

void nextPositionsList(Table* incominTable, stackNode** head, int pos_x, int pos_y, int end_x, int end_y)
{

    int i, j, k;
    int x, y; // Move fonksiyonunda yeri kaybetme ve tekrar çagırabilme özgürlügüne sahip ol

    int N;
    N = incominTable->N;

    Table* nextStepTable;
    Table* trash;

    for (i = pos_y; i < end_y + 1; i++)
    {
        for (j = pos_x; j < end_x + 1; j++)
        {
            if (isMoveable(incominTable, j, i))
            {
                for (k = 1; k <= 4; k++) // K birden başlıyor çünkü yönleri bu belirliyor
                {
                    if (isMoveOK(incominTable, j, i, k))
                    {
                        x = j;
                        y = i;
                        nextStepTable = copyTable(incominTable);
                        doMove(nextStepTable, &x, &y, k);

                        calculateScore(nextStepTable);
                        *head = push(*head, nextStepTable);

                        // printTable(((*head)->data)); //Tüm tabloları görebilirim
                        // system("Pause");

                        if (isMoveable(nextStepTable, x, y))
                        {
                            nextPositionsList(nextStepTable, head, x, y, x, y);
                        }
                    }
                }

            }
        }
    }

}

int minimax(Table* position, int depht, int maximizingPlayer, int alpha, int beta) //minimax algoritması , her tablonun değerini hesaplar .
{

    if ((depht == MAX_DEPTH) || isGameOver(position))
    {
        calculateScore(position);
        return position->pcScore;
    }

    //Olası hamleleri bulma
    stackNode* listHead = NULL;
    // position->turn = depht + 1;
    nextPositionsList(position, &listHead, 0, 0, position->N - 1, position->N - 1); 

    //Stack i gezmek icin
    stackNode* tempHead = listHead;


    if (listHead == NULL)
    {
        printf("Burada olmamalisin - Hata var");
        exit(1);
    }
    

    int resultScore;
    int thisNode;
    int maxScore = INT_MIN; 
    int minScore = INT_MAX; 

    //maximinizPlayer == 1 se gez ve aralarindan max olani sec.
    //maximinizPlayer == 0 se gez ve aralarindan min olani sec.
    

    if (maximizingPlayer == 1)
    {

        while ((tempHead != NULL)&&(beta>alpha))
        {
        	
            thisNode = minimax(tempHead->data, depht + 1, 0, alpha, beta);

            if (maxScore < thisNode)
            {
                maxScore = thisNode;
            }
            alpha = alpha < maxScore ? maxScore : alpha; // max(alpha,maxscore)
            
            tempHead = tempHead->next;
        }
        
        resultScore = maxScore;
    }
    else
    {
        while ((tempHead != NULL)&&(beta>alpha))
        {
            thisNode = minimax(tempHead->data, depht + 1, 1, alpha, beta);

            if (minScore > thisNode)
            {
                minScore = thisNode;
            }
            beta = beta > minScore ? minScore : beta; // min(beta,pcScore)
            
            tempHead = tempHead->next;

        }
        resultScore = minScore;
    }

    freeStack(listHead); //Kullanilan tablololari sil

    return resultScore;
    
}

Table* findBestMove(Table* position,int alpha,int beta) { // 'Oyun bittimi' kontrolu yapip cagir

    if(isGameOver(position)){
        return position;
    }
    int maxPoint = INT_MIN;
    int minPoint = INT_MAX;

    Table* lastTable;

    stackNode* listHead = NULL;
    nextPositionsList(position, &listHead, 0, 0, position->N - 1, position->N - 1);
    
    stackNode* tempHead = listHead;//listenin basini kaybetmemek icin

    Table* bestOne  = listHead->data;
    while (tempHead!=NULL)
    {
        tempHead->data->pcScore = minimax(tempHead->data,1,0,alpha,beta);
        if (bestOne->pcScore < tempHead->data->pcScore)                    //Daha iyi hamle geldiyse elimdekini degistir
        {
            bestOne = tempHead->data; // Deep copy degil
        }
        tempHead = tempHead->next;
    }
    


    lastTable = copyTable(bestOne); // en iyi tabloyu deepcopy yapıyoruz tablomuz freelenmesin
    freeStack(listHead);    //Kullanılan tablolar free leniyor
    
    return lastTable;

}

int main()
{

    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);   //Memory Leak bulmak icin
    // _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    printf("\e[?25l"); // Hide cursor
    // printf("\e[?25h"); // Enable cursor

    int i, j, N, start, eaten, undo;
    int x, y;
    char move;
    int Program = 1;
    Table* gameTable; // Turn = 0 ilk oyuncu ,turn = 1 ikinci oyuncu
    Table* oldTable;;
    Table* loadT;
    int alpha, beta;
    gameTable = malloc(sizeof(Table));


    printf("\n ______     __  __     __     ______   ______   __     ______   __  __   \n");
    printf("/\\  ___\\   /\\ \\/ /    /\\ \\   /\\  == \\ /\\  == \\ /\\ \\   /\\__  _\\ /\\ \\_\\ \\  \n");
    printf("\\ \\___  \\  \\ \\  _\"-.  \\ \\ \\  \\ \\  _-/ \\ \\  _-/ \\ \\ \\  \\/_/\\ \\/ \\ \\____ \\ \n");
    printf(" \\/\\_____\\  \\ \\_\\ \\_\\  \\ \\_\\  \\ \\_\\    \\ \\_\\    \\ \\_\\    \\ \\_\\  \\/\\_____\n");
    printf("  \\/_____/   \\/_/\\/_/   \\/_/   \\/_/     \\/_/     \\/_/     \\/_/   \\/_____/\n");
    printf("\n__________________________________________________________________________\n");

    printf("Welcome To Skippity Game \nby Habil\n\n");
    printf("Rules:\n");
    printf("1-There are 2 modes for game (2 Player or 1 Player).\n");
    printf("2-If it's your turn, choose a starting point x,y. (Example: 5 8)\n");
    printf("3-After you choose a starting point, you can move with W A S D.\n");
    printf("4-You are allowed to use a skipper to make multiple jumps.\n");
    printf("5-You can undo your last move by enterin 0.\n");
    printf("6-You can pass your turn by entering 1.\n");
    printf("7-The game continues until there are no moves left.\n");

    printf("--------------------\n");
    printf("Scoring:\n");
    printf("1-The player who has the most complete sets of skippers wins the game\n");
    printf("2-A complete set includes one of each skippers.\n");
    printf("3-If there is a tie, the player who has the most total skippers wins.\n");
    printf("4-If there is still a tie at this point, the game ends in a tie.\n");

    while (Program == 1)
    {
        /* code */
        alpha = INT_MIN;
        beta = INT_MAX;

        gameTable->turn = 0;

        gameTable->p[0].score = 0;
        gameTable->p[1].score = 0;

        for (j = 0; j < 2; j++)
        {
            for (i = 0; i < 5; i++)
            {
                gameTable->p[j].piece[i] = 0;
            }
        }

        time_t t;
        srand((unsigned)time(&t));

        do
        {
            printf("--------------------\n");
            printf("1-New Game\n");
            printf("2-Load Game\n");
            printf("Choose:");
            scanf_s("%d", &start);
        } while (start != 1 && start != 2);

        if (start == 1)
        {
            printf("--------------------\n");
            printf("1-Two player\n2-One Player(Against PC)\nChoose game type : ");
            scanf_s(" %hhu", &gameTable->gameType);
            do
            { // N is even
                printf("Enter size of game table(N must be even 6-20) : ");
                scanf_s(" %d", &N);
            } while ((N % 2 != 0) && (N < 4) && (N > 20));
            gameTable->N = (unsigned char)N;

            createTable(gameTable);
            gameTable->pcScore = 0;
            saveTable("data.txt", gameTable);
        }
        else
        {
            // Programı yükle game type table vs turn
            loadT = loadTable("data.txt");
            gameTable = copyTable(loadT); // Leak olabilir
            N = gameTable->N;

            printf("--------------------\n");
            printf("Loading Game ...\n");
            printf("Game Type : %d\n", gameTable->gameType);
            printf("It is %d. Players turn\n\n", (gameTable->turn) % 2 + 1);
            printf("Player 1:%d\n", gameTable->p[0].score);
            printf("Player 2:%d\n", gameTable->p[1].score);

            system("Pause");
        }

        // Oyun baslasdı

        while ((isGameOver(gameTable) == 0) && (Program == 1))
        {

            // Her hamlede kaydet
            saveTable("data.txt", gameTable);

            if (gameTable->gameType == 1)
            {
                N = gameTable->N;
                printTable(gameTable);
                printPlayer(gameTable);

                do
                {
                    while ((x = getchar()) != '\n' && x != EOF){}//Scanf sıkıntı çıkarmasın diye buffer temizleme

                    printf("\nPlayer-%d is Playing\n", (gameTable->turn % 2) + 1); 
                    printf("Choose a piece (x,y)(0 0 to exit):");
                    scanf_s(" %d %d",&x,&y);
                    if (x == 0)
                    {
                        freeTable(gameTable);
                        return 0;
                    }
                    x = x - 1; // Tabloda indeksler 0 dan basladıgı icin koordinatları birer azalt
                    y = y - 1;

                } while ((x >= N || y >= N || x < 0 || y < 0) || (isMoveable(gameTable, x, y) != 1));


                undo = -1; // Undo hakkı yok
                move = 0;
                while (isMoveable(gameTable, x, y) == 1 && move != -1)
                {
                    highlightPiece(gameTable, x, y);

                    do
                    {
                        
                        printf("Player%d Chose Direction \n(W,A,S,D)(0-undo, 1-apply):                \n", gameTable->turn % 2 + 1);// Goruntu duzgun oması ıcın bosluk var
                        scanf_s(" %c", &move);
                        move = convertToDirection(move);
                        
                        if (move == '0' && undo != -1)
                        {
                            undoMove(gameTable, &x, &y, undo, eaten);

                            gameTable->p[(gameTable->turn % 2)].piece[eaten]--; //Undo yaptıysa aldıgı taşı geri ver, skoru azalt
                            undo = -1;
                            printTable(gameTable);
                            printPlayer(gameTable);
                            highlightPiece(gameTable, x, y);
                        }
                        if (move == 49)
                        {              //-1
                            move = -1; // Oyunu pas geçti belirlemek için
                        }
                        
                    } while (move > 10);
                    if (move != -1)
                    {
                        if (isMoveOK(gameTable, x, y, move))    //Hareket yapılabilir mi?
                        {
                            eaten = doMove(gameTable, &x, &y, move);    //hareket yapılabilir ise yap
                            undo = move;    //Hareket ettigim yönü undo da saklıyorumm. Undo move fonksiyonu çagırabilmek icin
                        }
                        else
                        {
                            printf("\nYou can't go that way\n");
                            system("Pause");
                        }
                    }
                }

                gameTable->turn += 1;
            }

            
            else if (gameTable->gameType == 2)
            {
                
                //Oyun başladı
                N = gameTable->N;
                printTable(gameTable);
                printPlayer(gameTable);
                calculateScoreReal(gameTable);
                if(gameTable->turn % 2==0)
                {
                    do
                    {
                        while ((x = getchar()) != '\n' && x != EOF){}//Scanf sıkıntı çıkarmasın diye buffer temizleme

                        calculateScoreReal(gameTable);
                        printf("Player1 Score : %d\n", gameTable->p[0].score);
                        printf("Player2 Score : %d\n", gameTable->p[1].score);
                        printf("\nPlayer-%d is Playing\n", (gameTable->turn % 2) + 1); // Tek katlar pc , çift katlar oyuncu
                        printf("Choose a piece (x,y)or(0 0 to exit):");
                        scanf_s(" %d %d", &x, &y);
                        
                        if (x == 0)
                        {
                            printf("Game is closing.");
                            freeTable(gameTable);
                            return 0;
                        }
                        x = x - 1; // Tabloda indeksler 0 dan basladıgı icin
                        y = y - 1;

                    } while ((x >= N || y >= N || x < 0 || y < 0) || (isMoveable(gameTable, x, y) != 1));



                    undo = -1; // Undo hakkımız yok
                    move = 0;
                    while (isMoveable(gameTable, x, y) == 1 && move != -1)
                    {
                        highlightPiece(gameTable, x, y);

                        do
                        {
                            
                            printf("\n\nPlayer%d Chose Direction \n(W,A,S,D)(0-undo, 1-apply):                 \n", gameTable->turn % 2 + 1);
                            scanf_s(" %c", &move);
                            move = convertToDirection(move);// W,A,S,D yi sırasıyla 1,2,3,4 e çeviren fonksiyon

                            if (move == '0' && undo != -1) // Geçerli bit Hareket yönü mü?
                            {
                                undoMove(gameTable, &x, &y, undo, eaten);
                                if (gameTable->turn % 2 == 0)
                                {
                                    gameTable->p[0].piece[eaten] -= 1;
                                }
                                else
                                {
                                    gameTable->p[1].piece[eaten] -= 1;
                                }
                                undo = -1;
                                printTable(gameTable);
                                printPlayer(gameTable);
                                highlightPiece(gameTable, x, y);
                            }
                            if (move == 49)// 1 in char olarak degeri
                            {              
                                move = -1; // Oyunu pas geçti belirlemek için move = -1 yaptık 
                            }
                        } while (move > 10);// Geçerli bir girdi olmadıgı sürece yeniden iste
                        if (move != -1)
                        {
                            if (isMoveOK(gameTable, x, y, move))
                            {
                                eaten = doMove(gameTable, &x, &y, move);
                                undo = move;
                            }
                            else
                            {
                                printf("You can't go that way\n");
                                system("Pause");
                            }
                        }
                    }
                    gameTable->turn++; // Sıra bilgisayara geçsin

                
                }
                if(isGameOver(gameTable) == 0){

                    //Hamle sırası bilgisayarda
                
                    printTable(gameTable);
                    printPlayer(gameTable);
                    
                    oldTable = gameTable; // Eski tabloyu kaybetme farkı anlamak icin lazım
                    
                    gameTable = findBestMove(gameTable,alpha, beta); // En iyi hamleyi bul
                    
                    highlightDifference(oldTable,gameTable); // Eski ve yeni tablo arasında  farka bak , Bilgisayarın yaptıgı hamleyi anlamak için

                    freeTable(oldTable); // Eski tabloyla isimiz bitti
                    
                    gameTable->turn++; // Sıra rakibe geçsin
                    
                    system("Pause");    
                }
                
            }
            else
            {
                printf("\nPlease enter again:");
                scanf_s(" %hhu", &gameTable->gameType);
            }
        }

        // Buraya geldiyse oyun bitti
        // Skorları yazdır , tekrar oynamak ister mi sor

        printTable(gameTable);
        printPlayer(gameTable);

        calculateScoreReal(gameTable);
        printf("\n");
        printf("--------------\n");
        printf("Game is over\n");
        printf("Player1 Score :%d\n", gameTable->p[0].score);
        printf("Player2 Score :%d\n\n", gameTable->p[1].score);
        
        // kazanan
        if (gameTable->pcScore > 0){printf("Winner is Player 2\n");}
        else if (gameTable->pcScore < 0){printf("Winner is Player 1\n");}
        else{printf("It's a Tie \n");}

        printf("Do you want to play again ?\n");
        printf("Yes : 1\n");
        printf("No  : 0\n");
        scanf_s(" %d", &Program);
    }

    freeTable(gameTable);

    return 0;
}
