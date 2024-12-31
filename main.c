#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>


#define UA 72
#define DA 80
#define LA 75
#define RA 77
#define ES 27
#define BS 8 



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
void Prompt(char txt[]) {
    now = time(NULL);
    strcpy(mainPrompt, txt);
}
void endTime() {
    time_t yet;
    yet = time(NULL);
    if (yet >= now + timer * 1) {
        mainPrompt[0] = '\0';
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
        refresh();
    }
    return 0;
}

short input(char inp[]) {
    // initscr();
    // char inp[100];
    for (int i = 0; i < 100; i++) inp[i] = '\0';
    short i = 0, state = 1;
    while (state) {
        char c = getch();
        endTime();
        if (c) {
            switch(c) {
                case ES: {
                    return 0;
                } break;
                case BS: {
                    inp[i] = '\0';
                }
                case LA: {
                    i--;
                } break;
                case RA: {
                    if (inp[i+1]) i++;
                } break;
                case DA: {
                    inp[i] = '\0';
                    return 1;
                } break;
                case UA:
                case '\n': {
                    inp[i] = '\0';
                    return -1;
                }break;
                
                defualt : {
                    inp[i++] = c;
                }
            }
        }
        refresh();
    }
    refresh();
    return 0;

    // endwin();
}

int printMenu(struct button butts[], char title[], short len) {
    int row, col;
    getmaxyx(stdscr, row, col);
    while (1) {
        attron(A_STANDOUT);
        mvprintw(2, (col-strlen(title))/2, "%s", title);
        attroff(A_STANDOUT);
        for (short i = 0; i < len; i++) {
            if (butts[i].state) attron(COLOR_PAIR(2));
            else attron(COLOR_PAIR(1));
            mvprintw(i*2 + 5, butts[i].type ? (col/2) - strlen(butts[i].label) : (col - strlen(butts[i].label))/2, "%s", butts[i].label);
        }
    }
}


short menu (struct button butts[], short len, short end, short active, char title[]) {
    printMenu(butts, title, len);
    butts[active].state = 1;
    switch (butts[active].type) {
        case (0): {
            short res = click();
            switch(res) {
                case 2:
                    return butts[active].value[0]-'0';
                case 1: {
                    butts[active].state = 0;
                    active++;
                    return menu(butts, len, end, active, title);
                }
                case -1: {
                    butts[active].state = 0;
                    active--;
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
            short res = input(butts[active].value);
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
                    if ((*(butts[temp].check))(butts[temp].value)) {
                        return menu(butts, len, end, active, title);
                    }
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
        if (!strcmp(users[i], user)) return 1;
    }
    Prompt("Username already exists, please choose another");
    return 0;
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
        printf("Password too short! It must be at least 7 characters long.");
        return 0;
    }
    short a=0,b=0,c=0;
    for (short i = 0; pass[i] != '\0'; i++) {
        if (pass[i]>='0' && pass[i]<='9') a = 1;
        if (pass[i]>='A' && pass[i]<='Z') b = 1;
        if (pass[i]>='a' && pass[i]<='z') c = 1;
    }
    if (1-a) {
        printf("Password doesn't contain any digits!");
        return 0;
    }
    if (1-b) {
        printf("Password doesn't contain any uppercase letters!");
        return 0;
    }
    if (1-c) {
        printf("Password doesn't contain any lowercase letters!");
        return 0;
    }
    return 1;
}

void signupForm() {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    
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
    strcpy(butts[3].value,"2");
    for (short i = 0; i < 5; i++) {
        if (butts[i].type) {
            butts[i].len = 100;
            initInp(butts[i].value, butts[i].len);
        }
    }

    int ind = menu(butts, 5, 1, 0, "Sign Up Form");
    switch (ind) {
        case 0: 
            // Go Back To The Main Menu
            break;
        case 2:{
            printf("%s %s %s", butts[0].value, butts[1].value, butts[2].value);
            // Save Username
            endwin();
            return;
        } break;
        case 5:
            // Generate Password
    }
    
    

}
int main () {
    signupForm();   
    return 0;
}