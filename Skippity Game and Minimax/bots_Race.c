#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> //shift+alt+f to format
#include <math.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>


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

#define MAX_DEPTH 4

int searchCount = 0;
// signed char -127, 128 arası tutuyor sorun çıkabilir skorları bunla alma ama küçük rakamlar bunla
typedef struct P
{
    unsigned short score;
    signed char piece[5]; // 0A 1B 2C 3D 4E
} Player;

typedef struct T
{

    int pcScore; // Bot da kullanmak için
    unsigned short turn;
    unsigned char gameType;
    unsigned char N;
    signed char** data;

    Player p[2];
} Table;

typedef struct node
{
    Table* data;
    struct node* next;
} stackNode;

stackNode* pop(stackNode* head);
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
Table* findBestMove2(Table* position, int alpha, int beta);

void saveTable(const char* filename, Table* t);
Table* loadTable(const char* filename);

void calculateScore2(Table* t);
void nextPositionsList(Table* a, stackNode** b, int pos_x, int pos_y, int end_x, int end_y);
int minimax2(Table* position, int depht, int maximizingPlayer, int alpha, int beta);

stackNode* push(stackNode* head, Table* table)
{ // pushalyınca yeni head döndürülüyor

    stackNode* newNode = malloc(sizeof(stackNode)); //16 byte memory leak
    if (newNode == NULL)
    {
        printf("Stack Overflow\n");
        exit(1);
    }

    // copyTable(table,newNode->data);
    newNode->data = table;
    newNode->next = head;
    return newNode;
}

stackNode* pop(stackNode* head)
{ // yeni head i döndürür

    if (head == NULL)
    {
        printf("Stack is empty\n");
        return NULL;
    }

    stackNode* newNode = head;
    freeTable(newNode->data);
    free(newNode);

    head = head->next;
    return head;
}

void freeStack(stackNode* top) {
    stackNode* temp;
    while (top->next != NULL) {

        temp = top;
        top = top->next;
        // Free the Table contained in the current node
        if (temp->data != NULL) {
            freeTable(temp->data);
            temp->data = NULL;
        }
        free(temp);
        temp == NULL;
    }
    freeTable(top->data);
    free(top);
}

void createTable(Table* gameTable)
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

