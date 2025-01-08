#include "SDL.h"
#include <stdbool.h>
#include <math.h> // For sqrt and pow

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;

    // Screen dimensions
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    window = SDL_CreateWindow("Simple Paint Program",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Create a texture for pixel drawing
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!window || !renderer || !texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialization error: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }

    // Create a pixel buffer (ARGB format)
    Uint32 *pixels = calloc(SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(Uint32));

    if (!pixels) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate pixel buffer.");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    // Fill the buffer with white (background color)
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        pixels[i] = 0xFFFFFFFF; // White (ARGB: 255, 255, 255, 255)
    }

    bool running = true;
    bool drawing = false;
    int brushSize = 6; // Brush size (16x16)
    int lastX = -1, lastY = -1; // To track the last position of the mouse

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                drawing = true;
                lastX = event.motion.x;
                lastY = event.motion.y;
            } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
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
                    int steps = (int)sqrt(dx * dx + dy * dy); // Number of interpolation steps
                    for (int i = 0; i <= steps; i++) {
                        float t = (float)i / steps;
                        int interpX = lastX + t * dx;
                        int interpY = lastY + t * dy;

                        // Draw the brush at each interpolated position
                        for (int dy = 0; dy < brushSize; dy++) {
                            for (int dx = 0; dx < brushSize; dx++) {
                                int drawX = interpX + dx - brushSize / 2;
                                int drawY = interpY + dy - brushSize / 2;

                                // Check bounds to prevent drawing outside the screen
                                if (drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                                    pixels[drawY * SCREEN_WIDTH + drawX] = 0xFF000000; // Black (ARGB: 255, 0, 0, 0)
                                }
                            }
                        }
                    }
                }

                lastX = x;
                lastY = y;
            }
        }

        // Update the texture with the pixel buffer
        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));

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
