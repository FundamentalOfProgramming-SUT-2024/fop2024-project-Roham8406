/* By Roham Ghasemi Qomi; The Rogue; v:0.5.0*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include "track.c"



#define UA 3
#define DA 2
#define LA 4
#define RA 5
#define ES 27
#define BS 7 

#define MUSICS 4



struct button {
    short type;
    /*
        0 : click;
        1 : number;
        2 : txt;
        3 : email;
        4 : choose
    */
    char value[100];
    char label[100];
    short options;
    char list[10][30];
    short currentOption;
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

struct gamers {
    char username[30];
    int points;
    int golds;
    short matches;
    short time;
};

struct game {
    short colour;
    short difficulty;
    short music;
    /*
    struct maps
    */
};

struct track {
    char name[20];
    char dir[30]; 
};

struct gamer player;
struct game match;
struct track tracks[MUSICS];


void mainMenu();
void loginForm();

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
    short i = 0;
    strcpy(tracks[i].name, "Warfare");
    strcpy(tracks[i].dir, "tracks/Warfare.mp3");
    i++;
    strcpy(tracks[i].name, "Hitman");
    strcpy(tracks[i].dir, "tracks/Harfare.mp3");
    i++;
    strcpy(tracks[i].name, "In Dreams");
    strcpy(tracks[i].dir, "tracks/indreams.mp3");
    i++;
    strcpy(tracks[i].name, "Motivatio");
    strcpy(tracks[i].dir, "tracks/Motivation.mp3");
    i++;
    // play(tracks[0].dir);
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

    init_pair(100, COLOR_BLACK, COLOR_GREEN);
    init_pair(101, COLOR_YELLOW, COLOR_BLACK);
    init_pair(102, COLOR_WHITE, COLOR_BLACK);
    init_pair(103, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(104, COLOR_BLUE, COLOR_BLACK);
    init_pair(105, COLOR_CYAN, COLOR_BLACK);
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

short choose() {
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
                case ' ': {
                    return 2;
                } break;
                case DA:
                case '\n': {
                    return 1;
                } break;
                case UA: {
                    return -1;
                }break;
                case RA: {
                    return 3;
                } break;
                case LA: {
                    return 4;
                }break;
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
            for (short j = 0; j < col; j++) {mvprintw(i*2+5,j," ");}
            if (butts[i].state) attron(COLOR_PAIR(2));
            else attron(COLOR_PAIR(1));
            if (butts[i].type > 0 && butts[i].type < 4) mvprintw(i*2 + 5, (col/2) - strlen(butts[i].label), "%s %s", butts[i].label, butts[i].value);
            else if (butts[i].type == 0)                mvprintw(i*2 + 5, (col - strlen(butts[i].label))/2, "%s", butts[i].label);
            else if (butts[i].type == 4)                mvprintw(i*2 + 5, (col/2) - strlen(butts[i].label), "%s <- %s ->", butts[i].label, butts[i].list[butts[i].currentOption]);
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
        case (4): {
            short res = choose();
            short temp = active;
            switch(res) {
                case 1: {
                    active+=2;
                }
                case -1: {
                    butts[temp].state = 0;
                    active--;
                    active += len;
                    active %= len;
                    char r[10];
                    sprintf(r, "%d", butts[temp].currentOption);
                    (*(butts[temp].check))(r);
                    return menu(butts, len, end, active, title);
                }
                case 3: {
                    butts[temp].currentOption+=2;
                }
                case 4: {
                    butts[temp].currentOption--;
                    butts[temp].currentOption = (butts[temp].currentOption + butts[temp].options) % butts[temp].options;
                    return menu(butts, len, end, active, title);
                }
                case 2: {
                    char r[10];
                    sprintf(r, "%d", butts[temp].currentOption);
                    (*(butts[temp].check))(r);
                } break;
                case 0: {
                    if (end) return 0;
                }
            }
            return menu(butts, len, end, active, title);
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

short chooseColour(char r[]) {
    short d;
    sscanf(r, "%hd", &d);
    match.colour = d;
    return 1;
}

short chooseDifficulty(char r[]) {
    short d;
    sscanf(r, "%hd", &d);
    match.difficulty = d;
    return 1;
}

short chooseTrack(char r[]) {

    /*
    Incable of playing(game) and playing(sound)
    */
    short d;
    sscanf(r, "%hd", &d);
    char dir[30];
    strcpy(dir, tracks[d].dir);
    play(dir);
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
    time_t now;
    now = time(NULL);

    FILE *fptr;
    fptr = fopen("data/users.txt", "a");
    char line[200] = "\n";
    hash(butts[2].value);
    fprintf(fptr, "\n%s %s %s 0 0 0 %lld", butts[0].value, butts[2].value, butts[1].value, (long long int)now);
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
                    clear();
                    loginForm();
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

void continueForm() {
    initSCR();
    
    timer = 5;
    short i = 0, files[100], ver;
    
    FILE *fptr;
    fptr = fopen("games_saved/games.txt", "r");
    char line[30], user[30];
    while (fgets(line, 30, fptr)) {

        sscanf(line, "%s %hd", user, &ver);
        if (!strcmp(user, player.username)) {
            
            files[i++] = ver;
        }
    }
    fclose(fptr);
    files[i] = -1;
    struct button butts[100];
    for (short j = 0; j < i; j++) {
        butts[j].type = 0;
        sprintf(butts[j].label, "Game Id %hd", files[j]);
        butts[j].state = 0;
        sprintf(butts[j].value, "%hd", files[j]);
        printf("%d\n", j);
    }
    butts[0].state = 1;
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, i, 1, active, "Continue Game Id: ");
        if (ind == 0) {
            clear();
            endwin();
            mainMenu();
            return;
        }
        FILE *fptr;
        char dir[40];
        sprintf(dir, "games_saved/%s$%d.txt", player.username, ind);
        fptr = fopen(dir, "r");
        char file[200][200], line[200];
        short i = 0;
        while (fgets(line, 200, fptr)) {
            strcpy(file[i++], line);
        }
        fclose(fptr);

        /*
        Reading The Map And Start The Game
        */ 
        Prompt("Starting Game");  
    }
}

void printRanking(struct gamers li[], short offset) {
    /*
    Unable of changing font;
    Unable of adding emoji;
    */
    int row, col;
    getmaxyx(stdscr, row, col);
    attron(COLOR_PAIR(100));
    for (int i = 0; i< col; i++)mvprintw(2,i," ");
    mvprintw(2,0,"Rank");
    mvprintw(2,7,"Username");
    mvprintw(2,40,"Points");
    mvprintw(2,50,"Golds");
    mvprintw(2,60,"Matches");
    mvprintw(2,70,"Days");
    for (short i = offset; i < row + offset - 3; i++) {
        
        switch (i) {
            case 0: {
                attron(COLOR_PAIR(101));
                mvprintw(3+i-offset,1,"%s", "G");

            } break;
            case 1: {
                attron(COLOR_PAIR(102));
                mvprintw(3+i-offset,1,"S");

            } break;
            case 2: {
                attron(COLOR_PAIR(103));
                mvprintw(3+i-offset,1,"B");

            } break;
            default: {
                if (i%2) attron(COLOR_PAIR(104));
                else attron(COLOR_PAIR(105));
            }
        }
        if (1-player.anonymous && !strcmp(player.username, li[i].username)) {
            attron(A_BOLD);
            attron(A_ITALIC);
            attron(A_STANDOUT);
            for (short j = 0; j<col; j++) {
                mvprintw(3+i-offset, j, " ");
            }
        }
        mvprintw(3+i-offset,0,"%d",i+1);
        mvprintw(3+i-offset,7,"%s", li[i].username);
        mvprintw(3+i-offset,40,"%d", li[i].points);
        mvprintw(3+i-offset,50,"%d", li[i].golds);
        mvprintw(3+i-offset,60,"%hd", li[i].matches);
        mvprintw(3+i-offset,70,"%d", li[i].time/60/60/24);
        attroff(A_ITALIC);
        attroff(A_BOLD);
        attroff(A_STANDOUT);
    }
}


void Ranking() {
    initSCR();
    
    timer = 5;
    struct gamers li[1000];
    char username[30], unwanted[30];
    int points, golds;
    short matches, i = 0;
    unsigned long long tim;
    FILE *fptr;
    fptr = fopen("data/users.txt", "r");
    char line[200];
    while (fgets(line, 200, fptr)) {
        if (line[0] == '\0') continue;
        sscanf(line, "%s %s %s %d %d %hd %llu", username, unwanted, unwanted, &points, &golds, &matches, &tim);
        strcpy(li[i].username, username);
        li[i].points = points;
        li[i].golds = golds;
        li[i].matches = matches;
        time_t now;
        now = time(NULL);
        li[i].time = now - tim;
        i++;
    }
    fclose(fptr);
    for (int j = 0; j < i; j++) {
        for (int k = j; k < i; k++) {
            if (li[j].points < li[k].points) {
                struct gamers temp = li[j];
                li[j] = li[k];
                li[k] = temp;
            }
        }
    }
    strcat(li[0].username, " the Greatest");
    strcat(li[1].username, " the Greater");
    strcat(li[2].username, " the Great");
    short offset = 0, row = getmaxy(stdscr);
    while (1) {
        printRanking(li, offset);
        char c = getch();
        switch(c) {
            case ES: {
                clear();
                endwin();
                mainMenu();
                return;
            } break;
            case UA: {
                if (offset > 0) offset--;
                clear();
            } break;
            case LA: {
                if (offset > row-2) offset-=row-3;
                else offset = 0;
                clear();
            } break;
            case DA: {
                if (offset < i - row + 2) offset++;
                clear();
            } break;
            case RA: {
                if (offset < i - 2*row + 2) offset+=row-3;
                else offset = i-row+2;
                clear();
            } break;
        }
    }
}


void Setting() {
    initSCR();
    
    timer = 5;
    short i = 0, files[100], ver;
    
    struct button butts[3];
    butts[0].type = 4;
    strcpy(butts[0].label, "Colour of the Hero: ");
    butts[0].state = 1;
    butts[0].options = 6;
    butts[0].currentOption = 0;
    strcpy(butts[0].list[0], "Red");
    strcpy(butts[0].list[1], "Blue");
    strcpy(butts[0].list[2], "Magmenta");
    strcpy(butts[0].list[3], "Cyan");
    strcpy(butts[0].list[4], "Green");
    strcpy(butts[0].list[5], "White");
    butts[0].check = *chooseColour;
    butts[1].type = 4;
    strcpy(butts[1].label, "Difficulty of the Game: ");
    butts[1].state = 0;
    butts[1].options = 3;
    butts[1].currentOption = 0;
    strcpy(butts[1].list[0], "Noob");
    strcpy(butts[1].list[1], "So-So");
    strcpy(butts[1].list[2], "Legend");
    butts[1].check = *chooseDifficulty;
    butts[2].type = 4;
    strcpy(butts[2].label, "Background Music: ");
    butts[2].state = 0;
    butts[2].options = MUSICS;
    butts[2].currentOption = 0;
    for (short i = 0; i < MUSICS; i++) {
        strcpy(butts[2].list[i], tracks[i].name);
    }
    butts[2].check = *chooseTrack;
    
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, 3, 1, active, "Settings: ");
        if (ind == 0) {
            clear();
            endwin();
            mainMenu();
            return;
        }
        for (short i = 0; i < 3; i++) {
            (*(butts[i].check))(butts[i].list[butts[i].currentOption]);
        }

    }
}



