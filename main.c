/* By Roham Ghasemi Qomi; The Rogue; v:2.0.1*/
#include "map.c"

void mainMenu();
void loginForm();


void inits() {
    setlocale(LC_ALL, "en_US.UTF-8");
    sleep(1);
    srand(time(NULL));
    initscr();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    noecho();

    player.anonymous = 1;
    strcpy(player.username, "Guest");
    match.music = 7;
    short i = 0;
    strcpy(tracks[i].name, "N/A");
    strcpy(tracks[i].dir, "");
    strcpy(tracks[i].author, "");
    i++;
    strcpy(tracks[i].name, "The Grand Score");
    strcpy(tracks[i].dir, "tracks/Overture.mp3");
    strcpy(tracks[i].author, "Alexander Nakarada");
    i++;
    strcpy(tracks[i].name, "Forest Walk");
    strcpy(tracks[i].dir, "tracks/Forest_Walk.mp3");
    strcpy(tracks[i].author, "Alexander Nakarada");
    i++;
    strcpy(tracks[i].name, "In Dreams");
    strcpy(tracks[i].dir, "tracks/In_Dreams.mp3");
    strcpy(tracks[i].author, "Scott Buckly");
    i++;
    strcpy(tracks[i].name, "Chase");
    strcpy(tracks[i].dir, "tracks/Chase.mp3");
    strcpy(tracks[i].author, "Alexander Nakarada");
    i++;
    strcpy(tracks[i].name, "Powerful Trap Beat");
    strcpy(tracks[i].dir, "tracks/Powerful_Trap.mp3");
    strcpy(tracks[i].author, "Alex Productions");
    i++;
    strcpy(tracks[i].name, "Sneaky Snitch");
    strcpy(tracks[i].dir, "tracks/Sneaky_Snitch.mp3");
    strcpy(tracks[i].author, "Kevin MacLeod");
    i++;
    strcpy(tracks[i].name, "Hitman");
    strcpy(tracks[i].dir, "tracks/Hitman.mp3");
    strcpy(tracks[i].author, "Kevin MacLeod");
    i++;
    strcpy(tracks[i].name, "Motivation");
    strcpy(tracks[i].dir, "tracks/Motivation.mp3");
    strcpy(tracks[i].author, "Sakura Girl");
    i++;
    strcpy(tracks[i].name, "La Camracito");
    strcpy(tracks[i].dir, "tracks/La_Campracito.mp3");
    strcpy(tracks[i].author, "Roham Ghasemi");
    i++;
    strcpy(tracks[i].name, "Asian Graveyard");
    strcpy(tracks[i].dir, "tracks/Asian_Graveyard.mp3");
    strcpy(tracks[i].author, "Darren Curtis");
    i++;
    strcpy(tracks[i].name, "Adventure");
    strcpy(tracks[i].dir, "tracks/Adventure.mp3");
    strcpy(tracks[i].author, "Alexander Nakarada");
    i++;
    strcpy(tracks[i].name, "Shekasteh");
    strcpy(tracks[i].dir, "tracks/Shekasteh.mp3");
    strcpy(tracks[i].author, "Parviz Meshkatian");
    i++;
    strcpy(tracks[i].name, "Ke Khordam Az DahanBandi...");
    strcpy(tracks[i].dir, "tracks/Ke_Khordam.mp3");
    strcpy(tracks[i].author, "Homayoun Shajarian");
    i++;
    // playMusic(tracks[0].dir);
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
    short d;
    sscanf(r, "%hd", &d);
    if (*tracks[d].dir) playMusic(tracks[d].dir);
    else freeAudio();
    isMusic = *tracks[d].dir != 0; 
    match.music = d;
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
        int ind = menu(butts, 5, 1, active, "Sign Up Form",1);
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
                            printMenu(butts, "Sign Up Form", 5, -1);
                            time_t now = time(NULL);
                            while (1) {
                                if (time(NULL) > now + 1) break;
                                usleep(1000);
                            }
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
        int ind = menu(butts, 4, 1, active, "Password Finder Form",1);
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
        int ind = menu(butts, 4, 1, active, "Login Form",1);
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
                    printMenu(butts, "Login Form", 4, -1);
                    time_t now = time(NULL);
                    while (1) {
                        if (time(NULL) > now + 1) break;
                        usleep(1000);
                    }
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
    /* Core Dumps when there's no file to load*/
    initSCR();
    
    timer = 5;
    short i = 0;
    
    FILE *fptr;
    fptr = fopen("games_saved/games.txt", "r");
    char line[30], user[30], ver[30], files[100][30];
    while (fgets(line, 30, fptr)) {
        if (*line == '\n') continue;

        sscanf(line, "%s %s", user, ver);
        if (!strcmp(user, player.username)) {
            
            strcpy(files[i++],ver);
        }
    }
    fclose(fptr);
    files[i][0] = 0;
    struct button butts[100];
    for (short j = 0; j < i; j++) {
        butts[j].type = 0;
        sprintf(butts[j].label, "Game Id %s", files[j]);
        butts[j].state = 0;
        sprintf(butts[j].value, "%hd", j+1);
    }
    butts[0].state = 1;
    
    short state = 1, active = 0;
    if (i == 0) {
        butts[i].type = 0;
        strcpy(butts[i].label, "No File To Load...");
        strcpy(butts[i].value, "0");
        i++;
    }
    while (state) {
        int ind = menu(butts, i, 1, active, "Continue Game Id: ",1);
        if (ind == 0) {
            clear();
            endwin();
            mainMenu();
            return;
        }
        FILE *fptr;
        char dir[60];
        sprintf(dir, "games_saved/%s$%s.bin", player.username, files[ind-1]);
        fptr = fopen(dir, "rb");
        fread(&match, sizeof(struct game), 1, fptr);
        fclose(fptr);
        Prompt("Loading Game...");  
        strcpy(fileId, files[ind-1]);
        sleep(2);
        endwin();
        initSCR();
    
        initThemes();
        printMap(match.maps[match.level], match.level, 0);    // mvprintw(1,1,"%c",c);
        gplay();
        return;
    }
}

