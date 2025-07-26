#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

int passcount;
char x[100];
int busNum;

typedef struct BinarySearchTree BST;
struct BinarySearchTree
{
    int PassnNo;
    char name[10];
    struct BinarySearchTree *left;
    struct BinarySearchTree *right;
};

BST *root = NULL;
int busSeat[10][33] = {0}; // Fixed array size for indices 1-9 / 1-32

// Function declarations
int delete(int lineNumberToRemove);
void busLists();

void redColor()
{
    printf("\033[1;31m");
}

void magnetaColor()
{
    printf("\033[1;36m");
}

void green(){
    printf("\033[1;32m");
}

void resetColor()
{
    printf("\033[0m");
}

void loadDataFromFile();
void saveDataToFile();
void savePassengerData(BST *r, FILE *f);
BST *loadData(FILE *f);

void DisplaySeat(int choice)
{
    for (int i = 1; i <= 32; i++)
    {
        if (i < 10 && i > 0)
        {
            printf("0%d .", i);
        }
        else
        {
            printf("%d .", i);
        }
        
        if (busSeat[choice][i] == 0){
            green();
            printf("[EMPTY]");
            resetColor();
        }
        else if(busSeat[choice][i]==1){
            redColor();
            printf("[BOOKED]");
            resetColor();
        }
        printf(" ");
        if (i % 4 == 0)
            printf("\n");
    }
}

BST *insert(BST **r, int custId, char name[])
{
    if (*r == NULL)
    {
        *r = (BST *)malloc(sizeof(BST));
        (*r)->PassnNo = custId;
        if (*r == NULL)
        {
            printf("No memory…");
            return NULL;
        }
        else
        {
            (*r)->left = (*r)->right = NULL;
            strcpy((*r)->name, name);
        }
    }
    else
    {
        if ((*r)->PassnNo > custId)
        {
            (*r)->left = insert(&((*r)->left), custId, name);
        }
        else if ((*r)->PassnNo < custId)
        {
            (*r)->right = insert(&((*r)->right), custId, name);
        }
    }
    return *r;
}

void savePassengerData(BST *r, FILE *f)
{
    if (r != NULL)
    {
        savePassengerData(r->left, f);
        fprintf(f, "%d %s\n", r->PassnNo, r->name);
        savePassengerData(r->right, f);
    }
}

void saveDataToFile()
{
    FILE *f = fopen(x, "w");
    if (f != NULL)
    {
        savePassengerData(root, f);
        fclose(f);
    }
}

BST *loadData(FILE *f)
{
    BST *temp = NULL;
    int custId;
    char name[10];
    // Clear all cells in the array - fixed range
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 33; j++){
            busSeat[i][j]=0;
        }
    }
    while (fscanf(f, "%d %s", &custId, name) != EOF)
    {
        temp = insert(&temp, custId, name);
        int busNumber = custId / 1000;
        int seatNumber = custId % 100;
        busSeat[busNumber][seatNumber] = 1;
    }
    return temp;
}

void loadDataFromFile()
{
    FILE *f = fopen(x, "r");
    if (f != NULL)
    {
        root = loadData(f);
        fclose(f);
    }
}

int cost(BST *r)
{
    int cost;
    switch (busNum)
    {
        case 1:
            return 320;
            break;
        case 2:
            return 350;
            break;
        case 3:
            return 750;
            break;
        case 4:
            return 900;
            break;
        case 5:
            return 650;
            break;
        case 6:
            return 2600;
            break;
        case 7:
            return 1100;
            break;
        case 8:
            return 520;
            break;
        case 9:
            return 3200;
            break;
    }
}

BST *reservationInfo(BST *r, int s, int *custIDmatched)
{
    if (r == NULL)
        return NULL;
    BST *presentnode = r;
    while (presentnode)
    {
        if (presentnode->PassnNo == s)
        {
            *custIDmatched = 1;
            redColor();
            printf("\n-----------------------------------------------------------------");
            resetColor();
            green();
            printf("\n|| NAME: %10s ||", (presentnode->name));
            printf("\n|| CUSTOMER ID: %d ||", presentnode->PassnNo);
            printf("\n|| BUS NUMBER: %d ||", (presentnode->PassnNo) / 1000);
            printf("\n|| SEAT NUMBER: %d ||", (presentnode->PassnNo) % 100);
            printf("\n|| TICKET COST: Rs.%d ||", cost(presentnode));
            resetColor();
            redColor();
            printf("\n-----------------------------------------------------------------");
            resetColor();
            getch();
            return r;
        }
        else if (presentnode->PassnNo > s)
            presentnode = presentnode->left;
        else
            presentnode = presentnode->right;
    }
    return NULL;
}

