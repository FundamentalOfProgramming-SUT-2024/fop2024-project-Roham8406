/* ver: 2.0.2 */

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "inits.c"
#include "track.c"
// #include "menus.c"
#include <math.h>


#define MUSICS 14

#define MAXx 129
#define MAXy 35
#define Top 7
#define Bottom 6
#define Left 50
#define ax 2
#define bx 1
#define ay 1
#define by 2

// #define init_color(a,b,c,d) init_color(a,b*4,c*4,d*4)

void Ranking();

typedef struct {
    short x;
    short y;
} pair;

struct gamer {
    short anonymous;
    char username[10];
};

struct gamers {
    char username[30];
    int points;
    int golds;
    short matches;
    long time;
};


typedef struct {
    short state;
    short type;
    /*
    0
    1 Normal
    2 Hidden
    3 Known Hiddedn
    4 Locked
    5 Unlocked
    */
    short room;
    pair pos;
    short end;
} door;

typedef struct {
    short state;
    short type;
    /*
    0 No Room
    1 Normal
    2 Enchant
    3 Nightmare
    4 Treasure
    */
    pair tl;
    pair br;
    door doors[3];
    short pass;
    short nei[11];
    short neiC;
    short lockDT;
    short pwd;
    short mirror;
    short lockState;
} room;

typedef struct monster {
    short type;
    /*
        0: None;
        1: Deamon
        2: FBD
        3: Giant
        4: Snake
        5: Undeed
    */
    short level;
    short room;
    pair pos;
    short state;
    short health;
    short seen;
} monster;

typedef struct food {
    short type;
    /*
    0 No Food;
    1 Simple
    2 Excel
    3 Magical
    4: Rotten
    */
   short state;
} food;

struct game {
    short colour;
    short difficulty;
    short music;
    char maps[7][MAXy][MAXx];
    char seen[7][MAXy][MAXx];
    room rooms[7][12];
    short level;
    pair pos;
    short health;
    short hunger;
    food foods[5];
    short gold;
    short arm[5];
    short equArm;
    short elixir[3];
    short key;
    short brKey;
    monster mons[210];
    short monss;
    short lastShot;
    short intermission;
    short heal;
    short strength;
    short fast;
    pair last;
    short lastLevel;
};


struct gamer player;
struct game match;
struct track tracks[MUSICS];
time_t seen;
char fileId[50] = "";
char death = 0;




typedef struct {
    short type;
    pair pos;
    /*
    1-4 Edges
    5,6 Wall
    7 Floor
    8 Door
    9 Hidden Door
    10 Known Door
    11 Locked Door
    12 Unlocked Door
    13 Hallway
    20 Pile
    21 Hidden Trap
    22 Known Trap
    23 Upstairs
    24 Downstairs
    25 Normal Food
    26 Exp. Food
    27 Magical Food
    28 Rotten food
    29 Gold
    30 Black Gold
    31 Mace
    32 Sword
    33 Dagger
    34 Magical Wand
    35 Normal Arrow
    36 Used Dagger
    37 Used Magical Wand
    38 Used Normal Arrow
    39 Ancient Key
    40 Deamen
    41 FBD
    42 Giant
    43 Snake
    44 Undeed
    45 Treasure
    46 Health Elixir
    47 Speed Elixir
    48 Damage Elixir
    */
} obj;



char gamePrompt[100];
time_t now;
int gtimer = 3;
int gDELAY = 100000;
short wrong = 0;

void delgPrompt() {
    gamePrompt[0] = '\0';
    attron(COLOR_PAIR(26));
    for (int i = Top + MAXy + 2; i < Top + MAXy + 5; i++) {
        for (int j = 2; j < MAXx/2 + 12; j++){
            mvprintw(i, j, " ");
        }
    }
    gtimer = 3;
}
void gPrompt(char txt[]) {
    delgPrompt();
    now = time(NULL);
    strcpy(gamePrompt, txt);
    int row, col;
    getmaxyx(stdscr, row, col);
    attron(COLOR_PAIR(3));
    mvprintw(Top + MAXy + 3, 4, "%s", gamePrompt);
    attroff(COLOR_PAIR(3));
}
void gendTime() {
    time_t yet;
    yet = time(NULL);
    if (yet >= now + gtimer * 1) {
        delgPrompt();
    }
}

// void printMap(char [2 * MAXy][MAXx]);

short isIn(pair a, pair b, pair c) {
    if (b.x >= a.x && b.x <= c.x && b.y >= a.y && b.y <= c.y) return 1;
    return 0;
}
void insertMap(char map[MAXy][MAXx], room rooms[12], char map2[MAXy][MAXx]) {

    for (short i = 0; i < MAXy; i++) {
        for (short j = 0; j < MAXx; j++) {
            map[i][j] = 0;
            map2[i][j] = 0;
        }
    }

    for (short i = 0; i < 12 && rooms[i].state; i++) {
        map[rooms[i].tl.y][rooms[i].tl.x] = 1;
        map[rooms[i].tl.y][rooms[i].br.x] = 2;
        map[rooms[i].br.y][rooms[i].tl.x] = 3;
        map[rooms[i].br.y][rooms[i].br.x] = 4;
        
        for (short j = rooms[i].tl.x + 1; j < rooms[i].br.x; j++) {
            map[rooms[i].tl.y][j] = 5;
            map[rooms[i].br.y][j] = 5;
        }
        for (short j = rooms[i].tl.y + 1; j < rooms[i].br.y; j++) {
            map[j][rooms[i].tl.x] = 6;
            map[j][rooms[i].br.x] = 6;
        }
        for (short j = rooms[i].tl.x + 1; j < rooms[i].br.x; j++) {
            for (short k = rooms[i].tl.y + 1; k < rooms[i].br.y; k++) {
                map[k][j] = 7;
            }
        }

        map[rooms[i].doors[0].pos.y][rooms[i].doors[0].pos.x] = 8;
        for (short j = 0; j < 3; j++) {
            switch (rooms[i].doors[j].type) {
                case 1:
                    map[rooms[i].doors[j].pos.y][rooms[i].doors[j].pos.x] = 8;
                    break;
                case 2:
                    map[rooms[i].doors[j].pos.y][rooms[i].doors[j].pos.x] = 9;
                    break;
                case 4:
                    map[rooms[i].doors[j].pos.y][rooms[i].doors[j].pos.x] = 11;
                    break;
            }
        }
    }
}

void pointFinder(pair ptr, short *x, short *y) {
    *x = (ptr.x/(MAXx/(4*ax + 5*bx)) - bx)/(ax+bx);
    *y = (ptr.y/(MAXy/10) - 1)/3;

}

void markWay(char main[MAXy][MAXx], int map[MAXy][MAXx], pair end) {
    short j = end.y, i = end.x;
    if (map[j][i] == 1) return;
    main[j][i] = 13;
    short nx[] = {i,i+1,i,i-1};
    short ny[] = {j-1,j,j+1,j};
    short ks[] = {-1,-1,-1,-1};
    for (short counter = 0; counter < 4; counter++) {
        short k, state = 1;
        // for (short i = 0; i < 4; i++) {
        //     if (map[ny[k]][nx[k]] == -1 && map[ny[(k+2)%4]][nx[(k+2)%4]] == -1 && map[ny[(k+2)%4]][nx[(k+2)%4]] == 0) {
        //         end.x = nx[k];
        //         end.y = ny[k];
        //         markWay(main, map, end);
        //         return;
        //     }
        // }
        if (rand() % 6) {
            while (state) {
                k = rand() % 4;
                for (short l = 0; l < 4; l++) {
                    if (ks[l] == -1) {
                        ks[l] == k;
                        if (map[j][i] - map[ny[k]][nx[k]] == 1) {
                            end.x = nx[k];
                            end.y = ny[k];
                            markWay(main, map, end);
                            return;
                        }
                        break;
                    }
                    if (ks[l] == k) break;
                }
            }
        } else {
            while (state) {
                k = rand() % 4;
                if (map[ny[k]][nx[k]] > 0 && ny[k] >= 0 && ny[k] < MAXy && nx[k] >= 0 && nx[k] < MAXx) {
                    end.x = nx[k];
                    end.y = ny[k];
                    markWay(main, map, end);
                    return;
                }
            }
        }
    }
}

void CONNECT(room rooms[12], short ind) {
    if (rooms[ind].state == 2) return;
    rooms[ind].state = 2;
    for (short i = 0; i < rooms[ind].neiC; i++) {
        CONNECT(rooms, rooms[ind].nei[i]);
    }
}

short isConnected(room rooms[12]) {
    for (short i = 0; i < 12 && rooms[i].state; i++) {
        if (rooms[i].state == 1) return 0;
    }    
    return 1;
}

short roomFinder(room rooms[12], pair pnt) {
    for (short i = 0;  i < 12; i++) {
        if (isIn(rooms[i].tl, pnt, rooms[i].br)) return i;
    }
    return -1;
}
short isRoom() {
    return roomFinder(match.rooms[match.level], match.pos) != -1;
}

short isNightmare() {
    return isRoom() && match.rooms[match.level][roomFinder(match.rooms[match.level], match.pos)].type == 3;
}

short doorFinder(door doors[36], short ind, room rooms[12], char main[MAXy][MAXx]) {
    
    int map[MAXy][MAXx];
    for (short i = 0; i < MAXy; i++) {
        for (short j = 0; j < MAXx; j++) {
            map[i][j] = 0;
        }
    }
    for (short i = 0; i < 12 && rooms[i].state; i++) {
        for (short j = rooms[i].tl.x; j <= rooms[i].br.x; j++) {
            for (short k = rooms[i].tl.y; k <= rooms[i].br.y; k++) {
                map[k][j] = -1;
            }
        }
    }
    for (short i = 0; i < 36; i++) {
        if (doors[i].state && doors[i].room != doors[ind].room){
            map[doors[i].pos.y][doors[i].pos.x] = -2;
        }
    }
    map[doors[ind].pos.y][doors[ind].pos.x] = 1;
    int counter = 1;
    pair end;
    while (counter < 60) {
        for (short i = 0; i < MAXx; i++) {
            for (short j = 0; j < MAXy; j++) {
                if (map[j][i] == counter) {

                    short nx[] = {i,i+1,i,i-1};
                    short ny[] = {j-1,j,j+1,j};
                    for (short k = 0; k < 4; k++) {
                        if (nx[k] >= MAXx || nx[k] < 0 || ny[k] >= MAXy || ny[k] < 0) continue;
                        pair temp;
                        temp.x = nx[k];
                        temp.y = ny[k];
                        if (map[ny[k]][nx[k]] == -2) {
                            end.x = i;
                            end.y = j;
                            markWay(main, map, end);
                            for (short l = 0; l < 36; l++) {
                                if (doors[l].pos.x == nx[k] && doors[l].pos.y == ny[k]) {
                                    doors[l].state = 0;
                                    doors[ind].state = 0;
                                    rooms[doors[ind].room].nei[rooms[doors[ind].room].neiC] = doors[l].room;
                                    doors[ind].end = doors[l].room;
                                    rooms[doors[ind].room].neiC++;
                                    rooms[doors[l].room].nei[rooms[doors[l].room].neiC] = doors[ind].room;
                                    rooms[doors[l].room].neiC++;
                                    doors[l].end = doors[ind].room;
                                    break;
                                }
                            }
                            
                            return 1;
                            break;
                        }
                        if (map[ny[k]][nx[k]] == 0) map[ny[k]][nx[k]] = counter+1;
                    }
                }
            }
        }
        counter++;
    }
    return 0;
}
short checkDeamon(char map[MAXy][MAXx], short y, short x)  {
    
}
short checkFBM(char map[MAXy][MAXx], short y, short x)  {
    
}
short checkGiant(char map[MAXy][MAXx], short y, short x)  {
    
}
short checkSnake(char map[MAXy][MAXx], short y, short x)  {
    
}
short checkUndeed(char map[MAXy][MAXx], short y, short x)  {
    
}

short bplay();
void moveTo(short, short, short);
int battleRandomizer(room [12], char [MAXy][MAXx], char [MAXy][MAXx]);

void battle() {
    match.lastLevel = match.level;
    match.level = 5;
    match.last = match.pos;
    match.pos.x = MAXx/2;
    match.pos.y = MAXy/2;
    moveTo(match.pos.y, match.pos.x, 0);
    battleRandomizer(match.rooms[5], match.maps[5], match.seen[5]);
    clear();
    bplay();
    match.level = match.lastLevel;
    match.pos = match.last;
}

void monsAttack(short type) {
    match.health -= type*5 + 5;
    match.intermission = 20;
    char li[5][30] = {"Deamon", "Fire Breathing Monstor", "Giant", "Snake", "Undeed"};
    char temp[65];
    sprintf(temp, "You Are Being Attacked By The %s!", li[type-1]);
    death = type;
    gPrompt(temp);
}

