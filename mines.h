#ifndef MINES_H
#define MINES_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define MAP_WIDTH 20
#define MAP_HEIGHT 20



class Mines {
    public:
        void OnExecute();
        void OnInit();
        void OnRender();
        void OnLoop();
        void OnExit();
        void StartNewGame();
        void GenerateNewMap(int clickX, int clickY, int bombs);
    protected:
    private:
        const static int screenw = 600;
        const static int screenh = 600;

        const static int blockw = screenw/MAP_WIDTH, blockh = screenh/MAP_HEIGHT;

        SDL_Surface *screen;
        SDL_Surface *fpsScreen;
        SDL_Surface *timeScreen;
        SDL_Surface *timeOutlineScreen;
        SDL_Surface *mineBlock;
        SDL_Surface *mineBlockHover;
        SDL_Surface *mineBlockHoverDown;
        SDL_Surface *mineFlag;
        SDL_Surface *mineMine;
        SDL_Surface *mineUnknown;
        SDL_Surface *mineBoom;
        SDL_Surface *faceNeutral;
        SDL_Surface *faceScared;
        SDL_Surface *faceDead;

        int gameState;
        int closeRequested;
        Uint32 lastLoopTime = 0;

        int mouseButtonDown;
        int mouseButtonDownR;

        static const enum {
            stopped = 0,
            newgame,
            mapNotGenerated,
            playing,
            lost
        } gameStates;

        static const enum  {
            mine = 1,
            exploded = 2,
            isDubious = 64,
            isFlagged = 128,
            hover = 256,
            hoverDown = 512,
            shown = 1024
        } fieldFlags;

        static const unsigned int fieldNumFlag = 0x7800;
        static const unsigned int fieldNumShift = 11;


        TTF_Font *fpsfont;
        TTF_Font *timeFont;
        TTF_Font *timeFontOutline;
        SDL_Color colorFPS;
        SDL_Color colorBlack;
        SDL_Color colorWhite;
        SDL_Color colorRed;
        int fpsCount;
        int fps;
        Uint32 lastFPStime;

        long int gameStartTime;

        int kTimeTextAdvance;

        int cheatModeOn = 0;


        unsigned int field[MAP_HEIGHT][MAP_WIDTH];

};

#endif // MINES_H
