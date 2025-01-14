#include <math.h>  // For sqrt and pow
#include <stdbool.h>
#include <stdint.h>

#include "SDL.h"
#include "stack.h"

enum BrushShape { CIRCULAR, SQUARE, FILL, ERASE_BRUSH, ERASE_FILL };

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

void fill(uint32_t *pixels, int x, int y, uint32_t fillColor);
void drawIcon(uint32_t *pixels, int x, int y, uint32_t color, int size,
              int brushShape);
void drawSquare(uint32_t *pixels, int x, int y, uint32_t color,
                int side_length);
void drawCircle(uint32_t *pixels, int x, int y, uint32_t color, int radius);
uint32_t makeColorLighter(uint32_t color);

int main(int argc, char *argv[]) {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Event event;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s",
                 SDL_GetError());
    return 3;
  }

  window =
      SDL_CreateWindow("jpaint", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, 0);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // Create a texture for pixel drawing
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                              SCREEN_HEIGHT);

  if (!window || !renderer || !texture) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialization error: %s",
                 SDL_GetError());
    SDL_Quit();
    return 3;
  }

  // Create a pixel buffer (ARGB format)
  uint32_t *pixels = calloc(SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(uint32_t));

  if (!pixels) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to allocate pixel buffer.");
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 3;
  }

  // Fill the buffer with white (background color)
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    pixels[i] = 0xFFFFFFFF;  // White (ARGB: 255, 255, 255, 255)
  }

  bool running = true;
  bool drawing = false;
  int brushSize = 6;                 // Brush size (16x16)
  int lastX = -1, lastY = -1;        // To track the last position of the mouse
  uint32_t brushColor = 0xFF000000;  // Default brush color (black)
  int brushShape = CIRCULAR;         // Default brush shape

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_MOUSEBUTTONDOWN &&
                 event.button.button == SDL_BUTTON_LEFT) {
        drawing = true;
        lastX = event.motion.x;
        lastY = event.motion.y;
        if (brushShape == FILL) {
          fill(pixels, lastX, lastY, brushColor);
        }
      } else if (event.type == SDL_MOUSEBUTTONUP &&
                 event.button.button == SDL_BUTTON_LEFT) {
        drawing = false;
        lastX = -1;
        lastY = -1;
      } else if (event.type == SDL_MOUSEMOTION && drawing) {
        int x = event.motion.x;
        int y = event.motion.y;

        // Interpolate points between the last and current position
        if (lastX != -1 && lastY != -1) {
          int dx = x - lastX;
          int dy = y - lastY;
          int steps =
              (int)sqrt(dx * dx + dy * dy);  // Number of interpolation steps
          for (int i = 0; i <= steps; i++) {
            float t = (float)i / steps;
            int interpX = lastX + t * dx;
            int interpY = lastY + t * dy;

            // Draw the brush at each interpolated position
            for (int dy = -brushSize / 2; dy <= brushSize / 2; dy++) {
              for (int dx = -brushSize / 2; dx <= brushSize / 2; dx++) {
                int drawX = interpX + dx;
                int drawY = interpY + dy;

                // Check bounds to prevent drawing outside the screen
                if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 &&
                    drawY < SCREEN_HEIGHT) {
                  if (brushShape == CIRCULAR) {
                    // Draw circular brush
                    if (dx * dx + dy * dy <=
                        (brushSize / 2) * (brushSize / 2)) {
                      pixels[drawY * SCREEN_WIDTH + drawX] = brushColor;
                    }
                  } else if (brushShape == SQUARE) {
                    // Draw square brush
                    pixels[drawY * SCREEN_WIDTH + drawX] = brushColor;
                  } else if (brushShape == FILL) {
                    //
                  } else if (brushShape == ERASE_BRUSH) {
                    // erase brush
                    pixels[drawY * SCREEN_WIDTH + drawX] = 0xFFFFFFFF;
                  } else if (brushShape == ERASE_FILL) {
                    fill(pixels, drawX, drawY, 0xFFFFFFFF);
                  }
                }
              }
            }
          }
        }

        lastX = x;
        lastY = y;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            // Clear the screen to white
            for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
              pixels[i] = 0xFFFFFFFF;  // White (ARGB: 255, 255, 255, 255)
            }
            break;
          case SDLK_1:
            brushColor = 0xFF000000;  // Black
            break;
          case SDLK_2:
            brushColor = 0xFFFF0000;  // Red
            break;
          case SDLK_3:
            brushColor = 0xFF0000FF;  // Blue
            break;
          case SDLK_4:
            brushColor = 0xFF00FF00;  // Green
            break;
          case SDLK_5:
            brushColor = 0xFFFFA500;  // Orange
            break;
          case SDLK_q:
            brushShape = CIRCULAR;
            break;
          case SDLK_w:
            brushShape = SQUARE;
            break;
          case SDLK_e:
            brushShape = FILL;
            break;
          case SDLK_r:
            brushShape = ERASE_BRUSH;
            break;
          case SDLK_t:
            brushShape = ERASE_FILL;
            break;
        }
      }
    }

    drawIcon(pixels, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 40, brushColor, 20,
             brushShape);

    // Update the texture with the pixel buffer
    SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));

    // Render the texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  // Cleanup
  free(pixels);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void fill(uint32_t *pixels, int x, int y, uint32_t fillColor) {
  if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
    return;
  }

  Stack *stack = createStack(1000);
  uint32_t originalColor = pixels[y * SCREEN_WIDTH + x];
  if (originalColor == fillColor) {
    destroy(stack);
    return;
  }

  push(stack, x, y);
  while (!isEmpty(stack)) {
    Point p = pop(stack);
    x = p.x;
    y = p.y;

    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
      continue;
    }

    uint32_t currentColor = pixels[y * SCREEN_WIDTH + x];
    if (currentColor != originalColor) {
      continue;
    }

    pixels[y * SCREEN_WIDTH + x] = fillColor;

    if (x + 1 < SCREEN_WIDTH) {
      push(stack, x + 1, y);
    }
    if (x - 1 >= 0) {
      push(stack, x - 1, y);
    }
    if (y + 1 < SCREEN_HEIGHT) {
      push(stack, x, y + 1);
    }
    if (y - 1 >= 0) {
      push(stack, x, y - 1);
    }
  }
  destroy(stack);
}

