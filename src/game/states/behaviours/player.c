#include <behaviours.h>

void player_init(game_state_t * state, gsdl_props_t * props) {
    state -> player_img_data = calloc(4, sizeof(gsdl_anim_data_t));
    state -> player_img_data[0] = (gsdl_anim_data_t) { "idle", "res/player/idle/", 2, 30, 0 };
    state -> player_img_data[1] = (gsdl_anim_data_t) { "run", "res/player/run/", 4, 5, 0 };
    state -> player_img_data[2] = (gsdl_anim_data_t) { "power", "res/player/power/", 2, 30, 0 };
    state -> player_img_data[3] = (gsdl_anim_data_t) { "jump", "res/player/jump/", 1, 0, 0 };

    state -> player_prev_img_data = calloc(4, sizeof(gsdl_anim_data_t));
    state -> player_prev_img_data[0] = (gsdl_anim_data_t) { "idle", "res/player/idle/", 1, 30, 0 };
    state -> player_prev_img_data[1] = (gsdl_anim_data_t) { "run", "res/player/run/", 1, 30, 0 };
    state -> player_prev_img_data[2] = (gsdl_anim_data_t) { "power", "res/player/power/", 1, 30, 0 };
    state -> player_prev_img_data[3] = (gsdl_anim_data_t) { "jump", "res/player/jump/", 1, 0, 0 };

    gsdl_init_animated_img(&state -> player_anim_img, state -> player_img_data, 4);
    gsdl_init_animated_img(&state -> player_prev_img, state -> player_prev_img_data, 4);

    gsdl_load_animations(&state -> player_anim_img, props -> renderer, &props -> texture_storage, &props -> ptr_storage, 255);
    gsdl_load_animations(&state -> player_prev_img, props -> renderer, &props -> texture_storage, &props -> ptr_storage, 60);

    state -> curr_ghost = 0;

    // for some reason fails on smaller ones (acc)
    gsdl_create_phys_obj(&state -> player, SPAWN_POS, mk_v2((3 * (PSX + TSX)) / 40, 0), PSX - 2, TSY - 2);
    gsdl_init_particles(&state -> player_particles, 40, -2); 
    gsdl_create_cam(&state -> camera);

    gsdl_create_tex(&state -> bullet_texture, 20, 16, 240, 60, 50, 255, props -> renderer, &props -> texture_storage, &props -> ptr_storage);
    state -> bullet_texture.angle = 45;
    gsdl_create_circle(&state -> bullet_circle, 0, 0, 0, 255, 100, 150, 255, 1);
    gsdl_create_circle(&state -> bullet_circle_outline, state -> bullets[state -> bullet_len].pos.x, state -> bullets[state -> bullet_len].pos.y, 0, 255, 255, 255, 255, 0);
}

