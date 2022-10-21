#include <game.h>

game_state_t * game_init(gsdl_props_t * props) {
        config_file_t config = parse_config_file("res/settings.txt");
        void ** data = config_file_get(&config, "name");
        printf("%s\n", data[1]);
    gsdl_init((gsdl_init_info_t) {data[1], WIDTH, HEIGHT, RENDERER_ACC, FPS, VSYNC, RESIZABLE}, props);

    game_state_t * state; 

    { // Game state initialization
        state = calloc(1, sizeof(game_state_t));
        state -> g_state = 0;

        SDL_SetWindowMinimumSize(props -> win, WIDTH, HEIGHT);
        //SDL_SetWindowMaximumSize(props -> win, 2560, 1440);
        state -> integer_str = calloc(24, sizeof(char));
        state -> final_dbg_str = calloc(64, sizeof(char));
    }

    { // Actual game stuff
        //state -> player_animations;
        state -> game_controls = ht_create(PLAYER_ACTIONS);
        
    }

    { // Transition
        gsdl_create_circle(&state -> transition_circle, W_HALF, H_HALF - 20, 0, 100, 30, 34, 255, 1);
        state -> transition = 0;
    }

    { // Create render texture
        gsdl_create_render_tex(&state -> render_texture, WIDTH, HEIGHT, 0, props -> renderer, &props -> texture_storage);
        gsdl_create_phys_obj(&state -> render_texture_obj, mk_v2(0, 0), mk_v2(0, 0), WIDTH, HEIGHT);
        gsdl_create_phys_obj(&state -> mouse, mk_v2(0, 0), mk_v2(0, 0), 16, 16); 
    }

    { // Create menu UI

        gsdl_create_ui_label(&state -> game_name, W_HALF, H_HALF - 100, "UnR34lity", "./res/m5x7.ttf", cWHITE, 60, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        gsdl_center_ui_label(&state -> game_name);

        gsdl_create_ui_label(&state -> press_to_play, W_HALF, H_HALF, "Press Space to Play or Q to Quit", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        gsdl_center_ui_label(&state -> press_to_play);

        gsdl_create_circle(&state -> sharingan, W_HALF, H_HALF - 20, 148, 100, 30, 34, 255, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[0], W_HALF, H_HALF - 20, 50, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[1], W_HALF, H_HALF - 126, 20, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[2], W_HALF - 64, H_HALF + 24, 20, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[3], W_HALF + 64, H_HALF + 24, 20, cBLACK, 1);
    }

    { // Debug UI
        gsdl_create_ui_label(&state -> frame_time_label, 60, 10, "Calculating AVG Frame Time...", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        gsdl_create_ui_label(&state -> mem_used_label, 60, 30, "Calculating Memory Used (mb)...", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        memset(state -> frame_time_textures, 0, MAX_MS_SIZE);
        memset(state -> mem_used_textures, 0, MAX_MEM_USED);
    }

    { // Background Squares
        for (u32 u = 0; u < 30; u++) {
            u32 w = randnum_gen(32, 64);
            gsdl_create_phys_obj(&state -> square[u], mk_v2(randnum_gen(0, WIDTH), randnum_gen(0, HEIGHT)), mk_v2(0, randnum_gen(1, 3)), w, w);
            gsdl_create_tex(&state -> square_texture[u], state -> square[u].w, state -> square[u].h, 130, 60, 64, randnum_gen(50, 150), props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        }
    }

    //SDL_SetWindowFullscreen(props -> win, SDL_WINDOW_FULLSCREEN_DESKTOP);

    player_init(state, props);
    lvl_init(state, props);
    return state;
}


// setoolkit
void game_loop(gsdl_props_t * props, game_state_t * state) {
    { // Handle game states 
        switch (state -> g_state) {
            case 0:
                menu_loop(props, state);
                break;

            case 1:
                main_loop(props, state);
                break;

            default:
                props -> running = 0;
                break;
        }
    }
}

void game_cleanup(gsdl_props_t * props, game_state_t * state) {
    // add to pointer stg

    gsdl_destroy(props);    
}

/* hw 

    chem--
    urdu
    world history
    physics--
    design
    ps 1--
    business--

*/