void freeTable(Table* table) {
    int i;
    if (table != NULL) {
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

Table* copyTable(Table* src)
{
    if (src == NULL) return NULL;

    Table* newTable = malloc(sizeof(Table));

    int i, j, N;
    N = src->N;

    newTable->N = src->N;
    newTable->turn = src->turn;
    newTable->pcScore = src->pcScore;
    newTable->gameType = src->gameType;
    for (i = 0; i < 2; i++)
    {
        newTable->p[i].score = src->p[i].score;
        for (j = 0; j < 5; j++)
        {
            newTable->p[i].piece[j] = src->p[i].piece[j];
        }
    }
    newTable->data = malloc(sizeof(signed char*) * N);
    for (i = 0; i < N; i++)
    {
        newTable->data[i] = malloc(sizeof(signed char) * N); //32 byte leak
        for (j = 0; j < N; j++)
        {
            newTable->data[i][j] = src->data[i][j];
        }
    }
    return newTable;
}

void printPlayer(Table* gt)
{

    int i, j;
    calculateScore2(gt);

    for (j = 0; j < 2; j++)
    {
        if (j == gt->turn % 2)
        {
            printf(COLOR_WHITE "P%d " COLOR_RESET, j + 1);
        } // Sıra kimdeyse onun İsmi beyaz yazıyor
        else
        {
            printf("P%d ", j + 1);
        }

        for (i = 0; i < 5; i++)
        {
            if (gt->p[j].piece[i] == 0)
            {
                printf("\033[1;%dm_ ", 31 + i);
            }
            else
            {
                printf("\033[1;%dm%hhu ", 31 + i, gt->p[j].piece[i]);//Signed char %hhu
            }
        }

        if ((gt->pcScore < 0) && (j == 0)) // Önde olan kişiyi gösteriyor
        {
            printf(COLOR_WHITE "Lead " COLOR_RESET);
        }
        if ((gt->pcScore > 0) && (j == 1))
        {
            printf(COLOR_WHITE "Lead " COLOR_RESET);
        }

        printf("\n");
        printf(COLOR_RESET);
    }
}

void printTable(Table* table)
{

    system("cls"); /// DEGERLER TABLODA YANlış görüğbntoğiyrj
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

int isMoveable(Table* t, int x, int y)
{
    // Secilen nokta hareket ettirilebilir mi

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

int isMoveOK(Table* t, int x, int y, int direction) // Hareket yönünde uygun mu
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

void highlightPiece(Table* t, int x, int y)
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
    CURSOR_TO(0, N + 8); // N+ artabilir
}

int doMove(Table* t, int* x, int* y, int direction)
{
    int i, j, eaten;

    i = *y;
    j = *x;

    switch (direction)
    {
    case 1: // Yukarı
        t->data[i - 2][j] = t->data[i][j]; // Önce Taşı hareket ettiriyoruz
        t->data[i][j] = (signed char)-1;  //Aldıgmız yeri boşaltıyoruz
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

    if (t->turn % 2 == 0)
    {
        t->p[0].piece[eaten]++;
    }
    else
    {
        t->p[1].piece[eaten]++;
    }
    calculateScore2(t);

    return eaten;
}

void undoMove(Table* t, int* x, int* y, int direction, int eaten)
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

int convertToDirection(int a)
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
    //Oyun bitti mi

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

void saveTable(const char* filename, Table* t)
{
    int i, j;
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        perror("Can t open the file");
        return;
    }

    // Temel alanları kaydet
    fprintf(file, "%d %hu %hhu %hhu ", t->pcScore, t->turn, t->gameType, t->N); // pcScore  , gametype , N, turn , 0 1 2 3 4, 0 1 2 3 4

    for (i = 0; i < 2; i++)
    {
        // printf("Oyuncu skor:%d \n", t->p[i].score); // Hata vardı
        // system("Pause");
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

Table* loadTable(const char* filename)
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
        fscanf_s(file, " %hu", &(t->p[i].score)); // Player Kaydet
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

void calculateScore2(Table* t)
{

    int i, j;
    int set, skipper;
    int score;
    int flag;
    for (j = 0; j < 2; j++)
    {
        score = 0;
        set = INT_MAX;// sette minlemek için max alman lazım

        for (i = 0; i < 5; i++) // Minimun alırsan seti hesaplamış olursun
        {
            set = (t->p[j].piece[i] < set) ? t->p[j].piece[i] : set; // Kaç set oldugunu hesaplıyor
        }

        score = set * 40;

        flag = 1;
        while (flag == 1)
        {
            flag = 0;
            skipper = 0;
            for (i = 0; i < 5; i++)
            {
                if (t->p[j].piece[i] - set > 0)
                {
                    flag = 1;
                    skipper++;
                }
            }

            switch (skipper) {
            case 1:score += 1;break;
            case 2:score += 4;break;
            case 3:score += 9;break;
            case 4:score += 19;break;
            default:break;
            }
            set++;

        }


        t->p[j].score = score;
    }
    t->pcScore = t->p[1].score - t->p[0].score;
}

void calculateScore1(Table* t)
{

    int i, j;
    int set, skipper;
    int score;
    int flag;
    for (j = 0; j < 2; j++)
    {
        score = 0;
        set = INT_MAX;// sette minlemek için max alman lazım

        for (i = 0; i < 5; i++) // Minimun alırsan seti hesaplamış olursun
        {
            set = (t->p[j].piece[i] < set) ? t->p[j].piece[i] : set; // Kaç set oldugunu hesaplıyor
        }

        score = set * 40;

        flag = 1;
        while (flag == 1)
        {
            flag = 0;
            skipper = 0;
            for (i = 0; i < 5; i++)
            {
                if (t->p[j].piece[i] - set > 0)
                {
                    flag = 1;
                    skipper++;
                }
            }

            switch (skipper) {
            case 1:score += 1;break;
            case 2:score += 4;break;
            case 3:score += 9;break;
            case 4:score += 19;break;
            default:break;
            }
            set++;

        }


        t->p[j].score = score;
    }
    t->pcScore = t->p[1].score - t->p[0].score;
}

void calculateScoreReal(Table* t)
{

    int i, j;
    int set, skipper;
    int score;

    for (j = 0; j < 2; j++)
    {
        score = 0;
        set = INT_MAX;// sette minlemek için max alman lazım

        for (i = 0; i < 5; i++) // Minimun alırsan seti hesaplamış olursun
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

    int flag = 0;
    int N;
    N = incominTable->N;

    Table* nextStepTable;

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

                        calculateScore2(nextStepTable);
                        *head = push(*head, nextStepTable);

                        // searchCount++;
                        // printTable(((*head)->data)); //Tüm tabloları görebilirim
                        // system("Pause");

                        if (isMoveable(nextStepTable, x, y))
                        {
                            flag = 1;
                            nextPositionsList(nextStepTable, head, x, y, x, y);
                        }
                    }
                }

            }
        }
    }

}

int minimax2(Table* position, int depht, int maximizingPlayer, int alpha, int beta)
{

    if ((depht == MAX_DEPTH) || isGameOver(position))
    {
        searchCount++;
        calculateScore2(position);
        return (position->pcScore);
    }

    //Olası hamleleri bulma
    stackNode* listHead = NULL;
    // position->turn = depht + 1;
    nextPositionsList(position, &listHead, 0, 0, position->N - 1, position->N - 1);


    //Stack ı gezmek için
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

    //1 se gez ve aralarından max olanı seç.


    if (maximizingPlayer == 1)
    {

        while ((tempHead != NULL) && (beta > alpha))//tempHead->next olabilir
        {
            // tempHead->data->turn ++; //hiç gerek yok salla
            thisNode = minimax2(tempHead->data, depht + 1, 0, alpha, beta);//base al kendini döndürüyor gibi // Base scoreları

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
        while ((tempHead != NULL) && (beta > alpha))
        {
            thisNode = minimax2(tempHead->data, depht + 1, 1, alpha, beta);//base al kendini döndürüyor gibi // Base scoreları

            if (minScore > thisNode)
            {
                minScore = thisNode;
            }
            beta = beta > minScore ? minScore : beta; // max(alpha,pcScore)

            tempHead = tempHead->next;

        }
        resultScore = minScore;
    }

    freeStack(listHead); //Memory leak olabilir

    return resultScore;

}

int minimax1(Table* position, int depht, int maximizingPlayer, int alpha, int beta)
{

    if ((depht == MAX_DEPTH) || isGameOver(position))
    {
        searchCount++;
        calculateScore1(position);
        return -(position->pcScore);// -(position->pcScore)
    }

    //Olası hamleleri bulma
    stackNode* listHead = NULL;
    // position->turn = depht + 1;
    nextPositionsList(position, &listHead, 0, 0, position->N - 1, position->N - 1);

    //Stack ı gezmek için
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

    //1 se gez ve aralarından max olanı seç.


    if (maximizingPlayer == 1)
    {

        while ((tempHead != NULL) && (beta > alpha))//tempHead->next olabilir
        {
            // tempHead->data->turn ++; //hiç gerek yok salla
            thisNode = minimax1(tempHead->data, depht + 1, 0, alpha, beta);//base al kendini döndürüyor gibi // Base scoreları

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
        while ((tempHead != NULL) && (beta > alpha))
        {
            thisNode = minimax1(tempHead->data, depht + 1, 1, alpha, beta);//base al kendini döndürüyor gibi // Base scoreları

            if (minScore > thisNode)
            {
                minScore = thisNode;
            }
            beta = beta > minScore ? minScore : beta; // max(alpha,pcScore)

            tempHead = tempHead->next;

        }
        resultScore = minScore;
    }

    freeStack(listHead); //Memory leak olabilir

    return resultScore;

}

Table* findBestMove2(Table* position, int alpha, int beta) { // Oyun bittimi kontrolu yapıp çagır

    if (isGameOver(position)) {
        return position;
    }
    int maxPoint = INT_MIN;
    int minPoint = INT_MAX;

    Table* lastTable;

    stackNode* listHead = NULL;
    nextPositionsList(position, &listHead, 0, 0, position->N - 1, position->N - 1);

    stackNode* tempHead = listHead;//listenin başını kaybetmemek icin

    Table* bestOne = listHead->data;
    while (tempHead != NULL)
    {
        tempHead->data->pcScore = minimax2(tempHead->data, 1, 0, alpha, beta);
        if (bestOne->pcScore < tempHead->data->pcScore)//en iyi hamle degilse degistir
        {
            bestOne = tempHead->data;
        }
        tempHead = tempHead->next;
    }



    lastTable = copyTable(bestOne);
    freeStack(listHead);
    freeTable(position);

    return lastTable;

}

Table* findBestMove1(Table* position, int alpha, int beta) { // Oyun bittimi kontrolu yapıp çagır

    if (isGameOver(position)) {
        return position;
    }
    int maxPoint = INT_MIN;
    int minPoint = INT_MAX;

    Table* lastTable;

    stackNode* listHead = NULL;
    nextPositionsList(position, &listHead, 0, 0, position->N - 1, position->N - 1);

    stackNode* tempHead = listHead;//listenin başını kaybetmemek icin

    Table* bestOne = listHead->data;
    while (tempHead != NULL)
    {
        tempHead->data->pcScore = minimax1(tempHead->data, 1, 0, alpha, beta); /// Mzximizng player degistir
        if ((bestOne->pcScore) < (tempHead->data->pcScore))//en iyi hamle degilse degistir
        {
            bestOne = tempHead->data;
        }
        tempHead = tempHead->next;
    }



    lastTable = copyTable(bestOne);
    freeStack(listHead);
    freeTable(position);

    return lastTable;

}

int main()
{

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

    int i, j, k, N, start, eaten, undo;
    int x, y;
    char move;
    int Program = 1;
    Table* gameTable; // Turn = 0 ilk oyuncu ,turn = 1 ikinci oyuncu
    Table* oldTable;;
    Table* loadT;
    int alpha, beta;

    gameTable = malloc(sizeof(Table));

    // printf("\n ______     __  __     __     ______   ______   __     ______   __  __   \n");
    // printf("/\\  ___\\   /\\ \\/ /    /\\ \\   /\\  == \\ /\\  == \\ /\\ \\   /\\__  _\\ /\\ \\_\\ \\  \n");
    // printf("\\ \\___  \\  \\ \\  _\"-.  \\ \\ \\  \\ \\  _-/ \\ \\  _-/ \\ \\ \\  \\/_/\\ \\/ \\ \\____ \\ \n");
    // printf(" \\/\\_____\\  \\ \\_\\ \\_\\  \\ \\_\\  \\ \\_\\    \\ \\_\\    \\ \\_\\    \\ \\_\\  \\/\\_____\n");
    // printf("  \\/_____/   \\/_/\\/_/   \\/_/   \\/_/     \\/_/     \\/_/     \\/_/   \\/_____/\n");
    // printf("\n__________________________________________________________________________\n");

    // printf("Welcome To Skippity Game\n\n");
    // printf("Rules:\n");
    // printf("1-There are 2 mode for game (2 Player or 1 Player).\n");
    // printf("2-If it's your turn, choose a starting point x,y. (Example: 5 8)\n");
    // printf("3-After you choose starting point, you can move with W A S D.\n");
    // printf("5-You are allowed to use a skipper to make multiple jumps.\n");
    // printf("6-You can undo your last move by enterin 0.\n");
    // printf("7-You can pass your turn by enterin 1.\n");
    // printf("8-The game continues until there are no moves left.\n");

    // printf("--------------------\n");
    // printf("Scoring:\n");
    // printf("1-The player who has the most complete sets of skippers wins the game\n");
    // printf("1.1-A complete set includes one of each color skipper.\n");
    // printf("2-If there is a tie, the player who has the most total skippers wins.\n");
    // printf("3-If there is still a tie at this point, the game ends in a tie.\n");

    while (Program == 1)
    {
        /* code */
        alpha = INT_MIN;
        beta = INT_MAX;

        int p1, p2, tie;

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
        srand((unsigned)time(&t));  // Tabloyu rastgelee sayılarla dolduruyoruz. Bunun için lazım


        printf("--------------------\n");
        printf("1-1 Game\n2-10 Game\nChoose game type : ");
        scanf_s(" %hhu", &gameTable->gameType);
        do
        { // N is cift
            printf("Enter size of game table(N must be even 6-20) : ");
            scanf_s(" %d", &N);
        } while ((N % 2 != 0) && (N < 4) && (N > 20));

        gameTable->N = (unsigned char)N;
        gameTable->pcScore = 0;

        // Oyun baslasdı


        createTable(gameTable);
        while ((isGameOver(gameTable) == 0) && (Program == 1))
        {
            p1 = 0;
            p2 = 0;
            tie = 0;

            if (gameTable->gameType == 1)
            {
                gameTable->turn = 0;
                while ((isGameOver(gameTable) == 0) && (Program == 1))
                {

                    calculateScore2(gameTable);
                    printTable(gameTable);
                    printPlayer(gameTable);
                    printf("Player 1:%d\n", gameTable->p[0].score);
                    printf("Player 2:%d\n\n", gameTable->p[1].score);

                    system("Pause");

                    gameTable = findBestMove1(gameTable, INT_MIN, INT_MAX);
                    gameTable->turn++;

                    calculateScore2(gameTable);
                    printTable(gameTable);
                    printPlayer(gameTable);
                    printf("Player 1:%d\n", gameTable->p[0].score);
                    printf("Player 2:%d\n\n", gameTable->p[1].score);
                    system("Pause");

                    if (isGameOver(gameTable) == 0)
                    {
                        gameTable = findBestMove2(gameTable, INT_MIN, INT_MAX);
                        gameTable->turn++;
                        // gameTable->turn++;
                    }

                }

                calculateScoreReal(gameTable);
                // if (gameTable->pcScore < 0) { printf("Winner is Player 1\n");p1++; }
                // else if (gameTable->pcScore > 0) { printf("Winner is Player 2\n");p2++; }
                // else { printf("It's a Tie \n");tie++; }
                // printf("p1:%d  p2:%d  tie:%d\n", p1, p2, tie);
                // //system("Pause");



            }


            else if (gameTable->gameType == 2)
            {

                p1 = 0;
                p2 = 0;
                tie = 0;

                for (i = 0; i < 10; i++)
                {
                    // printf("%d",N);
                    freeTable(gameTable);
                    gameTable = malloc(sizeof(Table));
                    gameTable->N = N;
                    createTable(gameTable);
                    //Oyuncuları sıfırlar
                    gameTable->p[0].score = 0;
                    gameTable->p[1].score = 0;

                    for (j = 0; j < 2; j++)
                    {
                        for (k = 0; k < 5; k++)
                        {
                            gameTable->p[j].piece[k] = 0;
                        }
                    }



                    if (i % 2 == 0) {
                        gameTable->turn = 0;

                        while (isGameOver(gameTable) == 0)
                        {

                            calculateScore2(gameTable);
                            printTable(gameTable);
                            printPlayer(gameTable);
                            printf("Player 1:%d\n", gameTable->p[0].score);
                            printf("Player 2:%d\n\n", gameTable->p[1].score);

                            // system("Pause");

                            gameTable = findBestMove1(gameTable, INT_MIN, INT_MAX);
                            gameTable->turn++;

                            // calculateScore2(gameTable);
                            // printTable(gameTable);
                            // printPlayer(gameTable);
                            // printf("Player 1:%d\n", gameTable->p[0].score);
                            // printf("Player 2:%d\n\n", gameTable->p[1].score);
                            // system("Pause");

                            if (isGameOver(gameTable) == 0)
                            {
                                gameTable = findBestMove2(gameTable, INT_MIN, INT_MAX);
                                // gameTable->turn++;
                            }

                        }
                        calculateScoreReal(gameTable);
                        if (gameTable->pcScore < 0) { printf("Winner is Player 1\n");p1++; }
                        else if (gameTable->pcScore > 0) { printf("Winner is Player 2\n");p2++; }
                        else { printf("It's a Tie \n");tie++; }
                        printf("p1:%d  p2:%d  tie:%d\n", p1, p2, tie);
                        // system("Pause");


                    }
                    else {
                        gameTable->turn = 1;

                        while (isGameOver(gameTable) == 0)
                        {

                            calculateScoreReal(gameTable);
                            printTable(gameTable);
                            printPlayer(gameTable);
                            printf("Player 1:%d\n", gameTable->p[0].score);
                            printf("Player 2:%d\n\n", gameTable->p[1].score);

                            gameTable = findBestMove2(gameTable, INT_MIN, INT_MAX);
                            gameTable->turn++;

                            // calculateScore2(gameTable);
                            // printTable(gameTable);
                            // printPlayer(gameTable);
                            // printf("Player 1:%d\n", gameTable->p[0].score);
                            // printf("Player 2:%d\n\n", gameTable->p[1].score);

                            if (isGameOver(gameTable) == 0)
                            {
                                gameTable = findBestMove1(gameTable, INT_MIN, INT_MAX);
                                // gameTable->turn++;
                            }

                        }

                        calculateScoreReal(gameTable);
                        if (gameTable->pcScore < 0) { printf("Winner is Player 1\n");p1++; }
                        else if (gameTable->pcScore > 0) { printf("Winner is Player 2\n");p2++; }
                        else { printf("It's a Tie \n");tie++; }
                        printf("p1:%d  p2:%d  tie:%d\n", p1, p2, tie);
                        // system("Pause");

                    }

                }

                Program = 0;
            }
            else
            {
                printf("\nPlease enter again:");
                scanf_s(" %hhu", &gameTable->gameType);
            }
        }


        system("Pause");
        // Buraya geldiyse oyun bitti
        printTable(gameTable);
        printPlayer(gameTable);


        printf("\n");
        printf("--------------\n");
        printf("Game is over\n");
        printf("Player 1:%d\n", gameTable->p[0].score);
        printf("Player 2:%d\n\n", gameTable->p[1].score);
        printf("Search Count :%d\n\n", searchCount);


        // kazanan
        calculateScoreReal(gameTable);
        if (gameTable->pcScore < 0) { printf("Winner is Player 1\n"); }
        else if (gameTable->pcScore > 0) { printf("Winner is Player 2\n"); }
        else { printf("It's a Tie \n"); }

        printf("Do you want to play again ?\n");
        printf("Yes : 1\n");
        printf("No  : 0\n");
        scanf_s(" %d", &Program);
    }

    freeTable(gameTable);

    _CrtDumpMemoryLeaks();

    return 0;
}