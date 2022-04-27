#ifndef LVL_H 
    #define LVL_H

    #include <state.h>
    void lvl_init(game_state_t * state, gsdl_props_t * props);
    void lvl_update(game_state_t * state, gsdl_props_t * props);
    void lvl_render(game_state_t * state, gsdl_props_t * props);
#endif