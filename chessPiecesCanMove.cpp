#include <SDL3/SDL.h>
#include <iostream>
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
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bool isLightSquare = (file + rank) % 2 != 0;
            Color squareColor = isLightSquare ? WHITE_COLOR : GRAY_COLOR;
            Square_Pos square_pos = { file * square.width, (7 - rank) * square.height };
            drawSquare(renderer, square_pos, squareColor);
        }
    }
}

// Chessboard with pieces
// white : king 0 , queen 1 ,rook 2,knight 3,bisiop 4,pawn 5
// black : king 6 , queen 7 ,rook 8,knight 9,bisiop 10,pawn 11

int board[8][8] = {
    {8, 9, 10, 7, 6, 10, 9, 8},
    {11, 11, 11, 11, 11, 11, 11, 11},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {5, 5, 5, 5, 5, 5, 5, 5},
    {2, 3, 4, 1, 0, 4, 3, 2}
};

// Load textures
void loadTextures(SDL_Renderer* renderer, SDL_Texture* textures[12]) {
    string pieces[12] = { "WK.bmp","WQ.bmp","WR.bmp","WN.bmp","WB.bmp","WP.bmp",
                          "BK.bmp","BQ.bmp","BR.bmp","BN.bmp","BB.bmp","BP.bmp" };

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
}

// Render pieces correctly
void renderPieces(SDL_Renderer* renderer, SDL_Texture* textures[12]) {
    Square pieceSize = { 100, 100 };
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int piece = board[rank][file];
            if (piece == -1) continue;

            Square_Pos piece_pos = { file * pieceSize.width, (7 - rank) * pieceSize.height };
            SDL_FRect imageRect = { piece_pos.x, piece_pos.y, (float)pieceSize.width, (float)pieceSize.height };
            SDL_RenderTexture(renderer, textures[piece], NULL, &imageRect);
        }
    }
}

// Track movement
bool dragging = false;
int selectedPiece = -1;
int selectedX = -1, selectedY = -1;

// Check if two pieces belong to the same player
bool isSameColor(int piece1, int piece2) {
    if (piece1 == -1 || piece2 == -1) return false;
    return (piece1 <= 5 && piece2 <= 5) || (piece1 >= 6 && piece2 >= 6);
}

// Handle mouse click events
void handleMouseClick(int mouseX, int mouseY) {
    int file = mouseX / 100; //  mouseX = 250 then 250 / 100 = 2
    int rank = 7 - (mouseY / 100); // mouseY = 250 then 250 / 100 = 2.

    if (board[rank][file] != -1) {
        selectedPiece = board[rank][file];
        selectedX = file;
        selectedY = rank;
        dragging = true;
    }
}

// Handle mouse release for movement & capture
void handleMouseRelease(int mouseX, int mouseY) {
    int file = mouseX / 100; //  mouseX = 250 then 250 / 100 = 2
    int rank = 7 - (mouseY / 100); // mouseY = 250 then 250 / 100 = 2.

    if (dragging) {
        if (selectedX != file || selectedY != rank) {
            if (!isSameColor(selectedPiece, board[rank][file])) { // Prevent capturing own piece
                board[selectedY][selectedX] = -1;  // Remove from old position
                board[rank][file] = selectedPiece; // Place in new position (capture if needed)
            }
        }
        dragging = false;
        selectedPiece = -1;
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL3 Chessboard", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    SDL_Texture* textures[12] = { nullptr };
    loadTextures(renderer, textures);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                handleMouseClick(event.button.x, event.button.y);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                handleMouseRelease(event.button.x, event.button.y);
            }
        }

        setRenderColor(renderer, BLACK_COLOR);
        SDL_RenderClear(renderer);

        drawBoard(renderer);
        renderPieces(renderer, textures);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    for (int i = 0; i < 12; i++) {
        if (textures[i]) {
            SDL_DestroyTexture(textures[i]);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
