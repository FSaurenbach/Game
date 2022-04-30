#ifndef STATE_H
    #define STATE_H
    #include <engine.h>

    #define TITLE "UnR34lity"
    #define WIDTH        1280 
    #define HEIGHT       720
    #define W_HALF       WIDTH/2 
    #define H_HALF       HEIGHT/2
    #define RENDERER_ACC 1
    #define FPS          65 
    #define VSYNC        1
    #define RESIZABLE    1 

    #define MAX_MS_SIZE 40
    #define MAX_MEM_USED 120

    // implement rerun init function
    #define ARGS (gsdl_init_info_t) { TITLE, WIDTH, HEIGHT, RENDERER_ACC, FPS, VSYNC, RESIZABLE }

    #define PLAYER_STATES 2
    #define MAX_Y_MOMENTUM 12
    #define SPAWN_POS (v2_t) { 600, 200 }
    #define ASPECT_RATIO (f32) WIDTH/HEIGHT
    #define PSX 40 * (0.75 * ASPECT_RATIO)
    #define TSX 64 * (0.75 * ASPECT_RATIO)
    #define TSY 64 * (0.75 * ASPECT_RATIO)

    #define GHOST_LIST_SIZE 7 

    typedef struct {
        file_info_t map;
        gsdl_phys_obj_t * coll_tiles;
        u32 tile_len;
    } lvl_t;

    typedef struct game_state {
        u08 g_state;
        gsdl_img_t render_texture;
        gsdl_phys_obj_t render_texture_obj;
        gsdl_phys_obj_t mouse;

        char * integer_str;
        char * final_dbg_str;

        // transition
        gsdl_circle_t transition_circle;
        u08 transition;

        // Menu
        gsdl_ui_label_t game_name;
        gsdl_ui_label_t press_to_play;

        // Debug
        gsdl_ui_label_t frame_time_label;
        SDL_Texture * frame_time_textures[MAX_MS_SIZE];
        gsdl_ui_label_t mem_used_label;
        SDL_Texture * mem_used_textures[MAX_MEM_USED];

        gsdl_circle_t sharingan;
        gsdl_circle_t sharingan_tomoe[4];

        // player.h
        u08 player_img_idx; 

        gsdl_anim_data_t * player_img_data;
        gsdl_anim_data_t * player_prev_img_data;
        gsdl_animated_img_t player_anim_img;
        gsdl_animated_img_t player_prev_img;

        gsdl_phys_obj_t player_prev[GHOST_LIST_SIZE];
        u32 curr_ghost;

        gsdl_phys_obj_t player;
        gsdl_particles_t player_particles;

        f32 player_air_timer;
        i32 player_can_dash;
        f32 player_dash_timer;

        f32 player_shoot_delay;
        f32 player_shoot_timer;
        u08 player_shooting;
        u08 player_shoot_keyup;

        gsdl_circle_t bullet_circle;
        gsdl_circle_t bullet_circle_outline;
        f32 circle_timer;

        gsdl_img_t bullet_texture;
        gsdl_phys_obj_t bullets[50];
        i32 bullet_alpha[50];
        f32 bullet_created[50];
        i32 bullet_len;

        gsdl_cam_t camera;
        //
        lvl_t lvls[10];
        u08 current_map;
        u32 coin_count_for_level[10];
        u32 coins_collected;

        gsdl_img_t square_texture[30];
        gsdl_phys_obj_t square[30];

        i32 saved_dt_avg;
        i32 saved_mem_usage;
        i32 f_count;
    } game_state_t;

#endif