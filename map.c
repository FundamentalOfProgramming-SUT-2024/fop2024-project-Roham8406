/* ver: 0.2.1 */
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
#include <math.h>
// #include <unistd.h>
// #include <ncurses.h>
#include "menus.c"




// #define UA 3
// #define DA 2
// #define LA 4
// #define RA 5
// #define ES 27
// #define BS 7 

// #define init_color(a,b,c,d) init_color(a,b*4,c*4,d*4)










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
    32 Dagger
    33 Magic Wand
    34 Normal Arrow
    35 Sword
    36 Health Elixir
    37 Speed Elixir
    38 Damage Elixir
    39 Ancient Key
    */
} obj;



char gamePrompt[100];
time_t now;
int gtimer = 1;
int gDELAY = 100000;

void delgPrompt() {
    gamePrompt[0] = '\0';
    int row, col;
    getmaxyx(stdscr, row, col);
    for (int i = 0; i < col; i++){
        mvprintw(0, i, " ");
    }
}
void gPrompt(char txt[]) {
    delgPrompt();
    now = time(NULL);
    strcpy(gamePrompt, txt);
    int row, col;
    getmaxyx(stdscr, row, col);
    attron(COLOR_PAIR(3));
    mvprintw(0, 4, "%s", gamePrompt);
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
        for (short j = rooms[i].tl.x + 1; j < rooms[i].br.x; j++) {
            map[rooms[i].tl.y][j] = 5;
            map[rooms[i].br.y][j] = 5;
        }
        for (short j = rooms[i].tl.y + 1; j <= rooms[i].br.y; j++) {
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
        // switch (rooms[i].doors[2].type) {
        //     case 1:
        //         map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '+';
        //         break;
        //     case 3:
        //         map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '?';
        //         break;
        //     case 4:
        //         map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '@';
        //         break;
        //     case 5:
        //         map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '!';
        //         break;
        // }
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

void connect(room rooms[12], short ind) {
    if (rooms[ind].state == 2) return;
    rooms[ind].state = 2;
    for (short i = 0; i < rooms[ind].neiC; i++) {
        connect(rooms, rooms[ind].nei[i]);
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
    connect(rooms, 0);
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
    if (level == 4) if (isIn(rooms[5].tl, *stair, rooms[5].br)) rooms[4].type = 4; else rooms[5].type = 4;

    
    for (short i = 0; i < 12 && rooms[i].state; i++) {
        short piap = 9 - 3*match.difficulty;
        short food = 3 + 3*match.difficulty;
        short gold = 3 + 2*match.difficulty;
        short armo = 3 + 3*match.difficulty;
        short elix = 5 + 2*match.difficulty;
        short aKey = 7 + 2*match.difficulty;

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
        }
        if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 25;
        if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 25;
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
        }
        if (rand() % armo == 0) {
            int sym;
            switch (rand() % 5) {
                case 0: sym = 31; break;
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
                case 0: sym = 36; break;
                case 1: sym = 37; break;
                case 2: sym = 38; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rand() % elix == 0) {
            int sym;
            switch (rand() % 3) {
                case 0: sym = 36; break;
                case 1: sym = 37; break;
                case 2: sym = 38; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rooms[i].type == 2) {
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 36; break;
                    case 1: sym = 37; break;
                    case 2: sym = 38; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 36; break;
                    case 1: sym = 37; break;
                    case 2: sym = 38; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 36; break;
                    case 1: sym = 37; break;
                    case 2: sym = 38; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 36; break;
                    case 1: sym = 37; break;
                    case 2: sym = 38; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }

        }
        if (rand() % aKey == 0) map[x + rand() % length][y + rand() % width] = 39;

        
    }
    
    if (level) map[stair->y][stair->x] = 24;
    short r;
    do {r = (level == 4) ? 5 : rand() % k;}
    while(0);
    // while (isIn(rooms[k].tl, *stair, rooms[k].br) && rooms[k].type != 4); /* Sometimes falls in infinite loop */
    stair->x = rooms[r].tl.x + 2 + rand()%(rooms[r].br.x - rooms[r].tl.x - 3);
    stair->y = rooms[r].tl.y + 2 + rand()%(rooms[r].br.y - rooms[r].tl.y - 3);
    map[stair->y][stair->x] = 23;
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
        }
    }
        

    return sum;
}

