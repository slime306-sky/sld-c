#include <SDL3/SDL.h>

#define HIEGHT 600
#define WIDTH 900
#define WHITE_COLOR 255,255,255,255
#define A_GRAVITY 0.2

struct Circle 
{
    double x;
    double y;
    double radius;
    double v_x;
    double v_y;
};

void DrawCircle(SDL_Renderer* renderer,struct Circle circle) {
    double low_x = circle.x - circle.radius;
    double low_y = circle.y - circle.radius;
    double high_x = circle.x + circle.radius;
    double high_y = circle.y + circle.radius;

    double radius_squared = circle.radius * circle.radius;

    for (double x = low_x; x < high_x; x++) {
        
        for (double y = low_y; y < high_y; y++) {
            double center_distance_squared = (x - circle.x) * (x - circle.x) + (y - circle.y) * (y - circle.y);

            if (center_distance_squared < radius_squared) {
                SDL_SetRenderDrawColor(renderer, WHITE_COLOR);
                SDL_FRect rect = { x, y, 1, 1 }; 
                SDL_RenderFillRect(renderer, &rect);
            }

        }

    }

}


void step(struct Circle* circle) {
    circle->x += circle->v_x;
    circle->y += circle->v_y;
    circle->v_y += A_GRAVITY;

    //right
    if (circle->x + circle->radius > WIDTH) {
        circle->x = WIDTH - circle->radius;
        circle->v_x = -circle->v_x;
    }
    // left
    if (circle->x - circle->radius < 0) { 
        circle->x = circle->radius;
        circle->v_x = -circle->v_x;
    }
    //bottom
    if (circle->y + circle->radius > HIEGHT) {
        circle->y = HIEGHT - circle->radius;
        circle->v_y = -circle->v_y;
    }
    //top
    if (circle->y - circle->radius < 0) {
        circle->y = circle->radius;
        circle->v_y = -circle->v_y;
    }

}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("SDL3 Rectangle", WIDTH, HIEGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, 0);

    SDL_FRect rect = { 400, 100, 200, 150 }; // x, y, width, height

    int running = 1;
    SDL_Event event;

    struct Circle circle = (struct Circle){ 200,200,50,-5,-10 };

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        DrawCircle(renderer, circle);
        step(&circle);
        SDL_Delay(20);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
