#include <SDL3/SDL.h>
#include <iostream> // For debugging in C++

#define HEIGHT 600
#define WIDTH 900

struct Color {
    int r, g, b, a;
};

struct Square_Pos {
    float x, y;
};

struct Square {
    int width, height;
};

#define WHITE_COLOR Color{255, 255, 255, 255}
#define BLACK_COLOR Color{0, 0, 0, 255}
#define GRAY_COLOR  Color{128, 128, 128, 255}

#define SQUARE Square{75, 75}  // Smaller size to fit in window

void setRenderColor(SDL_Renderer* renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void drawSquare(SDL_Renderer* renderer, Square_Pos square_pos, Color color) {
    Square square = SQUARE;

    SDL_FRect rect = { square_pos.x, square_pos.y, square.width, square.height };

    setRenderColor(renderer, color);
    SDL_RenderFillRect(renderer, &rect);
}

void drawBoard(SDL_Renderer* renderer) {
    Square square = SQUARE;

    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            bool isLightSquare = (file + rank) % 2 != 0;
            Color squareColor = isLightSquare ? WHITE_COLOR : GRAY_COLOR;

            Square_Pos square_pos = { file * square.width, rank * square.height };

            drawSquare(renderer, square_pos, squareColor);
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL3 Chessboard", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        setRenderColor(renderer, BLACK_COLOR);
        SDL_RenderClear(renderer);

        drawBoard(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