void printMap(char map[MAXy][MAXx], short level, short cheat) {
    for (short i = 0; i < MAXy; i++) {
        for (short j = 0; j < MAXx; j++) {
            mvprintw(i+1,j, " ");
        }
    }
    char li[5][15] = {"Mace", "Dagger", "Magic Wand", "Normal Arrow", "Sword"};

    char invents[MAXx];
    sprintf(invents, "Gold: %d\tAncient Key: %d\tBroken Key: %d\t%s: %d\tHealth: %d\tHunger: %d", match.gold, match.key, match.brKey, li[match.equArm], match.arm[match.equArm], match.health, match.hunger);
    attron(COLOR_PAIR(4));
    mvprintw(1, 4, "%s", invents);
    attron(A_BOLD);
    for (short i = 0; i < MAXy; i++) {
        for (short j = 0; j < MAXx; j++) {
            short reveal = cheat == 1;
            int ch, cr, cr2 = -1, type;
            pair temp = {j, i};
            type = roomFinder(match.rooms[level], temp);
            
            if (type != -1) type = match.rooms[level][type].type;

            switch (type) {
                case (-1): {
                    if (match.seen[match.level][i][j]) reveal = 1;
                    if (abs(i - match.pos.y) + abs(j - match.pos.x) < 5  && 
                    (!isRoom() || match.maps[match.level][match.pos.y][match.pos.x] == 8 ||
                    match.maps[match.level][match.pos.y][match.pos.x] == 10 ||
                    match.maps[match.level][match.pos.y][match.pos.x] == 12 )) reveal = 1;
                } break;
                case 3: {
                    short room = roomFinder(match.rooms[level], match.pos);
                    if (abs(i - match.pos.y) < 3 && abs(j - match.pos.x) < 3 &&
                        match.rooms[match.level][room].type == 3) reveal = 1; 
                } break;
                default: {
                    short room = roomFinder(match.rooms[level], temp);
                    if (match.seen[match.level][match.rooms[match.level][room].tl.y][match.rooms[match.level][room].tl.x]) reveal = 1;
                }
            }
            if (reveal) {
                // if ((abs(match.pos.y - i) > 5 || abs(match.pos.x - j) > 5) && cheat == 2) cheat = 0;
                /* Incapable of showing near traps and hidden doors*/
                switch(map[i][j]) {
                    case 0: ch = ' '; cr2 = 26; break;
                    case 5: ch = '_'; cr = 0; break;
                    case 6: ch = '|'; cr = 0; break;
                    case 7: ch = '.'; cr = 1; break;
                    case 8: ch = '+'; cr = 0; break;
                    // case 9: ch = '?'; cr = 0; break;
                    case 9: ch = cheat == 2 ? '?' : (map[i-1][j] == 6 || map[i+1][j] ==  6) ? '|' : '_'; cr = 0; break;
                    case 10: ch = '?'; cr = 0; break;
                    case 11: ch = '@'; cr2 = 27; break;
                    case 12: ch = '@'; cr2 = 28; break;
                    case 13: ch = '#'; cr2 = 29; break;
                    case 14: ch = '&'; cr = 5; break;
                    case 20: ch = 'O'; cr = 0; break;
                    case 21: ch = cheat == 2 ? '^' : '.'; cr = 1; break;
                    case 22: ch = '^'; cr = 1; break;
                    case 23: ch = '>'; cr = 2; break;
                    case 24: ch = '<'; cr = 2; break;
                    case 25:
                    case 26:
                    case 27:
                    case 28: ch = 'F'; cr = 6; break;
                    case 29: ch = 'G'; cr = 4; break;
                    case 30: ch = 'B'; cr = 3; break;
                    case 31: ch = 'M'; cr = 5; break;
                    case 32: ch = 'D'; cr = 5; break;
                    case 33: ch = 'W'; cr = 5; break;
                    case 34: ch = 'A'; cr = 5; break;
                    case 35: ch = 'S'; cr = 5; break;
                    case 36: ch = 'H'; cr = 7; break;
                    case 37: ch = 'X'; cr = 7; break;
                    case 38: ch = 'E'; cr = 7; break;
                    case 39: ch = 'K'; cr = 5; break;

                    defualt: ch = ' '; cr2 = 26; break;

                }
            } else {
                ch = ' '; cr2 = 26;
            }
            if (cr2 != -1){
                attron(COLOR_PAIR(cr2));
            } else {
                attron(COLOR_PAIR(cr+type*10 + 20));
            }
            mvprintw(i+2,j,"%c", ch);
        }
    }
    attron(COLOR_PAIR(80 + match.colour));
    mvprintw(2+match.pos.y, match.pos.x, "!");
    attroff(A_BOLD);

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
    
    for (short i = 0; i < 4; i++) {
        for (short j = 0; j < col; j++) {
            for (short k = 0; k < row; k++) {
                match.maps[i][k][j] = 0;
            }
        }
        floorRandomizer(match.rooms[i], match.maps[i], i + 1, &first, &stair, match.seen[i]);
    }
    match.health = 100;
    match.hunger = 0;
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

    /* Enchanted Rooms */
    init_pair(40, COLOR_MAGENTA, 10);        //Walls
    init_pair(41, COLOR_MAGENTA, 10);        //Floor  
    init_pair(42, COLOR_WHITE, COLOR_MAGENTA); //Stairs
    init_pair(43, COLOR_BLACK, 10);          //Black Gold
    init_pair(44, COLOR_YELLOW, 10);         //Gold
    init_pair(45, 11, 10);                   //Armour
    init_pair(46, 12, 10);                   //Food
    init_pair(47, COLOR_MAGENTA, 10);        //Elixir

    /* Nightmare Rooms */
    init_pair(50, COLOR_BLACK, COLOR_MAGENTA);        //Walls
    init_pair(51, COLOR_BLACK, COLOR_MAGENTA);        //Floor  
    init_pair(52, COLOR_BLACK, COLOR_RED);            //Stairs
    init_pair(53, COLOR_BLACK, COLOR_MAGENTA);        //Black Gold
    init_pair(54, COLOR_YELLOW, COLOR_MAGENTA);       //Gold
    init_pair(55, 11, COLOR_MAGENTA);                 //Armour
    init_pair(56, 12, COLOR_MAGENTA);                 //Food
    init_pair(57, COLOR_MAGENTA, COLOR_MAGENTA);      //Elixir

    /* Treasure Room */
    init_pair(60, COLOR_YELLOW, 10);        //Walls
    init_pair(61, COLOR_MAGENTA, 10);        //Floor  
    init_pair(62, COLOR_WHITE, COLOR_MAGENTA); //Stairs
    init_pair(63, COLOR_BLACK, 10);          //Black Gold
    init_pair(64, COLOR_YELLOW, 10);         //Gold
    init_pair(65, 11, 10);                   //Armour
    init_pair(66, 12, 10);                   //Food
    init_pair(67, COLOR_MAGENTA, 10);        //Elixir

    /* HERO */
    init_pair(80, COLOR_RED, 10);
    init_pair(81, COLOR_BLUE, 10);
    init_pair(82, COLOR_MAGENTA, 10);
    init_pair(83, COLOR_CYAN, 10);
    init_pair(84, COLOR_GREEN, 10);
    init_pair(85, COLOR_WHITE, 10);
    


}



