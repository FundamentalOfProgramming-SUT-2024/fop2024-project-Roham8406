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
char mainPrompt[100];
time_t now;
int timer = 1;
int DELAY = 100000;

void Prompt(char txt[]) {
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
        mainPrompt[0] = '\0';
        int row, col;
        getmaxyx(stdscr, row, col);
        for (int i = 0; i < col; i++){
            mvprintw(row-1, i, " ");
        }
    }
}

void initInp(char inp[], short n) {
    for (short i = 0; i < n; i++) {
        inp[i] = '\0';
    }
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
                    // clear();
                    // printMenu(butts, title, len, i);
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



short checkUsername(char user[]) {
    char users[100][100];
    /*
        code to input users
    */
    strcpy(users[0], "Roham");
    strcpy(users[1], "Ali");
    strcpy(users[2], "");
    for (short i = 0; users[i][0] != '\0'; i++) {
        if (!strcmp(users[i], user)) {
            Prompt("Username already exists, please choose another");
            return 0;
        }
    }
    return 1;
}

short checkEmail(char email[]) {
    for (short i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') {
            short state = 0;
            for (short j = i; email[j] != '\0'; j++) {
                if (email[j] == '.') {
                    if (email[j+1] == '\n') return 0;
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
        pass[i] = rand() % 94 + 32;
    }
    for (short i = 0; i < len; i++) {
        if (pass[i]>='0' && pass[i]<='9') a = 1;
        if (pass[i]>='A' && pass[i]<='Z') b = 1;
        if (pass[i]>='a' && pass[i]<='z') c = 1;
    }
    if (a&&b&&c) return;
    randPass(pass);
}

void signupForm() {
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    noecho();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
    // char c=getch();
    
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
            case 0:
                state = 0; 
                // Go Back To The Main Menu
                break;
            case 2:{
                state = 0;
                // printf("%s %s %s", butts[0].value, butts[1].value, butts[2].value);
                // Save Username
                endwin();
                return;
            } break;
            case 5:
                randPass(butts[2].value);
                // printf("%s", butts[2].value);
                printMenu(butts, "Sign Up Form", 5, -1);
                active = 3;
        }
    }
    

}
int main () {
    srand(time(NULL));
    initscr();
    signupForm();   
    return 0;
}