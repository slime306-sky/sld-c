#include <SDL3/SDL.h>
#include <iostream>

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;

    SDL_Window* window = SDL_CreateWindow("SDL3 Rectangle", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    SDL_FRect rect = { 100, 100, 200, 150 }; // x, y, width, height
    bool running = true;
    bool dragging = false;
    SDL_Event event;
    float offsetX = 0, offsetY = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    float mx = event.button.x;
                    float my = event.button.y;
                    if (mx >= rect.x && mx <= rect.x + rect.w &&
                        my >= rect.y && my <= rect.y + rect.h) {
                        dragging = true;
                        offsetX = mx - rect.x;
                        offsetY = my - rect.y;
                    }
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = false;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION && dragging) {
                // Calculate new position
                float newX = event.motion.x - offsetX;
                float newY = event.motion.y - offsetY;

                // Clamp within window bounds
                if (newX < 0) newX = 0;
                if (newY < 0) newY = 0;
                if (newX > WINDOW_WIDTH - rect.w) newX = WINDOW_WIDTH - rect.w;
                if (newY > WINDOW_HEIGHT - rect.h) newY = WINDOW_HEIGHT - rect.h;

                // Update rect position
                rect.x = newX;
                rect.y = newY;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
