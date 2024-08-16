#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include "circle.h"

#define PI 3.14
#define GAME_WINDOW_WIDTH 640
#define GAME_WINDOW_HEIGHT 480

#define MAX_BULLETS 30
#define BULLET_SIZE 4

#define MAX_METEORS 10

struct Point {
  float x;
  float y;
};

struct Ship {
  struct Point position;
  struct Point velocity;
  float rotation;
  int thrusting;
};

struct State {
  struct Ship ship;
};

struct Bullet {
  struct Point position;
  struct Point velocity;
  int active;
};

struct Meteor {
    struct Point position;
    struct Point velocity;
    float size;
};

void update_meteors_positions(struct Meteor meteors[], float delta_time){
    // Update active bullets positions.
    for (int i = 0; i < MAX_METEORS; i++){
        meteors[i].position.x += meteors[i].velocity.x * delta_time;
        meteors[i].position.y += meteors[i].velocity.y * delta_time;

      // Check if meteors is outside the window.
      if (meteors[i].position.x < 0) {
          meteors[i].position.x = GAME_WINDOW_WIDTH;
      } else if (meteors[i].position.x > GAME_WINDOW_WIDTH) {
          meteors[i].position.x = 0;
      }

      if (meteors[i].position.y < 0) {
          meteors[i].position.y = GAME_WINDOW_HEIGHT;
      } else if (meteors[i].position.y > GAME_WINDOW_HEIGHT) {
          meteors[i].position.y = 0;
      }
    }
}

void init_meteors(SDL_Renderer *renderer, struct Meteor meteors[]) {
    for (int i = 0; i < MAX_METEORS; i++) {
        meteors[i].size = rand() % 20 + 20;  // Losowy rozmiar od 20 do 40
        meteors[i].position.x = rand() % GAME_WINDOW_WIDTH;
        meteors[i].position.y = rand() % GAME_WINDOW_HEIGHT;
        meteors[i].velocity.x = (rand() % 50) - 25;  // Losowa prędkość w zakresie -25 do 25
        meteors[i].velocity.y = (rand() % 50) - 25;
    }
}

void draw_meteors(SDL_Renderer *renderer, struct Meteor meteors[]) {
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);  // Szary kolor dla meteorytów
    for (int i = 0; i < MAX_METEORS; i++) {
        draw_circle(renderer, meteors[i].position.x, meteors[i].position.y, meteors[i].size / 2);
    }
}

void draw_bullets(SDL_Renderer *renderer, struct Bullet bullets[]){
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active){
      SDL_Rect bullet_rect;

      // Bullets are just small rectangles.
      bullet_rect.x = (int)bullets[i].position.x - BULLET_SIZE / 2;
      bullet_rect.y = (int)bullets[i].position.y - BULLET_SIZE / 2;

      // Fixed bullet size.
      bullet_rect.w = BULLET_SIZE;
      bullet_rect.h = BULLET_SIZE;

      // Draw filled rectangle.
      SDL_RenderFillRect(renderer, &bullet_rect);
    }
  }
}

void draw_ship(SDL_Renderer *renderer, struct Ship *ship, Uint32 current_time) {
  int ship_size = 30;

  struct Point p1 = {
      ship->position.x + ship_size * cos(ship->rotation * PI / 180),
      ship->position.y + ship_size * sin(ship->rotation * PI / 180)
  };

  struct Point p2 = {
      ship->position.x + (ship_size * 0.6) * cos((ship->rotation + 135) * PI / 180),
      ship->position.y + (ship_size * 0.6) * sin((ship->rotation + 135) * PI / 180)
  };

  struct Point p3 = {
      ship->position.x + (ship_size * 0.6) * cos((ship->rotation + 225) * PI / 180),
      ship->position.y + (ship_size * 0.6) * sin((ship->rotation + 225) * PI / 180)
  };

  if (ship->thrusting && ((current_time / 20) % 2 == 0)) {

        struct Point flame1 = {
            ship->position.x + (ship_size * 0.8) * cos((ship->rotation + 180) * PI / 180),
            ship->position.y + (ship_size * 0.8) * sin((ship->rotation + 180) * PI / 180)
        };

        struct Point flame2 = {
            ship->position.x + (ship_size * 0.5) * cos((ship->rotation + 160) * PI / 180),
            ship->position.y + (ship_size * 0.5) * sin((ship->rotation + 160) * PI / 180)
        };

        struct Point flame3 = {
            ship->position.x + (ship_size * 0.5) * cos((ship->rotation + 200) * PI / 180),
            ship->position.y + (ship_size * 0.5) * sin((ship->rotation + 200) * PI / 180)
        };

        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);  // Pomarańczowy kolor dla płomieni
        SDL_RenderDrawLine(renderer, p2.x, p2.y, flame1.x, flame1.y);
        SDL_RenderDrawLine(renderer, p3.x, p3.y, flame1.x, flame1.y);
        SDL_RenderDrawLine(renderer, flame2.x, flame2.y, flame3.x, flame3.y);
    }

  // change color to white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  // Draw triangle
  SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
  SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
  SDL_RenderDrawLine(renderer, p3.x, p3.y, p1.x, p1.y);
}

