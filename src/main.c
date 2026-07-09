#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#define MINIAUDIO_IMPLEMENTATION
#include "../depend/miniaudio.h"

unsigned long get_dir(char ***dirs){
    char *home = getenv("HOME");
    char *adr = malloc(strlen("/Music/playlist")+strlen(home)+1);
    strcpy(home, adr);
    strcat(adr, "/Music/playlist");
    DIR *dir = opendir("/home/noobedy/Music/playlist");
    if(dir == NULL){
        perror("directory open fail");
        *dirs = NULL;
        return 0;
    }
    *dirs = NULL;
    struct dirent *entry;
    unsigned long size = 0;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] == '.')
            continue;
        size++;
        *dirs = realloc(*dirs, size * sizeof(char*));
        (*dirs)[size-1] = strdup(entry->d_name);
    }
    closedir(dir);
    for(unsigned long i = 0; i < size; i++){
        char *buff = malloc(strlen("/home/noobedy/Music/playlist/")+strlen((*dirs)[i])+1);
        sprintf(buff,"/home/noobedy/Music/playlist/%s", (*dirs)[i]);
        free((*dirs)[i]);
        (*dirs)[i] = buff;
    }
    return size;
}

char key_pressed(){
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(STDIN_FILENO+1, &set, NULL, NULL, &timeout) > 0;
}

int main(){
    //configur varaible setup
    char running = 1, *music_adrass, ui;
    unsigned long randindex, played_size=0;

    //init engine
    ma_sound sound;
    ma_engine engine;
    if(ma_engine_init(NULL, &engine) != MA_SUCCESS){
        perror("init error");
        return 1;
    }

    //setup playlist
    char **playlist;
    unsigned long playlist_size = get_dir(&playlist);

    //setup playlist pointers
    char *played = malloc(playlist_size);

    //generating random seed
    srand(time(NULL));

    while(running){
        replay:
        //clear played if needed
        if(played_size == playlist_size){
            played_size = 0;
        }

        //choosing song index
        randindex = rand() % playlist_size;
        for(unsigned long i = 0; i < played_size; i++){
            if(played[i] == randindex){
                goto replay; 
            }
        }
        played[played_size] = randindex;
        played_size++;
        
        music_adrass = playlist[randindex];
        printf("%s\n", music_adrass);
        ma_sound_init_from_file(
            &engine,
            music_adrass,
            0,
            NULL, NULL,
            &sound
        );
        ma_sound_start(&sound);
        while(ma_sound_is_playing(&sound)){
            if(key_pressed()){
                ui = getchar();
                if(ui == 'q'){
                    running = 0;
                    break;
                }else if(ui == 'n'){
                    break;
                }else if(ui == 'c'){
                    printf("\033[2J\033[H\n");
                }
            }
        }
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }
    ma_engine_uninit(&engine);
    return 0;
}