void drawIcon(uint32_t *pixels, int x, int y, uint32_t color, int size,
              int brushShape) {
  drawSquare(pixels, x, y, makeColorLighter(color), size);
  if (brushShape == CIRCULAR) {
    drawCircle(pixels, x, y, color, size);
  } else if (brushShape == SQUARE || brushShape == FILL) {
    drawSquare(pixels, x, y, color, size);
  } else if (brushShape == ERASE_BRUSH) {
    drawSquare(pixels, x, y, 0xFF000000, size);
    drawSquare(pixels, x, y, 0xFFFFFFFF, size - 6);
  } else if (brushShape == ERASE_FILL) {
    drawSquare(pixels, x, y, makeColorLighter(0xFF000000), size);
    drawCircle(pixels, x, y, 0xFF000000, size);
    drawCircle(pixels, x, y, 0xFFFFFFFF, size - 3);
  }
}

void drawSquare(uint32_t *pixels, int x, int y, uint32_t color,
                int side_length) {
  for (int dy = -side_length / 2; dy <= side_length / 2; dy++) {
    for (int dx = -side_length / 2; dx <= side_length / 2; dx++) {
      int drawX = x + dx;
      int drawY = y + dy;

      if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 &&
          drawY < SCREEN_HEIGHT) {
        pixels[drawY * SCREEN_WIDTH + drawX] = color;
      }
    }
  }
}

void drawCircle(uint32_t *pixels, int x, int y, uint32_t color, int radius) {
  for (int dy = -radius / 2; dy <= radius / 2; dy++) {
    for (int dx = -radius / 2; dx <= radius / 2; dx++) {
      if (dx * dx + dy * dy < (radius / 2) * (radius / 2)) {
        int drawX = x + dx;
        int drawY = y + dy;

        if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 &&
            drawY < SCREEN_HEIGHT) {
          pixels[drawY * SCREEN_WIDTH + drawX] = color;
        }
      }
    }
  }
}

uint32_t makeColorLighter(uint32_t color) {
  uint8_t r = (color & 0x00FF0000) >> 16;
  uint8_t g = (color & 0x0000FF00) >> 8;
  uint8_t b = (color & 0x000000FF);

  r = (r + 255) / 2;
  g = (g + 255) / 2;
  b = (b + 255) / 2;

  return 0xFF000000 | (r << 16) | (g << 8) | b;
}