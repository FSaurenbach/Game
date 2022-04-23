#ifndef GAME_H
    #define GAME_H

    #include "./engine/engine.h"

    #define TITLE "UnR34lity"
    #define WIDTH        1280 
    #define HEIGHT       720
    #define W_HALF       WIDTH/2 
    #define H_HALF       HEIGHT/2
    #define RENDERER_ACC 1
    #define FPS          60 
    #define VSYNC        1
    #define RESIZABLE    1 

    // implement rerun init function
    #define ARGS (gsdl_init_info_t) { TITLE, WIDTH, HEIGHT, RENDERER_ACC, FPS, VSYNC, RESIZABLE }

    #define PLAYER_STATES 4
    #define MAX_Y_MOMENTUM 12
    #define SPAWN_POS (v2_t) { 0, -TSY * 7 }
    #define ASPECT_RATIO (f32) WIDTH/HEIGHT
    #define PSX 40 * (0.75 * ASPECT_RATIO)
    #define TSX 64 * (0.75 * ASPECT_RATIO)
    #define TSY 64 * (0.75 * ASPECT_RATIO)

    #define MAX_MS_SIZE 40 
    #define MAX_MEM_USED 120

    typedef struct game_state {
        u08 g_state;
        gsdl_img_t render_texture;
        gsdl_phys_obj_t render_texture_obj;
        gsdl_phys_obj_t mouse;

        // fps
        char * integer_str;
        char * final_dbg_str;

        // transition
        gsdl_circle_t transition_circle;
        u08 transition;

        // Menu
        gsdl_ui_label_t game_name;
        gsdl_ui_label_t press_to_play;
        gsdl_ui_label_t frame_time_label;
        SDL_Texture * frame_time_textures[MAX_MS_SIZE];
        gsdl_ui_label_t mem_used_label;
        SDL_Texture * mem_used_textures[MAX_MEM_USED];

        gsdl_circle_t sharingan;
        gsdl_circle_t sharingan_tomoe[4];

        // actual stuff
        u08 player_img_idx; 

        gsdl_anim_data_t * player_img_data;
        gsdl_animated_img_t player_anim_img;
        gsdl_phys_obj_t player;
        gsdl_particles_t player_particles;

        gsdl_circle_t bullet_circle;
        gsdl_circle_t bullet_circle_outline;
        f32 circle_timer;

        f32 player_air_timer;
        i32 player_can_dash;
        f32 player_dash_timer;

        gsdl_cam_t camera;
        gsdl_phys_obj_t tiles[150];
        gsdl_phys_obj_t tile_shadow[150];

        gsdl_img_t square_texture[30];
        gsdl_phys_obj_t square[30];

        gsdl_img_t bullet_texture;
        gsdl_phys_obj_t bullets[50];
        i32 bullet_alpha[50];
        f32 bullet_created[50];
        i32 bullet_len;
        f32 shoot_delay;
        f32 shoot_timer;
        u08 bullet_creating;
        u08 bullet_keyup;

        i32 saved_dt_avg;
        i32 saved_mem_usage;
        i32 f_count;
    } game_state_t;


    game_state_t * game_init(gsdl_props_t * props);
    void game_loop(gsdl_props_t * props, game_state_t * state);
    void game_cleanup(gsdl_props_t * props, game_state_t * state);

    void menu_handle_events(gsdl_props_t * props, game_state_t * state);
    void menu_update(gsdl_props_t * props, game_state_t * state);
    void menu_render(gsdl_props_t * props, game_state_t * state);
    void menu_loop(gsdl_props_t * props, game_state_t * state);

    void main_handle_events(gsdl_props_t * props, game_state_t * state);
    void main_update(gsdl_props_t * props, game_state_t * state);
    void main_render(gsdl_props_t * props, game_state_t * state);
    void main_loop(gsdl_props_t * props, game_state_t * state);

#endif