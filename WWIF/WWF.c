#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT "8000"         // Port to bind to
#define BUFSIZE 1024       // Buffer size for reading requests

// Structure to hold arguments for the handler function  (for threading)
typedef struct {
    int clientFd;
    const char *rootAddress;
} clientReqArgs;

//declaring functions here, so functions doesn't have to be in order
void tearDown();
void getLetterDistribution(char *arrayOfLetters, int letterCount[26]);
void addNewNode(char *myarray);
void printLinkedList();
int initialization();
struct wordListNode *getRandomWord();
bool CompareCount(int masterWord[26], int candidateWord[26]);
void addNewgameListNode(char *myarray);
void findWords(struct wordListNode *Mword);
void cleanupWordListNodes();
void cleanupGameListNodes();
void displayWorldHTML(char *buffer);
void acceptInput(char *guess);
bool isDone();
void handle_client_request(void *args);
void server(char *root_dir, const char *port);

// Structs and Globals
struct wordListNode {
    char mystring[30];
    struct wordListNode *next;  //it stores the address of the next struct to link them together like a linkedlist
};
struct wordListNode *first = NULL, *final = NULL;  //head and last of the list

struct gameListNode {
    char mystring[30];
    bool condition;
    struct gameListNode *next;
};
struct gameListNode *root = NULL, *last = NULL;   //head and last of the list

char masterWord[30];    //stores the global master word
int count = 0;          //a count for all the words you can make up with the master word
int numOfWordsLeft = 0;  //a count for all the words left to be found

void tearDown() {
    printf("All Done\n");
}

bool CompareCount(int masterWord[26], int candidateWord[26]) {
    for (int i = 0; i < 26; i++) {
        if (candidateWord[i] > masterWord[i]) {
            return false;   //returning false if even one of the index is 0 then returns false
        }
    }
    return true;
}

void getLetterDistribution(char *arrayOfLetters, int letterCount[26]) {
    for (int i = 0; i < 26; i++) letterCount[i] = 0;
    for (int x = 0; arrayOfLetters[x] != '\0'; x++) {   //looping until \0 because it marks the end of the array
        arrayOfLetters[x] = toupper(arrayOfLetters[x]); //Setting all the value in every index to 0
    }
    for (int i = 0; arrayOfLetters[i] != '\0'; i++) { //looping until \0 because it marks the end of the array
        //Subtracting a character by 'A' to get the correct index you need to increment in the letterCount array.
        int charPosition = arrayOfLetters[i] - 'A';
        letterCount[charPosition]++;   //incrementing a index in letterCount array by 1 so if it's index 0 that means there is 1 'A'.
    }
}

void addNewNode(char *myarray) {
    struct wordListNode *newNode = malloc(sizeof(struct wordListNode));
    strncpy(newNode->mystring, myarray, 30);   //to copy the string from one string to another
    newNode->next = NULL;
    if (first == NULL) {  //if the list in empty
        first = final = newNode;
    } else {              //if the list is not empty
        final->next = newNode;
        final = newNode;
    }
}

int initialization() {
    FILE *f = fopen("2of12.txt", "r"); //making a type file pointer and opening the file
    //checking if the file opened successfully
    if (f == NULL) {
        printf("No file found.\n");
        return 1;
    }
    char myArray[30];
    while (fscanf(f, "%s", myArray) != EOF) {  //stops when its end of file
        addNewNode(myArray);
        count++;
    }
    fclose(f);
    return count;
}

struct wordListNode *getRandomWord() {
    srand(time(NULL));
    int r = rand() % count;  //Doing Mod count to get a number between 0 - count
    struct wordListNode *temp = first;
    for (int i = 0; i < r; i++) {
        temp = temp->next;   //going upto the random number in the linked list
    }
    // strncpy(masterWord, temp->mystring, 30);
    // return temp;
    while (temp != NULL) {
        if (strlen(temp->mystring) > 6) {		//checking if that random master word's char length is greater than 6
            // printf("Master Word is: %s\n", temp->mystring);
            strcpy(masterWord, temp->mystring);
            return temp;
            // printf("The master word is: %s\n", temp->mystring);
        }
        temp = temp->next;
    }

    // if (strlen(temp->mystring) <= 6) {
    //     printf("Reached the end of the list without finding a master word.");
    // }

    return temp;
}

void addNewgameListNode(char *myarray) {
    struct gameListNode *newNode = malloc(sizeof(struct gameListNode));
    strncpy(newNode->mystring, myarray, 30);  //to copy the string from one string to another
    newNode->condition = false;
    newNode->next = NULL;
    if (root == NULL) {
        root = last = newNode;
    } else {
        last->next = newNode;
        last = newNode;
    }
}

