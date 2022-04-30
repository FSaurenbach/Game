#include <game.h>

void menu_handle_events(gsdl_props_t * props, game_state_t * state) {
    SDL_PollEvent(&props -> event);
    switch (props -> event.type) {
        case SDL_QUIT:
            props -> running = 0;
            break;
        
        case SDL_KEYDOWN:
            if (gsdl_check_key(props -> event, SDL_SCANCODE_Q)) {
                props -> running = 0;
            }
            if (gsdl_check_key(props -> event, SDL_SCANCODE_SPACE)) {
                state -> transition = 1;
            }

        default:
            break;
    }
}

void menu_update(gsdl_props_t * props, game_state_t * state) {
    if (state -> transition - 1 == 0) {
        state -> transition_circle.rad += 40;
        if (state -> transition_circle.rad > WIDTH) {
            state -> g_state = 1;
            state -> render_texture.angle = 0;
        }
    }

    if (state -> transition - 1 == 1) {
        state -> transition_circle.rad -= 40;
        if (state -> transition_circle.rad < 0) {
            state -> transition = 0;
        }
    }
}

void menu_render(gsdl_props_t * props, game_state_t * state) {
    gsdl_start_render(props, cBLACK, state -> render_texture.tex);
        { // Main rendering stuff
            gsdl_draw_circle(&state -> sharingan, props -> renderer);
            for (u08 i = 0; i < 4; i++) {
                gsdl_draw_circle(&state -> sharingan_tomoe[i], props -> renderer);
            }
            gsdl_draw_ui_label(&state -> press_to_play, props -> renderer);
            gsdl_draw_ui_label(&state -> game_name, props -> renderer);
            gsdl_draw_circle(&state -> transition_circle, props -> renderer);
        }
    gsdl_end_render(props);
    gsdl_render_all(props, cBLACK, &state -> render_texture, &state -> render_texture_obj, NULL, 1, WIDTH, HEIGHT);
}

void menu_loop(gsdl_props_t * props, game_state_t * state) { 
    // Handle events
    menu_handle_events(props, state);
    // Update
    menu_update(props, state);
    // Rendering
    menu_render(props, state); 
}

