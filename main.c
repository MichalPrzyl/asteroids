#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

#define PI 3.14
#define GAME_WINDOW_WIDTH 640
#define GAME_WINDOW_HEIGHT 480

struct Point {
  float x;
  float y;
};

struct Ship {
  struct Point position;
  float velocity;
  float rotation;
};

struct State {
  struct Ship ship;
};

void draw_ship(SDL_Renderer *renderer, struct Ship *ship) {
  int ship_size = 30;

  struct Point p1 = {
      ship->position.x + ship_size * cos(ship->rotation * PI / 180),
      ship->position.y + ship_size * sin(ship->rotation * PI / 180)};

  // Dwa kolejne punkty tworzące podstawę trójkąta
  struct Point p2 = {
      ship->position.x +
          (ship_size * 0.6) * cos((ship->rotation + 135) * PI / 180),
      ship->position.y +
          (ship_size * 0.6) * sin((ship->rotation + 135) * PI / 180)};

  struct Point p3 = {
      ship->position.x +
          (ship_size * 0.6) * cos((ship->rotation + 225) * PI / 180),
      ship->position.y +
          (ship_size * 0.6) * sin((ship->rotation + 225) * PI / 180)};

  // change color to white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  // Draw triangle
  SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
  SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
  SDL_RenderDrawLine(renderer, p3.x, p3.y, p1.x, p1.y);

  /* SDL_RenderDrawLine(renderer, 15, 15, 50, 50); */
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
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == NULL) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_Event e;
  int running = 1;

  struct Point initial_position = {.x = 50.0f, .y = 50.0f};

  struct Ship ship = {
      .position = initial_position, .velocity = 10, .rotation = 50};

  struct State state = {.ship = ship};

  Uint32 lastTime = SDL_GetTicks(); // Pobranie czasu na początku pętli

  float speed_multiplier = 300;
  float rotation_speed_multiplier = 300;

  while (running) {
    Uint32 currentTime = SDL_GetTicks(); // Pobranie aktualnego czasu
    float deltaTime =
        (currentTime - lastTime) / 1000.0f; // Obliczenie deltaTime w sekundach
    lastTime = currentTime;
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        running = 0;
      }
    }

    // Uzyskanie stanu klawiatury
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    if (keyboard_state[SDL_SCANCODE_D]) {
      state.ship.rotation += 1.0f * deltaTime * rotation_speed_multiplier;
    }
    if (keyboard_state[SDL_SCANCODE_A]) {
      state.ship.rotation -= 1.0f * deltaTime * rotation_speed_multiplier;
    }
    // New keboard states
    if (keyboard_state[SDL_SCANCODE_W]) {
      float x_pos_change = 2 * cos(state.ship.rotation * PI / 180) * deltaTime *
                           speed_multiplier;
      state.ship.position.x += x_pos_change;
      state.ship.position.y += 2 * sin(state.ship.rotation * PI / 180) *
                               deltaTime * speed_multiplier;
    }

    // Going beyond window width/height
    // X axis
    if (state.ship.position.x > GAME_WINDOW_WIDTH) {
      state.ship.position.x = 0; // Pojawia się po lewej stronie
    } else if (state.ship.position.x < 0) {
      state.ship.position.x =
          GAME_WINDOW_WIDTH; // Pojawia się po prawej stronie
    }
    // Y axis
    if (state.ship.position.y > GAME_WINDOW_HEIGHT) {
      state.ship.position.y = 0; // Pojawia się na górze
    } else if (state.ship.position.y < 0) {
      state.ship.position.y = GAME_WINDOW_HEIGHT; // Pojawia się na dole
    }

    printf("state.ship.position.x: %d\n", state.ship.position.x);
    // Renderer stuff
    // Clear renderer
    SDL_RenderClear(renderer);

    draw_ship(renderer, &state.ship);

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
