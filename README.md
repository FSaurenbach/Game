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

