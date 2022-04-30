#include <game.h>

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

    player_handle_events(state, props);
}

void main_update(gsdl_props_t * props, game_state_t * state) {
    lvl_update(state, props);
    player_update(state, props);
    // break up frame counting and animation

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

    for (u32 u = 0; u < 30; u++) {
        state -> square[u].move_t = 1;
        gsdl_update_phys_obj_vel(&state -> square[u], 0);
        gsdl_update_phys_obj_pos(&state -> square[u], NULL, 0);
        state -> square_texture[u].angle += 1; 

        if (state -> square[u].pos.y > HEIGHT + 64) {
            state -> square[u].pos.y = -100;
        }
    }
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
            state -> mem_used_label.label_text = "Mem Usage TOO High (> 120mb)";
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

        //gsdl_draw_phys_obj_rect(&state -> player, 244, 244, 255, 255, &state -> camera, props -> renderer);
        lvl_render(state, props);
        player_render(state, props);

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
        // Handle events
        main_handle_events(props, state);
        // Update
        main_update(props, state);
    gsdl_start_frame_time_rec(props);
        // Rendering
        main_render(props, state); 
    gsdl_stop_frame_time_rec(props);
}

