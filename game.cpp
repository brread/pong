#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include "src/include/SDL2/SDL.h"

class Game {
public:
    void init(const char *window_title, int window_width_in, int window_height_in) { // No error handling. I DON'T care!
        this->window_width = window_width_in;
        this->window_height = window_height_in;

        this->game_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width_in, window_height_in, SDL_WINDOW_SHOWN);
        this->game_renderer = SDL_CreateRenderer(this->game_window, -1, SDL_RENDERER_ACCELERATED);

        this->init_player(this->player1, 250);
        this->init_player(this->player2, window_width - 250);

        this->init_ball();

        this->last_frame = SDL_GetTicks();
    }

    bool running() { return is_running; }

    void handle_events() {
        SDL_Event e;

        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
                is_running = false;
    }

    void update() {
        int _, mouseY;

        SDL_GetMouseState(&_, &mouseY);

        this->player1.y = mouseY - this->player1.h / 2;

        int current_frame = SDL_GetTicks();
        float dt = (current_frame - this->last_frame);
        this->last_frame = current_frame;

        this->ball_x += this->ball_vx * dt;
        this->ball_y += this->ball_vy * dt;

        this->ball.x = (int)this->ball_x;
        this->ball.y = (int)this->ball_y;

        this->ball_update();
    }

    void render() {
        SDL_SetRenderDrawColor(this->game_renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(this->game_renderer);

        SDL_SetRenderDrawColor(this->game_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderFillRect(this->game_renderer, &this->player1);
        SDL_RenderFillRect(this->game_renderer, &this->player2);
        SDL_RenderFillRect(this->game_renderer, &this->ball);

        SDL_RenderPresent(this->game_renderer);

        // std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    void exit() {
        SDL_DestroyRenderer(this->game_renderer);
        SDL_DestroyWindow(this->game_window);

        SDL_Quit();
    }
private:
    const int pongWidth = 75;
    const int pongHeight = 350;

    const float ball_speed = 0.55;

    void init_player(SDL_Rect &player, int x) const {
        player.x = x - pongWidth / 2;
        player.y = (this->window_height - pongHeight) / 2;
        player.w = pongWidth;
        player.h = pongHeight;
    }

    void init_ball() {
        this->ball.w = 35;
        this->ball.h = 35;

        this->ball_x = (this->window_width / 2) - this->ball.w;
        this->ball_y = (this->window_height / 2) - this->ball.h;

        this->ball_vx = ball_speed;
        this->ball_vy = ball_speed;
    }

    void ball_update() {
        if (this->ball_x < 0) {
            this->ball_x = 1;
            this->ball_vx = fabsf(this->ball_vy);
        } else if (this->ball_x + this->ball.w > this->window_width) {
            this->ball_x = this->window_width - this->ball.w - 1;
            this->ball_vx = fabsf(this->ball_vx) * -1;
        }

        if (this->ball_y < 0) {
            this->ball_y = 1;
            this->ball_vy = fabsf(this->ball_vy);
        } else if (this->ball_y + this->ball.h > this->window_height) {
            this->ball_y = this->window_height - this->ball.h - 1;
            this->ball_vy = fabsf(this->ball_vy) * -1;
        }

        collide(this->player1, this->ball);
        collide(this->player2, this->ball);
    }

    void collide(SDL_Rect &player, SDL_Rect &pong_ball) {
        const float ballCenterX = this->ball_x + pong_ball.w / 2;
        const float ballCenterY = this->ball_y + pong_ball.h / 2;

        const float paddleCenterX = player.x + player.w / 2;
        const float paddleCenterY = player.y + player.h / 2;

        const float minDistX = player.w / 2 + pong_ball.w / 2; // min offset (or collision)
        const float minDistY = player.h / 2 + pong_ball.h / 2; // min offset (or collision)

        const float deltaX = paddleCenterX - ballCenterX; // offset
        const float deltaY = paddleCenterY - ballCenterY; // offset

        if (fabsf(deltaX) < minDistX && fabsf(deltaY) < minDistY) { // collision
            const float overlapX = minDistX - fabsf(deltaX);
            const float overlapY = minDistY - fabsf(deltaY);

            if (overlapX < overlapY) {
                const int bounceDir = ((deltaX < 0) * 2 - 1); // (delta < 0) ? 1 : -1

                this->ball_x += overlapX * bounceDir; // push out of paddle
                this->ball_vx = fabsf(this->ball_vx) * bounceDir; // bounce
            } else {
                const int bounceDir = ((deltaY < 0) * 2 - 1); // (delta < 0) ? 1 : -1

                this->ball_y += overlapY * bounceDir; // push out of paddle
                this->ball_vy = fabsf(this->ball_vy) * bounceDir; // bounce
            }
        }
    }

    bool is_running = true;

    int window_width;
    int window_height;

    int last_frame;

    SDL_Window *game_window;
    SDL_Renderer *game_renderer;

    SDL_Rect player1;
    SDL_Rect player2;

    SDL_Rect ball;

    float ball_x;
    float ball_y;

    float ball_vx;
    float ball_vy;
};