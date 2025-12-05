#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h> 
#include <iostream>
#include <ctime>
#include <string>
using namespace std;


const int BLOCK_SIZE = 25;
const int FIELD_WIDTH = 14;
const int FIELD_HEIGHT = 15;
const float SCALE = 1.2f;

const int SCREEN_WIDTH  = FIELD_WIDTH * BLOCK_SIZE * SCALE; 
const int SCREEN_HEIGHT = FIELD_HEIGHT * BLOCK_SIZE * SCALE;
const int SCORE_MARGIN = 20;

int field[FIELD_HEIGHT][FIELD_WIDTH] = {0};

int tetromino[7][4] = {
    {1,3,5,7}, 
    {2,4,5,7}, 
    {3,5,4,6}, 
    {3,5,4,7}, 
    {2,3,5,7}, 
    {3,5,7,6}, 
    {2,3,4,5}  
};

struct Point { int x, y; } a[4], b[4];


bool check() {
    for (int i = 0; i < 4; i++) {
        if (a[i].x < 0 || a[i].x >= FIELD_WIDTH || a[i].y >= FIELD_HEIGHT)
            return false;
        if (field[a[i].y][a[i].x])
            return false;
    }
    return true;
}

SDL_Color getColor(int id) {
    SDL_Color colors[8] = {
        {0,0,0,255},{0,255,255,255},{255,0,0,255},{0,255,0,255},
        {255,255,0,255},{255,165,0,255},{0,0,255,255},{255,0,255,255}
    };
    return colors[id % 8];
}


void spawnPiece(int &n) {
    n = rand() % 7;
    for (int i = 0; i < 4; i++) {
        a[i].x = tetromino[n][i] % 2 + FIELD_WIDTH / 2 - 1;
        a[i].y = tetromino[n][i] / 2;
    }
}

void resetField() {
    for (int i = 0; i < FIELD_HEIGHT; i++)
        for (int j = 0; j < FIELD_WIDTH; j++)
            field[i][j] = 0;
}