void bookTicket(int randomNum)
{
    busLists();
    int CustId, choice, seats;
    char name[10];
busChoice:
    magnetaColor();
    printf("\n\nCHOOSE YOUR BUS : ");
    resetColor();
    green();
    scanf("%d", &choice);
    resetColor();
    busNum = choice;
    if (choice <= 0 || choice > 9)
    {
        redColor();
        printf("-----------------------------------------------------------------------------------------\n");
        printf("\n\t\t\tENTER VALID BUS NUMBER !! \n");
        printf("-----------------------------------------------------------------------------------------\n");
        resetColor();
        getch();
        busNum = choice;
        goto busChoice;
    }
    printf("\n");
    DisplaySeat(choice);
busSeatChoice:
    magnetaColor();
    printf("\n\nNO. OF SEATS YOU NEED TO BOOK : ");
    resetColor();
    green();
    scanf("%d", &seats);
    resetColor();
    if (seats <= 0)
    {
        redColor();
        printf("-----------------------------------------------------------------------------------------\n");
        printf("\n\t\t\tENTER VALID SEAT NUMBER!!\n");
        printf("-----------------------------------------------------------------------------------------\n");
        resetColor();
        goto busSeatChoice;
    }
    else if (seats > 32)
    {
        redColor();
        printf("-----------------------------------------------------------------------------------------\n");
        printf("\n\t\t\tENTER VALID SEAT NUMBER WE HAVE ONLY 32 SEATS IN A BUS !!\n");
        printf("-----------------------------------------------------------------------------------------\n");
        resetColor();
        goto busSeatChoice;
    }
    int seatNumber;
    for (int i = 1; i <= seats; i++)
    {
        printf("\n\n==================================================================================\n\n");
    seat:
        magnetaColor();
        printf(" ENTER THE SEAT NUMBER: ");
        resetColor();
        green();
        scanf("%d", &seatNumber);
        resetColor();
        if (seatNumber <= 0)
        {
            redColor();
            printf("-----------------------------------------------------------------------------------------\n");
            printf("\n\t\t\tENTER VALID SEAT NUMBER!!\n\n");
            printf("-----------------------------------------------------------------------------------------\n");
            resetColor();
            goto seat;
        }
        else if (seatNumber > 32)
        {
            redColor();
            printf("-----------------------------------------------------------------------------------------\n");
            printf("\n\t\t\tENTER VALID SEAT NUMBER WE HAVE ONLY 32 SEATS IN A BUS !!\n\n");
            printf("-----------------------------------------------------------------------------------------\n");
            resetColor();
            goto seat;
        }
        CustId = choice * 1000 + seatNumber;
        busSeat[choice][seatNumber] = 1;
        magnetaColor();
        printf("\n ENTER THE PERSON FIRST NAME: ");
        resetColor();
        green();
        scanf("%s", name);
        resetColor();
        root = insert(&root, CustId, name);
        redColor();
        printf("\n YOUR CUSTOMER ID IS : ");
        resetColor();
        green();
        printf("%d", CustId);
        resetColor();
        printf("\n\n==================================================================================\n\n");
    }
    printf("\nYOUR RESERVATION NUMBER IS : ");
    green();
    printf("%d\n", randomNum);
    resetColor();
    redColor();
    printf("\nPLEASE NOTE DOWN YOUR RESERVATION NUMBER FOR CANCELING BOOKED TICKETS!!\n");
    resetColor();
    green();
    printf("\n PRESS ");
    resetColor();
    redColor();
    printf("'ENTER'");
    resetColor();
    green();
    printf(" KEY TO CONTINUE\n");
    resetColor();
    getch();
}