void player_handle_events(game_state_t * state, gsdl_props_t * props) {
    state -> player.vel = mk_v2(0, 0);
    if (props -> keys_pressed[SDL_SCANCODE_D] || props -> keys_pressed[SDL_SCANCODE_RIGHT]) {
        state -> player.move_r = 1;
    } else {
        state -> player.move_r = 0;
    }

    if (props -> keys_pressed[SDL_SCANCODE_A] || props -> keys_pressed[SDL_SCANCODE_LEFT]) {
        state -> player.move_l = 1;
    } else {
        state -> player.move_l = 0;
    }
    // fix animation with climbing o remove mecchanic

    if (props -> keys_pressed[SDL_SCANCODE_SPACE] || props -> keys_pressed[SDL_SCANCODE_UP]) {
        if (state -> player_air_timer < 2) {
            state -> player.y_momentum = -15;
            if (state -> player_air_timer < 0.75) {
                v2_t store = state -> player.pos;
                gsdl_update_particles(&state -> player_particles, -1, -1, -1, -1, 1, store.x + state -> player.w, store.y + state -> player.h, state -> player.w, state -> player.h / 4, 1, 5, 15);
            }
        }
    } 

    if (props -> keys_pressed[SDL_SCANCODE_S] || props -> keys_pressed[SDL_SCANCODE_DOWN]) {
        // fix problem with smaller squares not being registered
        if (state -> player_shoot_delay < 8 && state -> player_shoot_keyup <= 1) {
            state -> player.move_l = 0;
            state -> player.move_r = 0;
            state -> player.move_b = 0;
            state -> player.move_t = 0;

            gsdl_set_animation_state(&state -> player_anim_img, "power");
            gsdl_set_animation_state(&state -> player_prev_img, "power");

            state -> player_shooting = 1;
            u08 move_l = state -> player_anim_img.current_img.flip;
            u08 move_r = !state -> player_anim_img.current_img.flip;

            if (state -> bullet_created[state -> bullet_len] >= 1) {
                gsdl_create_phys_obj(&state -> bullets[state -> bullet_len], mk_v2(0, 0), mk_v2(0, 0), 0, 0);
                state -> bullet_created[state -> bullet_len] = 0;
            }

            state -> bullet_created[state -> bullet_len] = 0.5;
            if (move_r) {
                gsdl_create_phys_obj(&state -> bullets[state -> bullet_len], mk_v2(state -> player.pos.x + state -> player.w + 2, state -> player.pos.y), mk_v2(10, 0), state -> bullets[state -> bullet_len].w + 1, state -> bullets[state -> bullet_len].h + 1);
            } else {
                gsdl_create_phys_obj(&state -> bullets[state -> bullet_len], mk_v2(state -> player.pos.x - state -> player.w - 2, state -> player.pos.y), mk_v2(10, 0), state -> bullets[state -> bullet_len].w + 1, state -> bullets[state -> bullet_len].h + 1);
            }

            state -> bullets[state -> bullet_len].move_l = move_l;
            state -> bullets[state -> bullet_len].move_r = move_r;

            if (state -> player_shooting) {
                state -> bullet_alpha[state -> bullet_len] = 255;
            }
            if (!state -> player_shoot_keyup) {
                state -> player_shoot_keyup++;
            }
        }
    }

    if (props -> keys_pressed[SDL_SCANCODE_R]) {
        state -> player.pos = SPAWN_POS;
    }

    if (props -> keys_pressed[SDL_SCANCODE_W]) {
        if (state -> player_can_dash) {
            state -> player.y_momentum = 0;
            if (state -> player.move_l) {
                state -> player.vel.x -= 30;
            }  
            if (state -> player.move_r) {
                state -> player.vel.x += 30;
            }
            state -> player_dash_timer += 0.1;
        }
    }
    else {
        state -> player_dash_timer -= 0.025;
    }
}

