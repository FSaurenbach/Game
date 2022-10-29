- A little game and engine I am working on. Contains a small art
program as for making pixel art.

## To Compile
- Install Ninja tool and make sure to have Mingw or GCC
- Latest version of game has not been tested on other platforms, only Windows 10
- Run the run.bat file

## Other notes 

- A bunch of programs in one project and some of them are not mine, 
note that my art tool requires that palettes be less than 65 pixels
in width and 1 pixel in y. The max resolution it takes is 64x1, meaning
64 colors. Find a palette here: https://lospec.com/palette-list, open your
image editor and edit it a bit and then start playing.

- To set colorkey, you need to use surface and SDL_SetColorKey(surf, 1, SDL_MapRGB(r, g, b))
- ADD_MODE = adds colors
- MOD = modulates the colors
- BLEND_MODE = transparency
- NONE = nothing
- Optimize by using spritesheet
- JSON that stores information regarding animation and states that is just parsed like idle and stuff
with where they start and how long they are and when they end, gsdl_anim_data_t basically
- Blit texts and just don't make texture
- Make resource manager (get_surf and loads if not exist) and entity component system:   
    - entity_t with update function stored

- Point of creating this project:
    - Essentially create an effective SDL2 framework for use in creating games

- Features:
    - Made using C, SDL2, SDL_gfx library which can be found online and 
    inspired by DaFluffyPotato's games and my previous pygame games
    - Using VSC for creating the game:
        - .vscode
        - Utilizing VSC task system to compile using script

    - Build system is Ninja:
        - Highly efficient
        - Different platform support and target support
        - Using .bat script to compile
        - Fast & saves time
        - How it works:
            - Config file contains variables which determines how and what is to be compiled

    - Build folder contains different platform folders and resources..etc..
    - This project has different parts obviously and the abstraction is done in the following way:
        - SDL2 Library + SDL_gfx Library + Utils -> Engine -> TARGET
        - Utils (inspired by extd_cstd):
            - Features:
                - Smaller type names
                -++ File loading and writing
                -++ Hash table
                -++ Lists
                -++ Efficient logging system
                - Math functions (lerp, distance, 2d vectors, random numbers)
                - Small profiler
                -++ String conversion functions
                -++ Configuration file parser

        - Engine:
            - Features:
                -++ Init and uninitalization information struct
                -++ Props struct containing necessary program information
                - Debug functions:
                    - Frame time
                    - Memory used
                -++ Image & Animation functions
                - Music & SFX
                - UI functions
                - Physics functions
                -++ Camera
                - Circles
                - Particles
                - Drawing functions ||++ render all function
                -++ "Serizalization" functions
                -++ Grid & Color functions

    - I have written target because of the fact that there are different TARGETs which can be compiled:
        - Level editor (completely useless):
            - Features:
                - Somewhat parsing of data in .txt file
        - Pixel art editor:
            - Features:
                - Palette loader
                - Simple grid
                - Image saving 
                - Scaling of grid
        - Game:
            - Features:
                - Structure:
                    - Behaviours -> Game logic created and handled in the loops -> Different states of game (playing, menu)
                        - Each part has special events names in game.h
                        - State.h contains game state
                        - Each behaviour file has unique functions like a chunking-like system for tiles
                            - need to fix chunking behaviour
                            based on how many tiles can be 
                            displayed on the screen using
                            screen size 
    
    - I have also taken the courtesy of making this project in such a way that it is completely offline and
    online things don't need to be used. I already downloaded the .html version of beepbox for creating music.