void addMonster(short level, room rooms[12], short i, char map[MAXy][MAXx]) {
    short x = rooms[i].tl.x + 1;
    short y = rooms[i].tl.y + 1;
    short length = (rooms[i].br.x - rooms[i].tl.x - 1);
    short width  = (rooms[i].br.y - rooms[i].tl.y - 1);
    match.mons[match.monss].level = level;
    match.mons[match.monss].room = i;
    match.mons[match.monss].state = -1;
    match.mons[match.monss].seen = 0;
    match.mons[match.monss].pos.x = x + rand() % length;
    match.mons[match.monss].pos.y = y + rand() % width;
    short num, health, type;
    switch(rand()%10) {
        case 0:
        case 1:
        case 2: {
            type = 1;
            num = 40;
            health = 5;
        } break;
        case 3:
        case 4:
        case 5: {
            type = 2;
            num = 41;
            health = 10;
        } break;
        case 6:
        case 7: {
            type = 3;
            health = 15;
            num = 42;
        } break;
        case 8: {
            health = 20;
            type = 4;
            num = 43;
        } break;
        case 9: {
            health = 30;
            type = 5;
            num = 44;
        }
    }
    // map[match.mons[match.monss].pos.x][match.mons[match.monss].pos.y] = num;
    match.mons[match.monss].health = health;
    match.mons[match.monss].type = type;
    match.monss++;
    // short (*func[5])(char**, short, short) = {&checkDeamon, &checkFBM, &checkGiant, &checkSnake, &checkUndeed};
    // match.mons[match.monss].func = func[match.mons[match.monss].type];
}

void addBattleMonster(room rooms[12], short i, char map[MAXy][MAXx]) {
    short x = rooms[0].tl.x + 1;
    short y = rooms[0].tl.y + 1;
    short length = (rooms[0].br.x - rooms[0].tl.x - 1);
    short width  = (rooms[0].br.y - rooms[0].tl.y - 1);
    match.mons[i].level = 6;
    match.mons[i].room = 0;
    match.mons[i].state = -1;
    match.mons[i].seen = 1;
    match.mons[i].pos.x = x + rand() % length;
    match.mons[i].pos.y = y + rand() % width;
    short num, health, type;
    switch(rand()%10) {
        case 0:
        case 1:
        case 2: {
            type = 1;
            num = 40;
            health = 5;
        } break;
        case 3:
        case 4:
        case 5: {
            type = 2;
            num = 41;
            health = 10;
        } break;
        case 6:{
            type = 3;
            health = 15;
            num = 42;
        } break;
        case 7:
        case 8: {
            health = 20;
            type = 4;
            num = 43;
        } break;
        case 9: {
            health = 30;
            type = 5;
            num = 44;
        }
    }
    // map[match.mons[i].pos.x][match.mons[i].pos.y] = num;
    match.mons[i].health = health;
    match.mons[i].type = type;
    // short (*func[5])(char**, short, short) = {&checkDeamon, &checkFBM, &checkGiant, &checkSnake, &checkUndeed};
    // match.mons[match.monss].func = func[match.mons[match.monss].type];
}

int battleRandomizer(room rooms[12], char map[MAXy][MAXx], char map2[MAXy][MAXx]) {
    short level = 6;
    for (short i = 0; i < MAXx; i++) {
        for (short j = 0; j < MAXy; j++) {
            map2[j][i] = 1;
        }
    }
    for (short i = 0; i < 13; i++) {
        rooms[i].state = 0;
        rooms[i].doors[0].state = 0;
        rooms[i].doors[1].state = 0;
        rooms[i].doors[2].state = 0;
        for (int j = 0; j < 11; j++) {
            rooms[i].nei[j] = -1;
        }
        rooms[i].neiC = 0;
    }

    short row = MAXy, col = MAXx;
    int sum = 0, k = 0, drs = 0;
    rooms[0].state = 1;
    rooms[0].type = 5;
    rooms[0].tl.x = (col/3 + rand() % (col/15));
    rooms[0].tl.y = (row/4 + rand() % (5*row/28));
    rooms[0].br.x = (2*col/3 - rand() % (col/15));
    rooms[0].br.y = (3*row/4 - rand() % (5*row/28));
    for (short i = 1; i < 12; i++) rooms[i].state = 0;
    k = 1;
    insertMap(map, rooms, map2);
    
    short i = 0;
    short piap = 7 - 3*match.difficulty;
    short food = 4 + 3*match.difficulty;
    short gold = 5 + 2*match.difficulty;
    short armo = 5 + 3*match.difficulty;
    short elix = 5 + 2*match.difficulty;
    short mons = 4 - match.difficulty;

    short y = rooms[i].tl.x + 1;
    short x = rooms[i].tl.y + 1;

    short width = (rooms[i].br.x - rooms[i].tl.x - 1);
    short length  = (rooms[i].br.y - rooms[i].tl.y - 1);

    if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 20;
    if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 20;
    if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 20;
    if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 20;
    if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 20;
    if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 20;
    
    if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 25;
    if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 26;
    if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 27;
    if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 28;
    if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 25;
    if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 26;
    if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 27;
    if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 25;
    if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 26;
    if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 27;
    
    if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
    if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
    if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
    if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
    if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
    if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
    if (rand() % armo == 0) {
        int sym;
        switch (rand() % 4 + 1) {
            // case 0: sym = 31; break;
            case 1: sym = 32; break;
            case 2: sym = 33; break;
            case 3: sym = 34; break;
            case 4: sym = 35; break;
        }
        map[x + rand() % length][y + rand() % width] = sym;
    }
    if (rand() % armo == 0) {
        int sym;
        switch (rand() % 4 + 1) {
            // case 0: sym = 31; break;
            case 1: sym = 32; break;
            case 2: sym = 33; break;
            case 3: sym = 34; break;
            case 4: sym = 35; break;
        }
        map[x + rand() % length][y + rand() % width] = sym;
    }
    if (rand() % elix == 0) {
        int sym;
        switch (rand() % 3) {
            case 0: sym = 46; break;
            case 1: sym = 47; break;
            case 2: sym = 48; break;
        }
        map[x + rand() % length][y + rand() % width] = sym;
    }
    if (rand() % elix == 0) {
        int sym;
        switch (rand() % 3) {
            case 0: sym = 46; break;
            case 1: sym = 47; break;
            case 2: sym = 48; break;
        }
        map[x + rand() % length][y + rand() % width] = sym;
    }

    if (rand() % elix == 0) {
        int sym;
        switch (rand() % 3) {
            case 0: sym = 46; break;
            case 1: sym = 47; break;
            case 2: sym = 48; break;
        }
        map[x + rand() % length][y + rand() % width] = sym;
    }
    short j = 190;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;
    if (rand() % mons == 0) {addBattleMonster(rooms, j++, map);} else match.mons[j-1].type = 0;

    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);
    addBattleMonster(rooms, j++, map);

    return sum;
}


