#ifndef PLAYER_H
    #define PLAYER_H

    #include <state.h>
    void player_init(game_state_t * state, gsdl_props_t * props);
    void player_handle_events(game_state_t * state, gsdl_props_t * props);
    void player_update(game_state_t * state, gsdl_props_t * props);
    void player_render(game_state_t * state, gsdl_props_t * props);
#endif