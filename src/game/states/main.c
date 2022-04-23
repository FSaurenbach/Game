#include <game.h>

void handle_player_events(gsdl_props_t * props, game_state_t * state) {
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

    if (props -> keys_pressed[SDL_SCANCODE_SPACE] || props -> keys_pressed[SDL_SCANCODE_UP]) {
        if (state -> player_air_timer < 2 && state -> player.coll_l + state -> player.coll_r == 0) {
            state -> player.y_momentum = -11;
            if (state -> player_air_timer < 0.75) {
                v2_t store = state -> player.pos;
                gsdl_update_particles(&state -> player_particles, -1, -1, -1, -1, 1, store.x + state -> player.w, store.y + state -> player.h, state -> player.w, state -> player.h / 4, 1, 5, 15);
            }
        }
        if (state -> player.coll_l + state -> player.coll_r != 0) {
            state -> player.y_momentum = -5;
        }
    } 

    if (props -> keys_pressed[SDL_SCANCODE_Y]) {
        SDL_Delay(100);
    }

    if (props -> keys_pressed[SDL_SCANCODE_S] || props -> keys_pressed[SDL_SCANCODE_DOWN]) {
        // fix problem with smaller squares not being registered
        if (state -> shoot_delay < 8 && state -> bullet_keyup <= 1) {
            state -> player.move_l = 0;
            state -> player.move_r = 0;
            state -> player.move_b = 0;
            state -> player.move_t = 0;

            gsdl_set_animation_state(&state -> player_anim_img, "power");

            state -> bullet_creating = 1;
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

            if (state -> bullet_creating) {
                state -> bullet_alpha[state -> bullet_len] = 255;
            }
            if (!state -> bullet_keyup) {
                state -> bullet_keyup++;
            }
        }
    }
}

void main_handle_events(gsdl_props_t * props, game_state_t * state) {
    SDL_PollEvent(&props -> event);
    switch (props -> event.type) {
        case SDL_QUIT:
            props -> running = 0;
            break;

        default:
            break;
    }

    props -> keys_pressed = SDL_GetKeyboardState(NULL);
    if (props -> keys_pressed[SDL_SCANCODE_ESCAPE]) {
        state -> transition = 2;
    }

    if (props -> keys_pressed[SDL_SCANCODE_R]) {
        state -> player.pos = SPAWN_POS;
    }
    handle_player_events(props, state);
}