void printRanking(struct gamers li[], short offset, short self) {
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
        attron(A_UNDERLINE);
        attron(A_ITALIC);
        short type = 1;
        switch (i) {
            case 0: {                
                attron(COLOR_PAIR(101));
            } break;
            case 1: {
                attron(COLOR_PAIR(102));

            } break;
            case 2: {
                attron(COLOR_PAIR(103));
            } break;
            default: {
                attroff(A_UNDERLINE);
                attroff(A_ITALIC);
                if (i%2) attron(COLOR_PAIR(104));
                else attron(COLOR_PAIR(105));
                type = 0;
            }
        }
        if (i == self) {
            attron(A_BOLD);
            attron(A_ITALIC);
            attron(A_STANDOUT);
            for (short j = 0; j<col; j++) {
                mvprintw(3+i-offset, j, " ");
            }
        }
        switch (i) {
            case 0: {                
                mvprintw(3+i-offset,1,"%s", "\U0001F3C6");
            } break;
            case 1: {
                mvprintw(3+i-offset,1,"\U0001F948");

            } break;
            case 2: {
                mvprintw(3+i-offset,1,"\U0001F949");

            } break;
        }
        mvprintw(3+i-offset,0,"%d",i+1);
        if (type) {
            char upper[26][5] = {"Ⓐ","Ⓑ","Ⓒ","Ⓓ","Ⓔ","Ⓕ","Ⓖ","Ⓗ","Ⓘ","Ⓙ","Ⓚ","Ⓛ","Ⓜ",
                                 "Ⓝ","Ⓞ","Ⓟ","Ⓠ","Ⓡ","Ⓢ","Ⓣ","Ⓤ","Ⓥ","Ⓦ","Ⓧ","Ⓨ","Ⓩ"};
            char lower[26][5] = {"ⓐ","ⓑ","ⓒ","ⓓ","ⓔ","ⓕ","ⓖ","ⓗ","ⓘ","ⓙ","ⓚ","ⓛ","ⓜ",
                                 "ⓝ","ⓞ","ⓟ","ⓠ","ⓡ","ⓢ","ⓣ","ⓤ","ⓥ","ⓦ","ⓧ","ⓨ","ⓩ"};
            char digit[10][5] = {"⓪","①","②","③","④","⑤","⑥","⑦","⑧","⑨"};
            char under[5] = "␥";
            for (short j = 0; li[i].username[j] != '\0'; j++) {
                if      (li[i].username[j] >= 'A' && li[i].username[j] <= 'Z') mvprintw(3+i-offset,7+j,"%s", upper[li[i].username[j]-'A']);
                else if (li[i].username[j] >= 'a' && li[i].username[j] <= 'z') mvprintw(3+i-offset,7+j,"%s", lower[li[i].username[j]-'a']);
                else if (li[i].username[j] >= '0' && li[i].username[j] <= '9') mvprintw(3+i-offset,7+j,"%s", digit[li[i].username[j]-'0']);
                else if (li[i].username[j] == '_')                             mvprintw(3+i-offset,7+j,"%s", under);
                else mvprintw(3+i-offset,7+j,"%c", li[i].username[j]);
            }
        } 
        else mvprintw(3+i-offset,7,"%s", li[i].username);
        mvprintw(3+i-offset,40,"%d", li[i].points);
        mvprintw(3+i-offset,50,"%d", li[i].golds);
        mvprintw(3+i-offset,60,"%hd", li[i].matches);
        mvprintw(3+i-offset,70,"%ld", li[i].time/60/60/24);
        attroff(A_ITALIC);
        attroff(A_BOLD);
        attroff(A_STANDOUT);
        attroff(A_UNDERLINE);
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
    fgets(line, 200, fptr);
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
    short j = -1;
    if (1-player.anonymous) {
        for (; j < i; j++) {
            if (!strcmp(player.username, li[j].username)) {
                break;
            }
        }
    }
    for (short i = 0; i < 3; i++) {
        for (short j = 0; li[i].username[j] != '\0'; j++) {
            // if (li[i].username[j] >= 'A' && li[i].username[j] <= 'Z') li[i].username[j] += '\u24B6' - 'A';

            // li[i].username[j] += 65296 - '0';
        }
    }
    strcat(li[0].username, " the Greatest");
    strcat(li[1].username, " the Greater");
    strcat(li[2].username, " the Great");
    short offset = 0, row = getmaxy(stdscr);
    while (1) {
        printRanking(li, offset, j);
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
    butts[0].currentOption = match.colour;
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
    butts[1].currentOption = match.difficulty;
    strcpy(butts[1].list[0], "Noob");
    strcpy(butts[1].list[1], "So-So");
    strcpy(butts[1].list[2], "Legend");
    butts[1].check = *chooseDifficulty;
    butts[2].type = 4;
    strcpy(butts[2].label, "Background Music: ");
    butts[2].state = 0;
    butts[2].options = MUSICS;
    butts[2].currentOption = match.music;
    for (short i = 0; i < MUSICS; i++) {
        strcpy(butts[2].list[i], tracks[i].name);
    }
    butts[2].check = *chooseTrack;
    
    
    short state = 1, active = 0;
    while (state) {
        int ind = menu(butts, 3, 1, active, "Settings: ",1);
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

    while (1) {}
}



void mainMenu() {
    initSCR();
    int x, y;
    getmaxyx(stdscr, y, x);
    while (x < MAXx+Left-3 || y <Top + Bottom + MAXy-3) {
        mvprintw(y/2, x/2 - 25, "Your window is too small, stretch it & press Enter");
        while (1) {
            char c = getch();
            if (c == ' ') break;
        }
        getmaxyx(stdscr, y, x);
        clear();
    }
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
        int ind = menu(butts, i+1, 1, active, "Main Menu",1);
        switch (ind) {
            case 0: {
                if (confirm()) {
                    clear();
                    endwin();
                    printf("Thanks for playing our game! Hope to see you again, soon!");
                    exit(0);
                    return;
                }
            } break;
            case 1:{
                setNewGame();
                return;
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
    initAudio(tracks[match.music].dir); 
    mainMenu();   

    freeAudio();
    return 0;
}