void findWords(struct wordListNode *MasWord) {   //Accepting the master word as parameter
    int masterWord[26];     //int array for the master word
    getLetterDistribution(MasWord->mystring, masterWord);
    struct wordListNode *temp = first;
    while (temp != NULL) {
        int candidateDist[26];
        getLetterDistribution(temp->mystring, candidateDist);
        if (CompareCount(masterWord, candidateDist)) {   //if it's true then adding that word to the gamelist
            addNewgameListNode(temp->mystring);
            numOfWordsLeft++;
        }
        temp = temp->next;
    }
}

void cleanupWordListNodes() {
    struct wordListNode *current = first, *temp;
    while (current) {
        temp = current;
        current = current->next;
        free(temp);     //freeing the allocated space
    }
    first = final = NULL;
}

void cleanupGameListNodes() {
    struct gameListNode *current = root, *temp;
    while (current) {
        temp = current;
        current = current->next;
        free(temp);     //freeing the allocated space
    }
    root = last = NULL;
}

// void displayWorldHTML(char *buffer) {
//     struct gameListNode *temp = root;
//     char tempBuffer[256];
//     snprintf(buffer, BUFSIZE * 10, "<html><body><h1>Words Without Friends</h1>");
//     strcat(buffer, "<p>Master Word: ");
//     strcat(buffer, masterWord);
//     strcat(buffer, "</p><hr>");
//     while (temp) {
//         if (temp->condition) {
//             snprintf(tempBuffer, sizeof(tempBuffer), "<p>Found: %s</p>", temp->mystring);
//         } else {
//             snprintf(tempBuffer, sizeof(tempBuffer), "<p>");
//             for (int i = 0; i < strlen(temp->mystring); i++) strcat(tempBuffer, "- ");
//             strcat(tempBuffer, "</p>");
//         }
//         strcat(buffer, tempBuffer);
//         temp = temp->next;
//     }
//     strcat(buffer, "<form action=\"/words\" method=\"GET\">");
//     strcat(buffer, "<input type=\"text\" name=\"move\" autofocus></input>");
//     strcat(buffer, "<button type=\"submit\">Submit</button></form></body></html>");
// }

void displayWorldHTML(char *buffer) {
    struct gameListNode *temp = root;
    char tempBuffer[256];
    char formattedMasterWord[256];

    // Copy and format the masterWord
    strcpy(formattedMasterWord, masterWord);

    // Sorting the masterWord alphabetically
    for (int i = 0; i < strlen(formattedMasterWord) - 1; i++) {
        for (int j = i + 1; j < strlen(formattedMasterWord); j++) {
            if (formattedMasterWord[i] > formattedMasterWord[j]) {
                char tempChar = formattedMasterWord[i];
                formattedMasterWord[i] = formattedMasterWord[j];
                formattedMasterWord[j] = tempChar;
            }
        }
    }

    // Convert to uppercase and add spaces between characters
    char spacedMasterWord[256] = "";
    for (int i = 0; i < strlen(formattedMasterWord); i++) {
        char upperChar[3] = {toupper(formattedMasterWord[i]), ' ', '\0'};
        strcat(spacedMasterWord, upperChar);
    }

    // Create the HTML content
    snprintf(buffer, BUFSIZE * 10, "<html><body><h1>Words Without Friends</h1>");
    strcat(buffer, "<p>Master Word: ");
    strcat(buffer, spacedMasterWord); // Insert formatted master word
    strcat(buffer, "</p><hr>");

    // Generating the list of found/unfound words
    while (temp) {
        if (temp->condition) {
            snprintf(tempBuffer, sizeof(tempBuffer), "<p>Found: %s</p>", temp->mystring);
        } else {
            snprintf(tempBuffer, sizeof(tempBuffer), "<p>");
            for (int i = 0; i < strlen(temp->mystring); i++) strcat(tempBuffer, "- ");
            strcat(tempBuffer, "</p>");
        }
        strcat(buffer, tempBuffer);
        temp = temp->next;
    }

    // Adding the input form
    strcat(buffer, "<form action=\"/words\" method=\"GET\">");
    strcat(buffer, "<input type=\"text\" name=\"move\" autofocus></input>");
    strcat(buffer, "<button type=\"submit\">Submit</button></form></body></html>");
}


void acceptInput(char *guess) {
    //using toupper to uppercase all characters
    for (int i = 0; guess[i]; i++) {
        guess[i] = toupper(guess[i]);
    }
    struct gameListNode *temp = root;
    while (temp) {
        if (strcmp(temp->mystring, guess) == 0) {    //checking if the "guess" and the current list word matches
            temp->condition = true;                  //setting the current list word found condition to true
            numOfWordsLeft--;                       //decrementing the  number of words left to be found
            break;
        }
        temp = temp->next;
    }
}

// bool isDone() {
//     return numOfWordsLeft == 0;
// }