int main(int argc, char *argv[]) {
  // SDL Init
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    return 1;
  }

  // Window creation.
  SDL_Window *window = SDL_CreateWindow(
      "MP WINDOW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    SDL_Quit();
    return 1;
  }

  // Renderer creation
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == NULL) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_Event e;
  int running = 1;

  struct Point initial_position = {.x = 50.0f, .y = 50.0f};
  struct Ship ship = { .position = initial_position, .velocity = 1, .rotation = 50, .thrusting = 0 };
  struct State state = { .ship = ship };

  Uint32 lastTime = SDL_GetTicks(); // Get time before main loop

  float speed_multiplier = 10;
  float velocity_change_speed_multiplier = 12;
  float rotation_speed_multiplier = 300;
  float bullet_speed = 300;

  // Bullet list
  struct Bullet bullets[MAX_BULLETS];
  // Meteor list
  struct Meteor meteors[MAX_METEORS];

  init_meteors(renderer, meteors);

  while (running) {
    Uint32 currentTime = SDL_GetTicks();
    float delta_time = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_KEYDOWN) {

        // Shooting
        if (e.key.keysym.sym == SDLK_SPACE) {
          for (int i = 0; i < MAX_BULLETS; i++){
            // Get first inactive bullet and activate it as an active one.
            if (!bullets[i].active){
              bullets[i].active = 1;

              // offsetting the initial position of new bullet
              float offset_distance = 30.0f; // how far from center of player
              bullets[i].position.x = state.ship.position.x + offset_distance * cos(state.ship.rotation * PI / 180);
              bullets[i].position.y = state.ship.position.y + offset_distance * sin(state.ship.rotation * PI / 180);

              bullets[i].velocity.x = cos(state.ship.rotation * PI / 180) * bullet_speed;
              bullets[i].velocity.y = sin(state.ship.rotation * PI / 180) * bullet_speed;
              break;
            }
          }
        }
      }
      if (e.type == SDL_QUIT) {
        running = 0;
      }
    }

    // Keyboard state
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);

    if (keyboard_state[SDL_SCANCODE_D]) {
      state.ship.rotation += 1.0f * delta_time * rotation_speed_multiplier;
    }
    if (keyboard_state[SDL_SCANCODE_A]) {
      state.ship.rotation -= 1.0f * delta_time * rotation_speed_multiplier;
    }
    if (keyboard_state[SDL_SCANCODE_W]) {
      float x_vel_change = cos(state.ship.rotation * PI / 180) * delta_time * velocity_change_speed_multiplier;
      state.ship.velocity.x += x_vel_change;

      float y_vel_change = sin(state.ship.rotation * PI / 180) * delta_time * velocity_change_speed_multiplier;
      state.ship.velocity.y += y_vel_change;
      state.ship.thrusting = 1;
    } else{
      state.ship.thrusting = 0;
    }

    // Update active bullets positions.
    for (int i = 0; i < MAX_BULLETS; i++){
      if (bullets[i].active){
        bullets[i].position.x += bullets[i].velocity.x * delta_time;
        bullets[i].position.y += bullets[i].velocity.y * delta_time;
      }

      // Check if bullet is outside the window. If so, change it's 'active' field to 0.
      if (bullets[i].position.x < 0 || bullets[i].position.x > GAME_WINDOW_WIDTH ||
            bullets[i].position.y < 0 || bullets[i].position.y > GAME_WINDOW_HEIGHT) {
            bullets[i].active = 0;
        }
    }
    update_meteors_positions(meteors, delta_time);
    
    // Update player's ship position.
    state.ship.position.x += state.ship.velocity.x * delta_time * speed_multiplier;
    state.ship.position.y += state.ship.velocity.y * delta_time * speed_multiplier;
    // Going beyond window width/height
    // X axis
    if (state.ship.position.x > GAME_WINDOW_WIDTH) {
      state.ship.position.x = 0;
    } else if (state.ship.position.x < 0) {
      state.ship.position.x = GAME_WINDOW_WIDTH;
    }
    // Y axis
    if (state.ship.position.y > GAME_WINDOW_HEIGHT) {
      state.ship.position.y = 0;
    } else if (state.ship.position.y < 0) {
      state.ship.position.y = GAME_WINDOW_HEIGHT;
    }

    /* printf("state.ship.position.x: %f\n", state.ship.position.x); */
    // Renderer stuff
    // Clear renderer
    SDL_RenderClear(renderer);

    draw_ship(renderer, &state.ship, currentTime);
    draw_bullets(renderer, bullets);
    draw_meteors(renderer, meteors);

    // black color background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // show renderer on screen
    SDL_RenderPresent(renderer);
  }

  // Clear and end running
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
