/* By Roham Ghasemi Qomi; The Rogue; v:0.0.2*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>



#define UA 3
#define DA 2
#define LA 4
#define RA 5
#define ES 27
#define BS 7 



struct button {
    short type;
    /*
        0 : click;
        1 : number;
        2 : txt;
        3 : email;
    */
    char value[100];
    char label[100];
    short state;
    short len;
    short (*check) (char value[]);
};

struct gamer {
    short anonymous;
    char username[10];
    // struct inventory
    int point;
    int gold;
};

struct gamer player;

void mainMenu();

char mainPrompt[100];
time_t now;
int timer = 1;
int DELAY = 100000;

void delPrompt() {
    mainPrompt[0] = '\0';
    int row, col;
    getmaxyx(stdscr, row, col);
    for (int i = 0; i < col; i++){
        mvprintw(row-1, i, " ");
    }
}
void Prompt(char txt[]) {
    delPrompt();
    now = time(NULL);
    strcpy(mainPrompt, txt);
    int row, col;
    getmaxyx(stdscr, row, col);
    attron(COLOR_PAIR(3));
    mvprintw(row-1, (col - strlen(mainPrompt))/2, "%s", mainPrompt);
    attroff(COLOR_PAIR(3));
}
void endTime() {
    time_t yet;
    yet = time(NULL);
    if (yet >= now + timer * 1) {
        delPrompt();
    }
}

void inits() {
    srand(time(NULL));
    player.anonymous = 1;
    strcpy(player.username, "Guest");
}

void initInp(char inp[], short n) {
    for (short i = 0; i < n; i++) {
        inp[i] = '\0';
    }
}
void initSCR() {
    initscr();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    noecho();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
}

short click() {
    short state = 1;
    while (state) {
        usleep(DELAY);
        endTime();
        char c = getch();
        if (c) {
            switch(c) {
                case ES: {
                    return 0;
                } break;
                case DA: {
                    return 1;
                } break;
                case UA: {
                    return -1;
                }break;
                case '\n' : {
                    return 2;
                }
            }
        }
        usleep(DELAY);
    }
    return 0;
}

int printMenu(struct button butts[], char title[], short len, short ii) {
    // clear();
    // refresh();
    int row, col;
    getmaxyx(stdscr, row, col);
    
    // while (1) {
        attron(A_STANDOUT);
        mvprintw(2, (col-strlen(title))/2, "%s", title);
        attroff(A_STANDOUT);
        for (short i = 0; i < len; i++) {
            if (butts[i].state) attron(COLOR_PAIR(2));
            else attron(COLOR_PAIR(1));
            if (butts[i].type) mvprintw(i*2 + 5, (col/2) - strlen(butts[i].label), "%s %s", butts[i].label, butts[i].value);
            else               mvprintw(i*2 + 5, (col - strlen(butts[i].label))/2, "%s", butts[i].label);
            if (butts[i].state && ii != -1) {
                attron(A_STANDOUT);
                attron(A_UNDERLINE);
                mvprintw(i*2 + 5, (col/2) + ii + 1, "%c", butts[i].value[ii] == '\0' ? ' ' : butts[i].value[ii]);
                attroff(A_STANDOUT);
                attroff(A_UNDERLINE);
            }
            attron(COLOR_PAIR(1));

        }
        refresh();
        // if (c) printw("%c", c);
        usleep(DELAY);
    // }
}


short input(char inp[], struct button butts[], char title[], short len) {

    // for (int i = 0; i < 100; i++) inp[i] = '\0';
    short i = strlen(inp);
    while (1) {
        usleep(DELAY);
        char c = getch();
        endTime();
        if (c) {
            switch(c) {
                case ES: {
                    return 0;
                } break;
                case BS: {
                    if (i) {
                        memmove(inp+i, inp+i+1, strlen(inp)-i);
                    }
                }
                case LA: {
                    if (i) i--;
                    clear();
                    printMenu(butts, title, len, i);
                    // refresh();

                    /* In backspacing the cursor remains and is not deleted 
                        The last char remains in the string*/
                } break;
                case RA: {
                    if (inp[i]) i++;
                } break;
                case UA: {
                    inp[i] = '\0';
                    return -1;
                } break;
                case DA:
                case '\n': {
                    inp[i] = '\0';
                    return 1;
                }break;
                
                default:{
                    inp[i++] = c;
                }
            }
            printMenu(butts, title, len, i);
        }
        refresh();
    }
    refresh();
    // while (1) {}
    return 0;

    // endwin();
}


