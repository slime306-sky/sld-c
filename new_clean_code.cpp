#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <vector>

#define WIDTH 800
#define HEIGHT 800
#define WHITE_COLOR Color{255, 255, 255, 255}
#define BLACK_COLOR Color{0, 0, 0, 255}
#define GRAY_COLOR  Color{128, 128, 128, 255}
#define SQUARE_SIZE 100

struct Color {
	int r, g, b, a;
};

int board[8][8] = {
	{2, 3, 4, 1, 0, 4, 3, 2},
	{5, 5, 5, 5, 5, 5, 5, 5},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{11, 11, 11, 11, 11, 11, 11, 11},
	{8, 9, 10, 7, 6, 10, 9, 8}
};

using namespace std;

class gameUi {
public:
	struct Square_Pos {
		float x, y;
	};
	struct Square {
		int width, height;
	};
	struct Piece {
		char name;
		int id;
		int x, y;
		bool isWhite;
	};

public:
	void setColor(SDL_Renderer* renderer, Color color) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	}

	void drawSquare(SDL_Renderer* renderer, Square_Pos square_pos, Color color) {
		SDL_FRect rect = { square_pos.x, square_pos.y, SQUARE_SIZE, SQUARE_SIZE };
		setColor(renderer, color);
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

	void loadTextures(SDL_Renderer* renderer,SDL_Texture* textures[]) {
		string pieces[12] = { "WK.bmp", "WQ.bmp", "WR.bmp", "WN.bmp", "WB.bmp", "WP.bmp",
							  "BK.bmp", "BQ.bmp", "BR.bmp", "BN.bmp", "BB.bmp", "BP.bmp"
		};

		for (int i = 0; i < 12; i++) {
			string filePath = "assets/" + pieces[i];
			SDL_Surface* imageSurface = SDL_LoadBMP(filePath.c_str());
			if (!imageSurface) {
				printf("Failed to load %s: %s\n", pieces[i].c_str(), SDL_GetError());
				continue;
			}
			textures[i] = SDL_CreateTextureFromSurface(renderer, imageSurface);
			if(!textures[i]) cout<<"error";
		}
	}

	void renderPieces(SDL_Renderer* renderer,SDL_Texture* textures[]) {
		for (int rank = 0; rank < 8; rank++) {
			for (int file = 0; file < 8; file++) {
				int piece = board[rank][file];
				if (piece == -1) continue;
	
				SDL_FRect imageRect = { file * SQUARE_SIZE, (7 - rank) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE };
				//cout << textures[piece];
				SDL_RenderTexture(renderer, textures[piece], NULL, &imageRect);
			}
		}
	}
};


class gameLogic {

public:
	vector<string> moveHistory;

	bool isWhiteTurn = true;
	bool dragging = false;
	int selectedPiece = -1, selectedX = -1, selectedY = -1;
	float draggingX = 0, draggingY = 0;

	bool isPathClear(int fromX, int fromY, int toX, int toY) {
		int dx = (toX > fromX) ? 1 : (toX < fromX) ? -1 : 0;
		int dy = (toY > fromY) ? 1 : (toY < fromY) ? -1 : 0;
		int x = fromX + dx;
		int y = fromY + dy;
		while (x != toX || y != toY) {
			if (board[y][x] != -1) return false;
			x += dx;
			y += dy;
		}
		return true;
	}

	bool pawnMove(int piece, int fromX, int fromY, int toX, int toY, int board[8][8]) {
		int pieceValue = piece;
		bool isWhite = (6 > pieceValue);
		int direction = isWhite ? 1 : -1;


		// Move Forward (Single Step)
		if (fromX == toX && board[toY][toX] == -1) {
			if (toY - fromY == direction) {
				return true;
			}

			// Move Forward (Double Step - Only From Starting Row)
			if ((isWhite && fromY == 1) || (!isWhite && fromY == 6)) {
				if (toY - fromY == 2 * direction && board[fromY + direction][fromX] == -1 && board[toY][toX] == -1) {
					return true;
				}
			}
		}

		// Capture Diagonally
		if (abs(toX - fromX) == 1 && toY - fromY == direction) {
			if (board[toY][toX] != -1) {
				return true;
			}
		}

		// En Passant here


		return false;
	}