int floorRandomizer(room rooms[12], char map[MAXy][MAXx], short level, room *first, pair *stair, char map2[MAXy][MAXx]) {
    
    for (short i = 0; i < 13; i++) {
        rooms[i].state = 0;
        rooms[i].doors[0].state = 0;
        rooms[i].doors[1].state = 0;
        rooms[i].doors[2].state = 0;
        for (int j = 0; j < 11; j++) {
            rooms[i].nei[j] = -1;
        }
        rooms[i].neiC = 0;
    }

    short row = MAXy, col = MAXx;
    int sum = 0, k = 0, drs = 0;
    if (level == 5) {
        rooms[0].state = 1;
        rooms[0].type = 4;
        rooms[0].tl.x = (col/3 + rand() % (col/15));
        rooms[0].tl.y = (row/4 + rand() % (5*row/28));
        rooms[0].br.x = (2*col/3 - rand() % (col/15));
        rooms[0].br.y = (3*row/4 - rand() % (5*row/28));
        for (short i = 1; i < 12; i++) rooms[i].state = 0;
        k = 1;
        insertMap(map, rooms, map2);
    } else {
        door doors[36];

        for (short i = 0; i <36; i++) {
            doors[i].state = 0;
            doors[i].type = 0;
            doors[i].end = -1;
            
        }

        for (short i = 0; i < 4; i++) {
            for (short j = 0; j < 3; j++) {
                short ii, jj;
                pointFinder(first->tl, &ii, &jj);
                if ((i == ii && j == jj) || rand() % 3) {
                    rooms[k].state = 1;
                    rooms[k].type = 1;
                    if (i == ii && j == jj) {
                        rooms[k].tl = first->tl;
                        rooms[k].br = first->br;
                    } else {
                        rooms[k].tl.x = ((ax+bx)*i+bx)*(col/(4*ax + 5*bx)) + rand() % (col/(4*ax + 5*bx) - 5);
                        rooms[k].tl.y = (3*j+1)*(row/10) + rand() % (2*(row/10) - 5);
                        rooms[k].br.x = 5 + rooms[k].tl.x + rand() % (col/(4*ax + 5*bx) - 5 - rooms[k].tl.x + ((ax+bx)*i+bx)*(col/(4*ax + 5*bx)));
                        rooms[k].br.y = 5 + rooms[k].tl.y + rand() % (2*(row/10) - 5 - rooms[k].tl.y + (3*j+1)*(row/10));
                    }

                    short length = rooms[k].br.x - rooms[k].tl.x - 1, width = rooms[k].br.y - rooms[k].tl.y - 1;
                    short drp = rand() % (2*(width + length)), drp2, drp3;
                    if (drp % (width + length) >= length) {
                        rooms[k].doors[0].pos.x = (drp > width + length) ? rooms[k].tl.x : rooms[k].br.x;
                        rooms[k].doors[0].pos.y = rooms[k].tl.y + 1 + (drp % (width + length) - length);
                    }
                    else {
                        rooms[k].doors[0].pos.y = (drp > width + length) ? rooms[k].br.y : rooms[k].tl.y;
                        rooms[k].doors[0].pos.x = rooms[k].tl.x + 1 + (drp % (width + length));
                    }
                    rooms[k].doors[0].type = 1;
                    rooms[k].doors[0].state = 1;
                    rooms[k].doors[0].room = k;
                    doors[drs] = rooms[k].doors[0];
                    drs++;

                    if (rand() % 3) {
                        do {drp2 = rand() % (2*(width + length));} while (abs(drp2 - drp) <= 1);
                        drp3 = drp;
                        drp = drp2;
                        if (drp % (width + length) >= length) {
                            rooms[k].doors[1].pos.x = (drp > width + length) ? rooms[k].tl.x : rooms[k].br.x;
                            rooms[k].doors[1].pos.y = rooms[k].tl.y + 1 + (drp % (width + length) - length);
                        }
                        else {
                            rooms[k].doors[1].pos.y = (drp > width + length) ? rooms[k].br.y : rooms[k].tl.y;
                            rooms[k].doors[1].pos.x = rooms[k].tl.x + 1 + (drp % (width + length));
                        }
                        rooms[k].doors[1].type = rand() % 10 ? rand() % 10 ? 1 : 4 : 2;
                        rooms[k].doors[1].state = 1;
                        rooms[k].doors[1].room = k;
                        doors[drs] = rooms[k].doors[1];    
                        drs++;
                    } else {
                        rooms[k].doors[1].type = 0;
                    }


                    if (rand() % 5 > 2 && rooms[k].doors[1].type == 1) {
                        do {drp2 = rand() % (2*(width + length));} while (abs(drp2 - drp) <= 1 || abs(drp3 - drp2) <= 1);
                        drp = drp2;
                        if (drp % (width + length) >= length) {
                            rooms[k].doors[2].pos.x = (drp > width + length) ? rooms[k].tl.x : rooms[k].br.x;
                            rooms[k].doors[2].pos.y = rooms[k].tl.y + 1 + (drp % (width + length) - length);
                        }
                        else {
                            rooms[k].doors[2].pos.y = (drp > width + length) ? rooms[k].br.y : rooms[k].tl.y;
                            rooms[k].doors[2].pos.x = rooms[k].tl.x + 1 + (drp % (width + length));
                        }
                        rooms[k].doors[2].type = rand() % 10 ? 1 : 2;
                        rooms[k].doors[2].state = 1;
                        rooms[k].doors[2].room = k;
                        doors[drs] = rooms[k].doors[2];
                        drs++;
                    } else {
                        rooms[k].doors[2].type = 0;
                    }

                    sum += (length) * (width);
                    k++;
                }
            }
        }
        if (k < 6 || drs%2 || drs < 2*k-2) {return floorRandomizer(rooms, map, level, first, stair, map2);}
        insertMap(map, rooms, map2);
        for (short i = 0; i < 36; i++) {
            if (doors[i].state) {
                if (!doorFinder(doors, i, rooms, map)) return floorRandomizer(rooms, map, level, first, stair, map2);
            }
        }
        CONNECT(rooms, 0);
        if (!isConnected(rooms)) return floorRandomizer(rooms, map, level, first, stair, map2);


        for (short i = 0; i < 36 && doors[i].end != -1; i++) {
            if (doors[i].type == 2) {
                if (rooms[doors[i].end].doors[1].type == 2 || rooms[doors[i].end].doors[1].type == 0) {
                    rooms[doors[i].end].type = 2;
                }
            }
        }
        for (short i = 0; i < 12 && rooms[i].state; i++) {
            if (rooms[i].type == 1 && rand() % 9 == 0) rooms[i].type = 3; 
        }
    }
    // if (level == 4) {if (isIn(rooms[5].tl, *stair, rooms[5].br)) rooms[4].type = 4; else rooms[5].type = 4;}
    
    for (short i = 0; i < 12 && rooms[i].state; i++) {
        short piap = 9 - 3*match.difficulty;
        short food = 3 + 3*match.difficulty;
        short gold = 3 + 2*match.difficulty;
        short armo = 3 + 3*match.difficulty;
        short elix = 5 + 2*match.difficulty;
        short mons = 5 - match.difficulty;

        short y = rooms[i].tl.x + 1;
        short x = rooms[i].tl.y + 1;

        short width = (rooms[i].br.x - rooms[i].tl.x - 1);
        short length  = (rooms[i].br.y - rooms[i].tl.y - 1);
        if (rand() % piap == 0) map[x + 1 + rand() % (length-2)][y + 1 + rand() % (width-2)] = 20;
        if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
        if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
        if (rooms[i].type == 4) {
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 21;
        }
        if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 25;
        if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 25;
        if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 26;
        if (rand() % (2*food) == 0) map[x + rand() % length][y + rand() % width] = 27;
        if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 28;
        if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
        if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
        if (rooms[i].type == 4) {
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 29;
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 30;
        }
        if (rand() % armo == 0) {
            int sym;
            switch (rand() % 4 + 1) {
                // case 0: sym = 31; break;
                case 1: sym = 32; break;
                case 2: sym = 33; break;
                case 3: sym = 34; break;
                case 4: sym = 35; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rand() % elix == 0) {
            int sym;
            switch (rand() % 3) {
                case 0: sym = 46; break;
                case 1: sym = 47; break;
                case 2: sym = 48; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rand() % elix == 0) {
            int sym;
            switch (rand() % 3) {
                case 0: sym = 46; break;
                case 1: sym = 47; break;
                case 2: sym = 48; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rooms[i].type == 2) {
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 46; break;
                    case 1: sym = 47; break;
                    case 2: sym = 48; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 46; break;
                    case 1: sym = 47; break;
                    case 2: sym = 48; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 46; break;
                    case 1: sym = 47; break;
                    case 2: sym = 48; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 46; break;
                    case 1: sym = 47; break;
                    case 2: sym = 48; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
        }
        if (rooms[i].type != 2) {
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
        } if (rooms[i].type == 4) {
            addMonster(level, rooms, i, map);
            addMonster(level, rooms, i, map);
            addMonster(level, rooms, i, map);
            addMonster(level, rooms, i, map);
            addMonster(level, rooms, i, map);
            addMonster(level, rooms, i, map);
            addMonster(level, rooms, i, map);
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
            if (rand() % mons == 0) {addMonster(level, rooms, i, map);}
        }
    }
    for (short i = 0; i < match.monss; i++) {
        if (match.mons[i].level == level) {
            if (roomFinder(rooms, match.mons[i].pos) != match.mons[i].room) {
                return floorRandomizer(rooms, map, level, first, stair, map2);
            }
        } 
    }
    if (level != 5) {

        short i = rand() % k;
        map[rooms[i].tl.y + 1 + rand() % (rooms[i].br.y-3-rooms[i].tl.y)][rooms[i].tl.x +1+ rand() % (rooms[i].br.x-3-rooms[i].tl.x)] = 39;
        
        if (level) map[stair->y][stair->x] = 24;
        short r;
        short exe = 200;
        // if (level == 4) {
        //     if (isIn(rooms[5].tl, *stair, rooms[5].br)) r = 4; else r = 5;
        // } else {
            do {
                r = rand() % k;
            } while (isIn(rooms[k].tl, *stair, rooms[k].br) && exe--);
        // }
        stair->x = rooms[r].tl.x + 2 + rand()%(rooms[r].br.x - rooms[r].tl.x - 3);
        stair->y = rooms[r].tl.y + 2 + rand()%(rooms[r].br.y - rooms[r].tl.y - 3);
        map[stair->y][stair->x] = level == 4 ? 45 : 23;
        *first = rooms[r];
        for (short i = 0; i < 12 && rooms[i].state; i++) {
            if (rooms[i].doors[1].type == 4) {
                int j = rand() % 4;
                switch (j) {
                    case 0: map[rooms[i].tl.y+1][rooms[i].tl.x+1] = 14; break;
                    case 1: map[rooms[i].tl.y+1][rooms[i].br.x-1] = 14; break;
                    case 2: map[rooms[i].br.y-1][rooms[i].tl.x+1] = 14; break;
                    case 3: map[rooms[i].br.y-1][rooms[i].br.x-1] = 14; break;
                }
                rooms[i].pwd = rand() % 10000;
                rooms[i].mirror = rand()%3 ? 1 : 0;
                rooms[i].lockState = 0;
                if (level < 3 || rand()%(4-match.difficulty)) {
                    if (rand() % (4-match.difficulty)) rooms[i].lockDT = 1;
                    else {
                        rooms[i].lockDT = 10 + rand() % 30;
                    }
                } else {
                    int k;
                    do {k = rand() % 4;} while (k == j);
                    switch (k) {
                        case 0: map[rooms[i].tl.y+1][rooms[i].tl.x+1] = 14; break;
                        case 1: map[rooms[i].tl.y+1][rooms[i].br.x-1] = 14; break;
                        case 2: map[rooms[i].br.y-1][rooms[i].tl.x+1] = 14; break;
                        case 3: map[rooms[i].br.y-1][rooms[i].br.x-1] = 14; break;
                    }
                    rooms[i].lockDT =  2;
                }
            } else {
                rooms[i].lockDT = 0;
            }
        }
    }    

    return sum;
}

void printBorders() {
    attron(COLOR_PAIR(29));
    short pos[6][4] = {
        {0,0,Top + MAXy + Bottom,MAXx + Left},
        {Top-1,1,Top + MAXy,MAXx + 2},
        {Top + MAXy + 1, 1 ,Top + MAXy + Bottom - 1, MAXx/2+12},
        {Top + MAXy + 1, MAXx/2+13, Top + MAXy + Bottom - 1,MAXx+2},
        {Top - 1, MAXx + 3, Top + (MAXy + Bottom)/2 + 10, MAXx + Left - 1},
        {Top + (MAXy + Bottom)/2 + 11, MAXx + 3, Top + MAXy + Bottom - 1, MAXx + Left - 1},
    };
    for (short i = 0; i < 6; i++) {
        mvprintw(pos[i][0],pos[i][1],"\u2554");
        mvprintw(pos[i][2], pos[i][3], "\u255D");
        mvprintw(pos[i][0], pos[i][3],"\u2557");
        mvprintw(pos[i][2], pos[i][1], "\u255A");
        for (short j = pos[i][0] + 1; j < pos[i][2]; j++) {
            mvprintw(j, pos[i][1], "\u2551");
            mvprintw(j, pos[i][3], "\u2551");   
        }
        for (short j = pos[i][1] + 1; j < pos[i][3]; j++) {
            mvprintw(pos[i][0], j, "\u2550");
            mvprintw(pos[i][2], j, "\u2550");   
        }
    }
}

void printTop() {
    char sign[5][97] = {
        "   ___                                          ___                ___                         ",
        "  / _ \\ ___   ___ _  _  ____                   / _ )              / _ \\ ___   / /  ___ _   _   ",
        " / , _// _ \\ / _ // // // -_)                 / _  | / // /      / , _// _ \\ / _ \\/ _  / /  ' \\",
        "/_/|_| \\___/ \\_, / \\_,_/ \\__/                /____/  \\_, /      /_/|_| \\___//_//_/\\_,_/ /_/_/_/",
        "            /___/                                   /___/                                      "
    };
    for (short i = 0; i < 5; i++) {
        mvprintw(i+1, (MAXx + Left - 96) / 2, "%s", sign[i]);
    }
} 

void printInfo() {
    char li[5][15] = {"Mace", "Sword", "Dagger", "Magic Wand", "Normal Arrow"};

    char invents[Left*2];
    char health[80] = "", hunger[50] = "";
    for (short i = 0; i < 20; i++) {
        strcat(health, (match.health >= (i+1) * 5) ? "\u2588": "\u2591"); 
        strcat(hunger, (match.hunger >= (i+1) * 9) ? "\u2588": "\u2591"); 
    }
    attron(COLOR_PAIR(4));
    switch (match.level) {
        case 0: sprintf(invents, "First Floor"); break;
        case 1: sprintf(invents, "Second Floor"); break;
        case 2: sprintf(invents, "Third Floor"); break;
        case 3: sprintf(invents, "Fourth Floor"); break;
        case 4: sprintf(invents, "Treasure Chamber"); break;
        case 5: sprintf(invents, "Battle Room"); break;
    }
    mvprintw(Top + 1, MAXx + 2 + (Left - strlen(invents))/2, "%s", invents);
    sprintf(invents, "Gold:%23d", match.gold);
    mvprintw(Top + 3, MAXx + 2 + (Left - 28)/2, "%s", invents);
    sprintf(invents, "Ancient Key:%16d", match.key);
    mvprintw(Top + 5, MAXx + 2 + (Left - 28)/2, "%s", invents);
    sprintf(invents, "Broken Key:%17d", match.brKey);
    mvprintw(Top + 7, MAXx + 2 + (Left - 28)/2, "%s", invents);
    sprintf(invents, "Health: %s", health);
    mvprintw(Top + 9, MAXx + 2 + (Left - 28)/2, "%s", invents);
    sprintf(invents, "Hunger: %s", hunger);
    mvprintw(Top + 11, MAXx + 2 + (Left - 28)/2, "%s", invents);
    if (match.equArm) {
        sprintf(invents, "%s: %*d", li[match.equArm-1], (int) (26 - strlen(li[match.equArm-1])), match.arm[match.equArm-1]);
    } else {
        sprintf(invents, "%18s", "Unarmed");
    }
    mvprintw(Top + 13, MAXx + 2 + (Left - 28)/2, "%s", invents);
    switch (match.equArm) {
        case 1: {
            char weapon[9][60] = {
                "                  ____     ",
                "۷              . <    '    ",
                "۷ ᴗᴗᴗᴗᴗᴗᴗᴗᴗᴗᴗᴗ'         \\  ",
                "_ |||||||||||||      >   |>",
                "_ ||||||||||||| >        |>",
                "۸ ᴖᴖᴖᴖᴖᴖᴖᴖᴖᴖᴖᴖ         < . ",
                "۸             '     >   /  ",
                "                . ____'    "
            };
            for (short i = 0; i < 9; i++) {
                mvprintw(Top + 17 + i, MAXx + 2 + (Left - 28)/2, "%s", weapon[i]);
            }
        } break;
        case 0: {
            char weapon[6][60] = {
                " _   _ _   _  ___ _________  _______________ ",
                "| | | | \\ | |/ _ \\| ___ |  \\/  |  ____|  _  \\",
                "| | | |  \\| / /_\\ | |_/ | .  . | |___ | | | |",
                "| | | | . ` |  _  |    /| |\\/| |  ___|| | | |",
                "| |_| | |\\  | | | | |\\ \\| |  | | |____| |/ / ",
                " \\___/\\_| \\_\\_| |_\\_| \\_\\_|  |__\\____/|___/"
            };
            for (short i = 0; i < 6; i++) {
                mvprintw(Top + 17 + i, MAXx + 2 + (Left - 45)/2, "%s", weapon[i]);
            }
        } break;
        case 2: {
            char weapon[10][60] = {
                "               ___",
                "              ( ((",
                "               ) ))",
                "  .::.        / /(",
                " 'M .-;-.-.-/| ((:::::::::::::::::::::::.._",
                "(J ( ( ( ( ( |  ))   -=============-      _.>",
                " P -;----\\|(::::::::::::::::::::::::::::''",
                "  `::'        \\ \\(",
                "               ) ))",
                "               (_(("
            };
            for (short i = 0; i < 10; i++) {
                mvprintw(Top + 17 + i, MAXx + 2 + (Left - 45)/2, "%s", weapon[i]);
            }
        } break;
        case 3: {
            char weapon[11][60] = {
                "    /\\",
                "   /  \\....",
                "  /        \\...",   
                " /             \\____________________________",
                "|                           |  (@) _   (@) \\",
                " \\...                       |       (_)    }|",
                "    \\...................____| ( `--._.-' ) /",
                "                             \\ `-._    _.-'/",
                "                               \\   -.-'.-'",
                "                                 `-._.-'",
                "                                     "
            };
            for (short i = 0; i < 11; i++) {
                mvprintw(Top + 17 + i, MAXx + 2 + (Left - 45)/2, "%s", weapon[i]);
            }
        } break;
        case 4: {
            char weapon[6][60] = {
                "  .^.:.",
                " <.*.'::.  *     .*",
                "   █  .:''    .:'",
                "   █  '::.  .:'",
                "   █    ':::'    *",
                " * █     `*'"
            };
            for (short i = 0; i < 6; i++) {
                mvprintw(Top + 17 + i, MAXx + 2 + (Left - 19)/2, "%s", weapon[i]);
            }
        } break;

        case 5: {
            char weapon[3][60] = {
                "    _",
                ".-'` |___________________________//////",
                "`'-._|                           \\\\\\\\\\\\",
            };
            for (short i = 0; i < 3; i++) {
                mvprintw(Top + 17 + i, MAXx + 2 + (Left - 40)/2, "%s", weapon[i]);
            }
        } break;
    }
}

void printMusic() {
    attron(COLOR_PAIR(26));
    for (int i = Top + MAXy + 2; i < Top + MAXy + 5; i++) {
        for (int j = MAXx/2 + 14; j < MAXx + 2; j++){
            mvprintw(i, j, " ");
        }
    }
    attron(COLOR_PAIR(4));
    if (isMusic) {
        char music[80];
        char musics[6] = {0, match.music, 10, 3, 11, 5};
        sprintf(music, "\u266B\u266B %s by %s \u266B\u266B", tracks[musics[currentMusic]].name, tracks[musics[currentMusic]].author);
        mvprintw(MAXy+Top+3, MAXx/2 + 13 + MAXx/4 - strlen(music)/2, "%s", music);
    } else {
        char music[80];
        sprintf(music, "Music Is Muted!");
        mvprintw(MAXy+Top+3, MAXx/2 + 8 + MAXx/4 - strlen(music)/2, "%s", music);
    }
}

void printTip() {
    attron(COLOR_PAIR(26));
    for (int i = Top + (MAXy + Bottom)/2 + 12; i < Top + MAXy + Bottom - 1; i++) {
        for (int j = MAXx + 4; j < MAXx + Left - 1; j++){
            mvprintw(i, j, " ");
        }
    }
    attron(COLOR_PAIR(4));
    
    short max = Left - 5;

    char tip[200];
    short y = Top + (MAXy + Bottom)/2 + 12;
    strcpy(tip, tips[tipn]);
    while (strlen(tip) > max) {
        mvprintw(y++, MAXx + 4, "%.*s", max, tip);
        strcpy(tip, tip + max);
        if (y >= Top + MAXy + Bottom - 2) break;
    }
    mvprintw(y, MAXx + 4, "%.*s", max, tip);
}

void printMap(char map[MAXy][MAXx], short level, short cheat) {
    attron(COLOR_PAIR(26));
    for (short i = 0; i < MAXy + Top; i++) {
        for (short j = 0; j < Left + MAXx; j++) {
            mvprintw(i,j, " ");
        }
    }
    printBorders();
    printTop();
    printInfo();
    printMusic();
    printTip();

    attron(A_BOLD);
    for (short i = 0; i < MAXy; i++) {
        for (short j = 0; j < MAXx; j++) {
            short reveal = cheat == 1;
            int cr, cr2 = -1;
            char ch[5];
            pair temp = {j, i};
            int type = roomFinder(match.rooms[level], temp);
            
            if (type != -1) type = match.rooms[level][type].type;

            switch (type) {
                case (-1): {
                    if (match.seen[match.level][i][j]) reveal = 1;
                    if (abs(i - match.pos.y) + abs(j - match.pos.x) < 5 && !isRoom()) reveal = 1;
                } break;
                case 3: {
                    short room = roomFinder(match.rooms[level], match.pos);
                    if (abs(i - match.pos.y) < 3 && abs(j - match.pos.x) < 3 &&
                        match.rooms[match.level][room].type == 3) reveal = 1; 
                    if (match.rooms[match.level][room].type == 3) {
                        for (short i = 0; i < match.monss; i++) {
                            if (match.level + 1 == match.mons[i].level &&
                                room == match.mons[i].room) {
                                if (abs(match.mons[i].pos.y - match.pos.y) < 3 &&
                                    abs(match.mons[i].pos.x - match.pos.x) < 3) match.mons[i].seen = 1;
                                else match.mons[i].seen = 0;
                            }
                        }
                    }
                } break;
                default: {
                    if (abs(i - match.pos.y) + abs(j - match.pos.x) < 5  && 
                    (match.maps[match.level][i][j] == 8 ||
                    match.maps[match.level][i][j] == 10 ||
                    match.maps[match.level][i][j] == 12 )) reveal = 1;
                    short room = roomFinder(match.rooms[level], temp);
                    if (match.seen[match.level][match.rooms[match.level][room].tl.y][match.rooms[match.level][room].tl.x]) reveal = 1;
                }
            }
            char State = 1;
            if (cheat == 2 && abs(match.pos.y - i) < 5 && abs(match.pos.x - j) < 5) {
                State = 0;
                switch(map[i][j]) {
                    case 9: strcpy(ch, "?"); cr = 0; break;
                    case 21: strcpy(ch, "^"); cr = 1; break;
                    default: State = 1;
                }
            }
            if (reveal && State) {
                // char Cheat = ((abs(match.pos.y - i) < 10 && abs(match.pos.x - j) < 10) && cheat == 2) ? 1 : 0;
                switch(map[i][j]) {
                    case 0: strcpy(ch, " "); cr2 = 26; break;
                    case 1: strcpy(ch, "\u2554"); cr = 0; break;
                    case 2: strcpy(ch, "\u2557"); cr = 0; break;
                    case 3: strcpy(ch, "\u255A"); cr = 0; break;
                    case 4: strcpy(ch, "\u255D"); cr = 0; break;
                    case 5: strcpy(ch, "\u2550"); cr = 0; break;
                    case 6: strcpy(ch, "\u2551"); cr = 0; break;
                    case 7: strcpy(ch, "."); cr = 1; break;
                    case 8: strcpy(ch, "\u256C"); cr = 0; break;
                    // case 9: strcpy(ch, "?"); cr = 0; break;
                    case 9: strcpy(ch, (map[i-1][j] == 6 || map[i+1][j] ==  6) ? "\u2551" : "\u2550"); cr = 0; break;
                    case 10: strcpy(ch, "?"); cr = 0; break;
                    case 11: strcpy(ch, "@"); cr2 = 27; break;
                    case 12: strcpy(ch, "@"); cr2 = 28; break;
                    case 13: strcpy(ch, "\u2588"); cr2 = 29; break;
                    case 14: strcpy(ch, "&"); cr = 5; break;
                    case 20: strcpy(ch, "O"); cr = 0; break;
                    case 21: strcpy(ch, "."); cr = 1; break;
                    case 22: strcpy(ch, "^"); cr = 1; break;
                    case 23: strcpy(ch, ">"); cr = 2; break;
                    case 24: strcpy(ch, "<"); cr = 2; break;
                    case 25:
                    case 28: strcpy(ch, "f"); cr = 6; break;
                    case 26: strcpy(ch, "e"); cr = 6; break;
                    case 27: strcpy(ch, "m"); cr = 6; break;
                    case 29: strcpy(ch, "g"); cr = 4; break;
                    case 30: strcpy(ch, "B"); cr = 3; break;
                    case 31: strcpy(ch, "\u2692"); cr = 5; break;
                    case 32: strcpy(ch, "\u2694"); cr = 5; break;
                    case 33:
                    case 36: strcpy(ch, "\U0001F5E1"); cr = 5; break;
                    case 34:
                    case 37: strcpy(ch, "\u269A"); cr = 5; break; //1FA70
                    case 35:
                    case 38: strcpy(ch, "\u27B3"); cr = 5; break;
                    case 46: strcpy(ch, "H"); cr = 7; break;
                    case 47: strcpy(ch, "X"); cr = 7; break;
                    case 48: strcpy(ch, "E"); cr = 7; break;
                    case 39: strcpy(ch, "\u26B7"); cr = 4; break; //1403
                    // case 40: strcpy(ch, "D"); cr = 8; break;
                    // case 41: strcpy(ch, "F"); cr = 8; break;
                    // case 42: strcpy(ch, "G"); cr = 8; break;
                    // case 43: strcpy(ch, "S"); cr = 8; break;
                    // case 44: strcpy(ch, "U"); cr = 8; break;
                    case 45: strcpy(ch, "T"); cr = 2; break;

                    defualt: strcpy(ch, " "); cr2 = 26; break;

                }
            } else if (State) {
                strcpy(ch, " "); cr2 = 26;
            }
            if (cr2 != -1){
                attron(COLOR_PAIR(cr2));
            } else {
                attron(COLOR_PAIR(cr+type*10 + 20));
            }
            mvprintw(i + Top,j + 2,"%s", ch);
        }
    }
    for (short i = 0; i < match.monss; i++) {
        for (short room = 0; room < 12; room++) {
            if (match.rooms[match.level][room].type == 3) {
                for (short i = 0; i < match.monss; i++) {
                    if (match.level + 1 == match.mons[i].level &&
                        room == match.mons[i].room) {
                        if ((abs(match.mons[i].pos.y - match.pos.y) < 3 &&
                             abs(match.mons[i].pos.x - match.pos.x) < 3) ||
                             (match.mons[i].type == 4)) {
                                if (match.mons[i].seen) match.mons[i].seen = 1;
                             }
                        else match.mons[i].seen = 0;
                    }
                }
            }
        }
    }
    
                        
                    
    attron(COLOR_PAIR(80 + match.colour));
    mvprintw(Top+match.pos.y, match.pos.x + 2, "%s", "\u265A");
    attroff(A_BOLD);
    pair node = {match.pos.x, match.pos.y};
    short roomIn = roomFinder(match.rooms[level], node);
    attron(COLOR_PAIR(3));
    int counter = match.level == 5 ? 190:0;
    int moCount = match.level == 5 ? 210:match.monss;
    for (short i = counter; i < moCount; i++) {
        if (match.mons[i].type != 0 && match.mons[i].seen == 1 &&
            match.mons[i].level == match.level + 1) {
            char sym[6] = "DFGSU";
            mvprintw(Top+match.mons[i].pos.y, match.mons[i].pos.x + 2, "%c", sym[match.mons[i].type-1]);
        }
    }
    attroff(COLOR_PAIR(3));
}

void help () {
    char info[31][60] = {
        "Keys: ",
        "Esc\t\t\tPause ",
        "Arrow Up\t\tUp\t\t\t\tArrow Down\t\tDown ",
        "J\t\t\tUp\t\t\t\tK\t\t\tDown ",
        "8\t\t\tUp\t\t\t\t2\t\t\tDown ",
        "Arrow Left\t\tLeft\t\t\t\tArrow Right\t\tRight ",
        "H\t\t\tLeft\t\t\t\tL\t\t\tRight ",
        "4\t\t\tLeft\t\t\t\t6\t\t\tRight ",
        "Y\t\t\tTop Left\t\t\tU\t\t\tTop Right ",
        "7\t\t\tTop Left\t\t\t9\t\t\tTop Right ",
        "B\t\t\tBottom Left\t\t\tN\t\t\tBottom Right ",
        "1\t\t\tBottom Left\t\t\t3\t\t\tBottom Right ",
        "F -> <Direction>\tFast Move\t\t\tSpace -> <Direction>\tAttack",
        "G -> <Direction>\tMove Without Picking\t\t>\t\t\tMove Upsatirs ",
        "5\t\t\tPick\t\t\t\t<\t\t\tMove Downstairs ",
        "S\t\t\tScan\t\t\t\tO\t\t\tElixir Menu ",
        "M\t\t\tShow Map\t\t\tE\t\t\tFood Menu ",
        "K\t\t\tUse Ancient Key\t\t\tI\t\t\tWeapon Menu ",
        "",
        "Symbols: ",
        "\u265A\t\tHero ",
        "\u2551\t\tDoor\t\t\t\t^\t\tKnown Trap\t\t\t@\t\tLockable Door ",
        ".\t\tFloor\t\t\t\t?\t\tHidden Door\t\t\t&\t\tPassword Revealer ",
        "O\t\tPile\t\t\t\tSpace\t\tVoid Space\t\t\t\u2692\t\tMace ",
        ">\t\tUpstairs\t\t\tf\t\tRegular/Rotten Food\t\t\u2694\t\tSword ",
        "<\t\tDownstairs\t\t\te\t\tExcelent Food\t\t\t\U0001F5E1\t\tDagger ",
        "T\t\tTreasure\t\t\tm\t\tMagical Food\t\t\t\u269A\t\tWand ",
        "\u26B7\t\tAncient Key\t\t\tD\t\tDeamon\t\t\t\t\u27B3\t\tArrow ",
        "H\t\tHealth Elixir\t\t\tF\t\tDragon\t\t\t\tU\t\tUndeed ",
        "X\t\tStrength Elixir\t\t\tG\t\tGiant ",
        "E\t\tSpeed Elixir\t\t\tS\t\tSnake"
    };
    short x, y;
    getmaxyx(stdscr, y, x);
    for (short i = 0; i < x; i++) {
        for (short j = 0; j < y; j++) {
            mvprintw(j, i, " ");
        }
    }
    for (short i = 0; i < 31; i++) {
        mvprintw(y/2 - 15 + i, 3, "%s", info[i]);
    }
    while (1) {
        char c;
        c = getch();
        if (c == ES) {
            for (short i = 0; i < x; i++) {
                for (short j = 0; j < y; j++) {
                    mvprintw(j, i, " ");
                }
            }
            return;
        }; 
    }
}

void mapCreator() {
    short col = MAXx, row = MAXy;
    room first;
    pair stair;
    first.tl.x = (col/(4*ax + 5*bx)) + rand() % (col/(4*ax + 5*bx) - 5);
    first.tl.y = (row/10) + rand() % (2*(row/10) - 5);
    first.br.x = 5 + first.tl.x + rand() % ((ax+bx)*col/(4*ax + 5*bx) - 5 - first.tl.x);
    first.br.y = 5 + first.tl.y + rand() % (3*(row/10) - 5 - first.tl.y);
    match.pos.x = first.tl.x + 1;
    match.pos.y = first.tl.y + 1;
    match.level = 0;
    for (short i = 0; i < 5; i++) match.arm[i] = 0;
    for (short i = 0; i < 3; i++) match.elixir[i] = 0;
    for (short i = 0; i < 5; i++) match.foods[i].type = 0;
    match.arm[0] = 1;
    // match.arm[1] = 10;
    // match.arm[2] = 10;
    // match.arm[3] = 10;
    // match.arm[4] = 10;
    match.elixir[0] = 2;
    match.brKey = 0;
    match.equArm = 0;
    match.gold = 0;
    match.health = 100;
    match.hunger = 0;
    match.key = 0;
    
    for (short i = 0; i < 5; i++) {
        for (short j = 0; j < col; j++) {
            for (short k = 0; k < row; k++) {
                match.maps[i][k][j] = 0;
            }
        }
        if (i != 6) floorRandomizer(match.rooms[i], match.maps[i], i + 1, &first, &stair, match.seen[i]);
    }
    printMap(match.maps[match.level], match.level, 0);
    return;
}

void initThemes() {


    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);


    init_color(10, 0, 0, 200); //Backgorund Dark Blue
    init_color(11, 600, 600, 600); // Grey
    init_color(12, 600, 999, 200); // Yellowish Green

    init_pair(4, COLOR_CYAN, 10);
    init_pair(26, 10, 10);                   //Void
    init_pair(27, COLOR_RED, 10);            //Locked
    init_pair(28, COLOR_GREEN, 10);          //Unlocked
    init_pair(29, 11, 10);                   //Hallway
    /* General Rooms */
    init_pair(30, COLOR_CYAN, 10);           //Walls
    init_pair(31, COLOR_GREEN, 10);          //Floor  
    init_pair(32, COLOR_WHITE, COLOR_GREEN); //Stairs
    init_pair(33, COLOR_BLACK, 10);          //Black Gold
    init_pair(34, COLOR_YELLOW, 10);         //Gold
    init_pair(35, 11, 10);                   //Armour
    init_pair(36, 12, 10);                   //Food
    init_pair(37, COLOR_MAGENTA, 10);        //Elixir
    init_pair(38, COLOR_WHITE, 10);          //Monster

    /* Enchanted Rooms */
    init_pair(40, COLOR_MAGENTA, 10);        //Walls
    init_pair(41, COLOR_MAGENTA, 10);        //Floor  
    init_pair(42, COLOR_WHITE, COLOR_MAGENTA); //Stairs
    init_pair(43, COLOR_BLACK, 10);          //Black Gold
    init_pair(44, COLOR_YELLOW, 10);         //Gold
    init_pair(45, 11, 10);                   //Armour
    init_pair(46, 12, 10);                   //Food
    init_pair(47, COLOR_MAGENTA, 10);        //Elixir
    init_pair(48, COLOR_WHITE, 10);          //Monster

    /* Nightmare Rooms */
    init_pair(50, COLOR_BLACK, COLOR_MAGENTA);        //Walls
    init_pair(51, COLOR_BLACK, COLOR_MAGENTA);        //Floor  
    init_pair(52, COLOR_BLACK, COLOR_RED);            //Stairs
    init_pair(53, COLOR_BLACK, COLOR_MAGENTA);        //Black Gold
    init_pair(54, COLOR_YELLOW, COLOR_MAGENTA);       //Gold
    init_pair(55, 11, COLOR_MAGENTA);                 //Armour
    init_pair(56, 12, COLOR_MAGENTA);                 //Food
    init_pair(57, COLOR_MAGENTA, COLOR_MAGENTA);      //Elixir
    init_pair(58, COLOR_WHITE, COLOR_MAGENTA);        //Monster

    /* Treasure Room */
    init_pair(60, COLOR_YELLOW, 10);        //Walls
    init_pair(61, COLOR_MAGENTA, 10);        //Floor  
    init_pair(62, COLOR_WHITE, COLOR_MAGENTA); //Stairs
    init_pair(63, COLOR_BLACK, 10);          //Black Gold
    init_pair(64, COLOR_YELLOW, 10);         //Gold
    init_pair(65, 11, 10);                   //Armour
    init_pair(66, 12, 10);                   //Food
    init_pair(67, COLOR_MAGENTA, 10);        //Elixir
    init_pair(68, COLOR_WHITE, 10);          //Monster

    /* Battle Rooms */
    init_pair(70, COLOR_CYAN, 10);           //Walls
    init_pair(71, COLOR_GREEN, 10);          //Floor  
    init_pair(72, COLOR_WHITE, COLOR_GREEN); //Stairs
    init_pair(73, COLOR_BLACK, 10);          //Black Gold
    init_pair(74, COLOR_YELLOW, 10);         //Gold
    init_pair(75, 11, 10);                   //Armour
    init_pair(76, 12, 10);                   //Food
    init_pair(77, COLOR_MAGENTA, 10);        //Elixir
    init_pair(78, COLOR_WHITE, 10);          //Monster

    /* HERO */
    init_pair(80, COLOR_RED, 10);
    init_pair(81, COLOR_BLUE, 10);
    init_pair(82, COLOR_MAGENTA, 10);
    init_pair(83, COLOR_CYAN, 10);
    init_pair(84, COLOR_GREEN, 10);
    init_pair(85, COLOR_WHITE, 10);
    

    /* Locked Door */
    init_pair(90, COLOR_BLACK, COLOR_GREEN);
    init_pair(91, COLOR_BLACK, COLOR_YELLOW);
    init_pair(92, COLOR_BLACK, COLOR_RED);


}

void revealCode(short pass, short mir) {
    seen = time(NULL);
    char pwd[5];
    short counter = (1-mir)*3;
    mir = mir*2 - 1;
    while (counter >= 0 && counter <= 3) {
        pwd[counter] = pass%10 + '0';
        pass /= 10;
        counter += mir;
    }
    pwd[5] = 0;
    char message[30] = "Password of the lock is ";
    strcat(message, pwd);
    gPrompt(message);
    gtimer = 30;
}

void printpwd(short state, char nums[4], short counter) {
    init_pair(95, COLOR_BLACK, 11);
    attron(COLOR_PAIR(90+state));
    for (short i = MAXy*0.2; i < MAXy; i++) {
        for (short j = MAXx*0.2; j < MAXx * 0.8; j++) {
            mvprintw(i, j, "\u2592");
        }
    }
    attron(COLOR_PAIR(90 + 5));
    for (short i = MAXy*0.2+2; i < MAXy -2; i++) {
        for (short j = MAXx*0.2+2; j < MAXx * 0.8-2; j++) {
            mvprintw(i, j, " ");
        }
    }
    float is[4] = {0.33, 0.36, 0.64, 0.67};
    for (short i = 0; i < 4; i++) {
        if (counter == i) {
            mvprintw(MAXy*0.35-2, MAXx*is[i], "+");
            mvprintw(MAXy*0.35+2, MAXx*is[i], "-");
            attron(A_BLINK);
            attron(A_STANDOUT);
        }
        mvprintw(MAXy*0.35, MAXx*is[i], "%d", nums[i]);
        attroff(A_BLINK);
        attroff(A_STANDOUT);
    }
    if (counter == 4) {
        attron(A_BLINK);
        attron(A_STANDOUT);
    }
    char space[20] = {7,6,5,4,3,2,2,2,2,3,8,8,7,6,5,4,4,5};
    char keyhole[20][25] = {
        "_________",
        "/         \\",
        "/           \\",
        "/   _______   \\",
        "/   /       \\   \\",
        "|   |  () ()  |   |",
        "|   |   (_)   |   |",
        "|   |  () ()  |   |",
        "|    \\_______/    |",
        "\\____  ___  ____/",
        "\\/_ _\\/",
        "/     \\",
        "/_______\\",
        "/         \\",
        "/___________\\",
        "|#############|",
        "|#############|",
        "\\___________/"
    };
    for (short i = 0; i < 20; i++) {
    
        mvprintw(MAXy*0.3 + i, MAXx/2 - 10 +space[i], "%s", keyhole[i]);
        
    }
    attroff(A_BLINK);
    attroff(A_STANDOUT);
}

short pwd(short state, short counter) {
    char nums[4];
    for (short i = 0; i < 4; i++) nums[i] = 0;
    while (1) {
        printpwd(state, nums, counter);
        char c = getch();
        if (c) {
            if (c >= '0' && c <= '9' && counter != 4) {
                nums[counter] = c - '0';
                counter ++;
                continue;
            }
            switch (c) {
                case ES: case BS: return -1;
                case LA: counter = counter == 0 ? 4 : counter - 1; break;
                case '\n':
                case ' ': {
                    if (counter == 4) {
                        short sum = 0;

                        for (short i = 0; i < 4; i++) {                        
                            sum *= 10;
                            sum += nums[i];
                        }
                        return sum;
                    }
                }
                case RA: counter = counter == 4 ? 0 : counter + 1; break;
                case UA: {
                    if (counter == 4) counter = 0;
                    else {
                        nums[counter]++;
                        nums[counter]%=10;
                    }
                } break;
                case DA: {
                    if (counter == 4) counter = 0;
                    else {
                        nums[counter]--;
                        nums[counter]+=10;
                        nums[counter]%=10;
                    }
                } break;
                case 'k':
                case 'K': {
                    if (match.key) {
                        if (rand()%10) {
                            gPrompt("You used an ancient key!");
                            match.key --;
                            return -2;
                        } else {
                            gPrompt("You broke the key, loser...");
                            if (match.brKey) {
                                match.brKey --;
                            }
                            else {
                                match.key --;
                                match.brKey ++;
                            }
                        }
                    } else {
                        gPrompt("No ancient keys avaible!");
                    }
                }
            }
        }
    }
}

void securityEnabled(room *r) {
    r->lockState = 0;
} 

short openPwd(room *r) {
    if (r->lockState == 3) {
        securityEnabled(r);
        return 0;
    }
    if (r->lockDT > 10) {
        time_t n = time(NULL);
        if (n - seen > r->lockDT) r->pwd % 10000;
    }
    short res = pwd(r->lockState, 0);
    
    if (res == -2) {
        r->lockDT = 0;
        return 1;
    }
    if (res == -1) return 0;
    if (res == r->pwd) {
        if (r->lockDT == 2) {
            r->lockDT = 1;
            r->pwd = rand() % 1000;
            return 0;
        }
        r->lockDT = 0;
        return 1;
    }
    (r->lockState)++;
    openPwd(r);
}



short moveCheck(char map[MAXy][MAXx], short y, short x, short cy, short cx) {
    switch (map[y][x]) {
        case  0: case  1: case  2: case  3: case  4: case  5: case  6: case 20: return 0;
        case 11: {
            if (map[cy][cx] == 13) return 1;
            if (openPwd(&(match.rooms[match.level][roomFinder(match.rooms[match.level], match.pos)]))) {
                map[y][x] = 12;
                return 1;
            }
            return 0;
        } break;
        default: {
            int counter = match.level == 5 ? 190:0;
            int moCount = match.level == 5 ? 210:match.monss;
            for (short i = counter; i < moCount; i++) {
                if (match.level + 1 == match.mons[i].level &&
                    y == match.mons[i].pos.y &&
                    x == match.mons[i].pos.x &&
                    match.mons[i].type !=  0) return 0;
            }
            return 1; 
        }
    }
}

void moveMonster(short i) {
    /* Snake sometimes gets further*/

    if (abs(match.mons[i].pos.x - match.pos.x) <= 1 &&
        abs(match.mons[i].pos.y - match.pos.y) <= 1) {
        monsAttack(match.mons[i].type);
    } else {
        if (match.mons[i].state == 0) return;
        if (match.mons[i].pos.x > match.pos.x && moveCheck(match.maps[match.level], match.mons[i].pos.y,
            match.mons[i].pos.x-1, match.mons[i].pos.y, match.mons[i].pos.x))
            match.mons[i].pos.x--;
        else if (match.mons[i].pos.x < match.pos.x && moveCheck(match.maps[match.level], match.mons[i].pos.y,
                 match.mons[i].pos.x+1, match.mons[i].pos.y, match.mons[i].pos.x))
            match.mons[i].pos.x++;
        else if (match.mons[i].pos.y < match.pos.y && moveCheck(match.maps[match.level], match.mons[i].pos.y+1,
                 match.mons[i].pos.x, match.mons[i].pos.y, match.mons[i].pos.x))
            match.mons[i].pos.y++;
        else if (match.mons[i].pos.y > match.pos.y && moveCheck(match.maps[match.level], match.mons[i].pos.y-1,
                 match.mons[i].pos.x, match.mons[i].pos.y, match.mons[i].pos.x))
            match.mons[i].pos.y--;
        match.mons[i].state--;
    }
}

void moveMonsters() {
    int counter = match.level == 5 ? 190:0;
    int count = match.level == 5 ? 210:match.monss;
    for (short i = counter; i < count; i++) {
        if (match.mons[i].type != 0) {
            if (match.mons[i].level == match.level + 1) {
                if (match.mons[i].room != roomFinder(match.rooms[match.level], match.pos)) {
                    if (match.mons[i].type != 4) {
                        match.mons[i].state = -1;
                    } else if (match.mons[i].seen == 1) { 
                        moveMonster(i);
                        if (match.mons[i].state != 0) {
                            match.mons[i].state = -2;
                        }
                    }

                } else {
                    if (match.mons[i].state == -1) {
                        switch(match.mons[i].type) {
                            case 1:
                            case 2:
                            case 4:
                                match.mons[i].state = -2;
                                break;
                            case 3:
                            case 5:
                                match.mons[i].state = 5;
                        }
                    }
                    moveMonster(i);
                }
            }
        }
    }
}

void loseGame();

void changeMusic() {
    if (!isMusic) return;
    short type = match.rooms[match.level][roomFinder(match.rooms[match.level], match.pos)].type;
    if (type == -1) type = 1;
    if (type != currentMusic) {
        switch (type) {
            case 0: break;
            case 1: {
                playMusic(tracks[match.music].dir);
                currentMusic = 1;
            } break;
            case 2: {
                playMusic(tracks[10].dir);
                currentMusic = 2;
            } break;
            case 3: {
                playMusic(tracks[3].dir);
                currentMusic = 3;
            } break;
            case 4: {
                playMusic(tracks[11].dir);
                currentMusic = 4;
            } break;
            case 5: {
                playMusic(tracks[5].dir);
                currentMusic = 5;
            } break;
        }
    }
}

void foodRot() {
    for (short i = 0; i < 5; i++) {
        if (match.foods[i].type == 0) continue;
        match.foods[i].state--;
        if (match.foods[i].state < 0) {
            switch (match.foods[i].type) {
                case 1: {
                    match.foods[i].type = 4;
                } break;
                case 2:
                case 3: {
                    match.foods[i].type = 1;
                    match.foods[i].state = 60;
                }
            }
        } 
    }
}

void moveTo(short y, short x, short skip) {
    death = 0;
    if (match.hunger < 30 && match.intermission <= 0) {
        if (match.heal > 0) match.health += 2;
        else match.health ++;
    }
    if (match.hunger > 140 - 10*match.difficulty) match.health--;
    match.hunger ++;
    match.intermission --;
    match.fast --;
    match.strength --;
    match.heal --;
    if (match.hunger > 180) match.hunger = 180;
    if (match.intermission < 0) match.intermission = 0;
    if (match.fast < 0) match.fast = 0;
    if (match.strength < 0) match.strength = 0;
    if (match.heal < 0) match.heal = 0;
    match.pos.y = y;
    match.pos.x = x;
    if (match.fast % 2 == 0) moveMonsters();
    short night = isNightmare();
    if (!isRoom()) {
        match.seen[match.level][y][x] = 1;
    }
    else if (!night) {
        short room = roomFinder(match.rooms[match.level], match.pos);
        if (match.seen[match.level][match.rooms[match.level][room].tl.y][match.rooms[match.level][room].tl.x] == 0) {
            match.seen[match.level][match.rooms[match.level][room].tl.y][match.rooms[match.level][room].tl.x] = 1;
            for (short i = 0; i < match.monss; i++) {
                if (match.mons[i].level == match.level + 1 && match.mons[i].room == room) {
                    match.mons[i].seen = 1;
                }
            }
        }
        if (match.rooms[match.level][room].type == 2) {
            match.health -= 2;
            death = -2;
        }
    }

    short temp = 0;
    switch (match.maps[match.level][y][x]) {
        case 9: {
            gPrompt("You found a hidden door!");
            match.maps[match.level][y][x] = 10;
        } break;
        case 14: {
            short r = roomFinder(match.rooms[match.level], match.pos);
            short mir = match.rooms[match.level][r].mirror;
            short type = match.rooms[match.level][r].lockDT;
            switch (type) {
                case 0: {} break;
                case 1: 
                case 2: revealCode(match.rooms[match.level][r].pwd, mir); break;
                default: {
                    time_t n = time(NULL);
                    if (n - seen > type) match.rooms[match.level][r].pwd = rand() % 10000;
                    seen = n;
                    revealCode(match.rooms[match.level][r].pwd, mir);
                }
            }
        } break;
        case 21: {
            gPrompt("You are caught by a trap!");
            battle();
            // match.health -= match.difficulty + 1;
            match.maps[match.level][y][x] = 22;
        } break;
        case 25:
        case 26:
        case 27:
        case 28: {
            if (!skip) {
                short state = 1;
                for (short i = 0; i < 5; i++) {
                    if (!match.foods[i].type) {
                        if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                        else {
                            // match.foods[i].type = 10;
                            match.foods[i].type = match.maps[match.level][y][x] - 24;
                            match.foods[i].state = 60;
                            gPrompt("You picked the food up.");
                        }
                        match.maps[match.level][y][x] = 7;
                        state = 0;
                        break;
                    }
                }
                if (state) {
                    gPrompt("You can't pick any more food, please eat the rest, first.");
                }
            }
        } break;
        case 29: temp = 20;
        case 30: {
            if (!skip) {
                if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                else {
                    temp = (temp) ? temp : 1;
                    int amount = 22 + rand() % (100 / temp) - temp;
                    match.gold += amount;
                    char message[100];
                    sprintf(message, "You picked up %d gold coins; use them wisely!", amount);
                    gPrompt(message);
                }
                match.maps[match.level][y][x] = 7;
            }
        } break;
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38: {
            if (!skip) {
                if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                else {
                    char guns[8][20] = {"the Mace", "the Sword", "10 Daggers", "8 Magical Wands", "20 Normal Arrows", "the Dagger", "the Magical Wand", "the Normal Arrow",};
                    char gunCount[8] = {1,1,10,8,20,1,1,1};
                    char mess[2][12] = {"picked up", "retreated"};
                    match.arm[(match.maps[match.level][y][x] < 36) ? (match.maps[match.level][y][x] - 31) :
                              (match.maps[match.level][y][x] - 34)] += gunCount[match.maps[match.level][y][x] - 31];
                    char message[100];
                    sprintf(message, "You warrior! You %s %s!", guns[match.maps[match.level][y][x] - 31], mess[(match.maps[match.level][y][x] > 35)?1:0]);
                    gPrompt(message);
                }
                match.maps[match.level][y][x] = 7;
            }
        } break;
        case 46:
        case 47:
        case 48: {
            if (!skip) {
                if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                else {
                    char els[5][10] = {"Health", "Speed", "Damage"};
                    match.elixir[match.maps[match.level][y][x] - 46]++;
                    char message[100];
                    sprintf(message, "Merlin put that elixir for you! You picked up the %s Elixir!", els[match.maps[match.level][y][x] - 46]);
                    gPrompt(message);
                }
                match.maps[match.level][y][x] = 7;
            }
        } break;     
        case 39: {
            if (!skip) {
                if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                else {
                    match.key++;
                    gPrompt("You have taken up an ancient key, marked by the guards of Gizza");
                }
                match.maps[match.level][y][x] = 7;
            }
        } break;
    }

    foodRot();
    changeMusic();
    printMap(match.maps[match.level], match.level, 0);
    if (match.health < 0) loseGame(); /* MUST BE UNCOMMENTED */
}

void fastmove(short x, short y) {
    short allowed[6] = {7,8,9,10,12,13}, state = 1;
    while (state) {
        state = 0;
        for (short i = 0; i < 6; i++) {
            if (match.maps[match.level][match.pos.y+y][match.pos.x+x] == allowed[i]) {
                state = 1;
                break;
            }
        }
        if (match.maps[match.level][match.pos.y+y][match.pos.x+x] == 11)
            state = match.maps[match.level][match.pos.y][match.pos.x] == 13;

        int counter = match.level == 5 ? 190:0;
        int moCount = match.level == 5 ? 210:match.monss;
        for (short i = counter; i < moCount; i++) {
            if (match.mons[i].level == match.level + 1) {
                if (match.pos.y+y == match.mons[i].pos.y &&
                    match.pos.x+x == match.mons[i].pos.x) {
                    state = 0;
                    break;
                }
            }    
        }

        if (state) moveTo(match.pos.y + y, match.pos.x + x, 0);
    }
}

void winGame();

void attack(short prev) {
    char strength[5] = {5,10,12,15,5};
    for (int i = 0; match.strength > 0 && i < 5; i++) strength[i] *= 2;
    char range[3] = {5,10,5};
    char li[5][30] = {"Deamon", "Fire Breathing Monstor", "Giant", "Snake", "Undeed"}; 
    switch (match.equArm) {
        case 1:
        case 2: {
            short state = 1;

            int counter = match.level == 5 ? 190:0;
            int moCount = match.level == 5 ? 210:match.monss;
            
            for (short i = counter; i < moCount; i++) {
                if (match.mons[i].level == match.level + 1 &&
                    abs(match.mons[i].pos.x-match.pos.x) <= 1 &&
                    abs(match.mons[i].pos.y-match.pos.y) <= 1 &&
                    match.mons[i].type) {
                    match.mons[i].health -= strength[match.equArm-1];
                    if (match.mons[i].health < 0) match.mons[i].health = 0;
                    char message[100];
                    sprintf(message, "You hit the %s, it's health is %d as of now!", li[match.mons[i].type-1], match.mons[i].health);
                    gPrompt(message);        
                    if (match.mons[i].health == 0) {
                        sprintf(message, "You killed the %s!", li[match.mons[i].type-1]);
                        
                        match.mons[i].type = 0;
                        gPrompt(message);        
                    
                    }
                    state = 0;
                }
            }
            if (state) gPrompt("No Monster Nearby!");
        } break;
        case 3:
        case 4:
        case 5: {
            if (!prev) {
                char c;
                char new = -1;
                while (new == -1) {
                    c = getch();
                    switch (c) {
                        case UA:
                        case 'j':
                        case 'J':
                        case '8':
                        // case '^[[A':
                            new = 0;
                            break;
                        case DA:
                        case 'k':
                        case 'K':
                        case '2':
                        // case '^[[B':
                            new = 1;
                            break;
                        case LA:
                        case 'h':
                        case 'H':
                        case '4':
                        // case '^[[D':
                            new = 2;
                            break;
                        case RA:
                        case 'l':
                        case 'L':
                        case '6':
                        // case '^[[C':
                            new = 3;
                            break;
                        case '7':
                        case 'y':
                        case 'Y':
                        // case '^[[H': 
                            new = 4;
                            break;
                        case '9':
                        case 'u':
                        case 'U':
                        // case '^55~':
                            new = 5;
                            break;
                        case 'b':
                        case 'B':
                        case '1':
                        // case '^[[F':
                            new = 6;
                            break;
                        case '3':
                        case 'N':
                        case 'n':
                        // case '^[[6~':
                            new = 7;
                            break;
                        case 'a': {
                            new = match.lastShot;
                        } break;
                        case ES: {
                            return;
                        }
                    }
                }
                match.lastShot = new;
            }
            char x = match.pos.x;
            char y = match.pos.y;
            char xs[8] = {0,0,-1,1,-1,1,-1,1};
            char ys[8] = {-1,1,0,0,-1,-1,1,1};
            char ranges[3] = {5,10,5}; 
            char state = 1, state2 = 1;
            while (state2 && ranges[match.equArm-3]--) {
                y += ys[match.lastShot];
                x += xs[match.lastShot];
                switch(match.maps[match.level][y][x]) { 
                    case  0: case  1: case  2: case  3: case  4: case  5: case  6: case 20: case 11: {
                        state2 = 0;
                        y -= ys[match.lastShot];
                        x -= xs[match.lastShot];
                    } break;
                    default: {
                        int counter = match.level == 5 ? 190:0;
                        int moCount = match.level == 5 ? 210:match.monss;
                        for (short i = counter; i < moCount; i++) {
                            if (match.level + 1 == match.mons[i].level &&
                                y == match.mons[i].pos.y &&
                                x == match.mons[i].pos.x &&
                                match.mons[i].type !=  0)
                            {
                                match.mons[i].health -= strength[match.equArm-1];
                                if (match.mons[i].health < 0) match.mons[i].health = 0;
                                char message[100];
                                sprintf(message, "You hit the %s, it's health is %d as of now!", li[match.mons[i].type-1], match.mons[i].health);
                                gPrompt(message);        
                                if (match.mons[i].health == 0) {
                                    sprintf(message, "You killed the %s!", li[match.mons[i].type-1]);
                                    match.mons[i].type = 0;
                                    gPrompt(message);
                                }
                                state = 0;
                                state2 = 0;

                                if (match.equArm == 4) {
                                    match.mons[i].state = 0;
                                }
                            }
                        }
                    }
                }
            }
            match.arm[match.equArm - 1]--;
            if (match.arm[match.equArm - 1] == 0) {
                match.equArm = 0;
                gPrompt("You're out of shot!");
            }
            if (state) match.maps[match.level][y][x] = match.equArm + 33;
        }
    }
    short state = 1;
    if (match.level == 4) {
        for (short i = 0; i < match.monss; i++) {
            if (match.mons[i].level == 5 && match.mons[i].type != 0) {
                state = 0;
                break;
            }
        }
    } else {
        state = 0;
    }
    if (state) winGame();

}

void foodMenu(short i) {
    initSCR();
    
    timer = 5;
    char temp[20];
    char li[5][10] = {"No", "Regular", "Excellent", "Magical", "Regular"};
    struct button butts[5];
    
    for (short i = 0; i < 5; i++) {
        sprintf(temp, "%s Food", li[match.foods[i].type]);
        butts[i].type = 0;
        strcpy(butts[i].label, temp);
        sprintf(temp, "%hd", i+1);
        strcpy(butts[i].value, temp);
        butts[i].state = 0;
    }
    butts[i].state = 1;
    
    short state = 1, active = i;
    while (state) {
        int ind = menu(butts, 5, 1, active, "Choose food to eat!",1);
        switch (ind) {
            case 0: { 
                clear();
                printMap(match.maps[match.level], match.level, 0);
                return;
            } break;
            // case 1:{
            //     if (match.food[0]) {
            //         match.food[0]--;
            //         match.hunger -= 1 + match.difficulty;
            //         match.health += 3 - match.difficulty;
            //         if (match.hunger < 0) match.hunger = 0;
            //         Prompt("You ate regular food!");
            //         foodMenu(i);
            //         clear();
            //         return;
            //     } else {
            //         Prompt("You have no regular food to eat!");
            //     }
            // } break;
            default: { 
                switch (match.foods[ind-1].type) {
                    case 0:
                        Prompt("No food here!");
                        break;
                    case 1: {
                        match.foods[ind-1].type = 0;
                        match.health += 40;
                        match.hunger -= 30;
                        Prompt("You ate a Regular Food!");
                    } break;
                    case 2: {
                        match.foods[ind-1].type = 0;
                        match.health += 40;
                        match.hunger -= 30;
                        match.strength += 15;
                        Prompt("You ate an Excellent Food!");
                    } break;
                    case 3: {
                        match.foods[ind-1].type = 0;
                        match.health += 40;
                        match.hunger -= 30;
                        match.fast += 20;
                        Prompt("You ate a Magical Food!");
                    } break;
                    case 4: {
                        match.foods[ind-1].type = 0;
                        match.health -= 10;
                        Prompt("Ewe... that food was rotten");
                    } break;
                }
                strcpy(butts[ind-1].label, "No Food");
            }
        }
        active = ind - 1;
    }
}

void armMenu() {
    initSCR();
    
    timer = 5;
    char temp[80];
    char li[5][21] = {"        Mace        ", "       Sword        ", "      Dagger       ", "     Magic Wand     ", "    Normal Arrow    "};
    char liPrint[5][21] = {"Mace", "Sword", "Dagger", "Magic Wand", "Normal Arrow"};
    char li2[4][40] = {" ", "Short-Ranged Weapon Strength Count", " ", "Long-Ranged Weapon Strenth Range Count"};
    char strength[5] = {5,10,12,15,5};
    char range[3] = {5,10,5}; 
    struct button butts[10];
    char buttss[6] = {0,3,4,7,8,9};
    char buttss2[4] = {1,2,5,6};
     
    for (short i = 0; i < 6; i++) {
        if (i > 2) sprintf(temp, "%s %4hd     %2hd    %2hd   ", li[i-1], strength[i-1], range[i-3], match.arm[i-1]);
        else if (i) sprintf(temp, "%s %4hd     %2hd   ", li[i-1], strength[i-1], match.arm[i-1]>0);
        else sprintf(temp, "Unwield");
        butts[buttss[i]].type = 0;
        strcpy(butts[buttss[i]].label, temp);
        sprintf(temp, "%hd", i+1);
        strcpy(butts[buttss[i]].value, temp);
        butts[buttss[i]].state = 0;
    }
    for (short i = 0; i < 4; i++) {
        butts[buttss2[i]].type = 5;
        strcpy(butts[buttss2[i]].label, li2[i]);
        butts[buttss2[i]].state = 0;
    }

    

    butts[match.equArm].state = 1;
    
    short state = 1, active = buttss[match.equArm];
    while (state) {
        int ind = menu(butts, 10, 1, active, "Select Weapon To Wield!", 1);
        if (ind > 1) {
            if (match.arm[ind-2]) {
                match.equArm = ind-1;
                sprintf(temp, "The %s is now wielded!", liPrint[ind-2]);
                Prompt(temp);
            } else {
                Prompt("You have not any of this weapon!");
            }
            active = buttss[ind-1];
        } else if (ind == 1) {
            match.equArm = 0;
            Prompt("You are unarmed as of now");
            active = 0;
        } else {
            clear();
            printMap(match.maps[match.level], match.level, 0);
            return;
        }
    }
}
void elixirMenu(short i) {
    initSCR();
    
    timer = 5;
    char temp[40];
    char li[3][15] = {"Health", "Speed", "Damage"};
    struct button butts[3];
    
    for (short i = 0; i < 3; i++) {
        sprintf(temp, "%s: %hd", li[i], match.elixir[i]);
        butts[i].type = 0;
        strcpy(butts[i].label, temp);
        sprintf(temp, "%hd", i+1);
        strcpy(butts[i].value, temp);
        butts[i].state = 0;
    }
    butts[i].state = 1;
    
    short state = 1, active = i;
    while (state) {
        int ind = menu(butts, 3, 1, active, "Select Elixir To Drink!",1);
        if (ind) {
            if (match.elixir[ind-1]) {
                match.elixir[ind-1]--;
                sprintf(temp, "The %s Elixir Is Drunk!", li[ind-1]);
                switch (ind) {
                    case 1:
                        match.heal += 30;
                        break;
                    case 2:
                        match.fast += 20;
                        break;
                    case 3:
                        match.strength += 15;
                }
                clear();
                Prompt(temp);
                elixirMenu(ind-1);
                return;
            } else {
                Prompt("You have not any of this elixir!");
                active = ind - 1;
            }
        } else {
            clear();
            printMap(match.maps[match.level], match.level, 0);
            return;
        }
    }
}

short checkFilename(char name[]) {
    if (name[0] == '\0') {
        Prompt("Filename empty!");
        return 0;
    }
    if (strlen(name) > 12) {
        Prompt("Filename must be less than 13 characters!");
        return 0;
    } 
    for (short i = 0; name[i] != '\0'; i++) {
        if(!((name[i] >= 'A' && name[i] <= 'Z') || 
             (name[i] >= 'a' && name[i] <= 'z') ||
             (name[i] >= '0' && name[i] <= '9') ||
             (name[i] >= 'a' && name[i] <= 'z') ||
             (name[i] == '_'))) {
            Prompt("Filename must only contain \"0-9\", \"a-z\", \"A-Z\" & \"_\"!");
            return 0;
        }
    }
    return 1;
}

void saveFile(char name[]) {
    if (checkFilename(name)) {
        char dir[40];
        sprintf(dir, "games_saved/%s$%s.bin", player.username, name);
        FILE *fptr = fopen(dir, "wb");
        if (!fptr) {
            Prompt("Error aqquired while saving the game, please change the filename!");
            fclose(fptr);
            return;
        }
        else if (fwrite(&match, sizeof(struct game), 1, fptr)) {
            FILE *log;
            log = fopen("games_saved/games.txt", "r");
            char line[100], state = 1;
            while (state && fgets(line, 50, log)) {
                line[strcspn(line, "\n")] = 0;

                if (*line == 0) continue;
                char *username, *id;
                // sscanf("%s %s", username, id);
                username = strtok(line, " ");
                id = strtok(NULL, " ");
                // sscanf(line, "%s %s", username, id);
                // char c[100];
                // sprintf(c, "%s %s %d", username, id, 5);
                // Prompt(c);
                // while (true) {
                //     char c = getch();
                //     if (c) break;
                // }
                // sprintf(line, "%s %s %d %d", username, id, 5, strcmp(id, name)==0);
                // Prompt(line);
                // while (1) {
                //     char c = getch();
                //     if (c) break;
                // }
                if (strcmp(username, player.username)==0 && strcmp(id, name)==0) {
                    state = 0;
                    Prompt("Game Updated & Saved Successfully!");
                    // break;
                }
                
            }
            fclose(log);
            if (state) {
                log = fopen("games_saved/games.txt", "a");
                char line[50];
                sprintf(line, "%s %s", player.username, name);
                if (fprintf(log, "\n%s %s", player.username, name)) {
                    Prompt("Game saved successfully!");
                    fclose(log);
                    fclose(fptr);
                    return;
                }
                fclose(log);
            } else {
                return;
            }
        }
        fclose(fptr);
        Prompt("Unknown Error; play a bit longer and retry!");
        
    }
}

void pauseMenu() {
    initSCR();
    
    timer = 5;
    char temp[40];
    struct button butts[4];
    strcpy(butts[0].label, "Resume...");
    butts[0].type = 0;
    butts[0].state = 1;
    strcpy(butts[0].value, "1");
    short i = 1;
    if (!player.anonymous) {
        strcpy(butts[1].label, "Enter the Name to Save");
        butts[1].type = 1;
        butts[1].state = 0;
        butts[1].check = *(checkFilename);
        strcpy(butts[2].label, "Save!");
        butts[2].type = 0;
        butts[2].state = 0;
        strcpy(butts[2].value, "2");
        i = 3;
        initInp(butts[1].value, 100);
    }
    strcpy(butts[i].label, "Help");
    butts[i].type = 0;
    butts[i].state = 0;
    strcpy(butts[i].value, "3");
    i++;
    strcpy(butts[i].label, "Exit");
    butts[i].type = 0;
    butts[i].state = 0;
    strcpy(butts[i].value, "4");
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, i+1, 1, active, "Game Paused!",1);
        switch (ind) {
            case 1:
            case 0: {
                clear();
                printMap(match.maps[match.level], match.level,0);
                return;
            } break;
            case 2: {
                saveFile(butts[1].value);
                active = 2;
            } break;
            case 3: {
                help();
            } break;
            case 4: {
                if (confirm()) {
                    endwin();
                    death = -1;
                    loseGame();
                    return;
                }
            }
        }
    }
}
int calcPoint() {
    return match.gold * (1 + match.difficulty) * (1 + match.difficulty) * (1 + match.difficulty);
}