short menu (struct button butts[], short len, short end, short active, char title[]) {
    butts[active].state = 1;
    printMenu(butts, title, len, -1);
    
    switch (butts[active].type) {
        case (0): {
            short res = click();
            short temp = active;
            switch(res) {
                case 2:
                    return butts[active].value[0]-'0';
                case 1: {
                    active+=2;
                }
                case -1: {
                    butts[temp].state = 0;
                    active--;
                    active += len;
                    active %= len;
                    return menu(butts, len, end, active, title);
                }
                case 0: {
                    if (end) return 0;
                }
            }
            return menu(butts, len, end, active, title);
        }
        case 1:
        case 2:
        case 3: {
            short res = input(butts[active].value, butts, title, len);
            // printf("%s", butts[active].value);
            // endwin();
            short temp = active;
            switch (res) {
                case 0: {
                    if (end) return 0;
                } break;
                case 1: {
                    active+=2;
                }
                case -1: {
                    butts[temp].state = 0;
                    active--;
                    active += len;
                    active %= len;
                    (*(butts[temp].check))(butts[temp].value);
                    return menu(butts, len, end, active, title);
                }
            }
        }
    }
}

void hash(char txt[]) {
    /* Hash To Be Added
    char key[20] = "The Fortress!";
    short len = strlen(key);
    for (int i = 0; txt[i] != '\0'; i++) {
        txt[i] = txt[i] ^ ((key[i%len] + i) % 256);
    }
    */
}


short checkUsername(char user[]) {
    // char users[100][100];
    if (user[0] == '\0') {
        Prompt("Username empty!");
        return 0;
    }
    if (strlen(user) > 12) {
        Prompt("Username must be less than 13 characters!");
        return 0;
    } 
    for (short i = 0; user[i] != '\0'; i++) {
        if(!((user[i] >= 'A' && user[i] <= 'Z') || 
             (user[i] >= 'a' && user[i] <= 'z') ||
             (user[i] >= '0' && user[i] <= '9') ||
             (user[i] >= 'a' && user[i] <= 'z') ||
             (user[i] == '_'))) {
            Prompt("Username must only contain \"0-9\", \"a-z\", \"A-Z\" & \"_\"!");
            return 0;
        }
    }
    FILE *fptr;
    fptr = fopen("data/users.txt", "r");
    if (fptr == NULL) {
        fclose(fptr);
        return 1;
    }
    char line[200];
    fgets(line, 200, fptr);
    while(fgets(line, 200, fptr)) {
        char users[20];
        sscanf(line, "%s", users);
        if (!strcmp(users, user)) {
            Prompt("Username already exists, please choose another");
            fclose(fptr);
            return 0;
        }
    }
    fclose(fptr);

    return 1;
}

short checkEmail(char email[]) {
    if (strlen(email) > 25) {
        Prompt("Password too long! We can't handle emails longer than 25 characters!");
        return 0;
    }
    for (short i = 0; email[i] != '\0'; i++) {
        if (email[i] == ' ') {
            Prompt("Email can't contain any spaces!");
            return 0;
        }
        if (email[i] == '@') {
            short state = 0;
            for (short j = i; email[j] != '\0'; j++) {
                if (email[j] == ' ') {
                    Prompt("Email can't contain any spaces!");
                    return 0;
                }
                if (email[j] == '.') {
                    if (email[j+1] == '\0') {
                        Prompt("Email is not valid, please enter a correct email!");
                        return 0;
                    }
                    state = 1;
                }
            }
            if (state) return 1;
            break;
        }
    }
    Prompt("Email is not valid, please enter a correct email!");
    return 0;
}

short checkPassword(char pass[]) {
    if (strlen(pass) < 7) {
        Prompt("Password too short! It must be at least 7 characters long.");
        return 0;
    }
    if (strlen(pass) > 18) {
        Prompt("Password too long! It must be at most 18 characters long.");
        return 0;
    }
    short a=0,b=0,c=0;
    for (short i = 0; pass[i] != '\0'; i++) {
        if (pass[i]>='0' && pass[i]<='9') a = 1;
        if (pass[i]>='A' && pass[i]<='Z') b = 1;
        if (pass[i]>='a' && pass[i]<='z') c = 1;
    }
    if (1-a) {
        Prompt("Password doesn't contain any digits!");
        return 0;
    }
    if (1-b) {
        Prompt("Password doesn't contain any uppercase letters!");
        return 0;
    }
    if (1-c) {
        Prompt("Password doesn't contain any lowercase letters!");
        return 0;
    }
    return 1;
}

