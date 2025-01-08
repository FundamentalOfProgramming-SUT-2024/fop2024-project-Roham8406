/*version 0.8.0*/
#include <ncurses.h>
#include <time.h>

#define UA 3
#define DA 2
#define LA 4
#define RA 5
#define ES 27
#define BS 7 


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

void initInp(char inp[], short n) {
    for (short i = 0; i < n; i++) {
        inp[i] = '\0';
    }
}



short confirm() {
    Prompt("Are you sure of this action? Y/n");
    while (1) {
        usleep(DELAY);
        endTime();
        char c = getch();
        if (c) {
            switch(c) {
                case ES:
                case 'n': {
                    return 0;
                } break;
                case 'Y': {
                    return 1;
                } break;
            }
        }
        usleep(DELAY);
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
                case '\n':
                case ' ': {
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
        attron(COLOR_PAIR(1));
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
                attron(A_BLINK);
                mvprintw(i*2 + 5, (col/2) + ii + 1, "%c", butts[i].value[ii] == '\0' ? ' ' : butts[i].value[ii]);
                attroff(A_STANDOUT);
                attroff(A_UNDERLINE);
                attroff(A_BLINK);
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
