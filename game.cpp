#include <cmath>
#include "src/include/SDL2/SDL.h"
//#include "src/include/SDL2/SDL_ttf.h"

#define PI 3.14159

class Game {
public:
    void init(const char *window_title, int window_width_in, int window_height_in) { // No error handling. I DON'T care!
        this->window_width = window_width_in;
        this->window_height = window_height_in;

        this->game_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width_in, window_height_in, SDL_WINDOW_SHOWN);
        this->game_renderer = SDL_CreateRenderer(this->game_window, -1, SDL_RENDERER_ACCELERATED);

        this->init_player(this->player1, 250);
        this->init_player(this->player2, window_width - 250);

        this->player2_x = window_width - 250;

        this->init_ball();

        this->last_frame = SDL_GetTicks();

//        this->font = TTF_OpenFont("./Asap.ttf", 12);
    }

    bool running() { return is_running; }

    void handle_events() {
        SDL_Event e;

        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
                is_running = false;
    }

    void update() {
//        SDL_Delay(2);

        int _, mouseY;

        SDL_GetMouseState(&_, &mouseY);

        this->player1.y = mouseY - this->player1.h / 2;

        if (this->player1.y < 0) // clamp
            this->player1.y = 0;
        else if (this->player1.y + this->player1.h > this->window_height)
            this->player1.y = this->window_height - this->player1.h;

        int current_frame = SDL_GetTicks();
        float dt = (current_frame - this->last_frame);
        this->last_frame = current_frame;

        this->ball_x += this->ball_vx * dt;
        this->ball_y += this->ball_vy * dt;

        this->ball.x = (int)this->ball_x;
        this->ball.y = (int)this->ball_y;

        this->ball_update(dt);

        const int dir = ((this->player2_y + this->player2.h / 2 > this->ball_y + this->ball.h / 2) * 2 - 1);

        this->player2_y -= 0.6 * dir * dt;

        if (player2_y < 0)
            player2_y = 0;
        else if (player2_y + this->player2.h > this->window_height)
            player2_y = this->window_height - this->player2.h;

        this->player2.x = player2_x;
        this->player2.y = player2_y;
    }

    void render() {
        SDL_SetRenderDrawColor(this->game_renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(this->game_renderer);

        SDL_SetRenderDrawColor(this->game_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderFillRect(this->game_renderer, &this->player1);
        SDL_RenderFillRect(this->game_renderer, &this->player2);
        SDL_RenderFillRect(this->game_renderer, &this->ball);

        SDL_RenderPresent(this->game_renderer);

//         std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    void exit() {
        SDL_DestroyRenderer(this->game_renderer);
        SDL_DestroyWindow(this->game_window);

        SDL_Quit();
    }
private:
    void init_player(SDL_Rect &player, int x) const {
        player.x = x - pong_width / 2;
        player.y = (this->window_height - pong_height) / 2;
        player.w = pong_width;
        player.h = pong_height;
    }

    void init_ball() {
        this->ball.w = 15;
        this->ball.h = 15;

        this->ball_x = (this->window_width / 2) - this->ball.w;
        this->ball_y = (this->window_height / 2) - this->ball.h;

        this->ball_vx = ball_speed + ((double) rand() / (RAND_MAX)) * 0.2 - 0.1;
        this->ball_vy = ball_speed + ((double) rand() / (RAND_MAX)) * 0.2 - 0.1;
    }

    void ball_update(float dt) {
        if (this->ball_x < 0) {
            this->ball_x = 1;
            this->ball_vx = fabsf(this->ball_vx);
            this->ball_vx += ball_speedup * dt * (this->ball_vx / fabsf(this->ball_vx)); // speed up
        } else if (this->ball_x + this->ball.w > this->window_width) {
            this->ball_x = this->window_width - this->ball.w - 1;
            this->ball_vx = fabsf(this->ball_vx) * -1;
            this->ball_vx += ball_speedup * dt * (this->ball_vx / fabsf(this->ball_vx)); // speed up
        }

        if (this->ball_y < 0) {
            this->ball_y = 1;
            this->ball_vy = fabsf(this->ball_vy);
            this->ball_vy += ball_speedup * dt * (this->ball_vy / fabsf(this->ball_vy)); // speed up
        } else if (this->ball_y + this->ball.h > this->window_height) {
            this->ball_y = this->window_height - this->ball.h - 1;
            this->ball_vy = fabsf(this->ball_vy) * -1;
            this->ball_vy += ball_speedup * dt * (this->ball_vy / fabsf(this->ball_vy)); // speed up
        }

        collide(this->player1, this->ball, dt);
        collide(this->player2, this->ball, dt);
    }

    void collide(SDL_Rect &player, SDL_Rect &pong_ball, float dt) {
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
                this->ball_vx += ball_speedup * dt * (this->ball_vx / fabsf(this->ball_vx)); // speed up
            } else {
                const int bounceDir = ((deltaY < 0) * 2 - 1); // (delta < 0) ? 1 : -1

                this->ball_y += overlapY * bounceDir; // push out of paddle
                this->ball_vy = fabsf(this->ball_vy) * bounceDir; // bounce
                this->ball_vy += ball_speedup * dt * (this->ball_vy / fabsf(this->ball_vy)); // speed up
            }

            const float v_mag = sqrtf(this->ball_vx * this->ball_vx + this->ball_vy * this->ball_vy);

            float dir;

            if (this->ball_vx < 0)
                dir = ((double) rand() / (RAND_MAX)) * PI/2 + 3*PI/4;
            else
                dir = ((double) rand() / (RAND_MAX)) * PI/2 - PI/4;

            this->ball_vx = cosf(dir) * v_mag;
            this->ball_vy = sinf(dir) * v_mag;
        }
    }

    bool is_running = true;

    int window_width;
    int window_height;

    int last_frame;

    SDL_Window *game_window;
    SDL_Renderer *game_renderer;

//    TTF_Font *font;

    SDL_Rect player1;
    SDL_Rect player2;

    float player2_x;
    float player2_y;

    SDL_Rect ball;

    float ball_x;
    float ball_y;

    float ball_vx;
    float ball_vy;

    const int pong_width = 20;
    const int pong_height = 100;

    const float ball_speed = 0.75;
    const float ball_speedup = 0.0075;
};