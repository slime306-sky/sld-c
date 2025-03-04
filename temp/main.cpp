#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "slime.h"

using namespace std;

#define HIEGHT 600
#define WIDTH 900
#define A_GRAVITY 0.2
#define DAMPPING 0.9
#define DELTA_TIME 0.03

struct Circle
{
    double x;
    double y;
    double radius;
    double v_x;
    double v_y;
};

void DrawCircle(SDL_Renderer* renderer, struct Circle circle) {
    double low_x = circle.x - circle.radius;
    double low_y = circle.y - circle.radius;
    double high_x = circle.x + circle.radius;
    double high_y = circle.y + circle.radius;

    double radius_squared = circle.radius * circle.radius;

    for (double x = low_x; x < high_x; x++) {

        for (double y = low_y; y < high_y; y++) {
            double center_distance_squared = (x - circle.x) * (x - circle.x) + (y - circle.y) * (y - circle.y);

            if (center_distance_squared < radius_squared) {
                SetRenderDrawColor(renderer, WHITE);
                SDL_FRect rect = { x, y, 1, 1 };
                SDL_RenderFillRect(renderer, &rect);
            }

        }

    }

}


void step(struct Circle* circle) {
    circle->x += circle->v_x * DELTA_TIME;
    circle->y += circle->v_y * DELTA_TIME;
    //circle->v_y += A_GRAVITY * DAMPPING;

    //right
    if (circle->x + circle->radius > WIDTH) {
        circle->x = WIDTH - circle->radius;
        circle->v_x = -circle->v_x * DAMPPING;
    }
    // left
    if (circle->x - circle->radius < 0) {
        circle->x = circle->radius;
        circle->v_x = -circle->v_x * DAMPPING;
    }
    //bottom
    if (circle->y + circle->radius > HIEGHT) {
        circle->y = HIEGHT - circle->radius;
        circle->v_y = -circle->v_y * DAMPPING;
    }
    //top
    if (circle->y - circle->radius < 0) {
        circle->y = circle->radius;
        circle->v_y = -circle->v_y * DAMPPING;
    }

}

void renderText(SDL_Renderer* renderer, TTF_Font* font,string text, Circle circle,int x ,int y) {
    SDL_Color white = { 255,255,255,255 };

    string text1 = text + to_string(circle.radius);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text1.c_str(), text1.length(), white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_FRect textRect = { x,y,textSurface->w,textSurface->h };
    SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    TTF_Font* font = TTF_OpenFont("arial.ttf",24);

    SDL_Window* window = SDL_CreateWindow("SDL3 Rectangle", WIDTH, HIEGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    SDL_FRect rect = { 400, 100, 200, 150 }; // x, y, width, height
    int running = 1;
    SDL_Event event;

    Circle circle = { 200,200,50,-50,-30 };
    Circle circle2 = { 200,200,50,-30,-40 };

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_W || event.key.scancode == SDL_SCANCODE_UP) {
                    circle.radius += 1;
                }
                if (event.key.scancode == SDL_SCANCODE_E || event.key.scancode == SDL_SCANCODE_UP) {
                    circle2.radius += 1;
                }
                if (event.key.scancode == SDL_SCANCODE_S || event.key.scancode == SDL_SCANCODE_DOWN) {
                    circle.radius -= 5;
                }
                if (event.key.scancode == SDL_SCANCODE_Q || event.key.scancode == SDL_SCANCODE_DOWN) {
                    circle2.radius -= 5;
                }
                if (event.key.scancode == SDL_SCANCODE_A || event.key.scancode == SDL_SCANCODE_LEFT) {
                    circle.v_x += 5;
                }
                if (event.key.scancode == SDL_SCANCODE_D || event.key.scancode == SDL_SCANCODE_RIGHT) {
                    circle.v_x -= 5;
                }
            }
        }

        SetRenderDrawColor(renderer,BLACK);
        SDL_RenderClear(renderer);

        renderText(renderer,font,"circle 1 radius : ",circle,10,10);
        renderText(renderer, font, "circle 2 radius : ", circle2,10,40);

        DrawCircle(renderer, circle);
        step(&circle);
        DrawCircle(renderer, circle2);
        step(&circle2);
        //SDL_Delay(20);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
