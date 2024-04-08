#include "game.cpp"

int main(int argc, char *argv[]) {
    Game game;
    game.init("Pong", 1400, 900);

    while (game.running()) {
        game.handle_events();
        game.update();
        game.render();
    }

    game.exit();

    return 0;
}