/*version 2.0.2*/
#include <ncurses.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>

#define UA 3
#define DA 2
#define LA 4
#define RA 5
#define ES 27
#define BS 7 

#define TIPS 45


char mainPrompt[100];
time_t now;
int timer = 1;
int DELAY = 100000;

char tips[TIPS][200] = {
    "Have A Good Day, Mi Amigo! Press \"T\" to See Tips! Good Luck Playing This Game...",
    "Did you know that this game was first run on mini-systems at arcades?",
    "Did you know Rogue is older than Pac-Man>!",
    "Did you know that after Rogue was released, the games similar to the gaame were labeled as \"Roguelike\"?!",
    "Did you know \"Beneath Apple Manor(1978)\", the first Roguelike game, was released two years before Rogue?!",
    "Did you know Rogue was released in 1980! Five years sooner than C++!",
    "Press Escape Button in order to pause the game.",
    "Did you know you can move the Hero using your numpad when the numlock is turned on?",
    "Fatigued of pressing keys? Press F+Direction to have a fast move!",
    "Don't want to collect the object now? Press G+Direction to skip picking the object",
    "Standing on a object you haven't picked? Press 5 to collect it.",
    "You can move in 8 directions! If you disregard the stairs!",
    "Seeing a monster and don't know what to do? Press I to select a weapon, then press Space to attack, if your weapon is long-ranged select the direction after that.",
    "Using a Magic Wand spuns the enemy!",
    "Press > on a > character to move upstairs and < on a < character to move downstairs,",
    "Press > on T to go the Treasure Room",
    "Kill all enemies in Treasure Room or Battle to room in order to get out of there.",
    "The objective is to get to the Treasure Room and kill the enemies there, and then you'll win.",
    "You need gold, thee are not a goblin, though! Anyway, the more, the merrier!",
    "Too hard to locate the path? Press M and it'll be revealed!",
    "Want to know where is the hidden doors and traps? Press S to scan until five places.",
    "There are hidden doors out there!",
    "Some locked doors are too old and you must enter the password reversed!",
    "Some locked doors change their password after sometime",
    "Some locked doors are doubleLocked, you must have two passwords, but you can use a key only for once.",
    "Press K when the lock panel is revealed to unlock the door with the Ancient Key",
    "Ancient Keys might get broken on use and therefore the door won't be unlocked yet.",
    "Two Broken Keys are identical to one Ancient Key",
    "Don't know what to do with elixirs? Press O in order to open Elixir Menu!",
    "Your Health will be retreated if you're not too hungry.",
    "By moving you get hungry."
    "If you're to hungry, you risk your health.",
    "Health Elixir retreats your helath if you're not hungry more quickly for a period of time.",
    "Strenth Elixir, doubles your strenth for a period of time.",
    "Speed Elixir, doubles your speed relative to the monsters for a period of time.",
    "You can pick up the lost long-ranged shots!",
    "Too hungry? Eat food! How to do that? Press E and you'll figure it out!",
    "Rotten Food are identical with Regular Food!",
    "If you don't eat Regular Food for a long time, it'll rot.",
    "If you don't eat Magical/Excellent Food for a long time, it'll lose its perfectness.",
    "Not-Rotten Food retreats helath and hunger.",
    "Rotten Food risks your health.",
    "Excellent Elixir, doubles your strenth for a period of time.",
    "Magical Elixir, doubles your speed relative to the monsters for a period of time.",
    "Did you know if there was no pile, then the dungeons would wreck!",
};
char tipn = 0;

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
    char list[16][30];
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
    endTime();
    
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
            else if (butts[i].type == 5) {
                attron(COLOR_PAIR(1));
                attron(A_BOLD);
                attron(A_DIM);
                mvprintw(i*2 + 5, (col - strlen(butts[i].label))/2, "%s", butts[i].label);
                attroff(A_BOLD);
                attroff(A_DIM);
            }
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


short menu (struct button butts[], short len, short end, short active, char title[], short last) {

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
                    return menu(butts, len, end, active, title, temp);
                }
                case 0: {
                    if (end) return 0;
                }
            }
            return menu(butts, len, end, active, title, temp);
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
                    return menu(butts, len, end, active, title, temp);
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
                    return menu(butts, len, end, active, title, temp);
                }
                case 3: {
                    butts[temp].currentOption+=2;
                }
                case 4: {
                    butts[temp].currentOption--;
                    butts[temp].currentOption = (butts[temp].currentOption + butts[temp].options) % butts[temp].options;
                    return menu(butts, len, end, active, title, temp);
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
            return menu(butts, len, end, active, title, temp);
        } break;
        case (5): {
            if ((active - last + len)%len == 1) return menu(butts, len, end, active+1, title, last+1);
            else                                return menu(butts, len, end, active-1, title, last-1);
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
    // setlocale(LC_ALL, "");
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