void randPass(char pass[]) {
    short len = rand() % 7 + 7, a= 0, b= 0, c= 0;
    for (short i = 0; i < len; i ++) {
        pass[i] = rand() % 93 + 33;
    }
    for (short i = 0; i < len; i++) {
        if (pass[i]>='0' && pass[i]<='9') a = 1;
        if (pass[i]>='A' && pass[i]<='Z') b = 1;
        if (pass[i]>='a' && pass[i]<='z') c = 1;
    }
    if (a&&b&&c) return;
    randPass(pass);
}

short addUser(struct button butts[]) {
    FILE *fptr;
    fptr = fopen("data/users.txt", "a");
    char line[200] = "\n";
    hash(butts[2].value);
    fprintf(fptr, "\n%s %s %s 0 0 0", butts[0].value, butts[2].value, butts[1].value);
    fclose(fptr);
    return 1;
    /*
    Without Error Handling;
    */
}

void signupForm() {
    initSCR();
    
    timer = 5;

    struct button butts[5];
    butts[0].type = 2;
    strcpy(butts[0].label, "Enter Your Username: ");
    butts[0].state = 1;
    butts[0].check = *checkUsername;
    butts[1].type = 3;
    strcpy(butts[1].label, "Enter Your Email Address: ");
    butts[1].state = 0;
    butts[1].check = *checkEmail;
    butts[2].type = 2;
    strcpy(butts[2].label, "Enter Your Password: ");
    butts[2].state = 0;
    butts[2].check = *checkPassword;
    butts[3].type = 0;
    strcpy(butts[3].label, "Generate Password!");
    butts[3].state = 0;
    strcpy(butts[3].value,"5");
    butts[4].type = 0;
    strcpy(butts[4].label, "Submit!");
    butts[4].state = 0;
    strcpy(butts[4].value,"2");
    for (short i = 0; i < 5; i++) {
        if (butts[i].type) {
            butts[i].len = 100;
            initInp(butts[i].value, butts[i].len);
        }
    }
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, 5, 1, active, "Sign Up Form");
        // printf("%d", ind);
        switch (ind) {
            case 0: {
                clear();
                endwin();
                mainMenu();
                return;
            } break;
            case 2:{
                // state = 0;
                if (((*(butts[0].check))(butts[0].value)) &&
                    ((*(butts[1].check))(butts[1].value)) &&
                    ((*(butts[2].check))(butts[2].value))) {
                        if (addUser(butts)) {
                            Prompt("User Created.");
                            state = 0;
                        } else {
                            Prompt("Unknwon error, please try again");
                        }
                    }
                printMenu(butts, "Sign Up Form", 5, -1);
                active = 4;
                if (1-state){
                    endwin();
                    return;
                }
            } break;
            case 5:
                randPass(butts[2].value);
                // printf("%s", butts[2].value);
                printMenu(butts, "Sign Up Form", 5, -1);
                active = 3;
        }
    }
}

short VOID() {return 1;}

short isValidLogIn(struct button butts[]) {
    FILE *fptr;
    fptr = fopen("data/users.txt", "r");
    if (fptr == NULL) {
        fclose(fptr);
        return 0;
    }
    char line[200];
    fgets(line, 200, fptr);
    while(fgets(line, 200, fptr)) {
        char user[20], pass[20];
        sscanf(line, "%s %s", user, pass);
        if (!strcmp(user, butts[0].value)) {
            hash(pass);
            if (!strcmp(pass, butts[1].value)) {
                fclose(fptr);
                return 1;
            }
            Prompt("Password is wrong!");
            fclose(fptr);
            return 0;
        }
    }
    fclose(fptr);
    Prompt("Username not found!");
    return 0;
}

void authorize(struct button butts[]) {
    FILE *fptr;
    fptr = fopen("data/users.txt", "r");
    if (fptr == NULL) {
        fclose(fptr);
        return;
    }
    char line[200];
    fgets(line, 200, fptr);
    while(fgets(line, 200, fptr)) {
        char user[20], pass[20], email[30];
        sscanf(line, "%s %s %s", user, pass, email);

        if (!strcmp(user, butts[0].value)) {
            hash(pass);
            if (!strcmp(email, butts[1].value)) {
                char a[100] = "Password is: ";
                strcat(a, pass);
                strcpy(butts[2].label, a);
                clear();
                printMenu(butts, "Password Finder Form", 4, -1);
                fclose(fptr);
                return;
            }
            Prompt("Invalid authorization!");
            fclose(fptr);
            return;
        }
    }
    fclose(fptr);
    Prompt("Invalid authorization!");
    return;
}

