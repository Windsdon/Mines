#include "mines.h"

#include <time.h>
#include <math.h>

#include <iostream>

void Mines::OnExecute() {
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

    printf("Starting...\n");

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("Failed to init. Exiting.\n");
        return;
    }

    TTF_Init();

    fpsfont = TTF_OpenFont("cambriab.ttf", 20);
    timeFont = TTF_OpenFont("cambriab.ttf", 72);
    timeFontOutline = TTF_OpenFont("cambriab.ttf", 72);

    TTF_GlyphMetrics(timeFont,'0', 0, 0, 0, 0, &kTimeTextAdvance);

    TTF_SetFontOutline(timeFontOutline, 1);

    colorFPS.r = 0;
    colorFPS.g = 0xcc;
    colorFPS.b = 0x20;

    colorBlack.r = 0;
    colorBlack.g = 0;
    colorBlack.b = 0;

    colorWhite.r = 0xff;
    colorWhite.g = 0xff;
    colorWhite.b = 0xff;

    colorRed.r = 0xff;
    colorRed.g = 0;
    colorRed.b = 0;

    mineBlock = SDL_LoadBMP("block.bmp");
    mineBlockHover = SDL_LoadBMP("blockHover.bmp");
    mineBlockHoverDown = SDL_LoadBMP("blockHoverDown.bmp");
    mineFlag = SDL_LoadBMP("flag.bmp");
    mineBoom = SDL_LoadBMP("boom.bmp");
    mineUnknown = SDL_LoadBMP("wtf.bmp");
    faceNeutral = SDL_LoadBMP("face.bmp");
    faceScared = SDL_LoadBMP("faceScared.bmp");
    faceDead = SDL_LoadBMP("faceDead.bmp");

    //SDL_SetAlpha(mineFlag, SDL_SRCALPHA, 128);

    screen = SDL_SetVideoMode(screenw, screenh + 100, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

    srand(time(NULL));

    OnInit();
}

void Mines::OnInit() {
    gameState = newgame;
    closeRequested = 0;
    fps = 0;
    fpsCount = 0;

    SDL_Event event;
    while(gameState != stopped && !closeRequested) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                closeRequested = 1;
            }
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_1) {
                cheatModeOn = 1;
                printf("Turn cheat mode on.\n");
            }
            if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_1) {
                cheatModeOn = 0;
                printf("Turn cheat mode off.\n");

            }
        }
        OnLoop();
        OnRender();
    }

    OnExit();
}

