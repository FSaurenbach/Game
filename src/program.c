#include <game.h>

gsdl_props_t * game;
game_state_t * state;

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    void loop_em() {
        game_loop(game, state);
    }

    void gameplay() {
        emscripten_set_main_loop(loop_em, 0, 1);
    }
#else 
    void gameplay() {
        while (game -> running) {
            game_loop(game, state);
        }
    }
#endif

i32 main(i32 argv, char ** args) {
    game = calloc(1, sizeof(gsdl_props_t));
    state = game_init(game);

    if (!state) {
        logger_log(LOG_ERR, "Failed to initialize");
        return -1;
    } else {
        logger_log(LOG_SUCCESS, "Game state successfully initialized");
    }

    gameplay();

    game_cleanup(game, state);
    free(game);
    free(state);

    fflush(stdout);
    printf("\n");
    logger_log(LOG_SUCCESS, "Successfully Exited");
    return 0;
}