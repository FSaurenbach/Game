#ifndef GAME_H
    #define GAME_H

    #include <behaviours.h>
    #include <state.h>

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