short moveCheck(char map[MAXy][MAXx], short y, short x, short cy, short cx) {
    switch (map[y][x]) {
        case  0: case  1: case  2: case  3: case  4: case  5: case  6: case 20: return 0;
        case 11: return map[cy][cx] == 13;
        default: return 1; 
    }
}

void moveTo(short y, short x, short skip) {
    match.pos.y = y;
    match.pos.x = x;
    short night = isNightmare();
    if (!isRoom()) {
        match.seen[match.level][y][x] = 1;
    }
    else if (!night) {
        short room = roomFinder(match.rooms[match.level], match.pos);
        match.seen[match.level][match.rooms[match.level][room].tl.y][match.rooms[match.level][room].tl.x] = 1;
    }
    short temp = 0;
    switch (match.maps[match.level][y][x]) {
        case 9: {
            gPrompt("You found a hidden door!");
            match.maps[match.level][y][x] = 10;
        } break;
        case 14: {
            /*Revealing the code*/
        } break;
        case 21: {
            gPrompt("You are caught by a trap!");
            match.health -= match.difficulty + 1;
            match.maps[match.level][y][x] = 22;
        } break;
        case 25:
        case 26:
        case 27:
        case 28: {
            if (!skip) {
                short sum = 0;
                for (short i=0; i < 4; i++) sum += match.food[i];
                if (sum >= 5) gPrompt("You can't pick any more food, please eat the rest, first.");
                else {
                    if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                    else {
                        match.food[match.maps[match.level][y][x] - 25]++;
                        gPrompt("You picked the food up.");
                    }
                    match.maps[match.level][y][x] = 7;
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
        case 35: {
            if (!skip) {
                if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                else {
                    char guns[5][15] = {"Mace", "Dagger", "Magical Wand", "Normal Arrow", "Sword"};
                    match.arm[match.maps[match.level][y][x] - 31]++;
                    char message[100];
                    sprintf(message, "You warrior! You picked up the %s!", guns[match.maps[match.level][y][x] - 31]);
                    gPrompt(message);
                }
                match.maps[match.level][y][x] = 7;
            }
        } break;
        case 36:
        case 37:
        case 38: {
            if (!skip) {
                if(night) gPrompt("HAHAHAHA! I'm seeing a nightmare, a nightmare I could not bear!");
                else {
                    char els[5][10] = {"Health", "Speed", "Damage"};
                    match.elixir[match.maps[match.level][y][x] - 36]++;
                    char message[100];
                    sprintf(message, "Merlin put that elixir for you! You picked up the %s Elixir!", els[match.maps[match.level][y][x] - 36]);
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
    printMap(match.maps[match.level], match.level, 0);
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
        if (state) moveTo(match.pos.y + y, match.pos.x + x, 0);
    }
}

void foodMenu(short i) {
    initSCR();
    
    timer = 5;
    char temp[20];
    char li[4][10] = {"Regular", "Excellent", "Magical", "Rotten"};
    struct button butts[4];
    
    for (short i = 0; i < 4; i++) {
        sprintf(temp, "%s Food: %hd", li[i], match.food[i]);
        butts[i].type = 0;
        strcpy(butts[i].label, temp);
        sprintf(temp, "%hd", i+1);
        strcpy(butts[i].value, temp);
        butts[i].state = 0;
    }
    butts[i].state = 1;
    
    short state = 1, active = i;
    while (state) {
        int ind = menu(butts, 4, 1, active, "Choose food to eat!");
        switch (ind) {
            case 0: { 
                clear();
                printMap(match.maps[match.level], match.level, 0);
                return;
            } break;
            case 1:{
                if (match.food[0]) {
                    match.food[0]--;
                    match.hunger -= 1 + match.difficulty;
                    match.health += 3 - match.difficulty;
                    if (match.hunger < 0) match.hunger = 0;
                    Prompt("You ate regular food!");
                    foodMenu(i);
                    clear();
                    return;
                } else {
                    Prompt("You have no regular food to eat!");
                }
            } break;
            /* Other food is not available in this phase*/
        }
        active = ind - 1;
    }
}

void armMenu() {
    initSCR();
    
    timer = 5;
    char temp[35];
    char li[5][15] = {"Mace", "Dagger", "Magic Wand", "Normal Arrow", "Sword"};
    struct button butts[5];
    
    for (short i = 0; i < 5; i++) {
        sprintf(temp, "%s: %hd", li[i], match.arm[i]);
        butts[i].type = 0;
        strcpy(butts[i].label, temp);
        sprintf(temp, "%hd", i+1);
        strcpy(butts[i].value, temp);
        butts[i].state = 0;
    }
    butts[match.equArm].state = 1;
    
    short state = 1, active = match.equArm;
    while (state) {
        int ind = menu(butts, 5, 1, active, "Select Weapon To Wield!");
        if (ind) {
            if (match.arm[ind-1]) {
                match.equArm = ind-1;
                sprintf(temp, "The %s is now weilded!", li[ind-1]);
                Prompt(temp);
            } else {
                Prompt("You have not any of this weapon!");
            }
            active = ind - 1;
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
        int ind = menu(butts, 3, 1, active, "Select Elixir To Drink!");
        if (ind) {
            if (match.elixir[ind-1]) {
                match.elixir[ind-1]--;
                sprintf(temp, "The %s Elixir Is Drunk!", li[ind-1]);
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

void pauseMenu() {
    initSCR();
    
    timer = 5;
    char temp[40];
    struct button butts[4];
    strcpy(butts[0].label, "Resume...");
    butts[0].type = 0;
    butts[0].state = 1;
    strcpy(butts[0].value, "1");
    strcpy(butts[1].label, "Enter the Name to Save");
    butts[1].type = 1;
    butts[1].state = 0;
    butts[1].check = *(checkFilename);
    strcpy(butts[2].label, "Save!");
    butts[2].type = 0;
    butts[2].state = 0;
    strcpy(butts[2].value, "2");
    strcpy(butts[3].label, "Exit");
    butts[3].type = 0;
    butts[3].state = 0;
    strcpy(butts[3].value, "3");
    initInp(butts[1].value, 100);
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, 4, 1, active, "Game Paused!");
        switch (ind) {
            case 1:
            case 0: {
                clear();
                printMap(match.maps[match.level], match.level,0);
                return;
            } break;
            case 2: {
                // saveFile(butts);
                active = 2;
            } break;
            case 3: {
                if (confirm()) {
                    endwin();
                    abort();
                    printf("Thanks for playing");
                    return;
                }
            }
        }
    }
}



short gplay() {
    char prev;
    while (1) {
        short i = 0, j = 0;
        // usleep(gDELAY);
        // gendTime();
        char c = getch();
        if (c) {
        switch(c) {
            case ES: {
                clear();
                pauseMenu();
                return gplay();
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
                else if (moveCheck(match.maps[match.level], match.pos.y + j, match.pos.x + i, match.pos.y, match.pos.x))
                    moveTo(match.pos.y + j, match.pos.x + i, prev == 'g' || prev == 'G');
                else gPrompt("Impossible move!");
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
                else if (moveCheck(match.maps[match.level], match.pos.y + j, match.pos.x + i, match.pos.y, match.pos.x))
                    moveTo(match.pos.y + j, match.pos.x + i, prev == 'g' || prev == 'G');
                else gPrompt("Impossible move!");
            } break;
            case '5':
            case ' ': {
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
                else if (match.maps[match.level][match.pos.y][match.pos.x] == 23) {
                    match.level ++;
                    if (match.level == 4) {
                        clear();
                        // endwin();
                        mvprintw(1,1,"g%d f%d a%d a%d a%d a%d a%d e%d e%d e%d h%d k%d", match.gold, match.food[0], match.arm[0],
                        match.arm[1], match.arm[2], match.arm[3], match.arm[4], match.elixir[0],
                        match.elixir[1], match.elixir[2], match.health, match.key);
                        // while (1) {}
                        return 0;
                    }
                    short room = roomFinder(match.rooms[match.level], match.pos);
                    match.seen[match.level][match.rooms[match.level][room].tl.y][match.rooms[match.level][room].tl.x] = 1;

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

            // for (short i = 0; i < 4; i++) {
            //     printMap(match.maps[i]);
            //     printf("----------");
            // }
        }
        if (c==27) {endwin(); break;}
        if (c) prev = c;
        printMap(match.maps[match.level], match.level, 0);
    }
}


int main() {
    srand(time(NULL));
    mapCreator();

    initscr();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    noecho();

    start_color();
    initThemes();   
    gplay();

    return 0;
}