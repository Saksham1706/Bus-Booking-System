#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int passcount;
char x[100];
int busNum;
typedef struct BinarySearchTree BST;
int delete(int lineNumberToRemove);
void busLists();

struct BinarySearchTree
{
  int PassnNo;
  char name[10];
  struct BinarySearchTree *left;
  struct BinarySearchTree *right;
};

BST *root = NULL;// struct la initate kela of 10*32 size rectangular
int busSeat[10][32] = {0};
int busSeat2[10][32]={0};

void upd2arr(){
  FILE*g=fopen("passenger_data.txt","r");
  int id;char name[20];
  while(fscanf(g,"%d %s",&id,name) !=EOF){
    int bus,seat;
    bus=id/1000;
    seat=id%1000;
    busSeat2[bus][seat]=1;
  }
}

// for ui and colouring help of color code 
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
      printf("0%d .", i);//print as 01,02,03
    }
    else
    {
      printf("%d .", i);// print as 10,11 
    }

    FILE*f=fopen(x,"r");
    BST* temp=loadData(f);
    upd2arr();
    {
      if (busSeat2[choice][i] == 0){// checks the seat matrix if seat not booked
        green();
        printf("[EMPTY ]");
        resetColor();
      }
      else if(busSeat2[choice][i]==1){
        redColor();
        printf("[BOOKED]"); // reserved
        resetColor();
      }
    }
    printf("         ");
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
    if ((*r)->PassnNo > custId)// sorting of coustomer id 
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
    savePassengerData(r->left, f);// nav anhi number save kurnr
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

  for(int i=1;i<9;i++){
    for(int j=1;j<33;j++){
        busSeat[i][j]=0;
    }
  }

  while (fscanf(f, "%d %s", &custId, name) != EOF)
  {
    temp = insert(&temp, custId, name);
    int busNumber = custId / 1000;// as bus id is busnumber + 0 seat number it will give bus number
    int seatNumber = custId % 100;//  gives the tenths and unit place of seatnumber as it is seat number indirectly
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

int cost(BST *r,int busNum)
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

BST *reservationInfo(BST *r, int s, int *custIDmatched)// print the ticket , take info from prentnode
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
      printf("\n||              NAME: %10s                               ||", (presentnode->name));
      printf("\n||              CUSTOMER ID: %d                              ||", presentnode->PassnNo);
      printf("\n||              BUS NUMBER: %d                                  ||", (presentnode->PassnNo) / 1000);
      printf("\n||              SEAT NUMBER: %d                                ||", (presentnode->PassnNo) % 100);
      printf("\n||              TICKET COST: Rs.%d                            ||", cost(presentnode,(presentnode->PassnNo) / 1000));
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

void mainupd(int c, char n[]){

    FILE*g=fopen("passenger_data.txt","a");
    fprintf(g,"%d %s\n",c,n);
    fclose(g);
}

void bookTicket(int randomNum)
{
  busLists();
  int CustId, choice, seats;
  char name[10];

busChoice:
  magnetaColor();
  printf("\n\nCHOOSE YOUR BUS  : ");
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
    printf("   ENTER THE SEAT NUMBER: ");
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
    if(busSeat2[choice][seatNumber]==1){
      redColor();
      printf("-----------------------------------------------------------------------------------------\n");
      printf("\n\t\t\tTHIS SEAT IS ALREADY BOOKED!!\n\n");
      printf("-----------------------------------------------------------------------------------------\n");
      resetColor();
      goto busChoice;
    }

    CustId = choice * 1000 + seatNumber; // CustomerId
    busSeat[choice][seatNumber] = 1;
    magnetaColor();
    printf("\n   ENTER THE PERSON FIRST NAME: ");
    resetColor();
    green();
    scanf("%s", name);
    resetColor();
    root = insert(&root, CustId, name);
    redColor();
    printf("\n   YOUR CUSTOMER ID IS : ");
    resetColor();
    green();
    printf("%d", CustId);

    mainupd(CustId,name);

    resetColor();
    printf("\n\n==================================================================================\n\n");
  }
//   printf("\nYOUR RESERVATION NUMBER IS : ");
  
  green();
//   printf("%d\n", randomNum);
  resetColor();
  redColor();
  printf("\nPLEASE NOTE DOWN YOUR CUSTOMER ID FOR CANCELING BOOKED TICKETS!!\n");
  resetColor();
  green();
  printf("\n   PRESS ");
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
  int reservationNo;
  int seatNumber;
  int choice;
  char c;
  int seatCancel;

aa:
  {
    // printf("\nENTER YOUR RESERVATION NUMBER : ");
    // scanf("%d", &reservationNo);
    if (1)
    {
      redColor();
      printf("-----------------------------------------------------------------------------------------");
      printf("\nDo You Really Want to Cancel Booking ?");
      green();
      // printf("%d ", reservationNo);
      resetColor();
      printf("\n\nPRESS 'Y' FOR YES AND 'N' FOR NO : \n");
      printf("-----------------------------------------------------------------------------------------\n");
      resetColor();
      green();
      scanf(" %c", &c);
      resetColor();
      if (c == 'y' || c == 'Y')
      {
        // magnetaColor();
        // printf("   ENTER THE BUS NUMBER: ");
        // resetColor();
        // green();
        // scanf("%d", &choice);
        // resetColor();

        magnetaColor();
        printf("\n HOW MANY SEATS DO YOU WANT TO CANCEL : ");
        resetColor();
        green();
        scanf("%d", &seatCancel);
        resetColor();
        for (int i = 0; i < seatCancel; i++)
        {
          label:
          {
          magnetaColor();
          printf("   \nENTER THE BUS NUMBER: ");
          resetColor();
          green();
          scanf("%d", &choice);
          resetColor();
          magnetaColor();
          printf("   \nENTER THE SEAT NUMBER: ");
          resetColor();
          green();
          scanf("%d", &seatNumber);
          resetColor();

          int flag = delete(choice*1000 + seatNumber);
          if(flag==0){
            redColor();
            printf("-----------------------------------------------------------------------------------------");
            printf("\n\t\t\tEnter valid bus and seat number\n");
            printf("-----------------------------------------------------------------------------------------");
            resetColor();
            goto label;
          }
          // else if(flag==-1){
          //   redColor();
          //   printf("-----------------------------------------------------------------------------------------");
          //   printf("\n\t\t\tNo such seat number booked by you.\n");
          //   printf("-----------------------------------------------------------------------------------------");
          //   resetColor();
          //   c='n';
          //   continue;
          // }
          }
          busSeat[choice][seatNumber] = 0;
          busSeat2[choice][seatNumber]=0;
        }
        redColor();
        printf("-----------------------------------------------------------------------------------------");
        printf("\n\t\t\tYOUR RESERVATION HAS BEEN CANCELED !!\n\n");
        printf("\n\t\t\tPRESS 'ENTER' KEY TO CONTINUE \n");
        printf("-----------------------------------------------------------------------------------------");
        resetColor();
        getch();
        // DisplaySeat(choice);
      }

      else if (c == 'n' || c == 'N')
      {
        redColor();
        printf("-----------------------------------------------------------------------------------------");
        printf("\n\t\t\tYOUR RESERVATION CANCELATION HAS BEEN DENIED\n");
        printf("-----------------------------------------------------------------------------------------");
        resetColor();
      }
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

    FILE *file2 = fopen("passenger_data.txt", "r");
    if (file2 == NULL) {
        printf("Error opening file 2!\n");
        return 1;
    }

    // Create a temporary file to write non-matching lines
    FILE *tempFile = fopen("temp.txt", "w");
    FILE *tempFile2 = fopen("temp2.txt","w");
    if (tempFile == NULL) {
        fclose(file);
        printf("Error creating temporary file!\n");
        return 1;
    }

    if (tempFile2 == NULL) {
        fclose(file);
        printf("Error creating temporary file 2!\n");
        return 1;
    }

    char line[500],line2[500]; // Adjust the size according to your needs

    // Read each line from the original file
    int lineNum = 0,lineNum2=0;int num=0,num2=0;
    while (fgets(line, sizeof(line), file)) {
        lineNum++;
        // Extract the integer from the line
        int number;
        char string[50]; // Adjust the size according to your needs
        if (sscanf(line, "%d %[^\n]", &number, string) != 2) {
            printf("Invalid line format in line %d of \n", lineNum);
            continue; // Skip this line
        }

        // Check if the number matches the given lineNumberToRemove
        if (number != lineNumberToRemove) {
            // Write the line to the temporary file
            fputs(line, tempFile);
        }
        else{
            num++;
        }
    }

    if(num==0){
        return 0;
    }

    while (fgets(line2, sizeof(line2), file2)) {
        lineNum2++;
        // Extract the integer from the line
        int number;
        char string[50]; // Adjust the size according to your needs
        if (sscanf(line2, "%d %[^\n]", &number, string) != 2) {
            printf("Invalid line format in line %d of \n", lineNum2);
            continue; // Skip this line
        }

        // Check if the number matches the given lineNumberToRemove
        if (number != lineNumberToRemove) {
            // Write the line to the temporary file
            fputs(line2, tempFile2);
        }
        else{
            num2++;
        }
    }

    fclose(file);
    fclose(tempFile);
    fclose(file2);
    fclose(tempFile2);

    // Replace the original file with the temporary file
    unlink(x);
    rename("temp.txt", x);
    remove("passenger_data.txt");
    rename("temp2.txt", "passenger_data.txt");

    if(num2==0){
        return 0;
    }

    return 1;
}

void busLists()
{
  redColor();
  printf("-----------------------------------------------------------------------------------------");
  printf("\nBus.No\tName\t\t\tDestinations  \t\tCharges  \tTime\n");
  printf("-----------------------------------------------------------------------------------------");
  resetColor();
  printf("\n1\tChintamni Travels       Nagpur to Yeotmal       Rs.320   \t07:00  AM");
  printf("\n2\tPrabhat Travels     \tJodhpur To Jaipur       Rs.350    \t01:30  PM");
  printf("\n3\tJhakar Travels   \tJodhpur To New Delhi\tRs.750    \t03:50  PM");
  printf("\n4\tGuru Ganesh         \tNagpur To Pune    \tRs.900    \t01:00  AM");
  printf("\n5\tTayal Tours     \tNagpur To Indore \tRs.650    \t12:05  AM");
  printf("\n6\tRajhans     \t\tNagpur to Jodhpur    \tRs.2600    \t09:30  PM");
  printf("\n7\tUncle Travels        \tJodhpur To Indore       Rs.1100   \t11:00  PM");
  printf("\n8\tRoyal Rajasthan       \tJodhpur To Jaisalmer    Rs.520    \t08:15  AM");
  printf("\n9\tRSRTC        \t\tJodhpur To Nagpur       Rs.3200    \t08:00  PM");
  printf("\n");
  green();
  printf("\n   PRESS ");
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
  printf("\n\n   ENTER YOUR BUS NUMBER : ");
  resetColor();
  magnetaColor();
  scanf("%d", &busNum);
  resetColor();
  if (busNum <= 0 || busNum >= 10)
  {
    redColor();
    printf("-----------------------------------------------------------------------------------------");
    printf("\n\t\t\t  PLEASE ENTER CORRECT BUS NUMBER !!\n");
    printf("-----------------------------------------------------------------------------------------");
    resetColor();
    goto busInput;
  }
  printf("\n");
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
  strcat(x,".txt");
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
regstart:
  {
    char u[20], p[20], tu[20], tp[20];
    FILE *f = fopen("file.txt", "a+");
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
}

int main()
{

  redColor();
  printf("\n\n=========================================================================================\n");
  printf("\n\t\t\tWELCOME TO ONLINE BUS RESERVATION");
  printf("\n\n=========================================================================================\n\n");
  resetColor();
mainstart:
  {
    magnetaColor();
    printf("\n\n=========================================================================================\n");
    printf("\n\t\t\t  WHAT YOU WANT TO DO ?\n");
    printf("\n\t\t\t\t1. Login\n");
    printf("\n\t\t\t\t2. Signup\n");
    printf("\n\t\t\t\t3. Exit\n");
    printf("\n\n=========================================================================================\n");
    resetColor();
  }
  int choice1;
  magnetaColor();
  printf("\n\nEnter Your Choice : ");
  scanf("%d", &choice1);
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
    sleep(2);
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
  {

    redColor();
    printf("\n\n=========================================================================================");
    printf("\n\n\t\t\t\t   PLEASE SELECT YOUR OPTION!!");
    printf("\n\n=========================================================================================\n");
    resetColor();
    printf("\n");
    magnetaColor();
    printf("-----------------------------------------------------------------------------------------\n");
    printf(" \t\t\t\t  1. BUS LIST\n");
    printf(" \t\t\t\t  2. STATUS OF SEATS\n");
    printf(" \t\t\t\t  3. BOOK TICKET\n");
    printf(" \t\t\t\t  4. CANCEL BOOKING\n");
    printf(" \t\t\t\t  5. RESERVATION INFO\n");
    printf(" \t\t\t\t  6. LOGOUT\n");
    printf("-----------------------------------------------------------------------------------------\n");
    resetColor();
  }
  magnetaColor();
  printf("\n\n   ENTER YOUR CHOICE: ");
  resetColor();
  green();
  scanf("%d", &choice);
  printf("\n");
  resetColor();
  switch (choice)// applyimng witch cases and each switch case will go to its func and main menu will pop up again 
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
    // saveDataToFile();
    goto menu;
    break;

  case 5:
  {
    loadDataFromFile();
    int s;
    magnetaColor();
    printf("\nENTER YOUR CUSTOMER ID : ");// customer idf is genatead as bus no + 0 + seat number in  2 digit
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
    sleep(2);
    goto mainstart;
  }
  break;

  default:
  {
    redColor();
    printf("-----------------------------------------------------------------------------------------\n");
    printf("\n\n\t\t\tENTER CORRECT CHOICE....\n");
    resetColor();
    green();
    printf("\n   PRESS ");
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