void main_update(gsdl_props_t * props, game_state_t * state) {
    SDL_SetRenderTarget(props -> renderer, state -> render_texture.tex);

    // break up frame counting and animation
    if (state -> player_dash_timer != 0 && state -> player_dash_timer > 0.1) {
        state -> player_dash_timer -= 0.5;
        state -> player_can_dash = 0;
    }

    if (state -> player_dash_timer <= 0) {
        state -> player_dash_timer = 0;
        state -> player_can_dash = 1;
    }


    if (state -> transition - 1 == 0) {
        state -> transition_circle.rad -= 40;
        if (state -> transition_circle.rad < 0) {
            state -> transition = 0;
        }
    }

    if (state -> transition - 1 == 1) {
        state -> transition_circle.rad += 40;
        if (state -> transition_circle.rad > WIDTH) {
            state -> g_state = 0;
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

    if (state -> bullet_creating) {
        i32 cx = state -> bullets[state -> bullet_len].pos.x - state -> camera.x;
        i32 cy = state -> bullets[state -> bullet_len].pos.y - state -> camera.y;
        state -> shoot_delay += 0.25;
        if (state -> shoot_delay >= 8) {
            gsdl_set_animation_state(&state -> player_anim_img, "idle");
            state -> bullet_creating = 0;
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

    if (!state -> bullet_creating) {
        if (state -> bullet_len + 1 < 50) {
            state -> bullet_created[state -> bullet_len] = 1;
            state -> bullet_len++;
            state -> bullet_creating = 0;
        }
        if (!(state -> bullet_len + 1 < 50)) {
            state -> bullet_len = 0;
        }
        state -> bullet_keyup = 0;
    }

    if (state -> shoot_delay >= 8) {
        state -> shoot_timer += 0.25;
        if (state -> shoot_timer > 18) { 
            state -> shoot_delay = 0;
            state -> shoot_timer = 0;
        }
    }

    for (u32 u = 0; u < 30; u++) {
        state -> square[u].move_t = 1;
        gsdl_update_phys_obj_vel(&state -> square[u]);
        gsdl_update_phys_obj_pos(&state -> square[u], NULL, 0);
        state -> square_texture[u].angle += 1; 

        if (state -> square[u].pos.y > HEIGHT + 64) {
            state -> square[u].pos.y = -100;
        }
    }

    if (state -> player.move_l) {
        state -> player_anim_img.current_img.flip = 1;
    }
    if (state -> player.move_r) {
        state -> player_anim_img.current_img.flip = 0;
    }

    gsdl_calc_cam_pos(&state -> camera, props, &state -> player, TSX, TSY);
    gsdl_update_phys_obj_vel(&state -> player);

    if (props -> keys_pressed[SDL_SCANCODE_W]) {
        if (state -> player_can_dash) {
            if (state -> player.move_l) {
                state -> player.vel.x -= 10;
            }  
            if (state -> player.move_r) {
                state -> player.vel.x += 10;
            }
            state -> player_dash_timer += 0.1;
        }
    }

    gsdl_update_particles(&state -> player_particles, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    gsdl_update_phys_obj_pos(&state -> player, state -> tiles, 150);

    if ((state -> player.vel.x < 0 || state -> player.vel.x > 0) && state -> player.y_momentum == 0) {
        gsdl_set_animation_state(&state -> player_anim_img, "run");
    } 
    
    if ((state -> player.vel.x == 0 && state -> player.vel.y == 0) && state -> player.y_momentum == 0 && strcmp(state -> player_anim_img.state, "power") != 0) {
        gsdl_set_animation_state(&state -> player_anim_img, "idle");
    }
    
    if (((state -> player.y_momentum < 0 || state -> player.y_momentum >= MAX_Y_MOMENTUM) && state -> player_air_timer > 0.25)) {
        gsdl_set_animation_state(&state -> player_anim_img, "jump");
    }

    for (u32 u = 0; u < 50; u++) {
        gsdl_phys_obj_t obj = state -> bullets[u];
        if (state -> bullet_created[u] >= 1) {
            gsdl_update_phys_obj_vel(&obj);
            gsdl_update_phys_obj_pos(&obj, NULL, 0);
        }
        state -> bullets[u] = obj;
    } 


    i32 wall_slid = 0;
    if ((state -> player.coll_l + state -> player.coll_r) && (state -> player.pos.y > 1 * TSY || state -> player.pos.y < 1 * TSY)) {
        wall_slid = 1;
        state -> player.y_momentum = 0;
        state -> player_air_timer = 0;
    }
    if (!wall_slid) {
        state -> player.y_momentum += 1.5;
    }

    else {
        state -> player.y_momentum += 0.5;
    }

    if (state -> player.coll_b) {
        state -> player.y_momentum = 0;
        state -> player_air_timer = 0;
    } 
    if (!state -> player.coll_b && !wall_slid) {
        state -> player_air_timer += 0.25;
    }

    if (state -> player.y_momentum >= MAX_Y_MOMENTUM) {
        state -> player.y_momentum = MAX_Y_MOMENTUM;
    }

    gsdl_update_animations(&state -> player_anim_img);
}
void main_render(gsdl_props_t * props, game_state_t * state) {
    i32 changed_frame_time = 0;
    state -> saved_dt_avg = props -> dt;
    gsdl_get_frame_time_str("AVG Frame Time (ms): ", state -> integer_str, state -> final_dbg_str, props);
    if (state -> saved_dt_avg != props -> dt) {
        if (props -> dt >= MAX_MS_SIZE - 1) {
            state -> frame_time_label.label_text = "Frame Time Is TOO High (> 35ms)";
            state -> frame_time_textures[MAX_MS_SIZE - 1] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> frame_time_label.label_font, state -> frame_time_label.label_text, state -> frame_time_label.text_color));
        }

        if (props -> dt < MAX_MS_SIZE && state -> frame_time_textures[props -> dt] == 0x0) {
            state -> frame_time_label.label_text = state -> final_dbg_str;
            state -> frame_time_textures[props -> dt] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> frame_time_label.label_font, state -> frame_time_label.label_text, state -> frame_time_label.text_color));
        }
        state -> saved_dt_avg = props -> dt;
        changed_frame_time = 1;
    } 

    memset(state -> final_dbg_str, 0, strlen(state -> final_dbg_str));
    i32 changed_mem_usage = 0;
    state -> saved_mem_usage = props -> mb_used;
    gsdl_get_mem_used_str(state -> integer_str, state -> final_dbg_str, props);
    if ((state -> saved_mem_usage != props -> mb_used) || (state -> f_count == 0)) {
        if (props -> mb_used >= MAX_MEM_USED - 1) {
            state -> mem_used_label.label_text = "Mem Usage TOO High (> 50mb)";
            state -> mem_used_textures[MAX_MEM_USED - 1] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> mem_used_label.label_font, state -> mem_used_label.label_text, state -> mem_used_label.text_color));
        }

        if (props -> mb_used < MAX_MEM_USED && state -> mem_used_textures[props -> mb_used] == 0x0) {
            state -> mem_used_label.label_text = state -> final_dbg_str;
            state -> mem_used_textures[props -> mb_used] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> mem_used_label.label_font, state -> final_dbg_str, state -> mem_used_label.text_color));
        }
        state -> saved_mem_usage = props -> mb_used;
        changed_mem_usage = 1;
        state -> f_count++;
    } 
    gsdl_start_render(props, 60, 30, 34, 255, state -> render_texture.tex);
    { // Main rendering stuff   

        for (u32 u = 0; u < 30; u++) {
            gsdl_draw_phys_obj(&state -> square_texture[u], &state -> square[u], NULL, props -> renderer);
        }

        for (u32 u = 0; u < 50; u++) {
            gsdl_phys_obj_t obj = state -> bullets[u];
            gsdl_set_img_alpha((&state -> bullet_texture), state -> bullet_alpha[u]);
            gsdl_draw_phys_obj(&state -> bullet_texture, &obj, &state -> camera, props -> renderer);
        }            

        //gsdl_draw_phys_obj_rect(&state -> player, 244, 244, 255, 255, &state -> camera, props -> renderer);
        gsdl_draw_phys_obj(&state -> player_anim_img.current_img, &state -> player, &state -> camera, props -> renderer);
        gsdl_draw_circle(&state -> bullet_circle_outline, props -> renderer);
        gsdl_draw_circle(&state -> bullet_circle, props -> renderer);

        for (u32 u = 0; u < 150; u++) {
            gsdl_draw_phys_obj_rect(&state -> tile_shadow[u], 0, 0, 0, 255, &state -> camera, props -> renderer);
            gsdl_draw_phys_obj_rect(&state -> tiles[u], 255, u * 20, u * 20, 255, &state -> camera, props -> renderer);
        }

        gsdl_draw_particles(&state -> player_particles, &state -> camera, props -> renderer); 
        
        gsdl_draw_circle(&state -> transition_circle, props -> renderer); 

        if (changed_frame_time) {
            state -> frame_time_label.label_tex = state -> frame_time_textures[props -> dt];   
            SDL_QueryTexture(state -> frame_time_label.label_tex, NULL, NULL, &state -> frame_time_label.pos.w, &state -> frame_time_label.pos.h);
        }

        gsdl_draw_ui_label(&state -> frame_time_label, props -> renderer);

        if (changed_mem_usage) {
            state -> mem_used_label.label_tex = state -> mem_used_textures[props -> mb_used];
            SDL_QueryTexture(state -> mem_used_label.label_tex, NULL, NULL, &state -> mem_used_label.pos.w, &state -> mem_used_label.pos.h);
        }

        gsdl_draw_ui_label(&state -> mem_used_label, props -> renderer);
        memset(state -> final_dbg_str, 0, strlen(state -> final_dbg_str));

    }
    gsdl_end_render(props);
    gsdl_render_all(props, cBLACK, &state -> render_texture, &state -> render_texture_obj, NULL, 1, WIDTH, HEIGHT);
}
void main_loop(gsdl_props_t * props, game_state_t * state) { 
    gsdl_start_frame_time_rec(props);
    // Handle events
    main_handle_events(props, state);
    // Update
    main_update(props, state);
    // Rendering
    main_render(props, state); 
    gsdl_stop_frame_time_rec(props);
}

