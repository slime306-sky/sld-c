#include <SDL3/SDL.h>
#include <iostream> // For debugging in C++
#include <string>

using namespace std;

#define HEIGHT 800
#define WIDTH 800

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
#define SQUARE Square{100, 100}  





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

void imageLoader(SDL_Renderer* renderer) {
    int board[8][8] = {
            {8, 9, 10, 7, 6, 10, 9, 8},  // Black pieces
            {11, 11, 11, 11, 11, 11, 11, 11},  // Black pawns
            {-1, -1, -1, -1, -1, -1, -1, -1},  // Empty rows
            {-1, -1, -1, -1, -1, -1, -1, -1},
            {-1, -1, -1, -1, -1, -1, -1, -1},
            {-1, -1, -1, -1, -1, -1, -1, -1},
            {5, 5, 5, 5, 5, 5, 5, 5},  // White pawns
            {2, 3, 4, 1, 0, 4, 3, 2}   // White pieces
    };


    string pieces[12] = { "WK.bmp","WQ.bmp","WR.bmp","WN.bmp","WB.bmp","WP.bmp",
                               "BK.bmp","BQ.bmp","BR.bmp","BN.bmp","BB.bmp","BP.bmp"
    };

    Square pieceSize = { 100, 100 };

    SDL_Texture* textures[12] = { nullptr };

    // Load piece textures
    for (int i = 0; i < 12; i++) {
        string filePath = "assets/" + pieces[i];

        SDL_Surface* imageSurface = SDL_LoadBMP(filePath.c_str());
        if (!imageSurface) {
            printf("Failed to load %s: %s\n", pieces[i].c_str(), SDL_GetError());
            continue;
        }

        textures[i] = SDL_CreateTextureFromSurface(renderer, imageSurface);
        SDL_DestroySurface(imageSurface);

        if (!textures[i]) {
            printf("Failed to create texture for %s: %s\n", pieces[i].c_str(), SDL_GetError());
        }
    }

    // Rendering the pieces
    Square_Pos piece_pos = { 0, 0 };  // Reset piece position



    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            int piece = board[file][rank];
            
            if (piece == -1)
            {
                piece_pos.x = file * pieceSize.width;
                piece_pos.y = rank * pieceSize.height;
            }
            else {

                piece_pos.x = file * pieceSize.width;
                piece_pos.y = rank * pieceSize.height;
                SDL_FRect imageRect = { (float)piece_pos.y, (float)piece_pos.x, (float)pieceSize.width, (float)pieceSize.height };
                SDL_RenderTexture(renderer, textures[piece], NULL, &imageRect);
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        if (textures[i]) {
            SDL_DestroyTexture(textures[i]);
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
        imageLoader(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
