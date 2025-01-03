/* Version: 0.1.0*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void play(const char *dir) {
    if (dir == NULL || strlen(dir) == 0) {
        return;
    }
    char command[200];
    sprintf(command, "mpg123 -q \"%s\"", dir);
    int result = system(command);

}
