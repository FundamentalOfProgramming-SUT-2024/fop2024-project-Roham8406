/* ver: 0.1.0 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAXx 150
#define MAXy 40

struct game {
    short colour;
    short difficulty;
    short music;
    char maps[4][MAXy][MAXx];
    char seen[4][MAXy][MAXx];
};

struct game match;

typedef struct {
    short x;
    short y;
} pair;

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
    short nei[11];
    short neiC;
} room;

typedef struct {
    short type;
    pair pos;
    /*
    10 Pile
    11 Hidden Trap
    12 Known Trap
    13 Upstairs
    14 Downstairs
    15 Normal Food
    16 Exp. Food
    17 Magical Food
    18 Rotten food
    19 Gold
    20 Black Gold
    21 Mace
    22 Dagger
    23 Magic Wand
    24 Normal Arrow
    25 Sword
    26 Health Elixir
    27 Speed Elixir
    28 Damage Elixir
    */
} obj;

void printMap(char [2 * MAXy][MAXx]);

short isIn(pair a, pair b, pair c) {
    if (b.x >= a.x && b.x <= c.x && b.y >= a.y && b.y <= c.y) return 1;
    return 0;
}
void insertMap(char map[MAXy][MAXx], room rooms[12]) {

    for (short i = 0; i < MAXy; i++) {
        for (short j = 0; j < MAXx; j++) {
            map[i][j] = 0;
        }
    }

    for (short i = 0; i < 12 && rooms[i].state; i++) {
        for (short j = rooms[i].tl.x + 1; j < rooms[i].br.x; j++) {
            map[rooms[i].tl.y][j] = '_';
            map[rooms[i].br.y][j] = '_';
        }
        for (short j = rooms[i].tl.y + 1; j <= rooms[i].br.y; j++) {
            map[j][rooms[i].tl.x] = '|';
            map[j][rooms[i].br.x] = '|';
        }
        for (short j = rooms[i].tl.x + 1; j < rooms[i].br.x; j++) {
            for (short k = rooms[i].tl.y + 1; k < rooms[i].br.y; k++) {
                map[k][j] = '.';
            }
        }

        map[rooms[i].doors[0].pos.y][rooms[i].doors[0].pos.x] = '+';
        switch (rooms[i].doors[1].type) {
            case 1:
                map[rooms[i].doors[1].pos.y][rooms[i].doors[1].pos.x] = '+';
                break;
            case 2:
                map[rooms[i].doors[1].pos.y][rooms[i].doors[1].pos.x] = '?';
                break;
            case 4:
                map[rooms[i].doors[1].pos.y][rooms[i].doors[1].pos.x] = '@';
                break;
            case 5:
                map[rooms[i].doors[1].pos.y][rooms[i].doors[1].pos.x] = '!';
                break;
        }
        switch (rooms[i].doors[2].type) {
            case 1:
                map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '+';
                break;
            case 3:
                map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '?';
                break;
            case 4:
                map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '@';
                break;
            case 5:
                map[rooms[i].doors[2].pos.y][rooms[i].doors[2].pos.x] = '!';
                break;
        }
    }
}

void pointFinder(pair ptr, short *x, short *y) {
    *x = (ptr.x/(MAXx/9) - 1)/2;
    *y = (ptr.y/(MAXy/10) - 1)/3;

}