void login(struct button butts[]) {
    printf("Logging in");
    player.anonymous = 0;
    strcpy(player.username, butts[0].value);
}

void passResetForm() {
    initSCR();
    
    timer = 5;

    struct button butts[4];
    butts[0].type = 2;
    strcpy(butts[0].label, "Enter Your Username: ");
    butts[0].state = 1;
    butts[0].check = *VOID;
    butts[1].type = 3;
    strcpy(butts[1].label, "Enter Your Email Address: ");
    butts[1].state = 0;
    butts[1].check = *VOID;
    butts[2].type = 0;
    strcpy(butts[2].label, "Authorize!");
    butts[2].state = 0;
    strcpy(butts[2].value,"2");
    butts[3].type = 0;
    strcpy(butts[3].label, "Main Menu!");
    butts[3].state = 0;
    strcpy(butts[3].value,"0");
    for (short i = 0; i < 4; i++) {
        if (butts[i].type) {
            butts[i].len = 100;
            initInp(butts[i].value, butts[i].len);
        }
    }
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, 4, 1, active, "Password Finder Form");
        switch (ind) {
            case 0: { 
                clear();
                endwin();
                mainMenu();
                return;
            } break;
            case 2:{
                // state = 0;
                authorize(butts);
                active = 2;
            }
        }
    }
}


void loginForm() {
    initSCR();
    
    timer = 5;

    struct button butts[4];
    butts[0].type = 2;
    strcpy(butts[0].label, "Enter Your Username: ");
    butts[0].state = 1;
    butts[0].check = *VOID;
    butts[1].type = 2;
    strcpy(butts[1].label, "Enter Your Password: ");
    butts[1].state = 0;
    butts[1].check = *VOID;
    butts[2].type = 0;
    strcpy(butts[2].label, "Log In!");
    butts[2].state = 0;
    strcpy(butts[2].value,"2");
    butts[3].type = 0;
    strcpy(butts[3].label, "Forgot Password?!");
    butts[3].state = 0;
    strcpy(butts[3].value,"3");
    for (short i = 0; i < 4; i++) {
        if (butts[i].type) {
            butts[i].len = 100;
            initInp(butts[i].value, butts[i].len);
        }
    }
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, 4, 1, active, "Login Form");
        switch (ind) {
            case 0: { 
                clear();
                endwin();
                mainMenu();
                return;
            } break;
            case 2:{
                // state = 0;
                if (isValidLogIn(butts)) {
                    login(butts);
                    Prompt("Logged in successfully");
                    clear();
                    endwin();
                    mainMenu();
                    return;
                }
                active = 2; 

            } break;
            
            case 3: {
                clear();
                endwin();
                passResetForm();
                return;
            } break;
        }
    }
}

void mainMenu() {
    initSCR();
    
    timer = 5;

    struct button butts[5];
    butts[0].type = 0;
    strcpy(butts[0].label, "Enter Game As ");
    strcat(butts[0].label, player.username);
    butts[0].state = 1;
    strcpy(butts[0].value,"2");
    butts[1].type = 0;
    butts[1].state = 0;
    if (player.anonymous) {
        strcpy(butts[1].label, "Log In!");
        strcpy(butts[1].value, "3");
    } else {
        strcpy(butts[1].label, "Log Out");
        strcpy(butts[1].value, "7");
    }
    butts[2].type = 0;
    strcpy(butts[2].label, "Sign Up");
    butts[2].state = 0;
    strcpy(butts[2].value,"4");
    butts[3].type = 0;
    strcpy(butts[3].label, "See the GOATs of the Game");
    butts[3].state = 0;
    strcpy(butts[3].value,"5");
    butts[4].type = 0;
    strcpy(butts[4].label, "Setting");
    butts[4].state = 0;
    strcpy(butts[4].value,"6");
    
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, 5, 1, active, "Main Menu");
        switch (ind) {
            case 0: {
                clear();
                endwin();
                printf("Thanks for playing our game! Hope to see you again, soon!");
                return;
            } break;
            case 2:{
                // Go To the Game
            } break;
            case 3: {
                clear();
                endwin();
                loginForm();
                return;
            } break;
            case 4: {
                clear();
                endwin();
                signupForm();
                return;
            } break;
            case 5:{
                // Go To the Ranking
            } break;
            case 6:{
                // Go To the Setting
            } break;
            case 7:{
                player.anonymous = 1;
                strcpy(player.username, "Guest");
                clear();
                endwin();
                mainMenu();
                return;
            } break;
        }
    }
}



int main () {
    inits();    
    mainMenu();   
    return 0;
}