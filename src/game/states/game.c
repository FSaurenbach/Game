#include <game.h>

game_state_t * game_init(gsdl_props_t * props) {
    gsdl_init(ARGS, props);
    game_state_t * state; 

    { // Game state initialization
        state = calloc(1, sizeof(game_state_t));
        state -> g_state = 0;
        state -> integer_str = calloc(24, sizeof(char));
        state -> final_dbg_str = calloc(64, sizeof(char));
        SDL_SetWindowMinimumSize(props -> win, WIDTH, HEIGHT);
        //SDL_SetWindowMaximumSize(props -> win, 2560, 1440);
    }

    { // Actual game stuff
        //state -> player_animations;

        state -> player_img_data = calloc(4, sizeof(gsdl_anim_data_t));
        state -> player_img_data[0] = (gsdl_anim_data_t) { "idle", "res/player/idle/", 2, 30, 0 };
        state -> player_img_data[1] = (gsdl_anim_data_t) { "run", "res/player/run/", 4, 5, 0 };
        state -> player_img_data[2] = (gsdl_anim_data_t) { "power", "res/player/power/", 2, 30, 0 };
        state -> player_img_data[3] = (gsdl_anim_data_t) { "jump", "res/player/jump/", 1, 0, 0 };

        gsdl_init_animated_img(&state -> player_anim_img, state -> player_img_data, 4);
        gsdl_load_animations(&state -> player_anim_img, props -> renderer, &props -> texture_storage, &props -> ptr_storage, 255);

        // for some reason fails on smaller ones

        gsdl_create_phys_obj(&state -> player, SPAWN_POS, mk_v2((3 * (PSX + TSX)) / 40, 0), PSX - 2, TSY - 2);
        gsdl_create_cam(&state -> camera);

        for (u32 u = 0; u < 100; u++) {
            gsdl_phys_obj_t obj;
            i32 y = u - 10;
            if (y > 0) {
                gsdl_create_phys_obj(&obj, mk_v2(TSX * u, 100), mk_v2(0, 0), TSX, TSY);
            } else {
                gsdl_create_phys_obj(&obj, mk_v2(TSX * u, 100 - TSY), mk_v2(0, 0), TSX, TSY);
            }
            state -> tiles[u] = obj;
        }

        for (u32 u = 0; u < 50; u++) {
            gsdl_phys_obj_t obj;
            gsdl_create_phys_obj(&obj, mk_v2(TSX * u, 100 - TSY * 2), mk_v2(0, 0), TSX, TSY);
            state -> tiles[u + 100] = obj;
        }

        for (u32 u = 0; u < 150; u++) {
            gsdl_phys_obj_t obj = state -> tiles[u];
            obj.pos.x -= 1;
            obj.pos.y -= 1;
            obj.w += 2;
            obj.h += 2;
            state -> tile_shadow[u] = obj;
        }

        gsdl_create_tex(&state -> bullet_texture, 20, 16, 240, 60, 50, 255, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        state -> bullet_texture.angle = 45;
        gsdl_create_circle(&state -> bullet_circle, 0, 0, 0, 255, 100, 150, 255, 1);
        gsdl_create_circle(&state -> bullet_circle_outline, state -> bullets[state -> bullet_len].pos.x, state -> bullets[state -> bullet_len].pos.y, 0, 255, 255, 255, 255, 0);

        gsdl_init_particles(&state -> player_particles, 40, -2); 
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

        gsdl_create_ui_label(&state -> game_name, W_HALF, H_HALF - 100, TITLE, "./res/m5x7.ttf", cWHITE, 60, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        gsdl_center_ui_label(&state -> game_name);

        gsdl_create_ui_label(&state -> press_to_play, W_HALF, H_HALF, "Press Space to Play or Q to Quit", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        gsdl_center_ui_label(&state -> press_to_play);

        gsdl_create_ui_label(&state -> frame_time_label, 60, 10, "Calculating AVG Frame Time...", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        gsdl_create_ui_label(&state -> mem_used_label, 60, 30, "Calculating Memory Used (mb)...", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        memset(state -> frame_time_textures, 0, MAX_MS_SIZE);
        memset(state -> mem_used_textures, 0, MAX_MEM_USED);
    }

    { // Create circle / Sharingan thingy 
        gsdl_create_circle(&state -> sharingan, W_HALF, H_HALF - 20, 148, 100, 30, 34, 255, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[0], W_HALF, H_HALF - 20, 50, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[1], W_HALF, H_HALF - 126, 20, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[2], W_HALF - 64, H_HALF + 24, 20, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[3], W_HALF + 64, H_HALF + 24, 20, cBLACK, 1);
    }

    { // Rotated Squares
        for (u32 u = 0; u < 30; u++) {
            u32 w = randnum_gen(32, 64);
            gsdl_create_phys_obj(&state -> square[u], mk_v2(randnum_gen(0, WIDTH), randnum_gen(0, HEIGHT)), mk_v2(0, randnum_gen(1, 3)), w, w);
            gsdl_create_tex(&state -> square_texture[u], state -> square[u].w, state -> square[u].h, 130, 60, 64, randnum_gen(50, 150), props -> renderer, &props -> texture_storage, &props -> ptr_storage);
        }
    }

    //SDL_SetWindowFullscreen(props -> win, SDL_WINDOW_FULLSCREEN_DESKTOP);

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
    gsdl_serialize_img(&state -> render_texture, "render.png", props -> renderer);

    free(state -> integer_str);
    free(state -> final_dbg_str);
    // add to pointer stg

    gsdl_destroy(props);    
}