#include <SDL3/SDL.h>
#include <iostream>
#include <string>

using namespace std;

#define HEIGHT 800
#define WIDTH 800

struct Color { int r, g, b, a; };
struct Square_Pos { float x, y; };
struct Square { int width, height; };

#define WHITE_COLOR Color{255, 255, 255, 255}
#define BLACK_COLOR Color{0, 0, 0, 255}
#define GRAY_COLOR  Color{128, 128, 128, 255}
#define SQUARE_SIZE 100

void setRenderColor(SDL_Renderer* renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void drawSquare(SDL_Renderer* renderer, Square_Pos square_pos, Color color) {
    SDL_FRect rect = { square_pos.x, square_pos.y, SQUARE_SIZE, SQUARE_SIZE };
    setRenderColor(renderer, color);
    SDL_RenderFillRect(renderer, &rect);
}

void drawBoard(SDL_Renderer* renderer) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bool isLightSquare = (file + rank) % 2 != 0;
            Color squareColor = isLightSquare ? WHITE_COLOR : GRAY_COLOR;
            Square_Pos square_pos = { file * SQUARE_SIZE, (7 - rank) * SQUARE_SIZE };
            drawSquare(renderer, square_pos, squareColor);
        }
    }
}

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

SDL_Texture* textures[12] = { nullptr };

void loadTextures(SDL_Renderer* renderer) {
    string pieces[12] = { "WK.bmp", "WQ.bmp", "WR.bmp", "WN.bmp", "WB.bmp", "WP.bmp",
                          "BK.bmp", "BQ.bmp", "BR.bmp", "BN.bmp", "BB.bmp", "BP.bmp" };

    for (int i = 0; i < 12; i++) {
        string filePath = "assets/" + pieces[i];
        SDL_Surface* imageSurface = SDL_LoadBMP(filePath.c_str());
        if (!imageSurface) {
            printf("Failed to load %s: %s\n", pieces[i].c_str(), SDL_GetError());
            continue;
        }
        textures[i] = SDL_CreateTextureFromSurface(renderer, imageSurface);
        SDL_DestroySurface(imageSurface);
    }
}

void renderPieces(SDL_Renderer* renderer) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int piece = board[rank][file];
            if (piece == -1) continue;

            SDL_FRect imageRect = { file * SQUARE_SIZE, (7 - rank) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE };
            SDL_RenderTexture(renderer, textures[piece], NULL, &imageRect);
        }
    }
}

bool dragging = false;
int selectedPiece = -1, selectedX = -1, selectedY = -1;
float draggingX = 0, draggingY = 0;

void handleMouseClick(int mouseX, int mouseY) {
    int file = mouseX / SQUARE_SIZE;
    int rank = 7 - (mouseY / SQUARE_SIZE);

    if (board[rank][file] != -1) {
        selectedPiece = board[rank][file];
        selectedX = file;
        selectedY = rank;
        draggingX = mouseX - (SQUARE_SIZE / 2);
        draggingY = mouseY - (SQUARE_SIZE / 2);
        dragging = true;

        board[rank][file] = -1;
    }
}


void handleMouseMove(int mouseX, int mouseY) {
    if (dragging) {
        draggingX = mouseX - (SQUARE_SIZE / 2);
        draggingY = mouseY - (SQUARE_SIZE / 2);
    }
}

void handleMouseRelease(int mouseX, int mouseY) {
    int file = mouseX / SQUARE_SIZE;
    int rank = 7 - (mouseY / SQUARE_SIZE);

    if (dragging) {
        // Check if the target square has a piece of the same color
        if (board[rank][file] == -1 || (selectedPiece < 6) != (board[rank][file] < 6)) {
            board[rank][file] = selectedPiece; // Move the piece
        } else {
            board[selectedY][selectedX] = selectedPiece; // Restore if move is invalid
        }

        dragging = false;
        selectedPiece = -1;
    }
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL3 Chessboard", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    loadTextures(renderer);
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
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                handleMouseMove(event.motion.x, event.motion.y);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                handleMouseRelease(event.button.x, event.button.y);
            }
        }

        setRenderColor(renderer, BLACK_COLOR);
        SDL_RenderClear(renderer);
        drawBoard(renderer);
        renderPieces(renderer);

        if (dragging && selectedPiece != -1) {
            SDL_FRect imageRect = { draggingX, draggingY, SQUARE_SIZE, SQUARE_SIZE };
            SDL_RenderTexture(renderer, textures[selectedPiece], NULL, &imageRect);
        }

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 12; i++) {
        if (textures[i]) SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