void mainMenu() {
    initSCR();
    
    timer = 5;
    short i = 0;

    struct button butts[6];
    butts[i].type = 0;
    strcpy(butts[i].label, "Start New Game As ");
    strcat(butts[i].label, player.username);
    butts[i].state = 1;
    strcpy(butts[i].value,"1");
    i++;

    if (1 - player.anonymous) {
        butts[i].type = 0;
        strcpy(butts[i].label, "Continue Game As ");
        strcat(butts[i].label, player.username);
        butts[i].state = 0;
        strcpy(butts[i].value,"2");
        i++;
    }

    butts[i].type = 0;
    butts[i].state = 0;
    if (player.anonymous) {
        strcpy(butts[i].label, "Log In!");
        strcpy(butts[i].value, "3");
    } else {
        strcpy(butts[i].label, "Log Out");
        strcpy(butts[i].value, "7");
    }
    i++;

    butts[i].type = 0;
    strcpy(butts[i].label, "Sign Up");
    butts[i].state = 0;
    strcpy(butts[i].value,"4");
    i++;

    butts[i].type = 0;
    strcpy(butts[i].label, "See the GOATs of the Game");
    butts[i].state = 0;
    strcpy(butts[i].value,"5");
    i++;
    
    butts[i].type = 0;
    strcpy(butts[i].label, "Setting");
    butts[i].state = 0;
    strcpy(butts[i].value,"6");
    
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, i+1, 1, active, "Main Menu");
        switch (ind) {
            case 0: {
                clear();
                endwin();
                printf("Thanks for playing our game! Hope to see you again, soon!");
                return;
            } break;
            case 1:{
                // Go To the New Game
            } break;
            case 2:{
                clear();
                endwin();
                continueForm();
                return;
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
                clear();
                endwin();
                Ranking();
                return;
            } break;
            case 6:{
                clear();
                endwin();
                Setting();
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