void Mines::OnRender() {
    int i,j, hasExplosions = 0, hasDown = 0;
    SDL_Rect dst;
    SDL_Rect src;
    src.w = 30;
    src.h = 30;
    src.x = 0;
    src.y = 0;

    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
    dst.w = blockw;
    dst.h = blockh;

    char fpstext[10];
    char timetext[10];

    Uint32 timeNow = SDL_GetTicks();
    if(timeNow - lastFPStime >= 1000) {
        fps = fpsCount;
        fpsCount = 0;
        lastFPStime = timeNow;
        printf("fps: %i\n", fps);
    }

    int minutes = (SDL_GetTicks() - gameStartTime)/60000;
    int seconds = ((SDL_GetTicks() - gameStartTime)/1000)%60;

    sprintf(fpstext, "%03i fps", fps);
    sprintf(timetext, "%02u:%02u", minutes, seconds);
    fpsScreen = TTF_RenderText_Shaded(fpsfont, fpstext, colorFPS, colorBlack);
    if(gameStartTime == -1) {
        timeScreen = TTF_RenderText_Shaded(timeFont, "00:00", colorRed, colorBlack);
        timeOutlineScreen = TTF_RenderText_Blended(timeFontOutline, "00:00", colorWhite);
    } else {
        timeScreen = TTF_RenderText_Shaded(timeFont, timetext, colorRed, colorBlack);
        timeOutlineScreen = TTF_RenderText_Blended(timeFontOutline, timetext, colorWhite);
    }
    for(i = 0; i < MAP_WIDTH; i++) {
        for (j = 0; j < MAP_HEIGHT; j++) {
            dst.x = i * blockw;
            dst.y = j * blockh;

            if(field[j][i]&exploded) {
                hasExplosions = 1;
                SDL_BlitSurface(mineBoom, &src, screen, &dst);
            } else if(field[j][i]&shown) {
                int k = (field[j][i]>>fieldNumShift);
                if(k) {
                    SDL_Surface *num = TTF_RenderGlyph_Blended(fpsfont, k + 48, colorWhite);
                    dst.x += num->w/2;
                    dst.y += num->h/2;
                    SDL_BlitSurface(num, 0, screen, &dst);
                    SDL_FreeSurface(num);
                }

            } else if(field[j][i]&hoverDown) {

                hasDown = 1;
                SDL_BlitSurface(mineBlockHoverDown, &src, screen, &dst);
            } else if(field[j][i]&hover) {

                SDL_BlitSurface(mineBlockHover, &src, screen, &dst);
            } else {
                SDL_BlitSurface(mineBlock, &src, screen, &dst);
                if(field[j][i]&mine && cheatModeOn) {
                    SDL_BlitSurface(mineFlag, 0, screen, &dst);
                }
            }

            dst.w = 20;
            dst.h = 20;
            dst.x += 5;
            dst.y += 5;
            if (field[j][i]&isFlagged) {
                SDL_BlitSurface(mineFlag, &src, screen, &dst);
            }

            if (field[j][i]&isDubious) {
                SDL_BlitSurface(mineUnknown, &src, screen, &dst);
            }

        }

    }

    //printf("Cheat mode: %i\n", cheatModeOn);

    dst.w = 80;
    dst.h = 80;
    dst.y = screenh + 50 - 80/2;
    dst.x = screenw/2 - 80/2;

    if (hasExplosions) {
        SDL_BlitSurface(faceDead, 0, screen, &dst);

    } else if (hasDown) {
        SDL_BlitSurface(faceScared, 0, screen, &dst);

    } else {
        SDL_BlitSurface(faceNeutral, 0, screen, &dst);

    }

    fpsCount++;

    SDL_Rect dstFPS;
    dstFPS.w = fpsScreen->w;
    dstFPS.h = fpsScreen->h;
    dstFPS.y = screenh + 50 - dstFPS.h/2;
    dstFPS.x = screenw/4 - dstFPS.w/2;

    SDL_Rect dstTime;
    dstTime.w = timeScreen->w;
    dstTime.h = timeScreen->h;
    dstTime.y = screenh + 50 - dstTime.h/2;
    dstTime.x = 3*screenw/4 - dstTime.w/2 + kTimeTextAdvance/2;

    SDL_BlitSurface(fpsScreen, 0, screen, &dstFPS);
    SDL_BlitSurface(timeScreen, 0, screen, &dstTime);
    SDL_BlitSurface(timeOutlineScreen, 0, screen, &dstTime);

    SDL_FreeSurface(fpsScreen);
    SDL_FreeSurface(timeScreen);
    SDL_FreeSurface(timeOutlineScreen);

    SDL_Flip(screen);
}