int main(int argc, char* argv[]) {
    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); 

    Mix_Chunk* scoreSound = Mix_LoadWAV("score.wav"); 
    if (!scoreSound) {
        cout << "Failed to load sound: " << Mix_GetError() << endl;
    }

    SDL_Window* window = SDL_CreateWindow("Tetris SDL2 (Sound + Speed)",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 22);
    SDL_Texture* background = IMG_LoadTexture(renderer, "im.jpg");

    bool quit = false, paused = false, gameOver = false;
    int dx = 0, colorNum = 1, score = 0, n;
    Uint32 fallDelay = 600;        
    Uint32 currentDelay = fallDelay; 
    Uint32 lastTick = SDL_GetTicks();

    spawnPiece(n);
    SDL_Event e;

    
    while (!quit) {
        Uint32 currentTick = SDL_GetTicks();
        dx = 0;
        bool rotate = false;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;

            if (e.type == SDL_KEYDOWN) {
                if (gameOver && e.key.keysym.sym == SDLK_r) {
                    resetField();
                    score = 0;
                    gameOver = false;
                    spawnPiece(n);
                    colorNum = 1 + rand() % 7;
                    fallDelay = 600;
                    currentDelay = fallDelay;
                } 
                else if (!gameOver) {
                    if (e.key.keysym.sym == SDLK_LEFT) dx = -1;
                    else if (e.key.keysym.sym == SDLK_RIGHT) dx = 1;
                    else if (e.key.keysym.sym == SDLK_UP) rotate = true;
                    else if (e.key.keysym.sym == SDLK_SPACE) paused = !paused;
                    else if (e.key.keysym.sym == SDLK_DOWN) {
                        currentDelay = 100; 
                    }
                }
            }

            
            if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_DOWN) {
                currentDelay = fallDelay;
            }
        }

        if (quit) break;

        
        if (paused || gameOver) {
            SDL_RenderCopy(renderer, background, NULL, NULL);

            string msg = paused ? "PAUSED" : "GAME OVER";
            SDL_Color color = paused ? SDL_Color{255,255,255,255} : SDL_Color{255,50,50,255};

            SDL_Surface* textSurface = TTF_RenderText_Solid(font, msg.c_str(), color);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            int tw, th;
            SDL_QueryTexture(textTexture, NULL, NULL, &tw, &th);
            SDL_Rect dst = { SCREEN_WIDTH/2 - tw/2, SCREEN_HEIGHT/2 - th/2 - 60, tw, th };
            SDL_RenderCopy(renderer, textTexture, NULL, &dst);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);

            if (gameOver && font) {
                SDL_Color white = {255,255,255,255};
                string scoreText = "Final Score: " + to_string(score);
                SDL_Surface* scoreSurf = TTF_RenderText_Solid(font, scoreText.c_str(), white);
                SDL_Texture* scoreTex = SDL_CreateTextureFromSurface(renderer, scoreSurf);
                int sw, sh;
                SDL_QueryTexture(scoreTex, NULL, NULL, &sw, &sh);
                SDL_Rect scoreRect = { SCREEN_WIDTH/2 - sw/2, SCREEN_HEIGHT/2 - 10, sw, sh };
                SDL_RenderCopy(renderer, scoreTex, NULL, &scoreRect);
                SDL_FreeSurface(scoreSurf);
                SDL_DestroyTexture(scoreTex);

                SDL_Surface* msg2 = TTF_RenderText_Solid(font, "Press R to Restart", white);
                SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, msg2);
                int m2w, m2h;
                SDL_QueryTexture(t2, NULL, NULL, &m2w, &m2h);
                SDL_Rect d2 = { SCREEN_WIDTH/2 - m2w/2, SCREEN_HEIGHT/2 + 40, m2w, m2h };
                SDL_RenderCopy(renderer, t2, NULL, &d2);
                SDL_FreeSurface(msg2);
                SDL_DestroyTexture(t2);
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(100);
            continue;
        }

        
        for (int i = 0; i < 4; i++) b[i] = a[i];
        for (int i = 0; i < 4; i++) a[i].x += dx;
        if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];

        if (rotate) {
            Point p = a[1];
            for (int i = 0; i < 4; i++) {
                int x = a[i].y - p.y;
                int y = a[i].x - p.x;
                a[i].x = p.x - x;
                a[i].y = p.y + y;
            }
            if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];
        }

        
        if (currentTick - lastTick > currentDelay) {
            lastTick = currentTick;
            for (int i = 0; i < 4; i++) b[i] = a[i];
            for (int i = 0; i < 4; i++) a[i].y++;
            if (!check()) {
                for (int i = 0; i < 4; i++)
                    field[b[i].y][b[i].x] = colorNum;
                spawnPiece(n);
                colorNum = 1 + rand() % 7;
                currentDelay = fallDelay; 
                if (!check()) gameOver = true;
            }
        }

       
        for (int i = FIELD_HEIGHT - 1; i >= 0; i--) {
            int count = 0;
            for (int j = 0; j < FIELD_WIDTH; j++)
                if (field[i][j]) count++;
            if (count == FIELD_WIDTH) {
                for (int k = i; k > 0; k--)
                    for (int j = 0; j < FIELD_WIDTH; j++)
                        field[k][j] = field[k-1][j];
                score += 100;
                i++;
                Mix_PlayChannel(-1, scoreSound, 0); 
            }
        }

        
        SDL_RenderCopy(renderer, background, NULL, NULL);

        
        SDL_Surface* scoreSurf = TTF_RenderText_Solid(font, ("Score: " + to_string(score)).c_str(),
                                                      SDL_Color{255,255,255,255});
        SDL_Texture* scoreTex = SDL_CreateTextureFromSurface(renderer, scoreSurf);
        SDL_Rect scoreRect = { SCORE_MARGIN, SCORE_MARGIN, scoreSurf->w, scoreSurf->h };
        SDL_RenderCopy(renderer, scoreTex, NULL, &scoreRect);
        SDL_FreeSurface(scoreSurf);
        SDL_DestroyTexture(scoreTex);

        
        for (int i = 0; i < FIELD_HEIGHT; i++) {
            for (int j = 0; j < FIELD_WIDTH; j++) {
                if (!field[i][j]) continue;
                int size = BLOCK_SIZE * SCALE;
                int blockX = j * size;
                int blockY = i * size;
                SDL_Rect rect = {blockX, blockY, size, size};
                SDL_Color c = getColor(field[i][j]);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }

       
        SDL_Color c = getColor(colorNum);
        for (int i = 0; i < 4; i++) {
            int size = BLOCK_SIZE * SCALE;
            int blockX = a[i].x * size;
            int blockY = a[i].y * size;
            SDL_Rect rect = {blockX, blockY, size, size};
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(50);
    }

    
    Mix_FreeChunk(scoreSound);
    Mix_CloseAudio();
    SDL_DestroyTexture(background);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