void treasureRoom() {
    match.pos.x = MAXx/2;
    match.pos.y = MAXy/2;
    for (short i = 0; i < match.monss; i++) {
    if (match.mons[i].level == match.level + 1) {
            match.mons[i].seen = 1;
        }
    }
}
void mainMenu();

void deleteSave() {
    if (*fileId == 0) return;
    FILE *fptr, *temp;
    fptr = fopen("games_saved/games.txt", "r");
    temp = fopen("games_saved/temp.txt", "w");
    char line[200], user[20], id[30];
    short i = 0;
    while (fgets(line, 200, fptr)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == 0) continue;
        i++;
        sscanf(line, "%s %s", user, id);
        if (strcmp(player.username, user) || strcmp(fileId, id))  {
            line[strlen(line)] = '\n';
            fputs(line, temp);
        }
    }
    fclose(fptr);
    fclose(temp);
    
    fptr = fopen("games_saved/games.txt", "w");
    temp = fopen("games_saved/temp.txt", "r");
    while (fgets(line, 200, temp)) {
        fputs(line, fptr);
    }
    fclose(temp);
    fclose(fptr);
    sprintf(line, "games_saved/%s$%s.bin", player.username, fileId);
    remove(line);
}

void loseGame() {
    if (death != -1) deleteSave();
    if (isMusic) playMusic(tracks[match.music].dir);
    endwin();
    initSCR();
    char c, line[118];
    char ascii[31][101] = {
        "          ________________________                                  _                              ",
        "         /                        \\                                ...                             ",
        "        /                          \\                             ;::::;                            ",
        "       /           R.I.P.           \\                          ;::::; :;                           ",
        "      /      ~~~~~~~~~~~~~~~~~       \\                       ;:::::'   :;                          ",
        "     |    /|\\                 /|\\     |                     ;:::::;     ;.                         ",
        "     |     |                   |      |                    ,:::::'       ;           OOO\\          ",
        "     |     |    Beloved Soul   |      |                    ::::::;       ;          OOOOO\\         ",
        "     |     |                   |      |                    ;:::::;       ;         OOOOOOOO        ",
        "     |     |                   |      |                   ,;::::::;     ;'         / OOOOOOO       ",
        "     |     |                   |      |                  ;:::::::::`. ,,,;.       /  / DOOOOOO     ",
        "     |     |     Gone but      |      |                .';:::::::::::::::::;,    /  /     DOOOO    ",
        "     |     |  Never Forgotten  |      |               ,::::::;::::::;;;;::::;,  /  /        DOOO   ",
        "     |     |                   |      |              ;::::::'::::::;;;::::: ,# /  /        DOOOO   ",
        "     |     |      <Death>      |      |              ::::::::;::::::;;::: ;::#/  /          DOOO   ",
        "     |     |___________________|      |              :::::::::;:::::::: ;::::#  /            DOO   ",
        "     |   *      *       *      *      |              ::::::::`;:::::: ;::::::# /             DOO   ",
        "     |     ***        ***        o    |               ::::::::::;; ;:::::::::##               OO   ",
        "     |  o  ***  o  o  ***     o    o  |               :::::::::::;::::::::;:::#               OO   ",
        "     |     ***        ***        o    |               :::::::::::::::::;'`:;::#               O    ",
        "     |   *      *       *      *      |                :::::::::::::;' /  / `:#                    ",
        "     |________________________________|                 ::::::`:::::;'/  /    `#                   ",
        "      /        \\   _|_|_   /         \\                                                             ",
        "     /          \\ /     \\ /           \\                                                            ",
        "    /            |       |             \\                                                           ",
        "   /             |       |              \\                                                          ",
        "  /______________|_______|_______________\\                                                         ",
        "          ~~~~~~           ~~~~~~                                                                  ",
        "          ~~~~               ~~~~                                                                  ",
        "          ~                     ~                                                                  "
    };
    short x, y;
    getmaxyx(stdscr, y, x);
    for (short i = 0; i < y; i++) {
        for (short j = 0; j < x; j++) {
            mvprintw(i,j, " ");
        }
    }
    for (short i = 0; i < 30; i++) {
        mvprintw(y/2 - 15 + i, x/2 - 50, "%s", ascii[i]);
    }
    mvprintw(y/2 - 6, x/2 - 29 - strlen(player.username)/2, "%s", player.username);
    switch (death) {
        case (-2):
            sprintf(line, "Avada Kedavra...");
            break;
        case (-1):
            sprintf(line, "Commited Suicide...");
            break;
        case ( 0):
            sprintf(line, "Starved to Death...");
            break;
        default: {
            char mons[5][10] = {"Deamon", "Dragon", "Giant", "Snake", "Undeed"};
            sprintf(line, "Murdered by %s!", mons[death-1]);
        }
    }
    mvprintw(y/2 - 1, x/2 - 29 - strlen(line)/2, "%s", line);
    sprintf(line, "Thanks for playing! Game Is Over, Not Your Life, Though! Press Enter To Go Back To The Start Menu");
    mvprintw(y/2 + 18, (x-strlen(line))/2, "%s", line);
    while (1) {
        c = getch();
        if (c == ' ') {
            clear();
            endwin();
            mainMenu();
            return;
        }
    }
}