	bool isValidMove(int piece, int fromX, int fromY, int toX, int toY, int board[8][8]) {
		// Ensure target is within board limits
		if (toX < 0 || toX >= 8 || toY < 0 || toY >= 8) return false;
		switch (piece) {
		case 0: // King
		case 6: // black King
			return abs(toX - fromX) <= 1 && abs(toY - fromY) <= 1;
		case 1: // Queen
		case 7: // black Queen
			return (fromX == toX || fromY == toY || abs(fromX - toX) == abs(fromY - toY))
				&& isPathClear(fromX, fromY, toX, toY);
		case 2: // Rook
		case 8: // black Rook
			return (fromX == toX || fromY == toY)
				&& isPathClear(fromX, fromY, toX, toY);
		case 3: // Knight
		case 9: // black Knight
			return (abs(toX - fromX) == 2 && abs(toY - fromY) == 1) ||
				(abs(toX - fromX) == 1 && abs(toY - fromY) == 2);
		case 4: // Bishop
		case 10: //black Bishop
			return abs(fromX - toX) == abs(fromY - toY)
				&& isPathClear(fromX, fromY, toX, toY);

		case 5:  // Pawn
		case 11: {// black pawn
			return pawnMove(piece, fromX, fromY, toX, toY, board);
		}
		default:
			return false;
		}
	}

	string getSquareNotation(int file, int rank) {
		return string(1, 'a' + file) + to_string(rank + 1);
	}


	char getPieceNotation(int piece) {
		if (piece == -1) return '\0'; // No piece
		const string pieceLetters = "KQRNBP";
		return (piece % 6 == 5) ? '\0' : pieceLetters[piece % 6]; // Pawns don't get a letter
	}

	void recordMove(int fromFile, int fromRank, int toFile, int toRank) {
		if (fromFile == toFile && fromRank == toRank) return; // Ignore invalid moves

		string from = getSquareNotation(fromFile, fromRank);
		string to = getSquareNotation(toFile, toRank);
		bool isCapture = board[toRank][toFile] != -1;

		char pieceLetter = getPieceNotation(selectedPiece);  // Get K, Q, R, N, B (empty for pawns)

		bool needFile = false, needRank = false;
		for (int rank = 0; rank < 8; rank++) {
			for (int file = 0; file < 8; file++) {
				if (board[rank][file] == selectedPiece && !(rank == fromRank && file == fromFile)) {
					if (toFile == file) needFile = true;
					if (toRank == rank) needRank = true;
				}
			}
		}

		string moveNotation = "";

		// Add piece letter (not for pawns)
		if (pieceLetter != '\0') moveNotation += pieceLetter;

		// Disambiguation: Add file or rank if needed
		if (needFile) moveNotation += from[0];  // File letter (a-h)
		if (needRank) moveNotation += from[1];  // Rank number (1-8)

		// Capture notation
		if (isCapture) moveNotation += "x";

		// Destination square
		moveNotation += to;

		// Store and print move
		moveHistory.push_back(moveNotation);
		cout << (moveHistory.size() / 2 + 1) << (isWhiteTurn ? ". " : "... ") << moveNotation << endl;

	}

};

class controller {
public:

	struct Piece { char name; int id; int x, y; bool isWhite; };

	gameLogic gameLogic;
	vector<Piece> pieceInfo;
	Piece tempPiece;

	void initializePieces() {
		for (int file = 0; file < 8; file++) {
			for (int rank = 0; rank < 8; rank++) {
				int pieceID = board[file][rank];
				if (pieceID != -1) {
					Piece p = { 0, 0, 0, 0, true };;
					p.id = pieceID;
					p.x = file;
					p.y = rank;
					p.isWhite = pieceID < 6; // White pieces are represented by IDs 0-5, black pieces by 6-11

					p.name = (gameLogic.getPieceNotation(pieceID) == '\0') ? 'P' : gameLogic.getPieceNotation(pieceID);
					pieceInfo.push_back(p);
				}
			}
		}
	}

	void renderPieces(SDL_Renderer* renderer, SDL_Texture* textures[]) {
		for (Piece& p : pieceInfo) {
			SDL_FRect imageRect = {
				p.y * SQUARE_SIZE,
				(7 - p.x) * SQUARE_SIZE,
				SQUARE_SIZE, SQUARE_SIZE
			};
			//cout << textures[p.id];
			SDL_RenderTexture(renderer, textures[p.id], NULL, &imageRect);
		}
	}

	void updateLocation(int selectedX, int selectedY, int file, int rank) {
		cout << selectedX << file << selectedY << rank;
		for (Piece& p : pieceInfo) {
			if (p.x == selectedX && p.y == selectedY) {  // Fixed condition
				p.x = file;
				p.y = rank;
				return;  // Stop after finding the piece
			}
		}
	}

	void removePiece(int file, int rank) {
		for (auto it = pieceInfo.begin(); it != pieceInfo.end(); ) {
			if (it->x == rank && it->y == file) {
				tempPiece = *it;
				it = pieceInfo.erase(it);  // Erase and get the next valid iterator
			}
			else {
				++it;  // Move to the next element only if not erased
			}
		}
	}

	void resetMove() {
		pieceInfo.push_back(tempPiece);
	}

