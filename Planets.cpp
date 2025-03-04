#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "slime.h"
#include "math.h"

using namespace std;

#define HIEGHT 600
#define WIDTH 900
#define GRAVITY 0.2
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
    double distance,overlap,normalized_dx,normalized_dy,accelaration_gravity,ax,ay,relative_vx, relative_vy,velocity;
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
            circle->v_x = circle->v_x + (stuf.velocity * stuf.normalized_dx);
            circle->v_y = circle->v_y + (stuf.velocity * stuf.normalized_dy);
            circle2->v_x = circle2->v_x - (stuf.velocity * stuf.normalized_dx);
            circle2->v_y = circle2->v_y - (stuf.velocity * stuf.normalized_dy);
        }
    }
}

void collision_detection_BouncingOffEachOtherWithMass(Circle* circle, Circle* circle2, Physics& stuf) {

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

            double mass_circle1 = (circle2->mass * 2) / (circle->mass + circle2->mass) ;
            circle->v_x = circle->v_x - ((mass_circle1 * stuf.velocity * stuf.normalized_dx) * DELTA_TIME);
            circle->v_y = circle->v_y - ((mass_circle1 * stuf.velocity * stuf.normalized_dy) * DELTA_TIME);

            double mass_circle2 = (circle->mass * 2) / (circle->mass + circle2->mass);
            circle2->v_x = circle2->v_x - ((mass_circle2 * stuf.velocity * stuf.normalized_dx) * DELTA_TIME);
            circle2->v_y = circle2->v_y - ((mass_circle2 * stuf.velocity * stuf.normalized_dy) * DELTA_TIME);
        }
    }
}


void step(Circle* circle, Circle* circle2,Physics& stuf) {
    circle->x += circle->v_x * DELTA_TIME;
    circle->y += circle->v_y * DELTA_TIME;
    circle2->x += circle2->v_x * DELTA_TIME;
    circle2->y += circle2->v_y * DELTA_TIME;
    //circle->v_y += A_GRAVITY * DAMPPING;
    stuf.distance = sqrt(pow(circle->x - circle2->x, 2) + pow(circle->y - circle2->y, 2));
    stuf.normalized_dx = (circle2->x - circle->x) / stuf.distance;
    stuf.normalized_dy = (circle2->y - circle->y) / stuf.distance;

    collision_detection_BouncingOffEachOtherWithMass(circle, circle2, stuf);

    
    if(stuf.distance > 0) {
        stuf.accelaration_gravity = (GRAVITY * circle->mass * circle2->mass) / pow(stuf.distance, 2);
        stuf.ax = stuf.accelaration_gravity * stuf.normalized_dx;
        stuf.ay = stuf.accelaration_gravity * stuf.normalized_dy;

        circle->v_x += (stuf.ax * circle->mass) * DELTA_TIME;
        circle->v_y += (stuf.ay * circle->mass) * DELTA_TIME;
        circle2->v_x += (-stuf.ax * circle2->mass) * DELTA_TIME;
        circle2->v_y += (-stuf.ay * circle2->mass) * DELTA_TIME;
    }


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

    //right
    if (circle2->x + circle2->radius > WIDTH) {
        circle2->x = WIDTH - circle2->radius;
        circle2->v_x = -circle2->v_x * DAMPPING;
    }
    // left
    if (circle2->x - circle2->radius < 0) {
        circle2->x = circle2->radius;
        circle2->v_x = -circle2->v_x * DAMPPING;
    }
    //bottom
    if (circle2->y + circle2->radius > HIEGHT) {
        circle2->y = HIEGHT - circle2->radius;
        circle2->v_y = -circle2->v_y * DAMPPING;
    }
    //top
    if (circle2->y - circle2->radius < 0) {
        circle2->y = circle2->radius;
        circle2->v_y = -circle2->v_y * DAMPPING;
    }
}

void renderText(SDL_Renderer* renderer, TTF_Font* font,string text, double value,int x ,int y) {
    SDL_Color white = { 255,255,255,255 };

    string text1 = text + to_string(value);

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

    Physics stuf = { 0,0,0,0,0,0,0 };

    Circle circle = { 200,200,75,0,0,0,0.003};
    Circle circle2 = { 400,400,40,0,0,0,0.003};

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

        circle.mass = circle.density * PI * pow(circle.radius, 2);
        circle2.mass = circle2.density * PI * pow(circle2.radius, 2);

        renderText(renderer,font,"circle 1 mass : ",(double)circle.mass,10,10);
        renderText(renderer, font, "circle 2 mass : ", (double)circle2.mass,10,40);
        renderText(renderer, font, "distance : ",(double)stuf.distance, 10, 70);

        DrawCircle(renderer, circle);
        DrawCircle(renderer, circle2);

        SetRenderDrawColor(renderer,BLUE);
        SDL_RenderLine(renderer, circle.x, circle.y, circle2.x, circle2.y);
        step(&circle,&circle2,stuf);
        //SDL_Delay(20);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