void winGame() {
    if (isMusic) playMusic(tracks[match.music].dir);
    deleteSave();
    char chest[20][81] = {
        "*******************************************************************************",
        "          |                   |                  |                     |        ",
        " _________|________________.=\"\"_;=.______________|_____________________|_______ ",
        "|                   |  ,-\"_,=\"\"     `\"=.|                  |                   ",
        "|___________________|__\"=._o`\"-._        `\"=.______________|___________________",
        "          |                `\"=._o`\"=._      _`\"=._                     |        ",
        " _________|_____________________:=._o \"=._.\"_.-=\"'\"=.__________________|_______ ", 
        "|                   |    __.--\" , ; `\"=._o.\" ,-\"\"\"-._ \".   |                   ",
        "|___________________|_._\"  ,. .` ` `` ,  `\"-._\"-._   \". '__|___________________",
        "          |           |o`\"=._` , \"` `; .\". ,  \"-._\"-._; ;              |        ",
        " _________|___________| ;`-.o`\"=._; .\" ` '`.\"\\` . \"-._ /_______________|_______ ",
        "|                   | |o;    `\"-.o`\"=._``  '` \" ,__.--o;   |                   ",
        "|___________________|_| ;     (#) `-.o `\"=.`_.--\"_o.-; ;___|___________________",
        "____/______/______/___|o;._    \"      `\".o|o_.--\"    ;o;____/______/______/____",
        "/______/______/______/_\"=._o--._        ; | ;        ; ;/______/______/______/_",
        "____/______/______/______/__\"=._o--._   ;o|o;     _._;o;____/______/______/____",
        "/______/______/______/______/____\"=._o._; | ;_.--\"o.--\"_/______/______/______/_",
        "____/______/______/______/______/_____\"=.o|o_.--\"\"___/______/______/______/____",
        "/______/______/______/______/______/______/______/______/______/______/______/_",
        "*******************************************************************************"
    };
    short x, y;
    getmaxyx(stdscr, y, x);
    for (short i = 0; i < x; i++) {
        for (short j = 0; j < y; j++) {
            mvprintw(j,i," ");
        }
    }
    for (short i = 0; i < 20; i++) {
        mvprintw(y/2-20+i, x/2-40, "%s", chest[i]);
    }
    if (player.anonymous) {
        endwin();
        initSCR();
        char c, line[118];
        sprintf(line, "Thanks for playing! The game is ended, press Space to see the ranking!");
        mvprintw(y/2+5, (x-strlen(line))/2, "%s", line);
        sprintf(line, "You're not ranked since you're not logged in!");
        mvprintw(y/2+7, (x-strlen(line))/2, "%s", line);
        
        
        while (1) {
            c = getch();
            if (c == ' ') {
                clear();
                endwin();
                Ranking();
                return;
            }
        }
    } else {
        FILE *fptr, *temp;
        fptr = fopen("data/users.txt", "r");
        temp = fopen("data/temp.txt", "w");
        char username[30], email[30], password[30];
        int points, golds;
        unsigned long long time;
        short matches, i = 0;
        char line[200], user[20];
        fgets(line, 200, fptr);
        fputs(line, temp);
        while (fgets(line, 200, fptr)) {
            i++;
            if (line[0] == '\0') continue;
            sscanf(line, "%s %s %s %d %d %hd %llu", username, email, password, &points, &golds, &matches, &time);
            if (strcmp(player.username, username) == 0) {
                points += calcPoint();
                golds += match.gold;
                matches++;
                sprintf(line, "%s %s %s %d %d %hd %llu\n", username, email, password, points, golds, matches, time);
            }
            fputs(line, temp);
        }
        fclose(fptr);
        fclose(temp);
        

        endwin();
        initSCR();
        char c;
        short x, y;
        getmaxyx(stdscr, y, x);
        sprintf(line, "Thanks for playing! The game is ended, press Space to see the ranking!");
        mvprintw(y/2 + 5, (x-strlen(line))/2, "%s", line);
        
        
        fptr = fopen("data/users.txt", "w");
        temp = fopen("data/temp.txt", "r");
        while (fgets(line, 200, temp)) {
            fputs(line, fptr);
        }
        fclose(temp);
        fclose(fptr);
        while (1) {
            c = getch();
            if (c == ' ') {
                clear();
                endwin();
                Ranking();
                return;
            }
        }
    }
}