	void printMove() {
		cout << "-----------------------------------------------------------\n";
		for (Piece& p : pieceInfo) {
			cout << "Piece: " << p.name
				<< " | ID: " << p.id
				<< " | Position: ( x :" << p.x << ", y : " << p.y << ")"
				<< " | Color: " << (p.isWhite ? "White" : "Black")
				<< endl;
		}
	}

};


class handelAll {
	gameLogic gameLogic;
	gameUi gameUi;

public:
	void handleMouseClick(int mouseX, int mouseY) {
		int file = mouseX / SQUARE_SIZE;
		int rank = 7 - (mouseY / SQUARE_SIZE);
		int piece = board[rank][file];

		if (piece == -1 || (gameLogic.isWhiteTurn != (piece < 6))) return;  // Prevent selecting the wrong turn's piece
		gameLogic.selectedPiece = piece;
		gameLogic.selectedX = file;
		gameLogic.selectedY = rank;
		gameLogic.draggingX = mouseX - (SQUARE_SIZE / 2);
		gameLogic.draggingY = mouseY - (SQUARE_SIZE / 2);
		gameLogic.dragging = true;
		board[rank][file] = -1;
	}

	void handleMouseMove(int mouseX, int mouseY) {
		if (gameLogic.dragging) {
			gameLogic.draggingX = mouseX - (SQUARE_SIZE / 2);
			gameLogic.draggingY = mouseY - (SQUARE_SIZE / 2);
		}
	}

	void draggingWhen(SDL_Renderer* renderer,SDL_Texture* textures[]) {
		int file = gameLogic.draggingX / SQUARE_SIZE;
		int rank = 7 - (gameLogic.draggingY / SQUARE_SIZE);
		if (gameLogic.dragging && gameLogic.selectedPiece != -1) {
			SDL_FRect imageRect = { gameLogic.draggingX, gameLogic.draggingY, SQUARE_SIZE, SQUARE_SIZE };
			SDL_RenderTexture(renderer, textures[gameLogic.selectedPiece], NULL, &imageRect);
			cout << file+1 <<" "<< rank+1 << endl;
		}
	}

	void handleMouseRelease(int mouseX, int mouseY) {
		int file = mouseX / SQUARE_SIZE;
		int rank = 7 - (mouseY / SQUARE_SIZE);

		if (gameLogic.dragging) {
			// If the selected square is the same as the release position, just return the piece to its original position
			if (gameLogic.selectedX == file && gameLogic.selectedY == rank) {
				board[gameLogic.selectedY][gameLogic.selectedX] = gameLogic.selectedPiece;
			}
			else {
				int targetPiece = board[rank][file];

				// Friendly fire check
				if (targetPiece != -1 && (gameLogic.selectedPiece < 6) == (targetPiece < 6)) {
					// Piece is not moved
					board[gameLogic.selectedY][gameLogic.selectedX] = gameLogic.selectedPiece;
				}
				// Valid move check
				else if (gameLogic.isValidMove(gameLogic.selectedPiece, gameLogic.selectedX, gameLogic.selectedY, file, rank, board)) {
					// Record the move and update board
					gameLogic.recordMove(gameLogic.selectedX, gameLogic.selectedY, file, rank);
					board[rank][file] = gameLogic.selectedPiece;
					board[gameLogic.selectedY][gameLogic.selectedX] = -1;
					gameLogic.isWhiteTurn = !gameLogic.isWhiteTurn;
				}
				else {
					// Invalid move, return piece to original position
					board[gameLogic.selectedY][gameLogic.selectedX] = gameLogic.selectedPiece;
				}
			}

			// Reset dragging state and selected piece
			gameLogic.dragging = false;
			gameLogic.selectedPiece = -1;
		}
	}

};

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("SDL3 Chessboard", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	SDL_Texture* textures[12] = {nullptr};

	handelAll hendeler;
	gameUi gameUi;
	gameLogic gameLogic;
	controller controller;


	gameUi.loadTextures(renderer,textures);

	for (int i = 0; i < 12; i++)
	{
		if (textures[i] == nullptr) {
			cerr << "Texture 0 is NULL!" << endl;
		}
	}

	controller.initializePieces();
	controller.printMove();

	bool running = true;
	SDL_Event event;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
			else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				hendeler.handleMouseClick(event.button.x, event.button.y);
			}
			else if (event.type == SDL_EVENT_MOUSE_MOTION) {
				hendeler.handleMouseMove(event.motion.x, event.motion.y);
			}
			else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				hendeler.handleMouseRelease(event.button.x, event.button.y);
			}
		}

		gameUi.setColor(renderer, BLACK_COLOR);
		SDL_RenderClear(renderer);
		gameUi.drawBoard(renderer);
		controller.renderPieces(renderer,textures);
		hendeler.draggingWhen(renderer,textures);

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
