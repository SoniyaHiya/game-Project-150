# 🎮 Game Project-150 (Tetris Clone using C++ & SDL2)

This project is a **Tetris-style block puzzle game** built with **C++** and **SDL2**.  
It demonstrates basic 2D game development concepts — including rendering, event handling, keyboard input, scoring, and game states such as pause and game over.

---

## Table of Contents
1. [Features](#features)  
2. [Dependencies](#dependencies)  
3. [File Structure](#file-structure)  
4. [Header Files](#header-files)  
5. [SDL Template](#sdl-template)  
6. [Gameplay](#gameplay)  
7. [Controls](#controls)  
8. [How to Run](#how-to-run)  
9. [VS Code Run Steps](#vs-code-run-steps)  
10. [Tips](#tips)  
11. [Screenshots](#screenshots)  
12. [Contribution](#contribution)  
13. [License](#license)  

---

## Features ✨
- Classic Tetris gameplay  
- Pausing and restarting the game  
- Scoring system (100 points per line)  
- Colored Tetromino blocks  
- Background image support  
- Smooth piece rotation and collision detection  

---

## Dependencies 🛠️
- SDL2  
- SDL2_ttf  
- SDL2_image  
- C++17 (or later) compatible compiler  

---

## File Structure 📁
Tetris-SDL2/
│
├─ main.cpp # Main game logic
├─ im.jpg # Background image
└─ README.md # Project documentation

pgsql
Copy code

---

## Header Files 📝
1. **`#include <SDL2/SDL.h>`** – Provides SDL core functionality for window creation, rendering, and event handling.  
2. **`#include <SDL2/SDL_main.h>`** – Required for SDL on some platforms to correctly set up `main()` entry point.  
3. **`#include <SDL2/SDL_ttf.h>`** – For rendering text using TrueType fonts.  
4. **`#include <SDL2/SDL_image.h>`** – For loading PNG/JPG images as textures.  
5. **`#include <iostream>`** – Standard C++ input/output stream for debugging.  
6. **`#include <ctime>`** – For random number seeding (`srand(time(0))`).  
7. **`#include <string>`** – To work with string objects for score and messages.  

---

## SDL Template 🖼️
```cpp
SDL_Init(SDL_INIT_VIDEO);
TTF_Init();
IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

SDL_Window* window = SDL_CreateWindow("Tetris SDL2",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      SCREEN_WIDTH, SCREEN_HEIGHT,
                                      SDL_WINDOW_SHOWN);

SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
Explanation:

Initializes SDL, TTF, and IMG modules.

Creates a window and renderer.

Uses SDL_PollEvent in game loop to handle input.

Renders graphics using SDL_RenderCopy and SDL_RenderFillRect.

Cleans up with SDL_Destroy* and SDL_Quit at the end.

Gameplay 🎯
Game loop: Input → Update → Collision check → Render → Delay

Pieces fall automatically based on fallDelay

Collision detection prevents overlapping or going outside boundaries

Completed lines are removed and score is updated

Controls 🎮
Key	Action
Left Arrow	Move piece left
Right Arrow	Move piece right
Up Arrow	Rotate piece
Down Arrow	Fast drop
Space	Pause
R	Restart (after game over)

How to Run ▶️
Install SDL2, SDL2_ttf, SDL2_image

Place im.jpg in the project folder

Compile with:

bash
Copy code
g++ main.cpp -o Tetris -lSDL2 -lSDL2_ttf -lSDL2_image
Run the executable:

bash
Copy code
./Tetris
VS Code Run Steps 💻
Open the project in Visual Studio Code.

Click the three dots (...) at the top-right of the editor.

Select "Terminal" → "New Terminal".

A terminal will open at the bottom, showing something like:

powershell
Copy code
PS C:\Users\USER\Desktop\SDLtemplate>
Type the following command and press Enter:

bash
Copy code
./Tetris
The Tetris game will launch in a new window.

Tips 💡
Adjust BLOCK_SIZE, FIELD_WIDTH, FIELD_HEIGHT, or SCALE in the code to change the game resolution.

Use colorful background images (im.jpg) to enhance visuals.

Press Space to pause during gameplay.

Press R after game over to restart quickly.
