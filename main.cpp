#include <SDL.h>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL Rotating Square Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

    // Define camera parameters
    glm::vec3 cameraPosition = glm::vec3(7.0f, 7.0f, 6.0f);  // Camera position in world coordinates
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);     // Camera target point in world coordinates
    glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);         // Up vector for the camera

    // Initialize rotation angle
    float rotationAngle = 0.0f;
    const float angularSpeed = glm::two_pi<float>() / 360.0f;  // Adjust for desired rotation speed (e.g., one rotation per second)

    // Create a view matrix using glm::lookAt
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

    // Define square parameters
    glm::vec4 vertex1 = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 vertex2 = glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);
    glm::vec4 vertex3 = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 vertex4 = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);

    // Create a Perspective matrix using glm::perspective
    float fieldOfView = glm::radians(45.0f);
    float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    glm::mat4 perspectiveMatrix = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);

    int squareSize = 50;
    float angle = 0.0f;
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        // Create a model matrix for continuous rotation around the y-axis
        glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        // Increment rotation angle
        rotationAngle += angularSpeed;

        // Combine the model matrix and view matrix to get the final transformation matrix
        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        // Combine the model-view matrix and perspective matrix to get the final transformation matrix
        glm::mat4 modelViewPerspectiveMatrix = perspectiveMatrix * modelViewMatrix;

        // Transform the vertices of the square
        glm::vec4 transformedVertex1 = modelViewPerspectiveMatrix * vertex1;
        glm::vec4 transformedVertex2 = modelViewPerspectiveMatrix * vertex2;
        glm::vec4 transformedVertex3 = modelViewPerspectiveMatrix * vertex3;
        glm::vec4 transformedVertex4 = modelViewPerspectiveMatrix * vertex4;

        // Normalize the transformed vertices
        transformedVertex1 /= transformedVertex1.w;
        transformedVertex2 /= transformedVertex2.w;
        transformedVertex3 /= transformedVertex3.w;
        transformedVertex4 /= transformedVertex4.w;

        // Convert the normalized vertices to screen coordinates
        int x1 = static_cast<int>((transformedVertex1.x + 1.0f) * SCREEN_WIDTH / 2.0f);
        int y1 = static_cast<int>((1.0f - transformedVertex1.y) * SCREEN_HEIGHT / 2.0f);
        int x2 = static_cast<int>((transformedVertex2.x + 1.0f) * SCREEN_WIDTH / 2.0f);
        int y2 = static_cast<int>((1.0f - transformedVertex2.y) * SCREEN_HEIGHT / 2.0f);
        int x3 = static_cast<int>((transformedVertex3.x + 1.0f) * SCREEN_WIDTH / 2.0f);
        int y3 = static_cast<int>((1.0f - transformedVertex3.y) * SCREEN_HEIGHT / 2.0f);
        int x4 = static_cast<int>((transformedVertex4.x + 1.0f) * SCREEN_WIDTH / 2.0f);
        int y4 = static_cast<int>((1.0f - transformedVertex4.y) * SCREEN_HEIGHT / 2.0f);





        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set background color to white
        SDL_RenderClear(renderer);

        // Set the draw color to red
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Draw the square
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer, x2, y2, x4, y4);
        SDL_RenderDrawLine(renderer, x4, y4, x3, y3);
        SDL_RenderDrawLine(renderer, x3, y3, x1, y1);

        SDL_RenderPresent(renderer);

        // Cap the frame rate
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}