short bplay() {
    char prev;
    while (1) {
        short i = 0, j = 0;
        // usleep(gDELAY);
        gendTime();
        char c = getch();
        if (c) {
            switch(c) {
                case ES: {
                    clear();
                    pauseMenu();
                    return bplay();
                } break;
                // case '~': exit(0); /*DELETED*/
                case 't':
                case 'T': {
                    tipn = rand() % (TIPS - 1) + 1;
                    printTip();
                } break;
                case UA:
                case 'j':
                case 'J':
                case '8':
                // case '^[[A':
                    j = -2;
                case DA:
                case 'k':
                case 'K':
                case '2':
                // case '^[[B':
                    j++;
                case LA:
                case 'h':
                case 'H':
                case '4':
                // case '^[[D':
                    i = -2;
                case RA:
                case 'l':
                case 'L':
                case '6':
                // case '^[[C': 
                {
                    i++;
                    if (j != 0) i++;
                    if (prev == 'f' || prev == 'F') fastmove(i,j);
                    else if (moveCheck(match.maps[match.level], match.pos.y + j, match.pos.x + i, match.pos.y, match.pos.x)) {
                        wrong = 0;
                        moveTo(match.pos.y + j, match.pos.x + i, prev == 'g' || prev == 'G');
                    } else {
                        if (wrong == 1) gPrompt("Impossible move!");
                        else wrong = 1;
                    }
                } break;

                case '7':
                case 'y':
                case 'Y':
                // case '^[[H': 
                    i = -2;
                case '9':
                case 'u':
                case 'U':
                // case '^55~':
                {
                    i += 2;
                    j -= 2;
                }
                case 'b':
                case 'B':
                case '1':
                // case '^[[F': 
                    i -= 2;
                case '3':
                case 'N':
                case 'n':
                // case '^[[6~':
                {
                    i++;
                    j++;
                    if (prev == 'f' || prev == 'F') fastmove(i,j);
                    else if (moveCheck(match.maps[match.level], match.pos.y + j, match.pos.x + i, match.pos.y, match.pos.x)) {
                        moveTo(match.pos.y + j, match.pos.x + i, prev == 'g' || prev == 'G');
                        wrong = 0;
                    } else {
                        if (wrong == 1) gPrompt("Impossible move!");
                        else wrong = 1;
                    }
                } break;
                case 'a': {
                    if (match.equArm) attack(1);
                    else gPrompt("You have no weapon wielded!");
                } break;
                case ' ': {
                    if (match.equArm) {
                        attack(0);
                        break;
                    }
                }
                case '5': {
                    moveTo(match.pos.y, match.pos.x, 0);
                    i = 1;
                }
                case 'E':
                case 'e': {
                    clear();
                    foodMenu(i);
                } break;
                case 'i':
                case 'I': {
                    clear();
                    armMenu();
                } break;
                case 'O':
                case 'o': {
                    clear();
                    elixirMenu(0);
                } break;
                
            }
        }
        // if (c==27) {endwin(); break;}
        if (c) prev = c;
        printMap(match.maps[match.level], match.level, 1);
        short state = 1;
        for (short i = 190; i < 210; i++) {
            if (match.mons[i].type) state = 0;
        } 
        if (state) return 1;
    }
}