i32 alpha = 0;
void player_update(game_state_t * state, gsdl_props_t * props) {
    // fix dash physics and timer
    if (state -> player_dash_timer > 1) {
        state -> player_can_dash = 0;
    }
    /*
    if (!state -> player_can_dash) {
        state -> player_dash_timer -= 0.025;
    } */

    if (state -> player_dash_timer <= 0) {
        state -> player_dash_timer = 0;
        state -> player_can_dash = 1;
    }

    if (state -> player.move_l) {
        state -> player_anim_img.current_img.flip = 1;
    }
    if (state -> player.move_r) {
        state -> player_anim_img.current_img.flip = 0;
    }

    gsdl_calc_cam_pos(&state -> camera, props, &state -> player, TSX, TSY);
    gsdl_update_phys_obj_vel(&state -> player, 1);


    gsdl_update_particles(&state -> player_particles, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    gsdl_update_phys_obj_pos(&state -> player, state -> lvls[state -> current_map].coll_tiles, state -> lvls[state -> current_map].tile_len);

    if ((state -> player.vel.x < 0 || state -> player.vel.x > 0) && state -> player.y_momentum == 0 && state -> player_air_timer == 0) {
        gsdl_set_animation_state(&state -> player_anim_img, "run");
        gsdl_set_animation_state(&state -> player_prev_img, "run");
    } 
    
    if ((state -> player.vel.x == 0 && state -> player.vel.y == 0) && state -> player.y_momentum == 0 && strcmp(state -> player_anim_img.state, "power") != 0 && state -> player_air_timer == 0) {
        gsdl_set_animation_state(&state -> player_anim_img, "idle");
        gsdl_set_animation_state(&state -> player_prev_img, "idle");
    }
    
    //if ((state -> player.y_momentum < 0 || state -> player.y_momentum > 0.75 * 2) && state -> player_air_timer > 0) {
    if (!state -> player.coll_b && state -> player_air_timer > 0.25 && (state -> player.y_momentum > 0.75 * 10 || state -> player.y_momentum < 0)) {
        gsdl_set_animation_state(&state -> player_anim_img, "jump");
        gsdl_set_animation_state(&state -> player_prev_img, "jump");
    }

    state -> player.y_momentum += 0.6;
    if (state -> player.coll_b) {
        state -> player.y_momentum = 0;
        state -> player_air_timer = 0;
    } 
    if (!state -> player.coll_b) {
        state -> player_air_timer += 0.25;
    }

    if (state -> player.coll_t) {
        state -> player.y_momentum += 2;
    }

    if (state -> player.y_momentum >= MAX_Y_MOMENTUM) {
        state -> player.y_momentum = MAX_Y_MOMENTUM;
    }

    gsdl_update_animations(&state -> player_anim_img);
    // fix
    if (state -> player_dash_timer) {
        gsdl_update_animations(&state -> player_prev_img);
        if (state -> curr_ghost < GHOST_LIST_SIZE) {
            state -> player_prev[state -> curr_ghost] = state -> player;
            state -> curr_ghost++;
        } else {
            state -> curr_ghost = 0;
        }
        alpha = 60;
        gsdl_set_img_alpha((&state -> player_prev_img.current_img), 60);
    }
   // else {
    if (!state -> player_dash_timer) {
        gsdl_set_img_alpha((&state -> player_prev_img.current_img), alpha);
        alpha -= 2;
        if (alpha < 0) {
            alpha = 0;
        }
        if (alpha == 0) {
            for (u32 u = 0; u < GHOST_LIST_SIZE; u++) {
                state -> player_prev[u] = state -> player;
            }
        }
    }

    if (!state -> player_shooting) {
        if (state -> bullet_len + 1 < 50) {
            state -> bullet_created[state -> bullet_len] = 1;
            state -> bullet_len++;
            state -> player_shooting = 0;
        }
        if (!(state -> bullet_len + 1 < 50)) {
            state -> bullet_len = 0;
        }
        state -> player_shoot_keyup = 0;
    }

    if (state -> player_shoot_delay >= 8) {
        state -> player_shoot_timer += 0.25;
        if (state -> player_shoot_timer > 18) { 
            state -> player_shoot_delay = 0;
            state -> player_shoot_timer = 0;
        }
    }

    if (state -> player_shooting) {
        i32 cx = state -> bullets[state -> bullet_len].pos.x - state -> camera.x;
        i32 cy = state -> bullets[state -> bullet_len].pos.y - state -> camera.y;
        state -> player_shoot_delay += 0.25;
        if (state -> player_shoot_delay >= 8) {
            gsdl_set_animation_state(&state -> player_anim_img, "idle");
            gsdl_set_animation_state(&state -> player_prev_img, "idle");
            state -> player_shooting = 0;
            if (state -> bullets[state -> bullet_len].move_l) {
                state -> player.pos.x += 10;
            }
            if (state -> bullets[state -> bullet_len].move_r) {
                state -> player.pos.x -= 10;
            }
            gsdl_create_circle(&state -> bullet_circle, cx + 25, cy, 25, 255, 255, 255, 255, 1);
            gsdl_create_circle(&state -> bullet_circle_outline, cx + 25, cy, state -> bullet_circle.rad + 4, 255, 100, 150, 255, 1);
        }
    }

    if (state -> bullet_circle.rad > 0) {
        if (state -> circle_timer >= 0.5) {
            state -> bullet_circle.rad = -10;
            state -> bullet_circle_outline.rad = -10;
            state -> circle_timer = 0;
        } else {
            state -> circle_timer += 0.05;
        }
    }

    for (u32 u = 0; u < 50; u++) {
        gsdl_phys_obj_t obj = state -> bullets[u];
        if (state -> bullet_created[u] >= 1) {
            gsdl_update_phys_obj_vel(&obj, 0);
            gsdl_update_phys_obj_pos(&obj, NULL, 0);
        }
        state -> bullets[u] = obj;
    } 
}
void player_render(game_state_t * state, gsdl_props_t * props) {
    for (u32 u = 0; u < GHOST_LIST_SIZE; u++) { 
        gsdl_draw_phys_obj(&state -> player_prev_img.current_img, &state -> player_prev[u], &state -> camera, props -> renderer);
    }

    gsdl_draw_phys_obj(&state -> player_anim_img.current_img, &state -> player, &state -> camera, props -> renderer);
    gsdl_draw_particles(&state -> player_particles, &state -> camera, props -> renderer); 

    for (u32 u = 0; u < 50; u++) {
        gsdl_phys_obj_t obj = state -> bullets[u];
        gsdl_set_img_alpha((&state -> bullet_texture), state -> bullet_alpha[u]);
        gsdl_draw_phys_obj(&state -> bullet_texture, &obj, &state -> camera, props -> renderer);
    }  
    gsdl_draw_circle(&state -> bullet_circle_outline, props -> renderer);
    gsdl_draw_circle(&state -> bullet_circle, props -> renderer);

}