#include <iostream>
#include <cmath>
#include <SDL.h>
#include <SDL_ttf.h>

#define PI 3.14159

class Game {
public:
    void init(const char *window_title, int window_width_in, int window_height_in) { // No error handling. I DON'T care!
        this->window_width = window_width_in;
        this->window_height = window_height_in;

        this->game_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width_in, window_height_in, SDL_WINDOW_SHOWN);
        this->game_renderer = SDL_CreateRenderer(this->game_window, -1, SDL_RENDERER_ACCELERATED);

        SDL_SetRenderDrawBlendMode(this->game_renderer, SDL_BLENDMODE_BLEND);

        this->init_player(this->player1, 250);
        this->init_player(this->player2, window_width - 250);

        this->init_ball();

        this->player2_x = window_width - 250;
        this->player2_y = this->ball_y + this->ball.h / 2 - this->player2.h / 2;

        this->last_frame = SDL_GetTicks();

        TTF_Init();

        this->font = TTF_OpenFont("./Asap.ttf", 250);
    }

    bool running() { return is_running; }

    void handle_events() {
        SDL_Event e;

        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
                is_running = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN)
                this->is_game_started = true;
    }

    void update() {
//        SDL_Delay(2);

        int _, mouseY;

        SDL_GetMouseState(&_, &mouseY);

        int current_frame = SDL_GetTicks();
        float dt = (current_frame - this->last_frame);
        this->last_frame = current_frame;


        this->player1.y = mouseY - this->player1.h / 2;

        if (this->player1.y < 0) // clamp
            this->player1.y = 0;
        else if (this->player1.y + this->player1.h > this->window_height)
            this->player1.y = this->window_height - this->player1.h;

        if (this->is_game_started) {
            this->ball_x += this->ball_vx * dt;
            this->ball_y += this->ball_vy * dt;
        }

        this->ball.x = (int)this->ball_x;
        this->ball.y = (int)this->ball_y;

        this->ball_update(dt);

        if (this->is_game_started) {
            const int dir = ((this->player2_y + this->player2.h / 2 > this->ball_y + this->ball.h / 2) * 2 - 1);

            this->player2_y -= this->ai_paddle_speed * dir * dt;

//            this->player2_y = this->ball_y + this->ball.h / 2 - this->player2.h / 2;
        }


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

        SDL_SetRenderDrawColor(this->game_renderer, 0xFF, 0xFF, 0xFF, 0x55);

        int net_spacing = 25;
        int net_width = 10;

        for (int i = 0; i < this->window_height / (net_spacing * 2) + 1; i++) {
            SDL_Rect net{this->window_width / 2 - net_width / 2, i * net_spacing * 2, net_width, net_spacing};
            SDL_RenderFillRect(this->game_renderer, &net);
        }

        SDL_SetRenderDrawColor(this->game_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderFillRect(this->game_renderer, &this->player1);
        SDL_RenderFillRect(this->game_renderer, &this->player2);
        SDL_RenderFillRect(this->game_renderer, &this->ball);

        SDL_Color text_color = {255, 255, 255, 155 };

        SDL_Surface *p1_surf = TTF_RenderText_Blended(this->font, std::to_string(this->player1_score).c_str(), text_color);
        SDL_Texture *p1_tex = SDL_CreateTextureFromSurface(this->game_renderer, p1_surf);

        SDL_Rect p1_rect{ this->window_width / 4 - 50, 50, 85, 150 };

        SDL_Surface *p2_surf = TTF_RenderText_Blended(this->font, std::to_string(this->player2_score).c_str(), text_color);
        SDL_Texture *p2_tex = SDL_CreateTextureFromSurface(this->game_renderer, p2_surf);

        SDL_Rect p2_rect{ (this->window_width - this->window_width / 4) - 50, 50, 85, 150 };


        SDL_RenderCopy(this->game_renderer, p1_tex, nullptr, &p1_rect);
        SDL_RenderCopy(this->game_renderer, p2_tex, nullptr, &p2_rect);

        SDL_FreeSurface(p1_surf);
        SDL_FreeSurface(p2_surf);

        SDL_DestroyTexture(p1_tex);
        SDL_DestroyTexture(p2_tex);

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

        this->ball_x = (this->window_width / 2) - this->ball.w / 2;
        this->ball_y = (this->window_height / 2) - this->ball.h / 2;

        this->ball_vx = ball_speed + ((double) rand() / (RAND_MAX)) * 0.4 - 0.2;
        this->ball_vy = ball_speed + ((double) rand() / (RAND_MAX)) * 0.4 - 0.2;
    }

    void ball_update(float dt) {
        if (this->ball_x < 0) {
            init_ball();

            this->is_game_started = false;

            this->player2_score++;
        } else if (this->ball_x + this->ball.w > this->window_width) {
            init_ball();

            this->is_game_started = false;

            this->player1_score++;
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

    bool is_game_started = false;

    TTF_Font *font;

    SDL_Rect player1;
    SDL_Rect player2;

    float player2_x;
    float player2_y;

    int player1_score = 0;
    int player2_score = 0;

    SDL_Rect ball;

    float ball_x;
    float ball_y;

    float ball_vx;
    float ball_vy;

    const int pong_width = 20;
    const int pong_height = 100;

    const float ball_speed = 0.7;
    const float ball_speedup = 0.01;

    const float ai_paddle_speed = 0.75;
};