short gplay() {
    // battle();
    char prev;
    while (1) {
        short i = 0, j = 0;
        // usleep(gDELAY);
        gendTime();
        char c = getch();
        if (c) {
            switch(c) {
                case ES: {
                    clear();
                    pauseMenu();
                    return gplay();
                } break;
                case 't':
                case 'T': {
                    tipn = rand() % (TIPS - 1) + 1;
                    printTip();
                } break;
                case 'M':
                case 'm': {
                    if (match.difficulty == 2) gPrompt("You're not a noob to cheat via seeing the map! Discover it yourself!");
                    else {
                        while (c=='M' || c=='m') {
                            c = getch();
                            if (c == 0) break;
                            printMap(match.maps[match.level], match.level, 1);
                        }
                        clear();
                        printMap(match.maps[match.level], match.level, 0);

                    }
                } break;
                case 's':
                case 'S': {
                    while (c=='s' || c=='S') {
                        c = getch();
                        if (c == 0) break;
                        printMap(match.maps[match.level], match.level, 2);
                    }
                    clear();
                    printMap(match.maps[match.level], match.level, 0);
                } break;
                case UA:
                case 'j':
                case 'J':
                case '8':
                // case '^[[A':
                    j = -2;
                case DA:
                case 'k':
                case 'K':
                case '2':
                // case '^[[B':
                    j++;
                case LA:
                case 'h':
                case 'H':
                case '4':
                // case '^[[D':
                    i = -2;
                case RA:
                case 'l':
                case 'L':
                case '6':
                // case '^[[C': 
                {
                    i++;
                    if (j != 0) i++;
                    if (prev == 'f' || prev == 'F') fastmove(i,j);
                    else if (moveCheck(match.maps[match.level], match.pos.y + j, match.pos.x + i, match.pos.y, match.pos.x)){
                        wrong = 0;
                        moveTo(match.pos.y + j, match.pos.x + i, prev == 'g' || prev == 'G');
                    } else {
                        if (wrong == 1) gPrompt("Impossible move!");
                        else wrong = 1;
                    }
                } break;

                case '7':
                case 'y':
                case 'Y':
                // case '^[[H': 
                    i = -2;
                case '9':
                case 'u':
                case 'U':
                // case '^55~':
                {
                    i += 2;
                    j -= 2;
                }
                case 'b':
                case 'B':
                case '1':
                // case '^[[F': 
                    i -= 2;
                case '3':
                case 'N':
                case 'n':
                // case '^[[6~':
                {
                    i++;
                    j++;
                    if (prev == 'f' || prev == 'F') fastmove(i,j);
                    else if (moveCheck(match.maps[match.level], match.pos.y + j, match.pos.x + i, match.pos.y, match.pos.x)) {
                        wrong = 0;
                        moveTo(match.pos.y + j, match.pos.x + i, prev == 'g' || prev == 'G');
                    } else {
                        if (wrong == 1) gPrompt("Impossible move!");
                        else wrong = 1;
                    }
                } break;
                case 'a': {
                    if (match.equArm) attack(1);
                    else gPrompt("You have no weapon wielded!");
                } break;
                case ' ': {
                    if (match.equArm) {
                        attack(0);
                        break;
                    }
                }
                case '5': {
                    moveTo(match.pos.y, match.pos.x, 0);
                    i = 1;
                }
                case '<': {

                    if (match.maps[match.level][match.pos.y][match.pos.x] == 24) {
                        match.level --;
                        gPrompt("Going to the last floor...");
                    }
                    else if (!i) gPrompt("No stairs here so far!");
                    if (!i) j = 1;
                }
                case '>': {
                    if (j) {}
                    else if (match.maps[match.level][match.pos.y][match.pos.x] == 23 || match.maps[match.level][match.pos.y][match.pos.x] == 45) {
                        match.level ++;
                        if (match.level == 4) {
                            clear();
                            // endwin();
                            treasureRoom();
                            // winGame();
                            // while (1) {}
                            // return 0;
                        }
                        short room = roomFinder(match.rooms[match.level], match.pos);
                        match.seen[match.level][match.rooms[match.level][room].tl.y][match.rooms[match.level][room].tl.x] = 1;
                        for (short i = 0; i < match.monss; i++) {
                            if (match.mons[i].level == match.level + 1 && match.mons[i].room == room) {
                                match.mons[i].seen = 1;
                            }
                        }
                        gPrompt("Going to the next floor...");

                    }
                    else if (!i) gPrompt("No stairs here so far!");
                } break;
                case 'E':
                case 'e': {
                    clear();
                    foodMenu(i);
                } break;
                case 'i':
                case 'I': {
                    clear();
                    armMenu();
                } break;
                case 'O':
                case 'o': {
                    clear();
                    elixirMenu(0);
                } break;
                
            }
        }
        // if (c==27) {endwin(); break;}
        if (c) prev = c;
        printMap(match.maps[match.level], match.level, 0);
    }
}


void setNewGame() {
    Prompt("Initializing Map...");
    time_t a = time(NULL);
    while (1) {
        time_t b = time(NULL);
        if (b-a > 1) break;
    }
    clear();
    initThemes();   

    mapCreator();
    short r = roomFinder(match.rooms[0], match.pos);
    match.seen[0][match.rooms[0][r].tl.y][match.rooms[0][r].tl.x] = 1;
    for (short i = 0; i < match.monss; i++) {
        if (match.mons[i].level == 1 && match.mons[i].room == r) {
            match.mons[i].seen = 1;
        }
    }
    currentMusic = match.rooms[0][r].type;

    printMap(match.maps[match.level], match.level, 0);
    gplay();

}