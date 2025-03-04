#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "slime.h"
#include "math.h"
#include <vector>
#include <random>

using namespace std;

#define HIEGHT 800
#define WIDTH 900
#define GRAVITY 0.1
#define DAMPPING 0.9
#define DELTA_TIME 0.03
#define PI 3.14159


struct Circle
{
	double x;
	double y;
	double radius;
	double v_x;
	double v_y;
	double mass;
	double density;
};

struct Physics
{
	double distance, overlap, normalized_dx, normalized_dy, accelaration_gravity, ax, ay, relative_vx, relative_vy, velocity;
};

void DrawCircle(SDL_Renderer* renderer, vector<Circle> circles) {

	for (Circle circle : circles)
	{
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

}

void collision_detection_PushingCirclesApart(Circle* circle, Circle* circle2, Physics& stuf) {

	if (stuf.distance < (circle->radius + circle2->radius)) {

		stuf.overlap = (circle->radius + circle2->radius) - stuf.distance;

		stuf.relative_vx = circle2->v_x - circle->v_x;
		stuf.relative_vy = circle2->v_y - circle->v_y;

		stuf.velocity = (stuf.relative_vx * stuf.normalized_dx) + (stuf.relative_vy * stuf.normalized_dy);

		if (stuf.velocity != 0) {
			circle->x -= (stuf.overlap / 2) * stuf.normalized_dx;
			circle->y -= (stuf.overlap / 2) * stuf.normalized_dy;
			circle2->x += (stuf.overlap / 2) * stuf.normalized_dx;
			circle2->y += (stuf.overlap / 2) * stuf.normalized_dy;
		}
	}

}

void collision_detection_BouncingOffEachOther(Circle* circle, Circle* circle2, Physics& stuf) {
	if (stuf.distance < (circle->radius + circle2->radius)) {
		stuf.overlap = (circle->radius + circle2->radius) - stuf.distance;

		stuf.relative_vx = circle2->v_x - circle->v_x;
		stuf.relative_vy = circle2->v_y - circle->v_y;

		circle->x -= (stuf.overlap / 2) * stuf.normalized_dx;
		circle->y -= (stuf.overlap / 2) * stuf.normalized_dy;
		circle2->x += (stuf.overlap / 2) * stuf.normalized_dx;
		circle2->y += (stuf.overlap / 2) * stuf.normalized_dy;

		stuf.velocity = (stuf.relative_vx * stuf.normalized_dx) + (stuf.relative_vy * stuf.normalized_dy);

		if (stuf.velocity != 0) {
			circle->v_x += stuf.velocity * stuf.normalized_dx;
			circle->v_y += stuf.velocity * stuf.normalized_dy;
			circle2->v_x -= stuf.velocity * stuf.normalized_dx;
			circle2->v_y -= stuf.velocity * stuf.normalized_dy;
		}
	}
}

void step(vector<Circle>& circles, Physics& stuf) {
	int circles_count = circles.size();

	for (int i = 0; i < circles_count; i++) {
		circles[i].x += circles[i].v_x * DELTA_TIME;
		circles[i].y += circles[i].v_y * DELTA_TIME;
		circles[i].v_y += GRAVITY;

		for (int j = 0; j < circles_count; j++) {
			if (i == j) continue; // Skip self-collision

			Circle* circle = &circles[i];
			Circle* circle2 = &circles[j];

			stuf.distance = sqrt(pow(circle->x - circle2->x, 2) + pow(circle->y - circle2->y, 2));
			if (stuf.distance == 0) continue; // Avoid division by zero

			stuf.normalized_dx = (circle2->x - circle->x) / stuf.distance;
			stuf.normalized_dy = (circle2->y - circle->y) / stuf.distance;

			collision_detection_BouncingOffEachOther(circle, circle2, stuf);
		}
	}

	// Handle wall collisions
	for (Circle& circle : circles) {
		if (circle.x + circle.radius > WIDTH) {
			circle.x = WIDTH - circle.radius;
			circle.v_x = -circle.v_x * DAMPPING;
		}
		if (circle.x - circle.radius < 0) {
			circle.x = circle.radius;
			circle.v_x = -circle.v_x * DAMPPING;
		}
		if (circle.y + circle.radius > HIEGHT) {
			circle.y = HIEGHT - circle.radius;
			circle.v_y = -circle.v_y * DAMPPING;
		}
		if (circle.y - circle.radius < 0) {
			circle.y = circle.radius;
			circle.v_y = -circle.v_y * DAMPPING;
		}
	}
}


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);


	SDL_Window* window = SDL_CreateWindow("SDL3 Bouncing Circles", WIDTH, HIEGHT, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	vector<Circle> circles;

	int ball;
	cout << "how many ball :";
	cin >> ball;
	int k = 10, l = 5;
	for (int i = 0; i < ball; i++) {
		if (k > 700)
		{
			l += 10;
			k = 10;
		}
		circles.push_back({ (double)k, (double)l, 5, 1, 0, 1, 1 });
		k += 10;
	}

	int running = 1;
	SDL_Event event;
	Physics stuf = { 0,0,0,0,0,0,0 };

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = 0;
			}
		}

		SetRenderDrawColor(renderer, { 0, 0, 0, 255 });
		SDL_RenderClear(renderer);

		step(circles, stuf);
		DrawCircle(renderer, circles);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
