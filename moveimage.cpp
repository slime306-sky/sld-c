#include <SDL3/SDL.h>
#include <iostream>

int main(){
    SDL_Init(SDL_INIT_VIDEO);

    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;

    SDL_Window* window = SDL_CreateWindow("SDL3 Image Move", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // Load image as surface
    SDL_Surface* imageSurface = SDL_LoadBMP("shikanoko.bmp"); // Make sure this file exists!

    // Convert surface to texture
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_DestroySurface(imageSurface); // No longer needed after creating texture

    SDL_FRect imageRect = { 100, 100, 100, 100 }; // x, y, width, height

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
                    if (mx >= imageRect.x && mx <= imageRect.x + imageRect.w &&
                        my >= imageRect.y && my <= imageRect.y + imageRect.h) {
                        dragging = true;
                        offsetX = mx - imageRect.x;
                        offsetY = my - imageRect.y;
                    }
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = false;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION && dragging) {
                float newX = event.motion.x - offsetX;
                float newY = event.motion.y - offsetY;

                // Restrict movement within window bounds
                if (newX < 0) newX = 0;
                if (newY < 0) newY = 0;
                if (newX > WINDOW_WIDTH - imageRect.w) newX = WINDOW_WIDTH - imageRect.w;
                if (newY > WINDOW_HEIGHT - imageRect.h) newY = WINDOW_HEIGHT - imageRect.h;

                imageRect.x = newX;
                imageRect.y = newY;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the image texture
        SDL_RenderTexture(renderer, imageTexture, NULL, &imageRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