void markWay(char main[MAXy][MAXx], int map[MAXy][MAXx], pair end) {
    short j = end.y, i = end.x;
    if (map[j][i] == 1) return;
    main[j][i] = '#';
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
                if (map[ny[k]][nx[k]] > 0) {
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
    for (short i = 0; rooms[i].state && i < 12; i++) {
        if (isIn(rooms[i].tl, pnt, rooms[i].br)) return i;
    }
    return -1;
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

int floorRandomizer(room rooms[12], char map[MAXy][MAXx], short level, room *first, pair *stair) {
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
                    rooms[k].tl.x = (2*i+1)*(col/9) + rand() % (col/9 - 5);
                    rooms[k].tl.y = (3*j+1)*(row/10) + rand() % (2*(row/10) - 5);
                    rooms[k].br.x = 5 + rooms[k].tl.x + rand() % (col/9 - 5 - rooms[k].tl.x + (2*i+1)*(col/9));
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
                    rooms[k].doors[2].type = rand() % 10 ? 1 : 4;
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
    if (k < 6 || drs%2 || drs < 2*k-2) {return floorRandomizer(rooms, map, level, first, stair);}
    insertMap(map, rooms);
    for (short i = 0; i < 36; i++) {
        if (doors[i].state) {
            if (!doorFinder(doors, i, rooms, map)) return floorRandomizer(rooms, map, level, first, stair);
        }
    }
    connect(rooms, 0);
    if (!isConnected(rooms)) return floorRandomizer(rooms, map, level, first, stair);


    for (short i = 0; i < 36 && doors[i].end != -1; i++) {
        if (doors[i].type == 2) {
            if (rooms[doors[i].end].doors[1].type == 2 || rooms[doors[i].end].doors[1].type == 0) {
                rooms[doors[i].end].type = 2;
            }
        }
    }
    for (short i = 0; i < 12 && rooms[i].state; i++) {
        if (rooms[i].state == 1 && rand() % 3 == 0) rooms[i].type = 3; 
    }
    if (level == 4) if (isIn(rooms[5].tl, *stair, rooms[5].br)) rooms[4].type = 4; else rooms[5].type = 4;

    
    for (short i = 0; i < 12 && rooms[i].state; i++) {
        short piap = 9 - 3*match.difficulty;
        short food = 3 + 3*match.difficulty;
        short gold = 3 + 2*match.difficulty;
        short armo = 3 + 3*match.difficulty;
        short elix = 5 + 2*match.difficulty;

        short y = rooms[i].tl.x + 1;
        short x = rooms[i].tl.y + 1;

        short width = (rooms[i].br.x - rooms[i].tl.x - 1);
        short length  = (rooms[i].br.y - rooms[i].tl.y - 1);
        if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 'O';
        if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 'T';
        if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 'T';
        if (rooms[i].type == 4) {
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 'T';
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 'T';
            if (rand() % piap == 0) map[x + rand() % length][y + rand() % width] = 'T';
        }
        if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 'F';
        if (rand() % food == 0) map[x + rand() % length][y + rand() % width] = 'F';
        if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
        if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
        if (rooms[i].type == 4) {
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
            if (rand() % gold == 0) map[x + rand() % length][y + rand() % width] = 'G';
            if (rand() % (gold*7) == 0) map[x + rand() % length][y + rand() % width] = 'B';
        }
        if (rand() % armo == 0) {
            int sym;
            switch (rand() % 5) {
                case 0: sym = 'Q'; break;
                case 1: sym = 'W'; break;
                case 2: sym = 'E'; break;
                case 3: sym = 'R'; break;
                case 4: sym = 'Y'; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rand() % elix == 0) {
            int sym;
            switch (rand() % 3) {
                case 0: sym = 'H'; break;
                case 1: sym = 'S'; break;
                case 2: sym = 'D'; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rand() % elix == 0) {
            int sym;
            switch (rand() % 3) {
                case 0: sym = 'H'; break;
                case 1: sym = 'S'; break;
                case 2: sym = 'D'; break;
            }
            map[x + rand() % length][y + rand() % width] = sym;
        }
        if (rooms[i].type == 2) {
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 'H'; break;
                    case 1: sym = 'S'; break;
                    case 2: sym = 'D'; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 'H'; break;
                    case 1: sym = 'S'; break;
                    case 2: sym = 'D'; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 'H'; break;
                    case 1: sym = 'S'; break;
                    case 2: sym = 'D'; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }
            if (rand() % elix == 0) {
                int sym;
                switch (rand() % 3) {
                    case 0: sym = 'H'; break;
                    case 1: sym = 'S'; break;
                    case 2: sym = 'D'; break;
                }
                map[x + rand() % length][y + rand() % width] = sym;
            }

        }
        
    }
    
    if (level != 1) map[stair->y][stair->x] = '>';
    short r;
    do {r = (level == 4) ? 5 : rand() % k;}
    while(0);
    // while (isIn(rooms[k].tl, *stair, rooms[k].br) && rooms[k].type != 4); /* Sometimes falls in infinite loop */
    stair->x = rooms[r].tl.x + 1 + rand()%(rooms[r].br.x - rooms[r].tl.x - 1);
    stair->y = rooms[r].tl.y + 1 + rand()%(rooms[r].br.y - rooms[r].tl.y - 1);
    map[stair->y][stair->x] = '<';
    *first = rooms[r];
        

    return sum;
}

void printMap(char map[MAXy][MAXx]) {
    for (short i = 0; i < MAXy; i++) {
        for (short j = 0; j < MAXx; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

void mapCreator() {
    room rooms[4][12];
    short col = MAXx, row = MAXy;
    room first;
    pair stair;
    first.tl.x = (col/9) + rand() % (col/9 - 5);
    first.tl.y = (row/10) + rand() % (2*(row/10) - 5);
    first.br.x = 5 + first.tl.x + rand() % (2*col/9 - 5 - first.tl.x);
    first.br.y = 5 + first.tl.y + rand() % (3*(row/10) - 5 - first.tl.y);
    for (short i = 0; i < 4; i++) {
        floorRandomizer(rooms[i], match.maps[i], i, &first, &stair);
    }
}


int main() {
    srand(time(NULL));
    
    mapCreator();
    
    for (short i = 0; i < 4; i++) {
        printMap(match.maps[i]);
        printf("----------");
    }
    
    return 0;
}