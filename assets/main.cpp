#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TILE_SIZE 20

typedef struct { int x, y; } Point;
typedef struct {
    Point body[SCREEN_WIDTH * SCREEN_HEIGHT / (TILE_SIZE * TILE_SIZE)];
    int length;
    Point direction;
} Snake;

SDL_Texture *load_texture(SDL_Renderer *renderer, const char *file) {
    SDL_Surface *surface = IMG_Load(file);
    if (!surface) {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        printf("TTF_RenderText_Solid error: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio Error: %s\n", Mix_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Snake Game",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("assets/arial.ttf", 24);
    if (!font) { printf("Font load error: %s\n", TTF_GetError()); return 1; }

    Mix_Chunk *eatSound = Mix_LoadWAV("assets/eat.wav");
    SDL_Texture *foodTexture = load_texture(renderer, "assets/food.png");
    SDL_Texture *snakeTexture = load_texture(renderer, "assets/snake.png");
    if (!eatSound || !foodTexture || !snakeTexture) {
        printf("Asset loading failed.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));
    Snake snake = {{0}, 5, {1, 0}};
    for (int i = 0; i < snake.length; ++i)
        snake.body[i] = (Point){snake.length - i - 1, 0};

    Point food = {rand() % (SCREEN_WIDTH / TILE_SIZE),
                  rand() % (SCREEN_HEIGHT / TILE_SIZE)};
    bool running = true;
    SDL_Event e;
    int score = 0;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP: if (snake.direction.y == 0) snake.direction = (Point){0, -1}; break;
                    case SDLK_DOWN: if (snake.direction.y == 0) snake.direction = (Point){0, 1}; break;
                    case SDLK_LEFT: if (snake.direction.x == 0) snake.direction = (Point){-1, 0}; break;
                    case SDLK_RIGHT: if (snake.direction.x == 0) snake.direction = (Point){1, 0}; break;
                }
            }
        }

        // Move snake
        Point tail = snake.body[snake.length - 1];
        for (int i = snake.length - 1; i > 0; --i)
            snake.body[i] = snake.body[i - 1];
        snake.body[0].x += snake.direction.x;
        snake.body[0].y += snake.direction.y;

        // Collision check
        if (snake.body[0].x < 0 || snake.body[0].x >= SCREEN_WIDTH / TILE_SIZE ||
            snake.body[0].y < 0 || snake.body[0].y >= SCREEN_HEIGHT / TILE_SIZE)
            running = false;

        for (int i = 1; i < snake.length; ++i)
            if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y)
                running = false;

        // Eat food
        if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
            snake.body[snake.length] = tail;  // grow
            snake.length++;
            score++;
            food = (Point){rand() % (SCREEN_WIDTH / TILE_SIZE),
                           rand() % (SCREEN_HEIGHT / TILE_SIZE)};
            Mix_PlayChannel(-1, eatSound, 0);
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect foodRect = {food.x * TILE_SIZE, food.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        SDL_RenderCopy(renderer, foodTexture, NULL, &foodRect);

        for (int i = 0; i < snake.length; ++i) {
            SDL_Rect rect = {snake.body[i].x * TILE_SIZE, snake.body[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(renderer, snakeTexture, NULL, &rect);
        }

        char scoreText[32];
        sprintf(scoreText, "Score: %d", score);
        render_text(renderer, font, scoreText, (SDL_Color){255, 255, 255, 255}, 10, 10);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    // Cleanup
    SDL_DestroyTexture(foodTexture);
    SDL_DestroyTexture(snakeTexture);
    Mix_FreeChunk(eatSound);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
