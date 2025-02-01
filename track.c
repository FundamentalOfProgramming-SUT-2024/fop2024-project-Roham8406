/* Ver 1.6.5 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <unistd.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct track {
    char name[20];
    char dir[30]; 
};


Mix_Music *Music;

int initAudio(char dir[]) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return 0;
    }
    Music = Mix_LoadMUS(dir);
    if (!Music) {
        return 0;
    }
    if (Mix_PlayMusic(Music, -1) == -1) {
        return 0;
    }
    return 1;
}

void freeAudio() {
    Mix_FreeMusic(Music);
    Mix_CloseAudio();
    SDL_Quit();
}

void playMusic(char dir[]) {
    freeAudio();
    initAudio(dir);
}