void cancel(int randomNum)
{
    int reservationNo = randomNum;
    int seatNumber;
    int choice;
    char c;
    int seatCancel;
aa:
    if (1)
    {
        redColor();
        printf("-----------------------------------------------------------------------------------------");
        printf("\nRESERVATION NUMBER IS IT CORRECT ?");
        green();
        printf("%d ", reservationNo);
        resetColor();
        printf("\n\nPRESS 'Y' FOR YES AND 'N' FOR NO : \n");
        printf("-----------------------------------------------------------------------------------------\n");
        resetColor();
        green();
        scanf(" %c", &c);
        resetColor();
        if (c == 'y' || c == 'Y')
        {
            magnetaColor();
            printf(" ENTER THE BUS NUMBER: ");
            resetColor();
            green();
            scanf("%d", &choice);
            resetColor();
            magnetaColor();
            printf("\n HOW MANY SEATS DO YOU WANT TO CANCEL : ");
            resetColor();
            green();
            scanf("%d", &seatCancel);
            resetColor();
            for (int i = 0; i < seatCancel; i++)
            {
            label:
                magnetaColor();
                printf(" \nENTER THE SEAT NUMBER: ");
                resetColor();
                green();
                scanf("%d", &seatNumber);
                resetColor();
                int flag = delete(choice*1000 + seatNumber);
                if(flag==0){
                    redColor();
                    printf("-----------------------------------------------------------------------------------------");
                    printf("\n\t\t\tEnter valid seat number\n");
                    printf("-----------------------------------------------------------------------------------------");
                    resetColor();
                    goto label;
                }
                busSeat[choice][seatNumber] = 0;
            }
            // Re-sync in-memory BST + seat map with the already-edited file
            loadDataFromFile();
            
            redColor();
            printf("-----------------------------------------------------------------------------------------");
            printf("\n\t\t\tYOUR RESERVATION HAS BEEN CANCELED !!\n\n");
            printf("\n\t\t\tPRESS 'ENTER' KEY TO CONTINUE \n");
            printf("-----------------------------------------------------------------------------------------");
            resetColor();
            getch();
        }
        else if (c == 'n' || c == 'N')
        {
            redColor();
            printf("-----------------------------------------------------------------------------------------");
            printf("\n\t\t\tYOUR RESERVATION CANCELATION HAS BEEN DENIED\n");
            printf("-----------------------------------------------------------------------------------------");
            resetColor();
        }
        else
        {
            redColor();
            printf("-----------------------------------------------------------------------------------------");
            printf("\n\t\t\tNOT FOUND!! ENTER THE CORRECT RESERVATION NUMBER\n");
            printf("-----------------------------------------------------------------------------------------");
            resetColor();
            goto aa;
        }
    }
}