bool isDone() {
    struct gameListNode *temp = root;
    while (temp != NULL) {
        if (!temp->condition) {
            return false; // If any word is still not found, the game is not done.
        }
        temp = temp->next;
    }
    return true; // All words are found; the game is over.
}

// //Using a do while in this method because displayWorld() and acceptInput needs to call at least 1 time.
// void gameLoop() {
//     int a = initialization();		//Calling initialization
//     findWords(getRandomWord());
//     char temp;
//     do {
//         for (int i = 0; i < strlen(masterWord) - 1; i++) {		//Two for loops to keep track of 2 chars
//             for (int k = i + 1; k < strlen(masterWord); k++) {
//                 if (masterWord[i] > masterWord[k]) {
//                     temp = masterWord[i];				//storing current word in a temp
//                     masterWord[i] = masterWord[k];		//next char is the char from before
//                     masterWord[k] = temp;				//and previous char is what storred in temp
//                 }
//             }
//         }
//         for (int x = 0; x <= sizeof(masterWord); x++) {
//             masterWord[x] = toupper(masterWord[x]); 		//setting all the chars in the array to uppercase/
//         }
//         printf("Master word: %s\n", masterWord);
//         printf("---------------------------------\n");
//
//         displayWorld();
//         acceptInput();
//         // displayWorld();
//
//     } while(!isDone());
//
// }


void handle_client_request(void *args) {
    clientReqArgs *request = (clientReqArgs *)args;
    int client_fd = request->clientFd;
    char buffer[BUFSIZE] = {0};
    recv(client_fd, buffer, BUFSIZE - 1, 0);

    if (strstr(buffer, "GET /words?move=")) {       //parsing the buffer received from the URL and if its end with a move=
        char *move = strstr(buffer, "move=") + 5;
        char guess[30];
        sscanf(move, "%29[^& ]", guess);
        acceptInput(guess);
    } else if (strstr(buffer, "GET /words")) {      //else if it ends with /words it opens up a new game
        cleanupGameListNodes();
        findWords(getRandomWord());
    }

    char html_body[BUFSIZE * 10];
    if (isDone()) {                            //if isDone is true meaning every word has been found it will say "congratulations" and gives an hyperlink to a new game
        snprintf(html_body, sizeof(html_body),
                 "<html><body>Congratulations! You solved it! "
                 "<a href=\"/words\">Start a new game?</a></body></html>");
    } else {
        displayWorldHTML(html_body);        //if isdone returns false, calls displayworldhtml
    }

    char response[BUFSIZE * 10];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"
             "Content-Length: %lu\r\nConnection: close\r\n\r\n%s",
             strlen(html_body), html_body);

    send(client_fd, response, strlen(response), 0);     //sending the 200 ok response
    close(client_fd);
    free(request);
}

void server(char *root_dir, const char *port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);    //filling memory with the size of hints
    hints.ai_family = AF_INET;          // Use IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP
    hints.ai_flags = AI_PASSIVE;        // Use local IP

    if (getaddrinfo(NULL, port, &hints, &res) != 0) {   //getting information to make a socket, error if failed
        perror("getaddrinfo failed");
        exit(1);
    }

    int serverFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);   //creating a socket, error if failed
    if (serverFd < 0) {
        perror("Socket failed");
        exit(1);
    }

    if (bind(serverFd, res->ai_addr, res->ai_addrlen) != 0) {    //binding sockets with the port, error if failed
        perror("Bind failed");
        exit(1);
    }

    freeaddrinfo(res);       //freeing the res, since it's no longer needed and it was an allocated struct

    if (listen(serverFd, 10) != 0) {         //listing to server
        perror("Listen failed");
        exit(1);
    }

    printf("Server listening on port %s\n", port);  //printing the port number on terminal

    while (1) {
        struct sockaddr_storage clientAddress;
        socklen_t addressSize = sizeof(clientAddress);
        int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &addressSize);        //Accepting clients connection, error if failed
        if (clientFd < 0) {
            perror("Accept failed");
            continue;
        }

        clientReqArgs *args = malloc(sizeof(clientReqArgs));     //allocating memory for arguments
        args->clientFd = clientFd;
        args->rootAddress = root_dir;

        pthread_t thread;           //creating a new thread to handle client request
        pthread_create(&thread, NULL, (void *(*)(void *))handle_client_request, (void *)args);
        pthread_detach(thread);
    }
    close(serverFd);        // Closing the server socket
}

int main(int argc, char *argv[]) {
    initialization();
    server(argv[1], PORT);
    //calling the linked list clean up freeing functions
    cleanupWordListNodes();
    cleanupGameListNodes();
    return 0;

    //gcc WWF.c -o WWF  
    //to run the server, after compiling, just run by "./wwf4_Blesson_Binoy" without any command line arguments
    // "http://localhost:8000/words" on the browser
}


