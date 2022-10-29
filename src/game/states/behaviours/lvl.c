#include <behaviours.h>

void lvl_init(game_state_t * state, gsdl_props_t * props) {
    char * current_map = calloc(128, sizeof(char));
    char * prefix = "res/maps/";
    char * final = calloc(129, sizeof(char));

    for (u08 u = 0; u < 10; u++) {
        char * curr_frame = i32_to_str(u);
        strncat(current_map, prefix, strlen(prefix));
        strncat(current_map, curr_frame, strlen(curr_frame));
        strncat(current_map, ".txt", strlen(".txt"));

        memcpy(final, current_map, strlen(current_map));
        final[strlen(current_map) + 1] = '\0';

        state -> lvls[u].map = txt_file_query(final);

        state -> lvls[u].coll_tiles = calloc(state -> lvls[u].map.len, sizeof(gsdl_phys_obj_t)); 
        state -> lvls[u].tile_len = 0;

        memset(current_map, 0, strlen(current_map) * sizeof(char));
        memset(final, 0, strlen(final) * sizeof(char));
    }

    i32 x = 0;
    i32 y = 0;
    
    for (u08 u = 0; u < 10; u++) {
        state -> coin_count_for_level[u] = 0;
        for (u32 t = 0; t < state -> lvls[u].map.len; t++) {
            if (state -> lvls[u].map.content[t] != '0' && state -> lvls[u].map.content[t] != '3' && state -> lvls[u].map.content[t] != '4') {
                state -> lvls[u].coll_tiles[state -> lvls[u].tile_len].pos.x = x * TSX;
                state -> lvls[u].coll_tiles[state -> lvls[u].tile_len].pos.y = y * TSY;
                state -> lvls[u].coll_tiles[state -> lvls[u].tile_len].w = TSX;
                state -> lvls[u].coll_tiles[state -> lvls[u].tile_len].h = TSY;
                state -> lvls[u].tile_len++;
            }
            
            if (state -> lvls[u].map.content[t] == '3') {
                state -> coin_count_for_level[u] += 1;
            }

            if (state -> lvls[u].map.content[t] != '\n') {
                x++;
            }

            if (state -> lvls[u].map.content[t] == '\n') {
                x = 0;
                y++;
            }
        }
        x = 0;
        y = 0;
    }

}

void lvl_update(game_state_t * state, gsdl_props_t * props) {
    i32 x = 0;
    i32 y = 0;
    for (u32 u = 0; u < state -> lvls[state -> current_map].map.len; u++) {
        if (state -> lvls[state -> current_map].map.content[u] == '3') {
            gsdl_phys_obj_t obj; 
            obj.pos.x = x * TSX;
            obj.pos.y = y * TSY;
            obj.w = TSX;
            obj.h = TSY;
            gsdl_phys_obj_t p = state -> player;
            if (gsdl_phys_obj_coll_detect(p, obj)) {
                state -> coins_collected++;
                state -> lvls[state -> current_map].map.content[u] = '0';
                if (state -> coins_collected >= state -> coin_count_for_level[state -> current_map]) {
                    state -> current_map++;
                    state -> player.pos = SPAWN_POS;
                    state -> coins_collected = 0;
                }
            }
        }

        if (state -> lvls[state -> current_map].map.content[u] == '4') {
            gsdl_phys_obj_t obj; 
            obj.pos.x = x * TSX;
            obj.pos.y = y * TSY;
            obj.w = TSX;
            obj.h = TSY;
            gsdl_phys_obj_t p = state -> player;
            if (gsdl_phys_obj_coll_detect(p, obj)) {
                state -> player.pos = SPAWN_POS;
            }
        }

        if (state -> lvls[state -> current_map].map.content[u] != '\n') {
            x++;
        }

        if (state -> lvls[state -> current_map].map.content[u] == '\n') {
            x = 0;
            y++;
        }
    }

}

void lvl_render(game_state_t * state, gsdl_props_t * props) {
    i32 x = 0;
    i32 y = 0;
    for (u32 u = 0; u < state -> lvls[state -> current_map].map.len; u++) {
        gsdl_phys_obj_t obj; 
        obj.pos.x = x * TSX;
        obj.pos.y = y * TSX;
        obj.w = TSX;
        obj.h = TSY;
        // or 1280/(64 * (0.75 * (1280/720))) ->  SW/TSX
        // 15 tiles render
        i32 r_dst = 14;
        if (state -> player.pos.x - (r_dst * TSX) < obj.pos.x && obj.pos.x < state -> player.pos.x + (r_dst * TSX)) {
            if (state -> lvls[state -> current_map].map.content[u] == '1' || state -> lvls[state -> current_map].map.content[u] == '2') {
                gsdl_draw_phys_obj_rect(&obj, 255, obj.pos.x * 2, 100, 255, &state -> camera, props -> renderer);
            }

            if (state -> lvls[state -> current_map].map.content[u] == '3') {
                gsdl_draw_phys_obj_rect(&obj, 255, 255, 100, 255, &state -> camera, props -> renderer);
            }

            if (state -> lvls[state -> current_map].map.content[u] == '4') {
                gsdl_draw_phys_obj_rect(&obj, 255, 10, 10, 255, &state -> camera, props -> renderer);
            }
        }

        if (state -> lvls[state -> current_map].map.content[u] != '\n') {
            x++;
        }

        if (state -> lvls[state -> current_map].map.content[u] == '\n') {
            x = 0;
            y++;
        }
    }
}