int delete(int lineNumberToRemove) {
    FILE *file = fopen(x, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        fclose(file);
        printf("Error creating temporary file!\n");
        return 1;
    }

    char line[100];
    int lineNum = 0;
    int num = 0;
    while (fgets(line, sizeof(line), file)) {
        lineNum++;
        int number;
        char string[50];
        if (sscanf(line, "%d %[^\n]", &number, string) != 2) {
            printf("Invalid line format in line %d\n", lineNum);
            continue;
        }

        if (number != lineNumberToRemove) {
            fputs(line, tempFile);
        }
        else{
            num++;
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(x);
    rename("temp.txt", x);
    if(num==0){
        return 0;
    }
    return 1;
}

void busLists()
{
    redColor();
    printf("=============================================================================================\n");
    printf("Bus.No\t%-20s\t%-25s\t%-10s\t%-10s\n", "Name", "Destinations", "Charges", "Time");
    printf("=============================================================================================\n");
    resetColor();
    printf("1\t%-20s\t%-25s\t%-10s\t%-10s\n", "Chintamni Travels", "Nagpur to Yeotmal", "Rs.320", "07:00 AM");
    printf("2\t%-20s\t%-25s\t%-10s\t%-10s\n", "Prabhat Travels", "Jodhpur To Jaipur", "Rs.350", "01:30 PM");
    printf("3\t%-20s\t%-25s\t%-10s\t%-10s\n", "Jhakar Travels", "Jodhpur To New Delhi", "Rs.750", "03:50 PM");
    printf("4\t%-20s\t%-25s\t%-10s\t%-10s\n", "Guru Ganesh", "Nagpur To Pune", "Rs.900", "01:00 AM");
    printf("5\t%-20s\t%-25s\t%-10s\t%-10s\n", "Tayal Tours", "Nagpur To Indore", "Rs.650", "12:05 AM");
    printf("6\t%-20s\t%-25s\t%-10s\t%-10s\n", "Rajhans", "Nagpur to Jodhpur", "Rs.2600", "09:30 PM");
    printf("7\t%-20s\t%-25s\t%-10s\t%-10s\n", "Uncle Travels", "Jodhpur To Indore", "Rs.1100", "11:00 PM");
    printf("8\t%-20s\t%-25s\t%-10s\t%-10s\n", "Royal Rajasthan", "Jodhpur To Jaisalmer", "Rs.520", "08:15 AM");
    printf("9\t%-20s\t%-25s\t%-10s\t%-10s\n", "RSRTC", "Jodhpur To Nagpur", "Rs.3200", "08:00 PM");
    redColor();
    printf("=============================================================================================\n");
    resetColor();
    green();
    printf("\n PRESS ");
    resetColor();
    redColor();
    printf("'ENTER'");
    resetColor();
    green();
    printf(" KEY TO CONTINUE\n");
    resetColor();
    getch();
}

void status()
{
    int busNum;
    busLists();
busInput:
    green();
    printf("\n\n ENTER YOUR BUS NUMBER : ");
    resetColor();
    magnetaColor();
    scanf("%d", &busNum);
    resetColor();
    if (busNum <= 0 || busNum >= 10)
    {
        redColor();
        printf("-----------------------------------------------------------------------------------------");
        printf("\n\t\t\t PLEASE ENTER CORRECT BUS NUMBER !!\n");
        printf("-----------------------------------------------------------------------------------------");
        resetColor();
        goto busInput;
    }
    printf("\n");
    loadDataFromFile();
    DisplaySeat(busNum);
    getch();
}

int login(int *passcount)
{
    char u[20], p[20], tu[20], tp[20];
    FILE *f = fopen("file.txt", "r");
    green();
    printf("\nEnter Username : ");
    scanf("%s", u);
    printf("Enter Password : ");
    scanf("%s", p);
    resetColor();
    strcpy(x,u);
    while (fscanf(f, "%s %s", tu, tp) != EOF)
    {
        if (strcmp(u, tu) == 0)
        {
            if (strcmp(p, tp) == 0)
            {
                redColor();
                printf("-----------------------------------------------------------------------------------------\n");
                printf("\t\t\t\tLogged in successfully !\n");
                printf("\t\t\t\tPress any key to continue\n");
                printf("-----------------------------------------------------------------------------------------\n");
                getch();
                resetColor();
                return 1;
            }
            else
            {
                (*passcount)++;
                if ((*passcount) < 3)
                {
                    redColor();
                    printf("\n-----------------------------------------------------------------------------------------\n");
                    printf("\t\t\tIncorrect Password\n");
                    printf("\t\t\tRe-enter the username and password\n");
                    printf("-----------------------------------------------------------------------------------------\n");
                    resetColor();
                    int temp = login(passcount);
                    return temp;
                }
                else
                {
                    redColor();
                    printf("\n-----------------------------------------------------------------------------------------\n");
                    printf("\t\t\tYou have entered the incorrect password 3 times... \n");
                    printf("\t\t\tExiting from the system\n");
                    printf("\n-----------------------------------------------------------------------------------------\n");
                    resetColor();
                    exit(0);
                }
            }
        }
    }
    redColor();
    printf("\n-----------------------------------------------------------------------------------------\n");
    printf("\t\t\tUser not found !\n");
    printf("\t\t\tPress any key to continue\n\n");
    printf("-----------------------------------------------------------------------------------------\n");
    resetColor();
    getch();
    return 0;
    fclose(f);
}

void reg()
{
    char u[20], p[20], tu[20], tp[20];
    FILE *f;
regstart:
    f = fopen("file.txt", "a+");
    green();
    printf("\nEnter Username : ");
    scanf("%s", u);
    printf("Enter Password : ");
    scanf("%s", p);
    resetColor();
    while (fscanf(f, "%s %s", tu, tp) != EOF)
    {
        if (strcmp(u, tu) == 0)
        {
            redColor();
            printf("\n-----------------------------------------------------------------------------------------\n");
            printf("\t\t\tUsername already exist! Try another username\n");
            printf("-----------------------------------------------------------------------------------------\n");
            resetColor();
            fclose(f);
            goto regstart;
        }
    }
    fprintf(f, "%s %s\n", u, p);
    redColor();
    printf("\n-----------------------------------------------------------------------------------------\n");
    printf("\t\t\tAccount created successfully\n");
    printf("\t\t\tPress any key to continue\n\n");
    printf("-----------------------------------------------------------------------------------------\n");
    resetColor();
    getch();
    fclose(f);
}

int main()
{
    redColor();
    printf("\n\n=========================================================================================\n");
    printf("\n\t\t\tWELCOME TO ONLINE BUS RESERVATION");
    printf("\n\n=========================================================================================\n\n");
    resetColor();
mainstart:
    magnetaColor();
    printf("\n\n=========================================================================================\n");
    printf("\n\t\t\t WHAT YOU WANT TO DO ?\n");
    printf("\n\t\t\t\t1. Login\n");
    printf("\n\t\t\t\t2. Signup\n");
    printf("\n\t\t\t\t3. Exit\n");
    printf("\n\n=========================================================================================\n");
    resetColor();

    int choice1;
    redColor();
    scanf("\n%d", &choice1);
    resetColor();
    switch (choice1)
    {
        case 1:
        {
            int loginres = login(&passcount);
            if (loginres == 0)
            {
                goto mainstart;
            }
            else
            {
                loadDataFromFile();
            }
            break;
        }
        case 2:
        {
            reg();
            goto mainstart;
        }
        case 3:
        {
            redColor();
            printf("-----------------------------------------------------------------------------------------\n");
            printf("\t\t\t\tExiting....\n");
            printf("-----------------------------------------------------------------------------------------\n");
            resetColor();
            exit(0);
        }
        default:
        {
            redColor();
            printf("-----------------------------------------------------------------------------------------\n");
            printf("\t\t\tPlease enter the correct option !");
            printf("-----------------------------------------------------------------------------------------\n");
            resetColor();
            getch();
            goto mainstart;
        }
    }

    int choice;
    int randomNum;
    srand(time(NULL));
    randomNum = rand();
menu:
    redColor();
    printf("\n\n=========================================================================================");
    printf("\n\n\t\t\t\t PLEASE SELECT YOUR OPTION!!");
    printf("\n\n=========================================================================================\n");
    resetColor();
    printf("\n");
    magnetaColor();
    printf("-----------------------------------------------------------------------------------------\n");
    printf(" \t\t\t\t 1. BUS LIST\n");
    printf(" \t\t\t\t 2. STATUS OF SEATS\n");
    printf(" \t\t\t\t 3. BOOK TICKET\n");
    printf(" \t\t\t\t 4. CANCEL BOOKING\n");
    printf(" \t\t\t\t 5. RESERVATION INFO\n");
    printf(" \t\t\t\t 6. EXIT\n");
    printf("-----------------------------------------------------------------------------------------\n");
    resetColor();

    magnetaColor();
    printf("\n\n ENTER YOUR CHOICE: ");
    resetColor();
    green();
    scanf("%d", &choice);
    printf("\n");
    resetColor();
    switch (choice)
    {
        case 1:
            busLists();
            goto menu;
            break;
        case 2:
            status();
            goto menu;
            break;
        case 3:
            bookTicket(randomNum);
            saveDataToFile();
            goto menu;
            break;
        case 4:
            cancel(randomNum);
            goto menu;
            break;
        case 5:
        {
            loadDataFromFile();
            int s;
            magnetaColor();
            printf("\nENTER YOUR CUSTOMER ID : ");
            resetColor();
            green();
            scanf("%d", &s);
            resetColor();
            int custIDmatched = 0;
            reservationInfo(root, s, &custIDmatched);
            if (custIDmatched == 0)
            {
                redColor();
                printf("-----------------------------------------------------------------------------------------\n");
                printf("\n\n\t\t\tENTER VALID CUSTOMER ID!!");
                printf("\n\t\t\tPRESS 'ENTER' KEY TO CONTINUE\n ");
                printf("-----------------------------------------------------------------------------------------\n");
                resetColor();
                getch();
            }
            goto menu;
            break;
        }
        case 6:
        {
            redColor();
            printf("\n\n=========================================================================================\n");
            resetColor();
            green();
            printf("\n\n\t\t\tTHANK YOU FOR USING OUR SERVICE....\n\n");
            resetColor();
            redColor();
            printf("\n\n=========================================================================================\n");
            resetColor();
            exit(0);
        }
        break;
        default:
        {
            redColor();
            printf("-----------------------------------------------------------------------------------------\n");
            printf("\n\n\t\t\tENTER CORRECT CHOICE....\n");
            resetColor();
            green();
            printf("\n PRESS ");
            resetColor();
            redColor();
            printf("'ENTER'");
            resetColor();
            green();
            printf(" KEY TO CONTINUE\n");
            resetColor();
            getch();
            goto menu;
        }
        break;
    }
    return 0;
}