void Mines::OnLoop() {
    if(gameState != playing && gameState != mapNotGenerated) {
        if(gameState == newgame) {
            StartNewGame();
        }
        return;
    }

    int i, j;
    int mousex, mousey, mousebutton, mouseblockx, mouseblocky;
    mousebutton = SDL_GetMouseState(&mousex, &mousey);
    mouseblockx = mousex/blockw;
    mouseblocky = mousey/blockh;

    if(mousex >= screenw/2 - 40 && mousex <= screenw/2 + 40 &&
            mousey >= screenh && mousebutton&SDL_BUTTON_LMASK) {
        StartNewGame();
        return;

    }

    for(i = 0; i < MAP_HEIGHT; i++) {
        for(j = 0; j < MAP_WIDTH; j++) {
            unsigned int *f = &field[i][j];
            if (mouseblocky == i && mouseblockx == j) {
                *f |= hover;
                if(SDL_BUTTON_RMASK&mousebutton) {
                    if(!mouseButtonDownR) {
                        mouseButtonDownR = 1;
                        if(*f&isFlagged) {
                            *f &= ~isFlagged;
                            *f |= isDubious;
                        } else if(*f&isDubious) {
                            *f &= ~isDubious;
                        } else {
                            *f |= isFlagged;
                        }
                    }
                }
                if(SDL_BUTTON_LMASK&mousebutton) {
                    if(!mouseButtonDown) {
                        mouseButtonDown = 1;
                    }
                    *f |= hoverDown;
                } else {
                    if(*f&hoverDown) {
                        if(gameState == mapNotGenerated) {
                            GenerateNewMap(j, i, 20);
                        }
                        if(*f&mine) {
                            *f |= exploded;
                        } else {
                            if(!(*f>>fieldNumShift)) {
                                printf("Tile at (%i, %i) is empty\n", j, i);
                                int floodFillArray[MAP_WIDTH*MAP_HEIGHT*4][3] = {j, i, 1},
                                        k, l, m, countArray = 1, active = 1;
                                do {
                                    for(k = 0; k < countArray; k++) {
                                        int currentX = floodFillArray[k][0];
                                        int currentY = floodFillArray[k][1];
                                        int *currentA = &floodFillArray[k][2];
                                        if(*currentA) {
                                            int addList[8][3] = {
                                                { currentX - 1, currentY },
                                                { currentX + 1, currentY },
                                                { currentX, currentY - 1 },
                                                { currentX, currentY + 1 },
                                                { currentX + 1, currentY + 1 },
                                                { currentX + 1, currentY - 1 },
                                                { currentX - 1, currentY + 1 },
                                                { currentX - 1, currentY - 1 }
                                            };

                                            for(l = 0; l < 8; l++) {
                                                if(addList[l][0] < 0 || addList[l][1] < 0 ||
                                                        addList[l][0] >= MAP_WIDTH || addList[l][0] >= MAP_HEIGHT) {
                                                    continue;
                                                }
                                                int addThis = 1;
                                                for(m = 0; m < countArray; m++) {
                                                    int *now = floodFillArray[m];
                                                    if(now[0] == addList[l][0] && now[1] == addList[l][1]) {
                                                        printf("Repeated excluded. Total elements: %d\n", countArray);
                                                        addThis = 0;
                                                        break;
                                                    }
                                                }

                                                if(addThis) {
                                                    int *newSpace;
                                                    newSpace = floodFillArray[countArray++];
                                                    newSpace[0] = addList[l][0];
                                                    newSpace[1] = addList[l][1];
                                                    newSpace[2] = (field[addList[l][1]][addList[l][0]]>>fieldNumShift) == 0;
                                                    if(newSpace[2]){
                                                        active++;
                                                    }
                                                }
                                            }
                                            active--;
                                            *currentA = 0;
                                        }
                                    }
                                } while(active > 0);
                                printf("Active: %d", active);
                                for(k = 0; k < countArray; k++) {
                                    field[floodFillArray[k][1]][floodFillArray[k][0]] |= shown;
                                    printf("%i %i - ", floodFillArray[k][0], floodFillArray[k][1]);
                                }

                            } else {
                                *f |= shown;
                            }
                        }

                    }
                    *f &= ~hoverDown;
                }
            } else {
                *f &= ~(hover|hoverDown);
            }
        }
    }

    if(!(SDL_BUTTON_LMASK&mousebutton)) {
        mouseButtonDown = 0;
    }
    if(!(SDL_BUTTON_RMASK&mousebutton)) {
        mouseButtonDownR = 0;
    }
}

void Mines::StartNewGame() {
    int i, j;

    for(i = 0; i < MAP_HEIGHT; i++) {
        for(j = 0; j < MAP_WIDTH; j++) {
            field[i][j] = 0;
        }
    }

    gameState = mapNotGenerated;

    gameStartTime = -1;
}

void Mines::GenerateNewMap(int clickx, int clicky, int bombs) {
    int i, j, k, l, m;

    printf("%i %i\n", clickx, clicky);

    for(k = 0; k < bombs; k++) {
        do {
            i = rand()%MAP_WIDTH;
            j = rand()%MAP_HEIGHT;
        } while((i == clickx && j == clicky) && !(field[j][i]&mine));
        field[j][i] |= mine;
    }

    for(i = 0; i < MAP_WIDTH; i++) {
        for(j = 0; j < MAP_HEIGHT; j++) {
            int minx = (i - 1 < 0) ? 0 : i - 1, maxx = (i + 1 == MAP_WIDTH) ? i : i + 1;
            int miny = (j - 1 < 0) ? 0 : j - 1, maxy = (j + 1 == MAP_HEIGHT) ? j : j  + 1;
            //printf("Scan (%i, %i) - from (%i, %i) to (%i, %i)\n", i, j, minx, miny, maxx, maxy);
            for(l = minx; l <= maxx; l++) {
                for(m = miny; m <= maxy; m++) {
                    if (field[m][l]&mine) {
                        field[j][i] = (field[j][i]&~fieldNumFlag) | (((field[j][i]>>fieldNumShift) + 1)<<fieldNumShift);
                    }
                }
            }
        }
    }
    gameStartTime = SDL_GetTicks();
    gameState = playing;
}

void Mines::OnExit() {

}
