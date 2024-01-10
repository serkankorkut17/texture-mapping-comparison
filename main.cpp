#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

struct Point {
    float x, y;
};

struct TexCoord {
    float s, t;
};

void calculateBarycentricCoordinates(const Point& p, const Point& v1, const Point& v2, const Point& v3, float& alpha, float& beta, float& gamma) {
    float detT = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y);
    alpha = ((v2.y - v3.y) * (p.x - v3.x) + (v3.x - v2.x) * (p.y - v3.y)) / detT;
    beta = ((v3.y - v1.y) * (p.x - v3.x) + (v1.x - v3.x) * (p.y - v3.y)) / detT;
    gamma = 1.0f - alpha - beta;
}

TexCoord affineTextureMapping(const Point& p, const Point& v1, const Point& v2, const Point& v3, const TexCoord& t1, const TexCoord& t2, const TexCoord& t3) {
    float alpha, beta, gamma;
    calculateBarycentricCoordinates(p, v1, v2, v3, alpha, beta, gamma);

    // Interpolate texture coordinates using barycentric coordinates
    float s = alpha * t1.s + beta * t2.s + gamma * t3.s;
    float t = alpha * t1.t + beta * t2.t + gamma * t3.t;

    return {s, t};
}

TexCoord perspectivelyCorrectTextureMapping(const Point& p, const Point& v1, const Point& v2, const Point& v3, const TexCoord& t1, const TexCoord& t2, const TexCoord& t3, float v1z, float v2z, float v3z) {
    float alpha, beta, gamma;
    calculateBarycentricCoordinates(p, v1, v2, v3, alpha, beta, gamma);

    // Interpolate texture coordinates using barycentric coordinates
    float s = (alpha * t1.s / v1z + beta * t2.s / v2z + gamma * t3.s / v3z) / (alpha / v1z + beta / v2z + gamma / v3z);
    float t = (alpha * t1.t / v1z + beta * t2.t / v2z + gamma * t3.t / v3z) / (alpha / v1z + beta / v2z + gamma / v3z);

    return {s, t};
}

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Load image
    SDL_Surface* textureSurface = IMG_Load("../happy.png");
    if (!textureSurface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    std::cout << "Image loaded successfully" << std::endl;




    window = SDL_CreateWindow("Affine Texture vs Perspectively Correct Texture Maps", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

    // Define camera parameters
    glm::vec3 cameraPosition = glm::vec3(2.0f, 2.0f, 6.0f);  // Camera position in world coordinates
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);     // Camera target point in world coordinates
    glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);         // Up vector for the camera

    // Initialize rotation angle
    float rotationAngle = 0.0f;
    const float angularSpeed = glm::two_pi<float>() / 720.0f;  // Adjust for desired rotation speed (e.g., one rotation per second)

    // Define square parameters
    glm::vec4 vertex1 = glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);
    glm::vec4 vertex2 = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
    glm::vec4 vertex3 = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 vertex4 = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

    // Create a Perspective matrix using glm::perspective
    float fieldOfView = glm::radians(45.0f);
    float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    glm::mat4 perspectiveMatrix = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);


    bool isAffine = true;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                // Handle key presses for camera movement
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        cameraPosition.z += 1.0f;
                        break;
                    case SDLK_DOWN:
                        cameraPosition.z -= 1.0f;
                        break;
                    case SDLK_LEFT:
                        cameraPosition.x -= 1.0f;
                        break;
                    case SDLK_RIGHT:
                        cameraPosition.x += 1.0f;
                        break;
                    case SDLK_a:
                        isAffine = !isAffine;
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Create a view matrix using glm::lookAt
        glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
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
        // z values for perspective correction
        float v1z, v2z, v3z, v4z, v5z, v6z;
        v1z = transformedVertex1.z;
        v2z = transformedVertex2.z;
        v3z = transformedVertex3.z;
        v4z = transformedVertex1.z;
        v5z = transformedVertex4.z;
        v6z = transformedVertex3.z;

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

        // Define vertices of the triangle in screen space
        Point v1 = {static_cast<float>(x1), static_cast<float>(y1)};
        Point v2 = {static_cast<float>(x2), static_cast<float>(y2)};
        Point v3 = {static_cast<float>(x3), static_cast<float>(y3)};

        Point v4 = {static_cast<float>(x1), static_cast<float>(y1)};
        Point v5 = {static_cast<float>(x4), static_cast<float>(y4)};
        Point v6 = {static_cast<float>(x3), static_cast<float>(y3)};

        // Define texture coordinates corresponding to the vertices
        TexCoord t1 = {0.0f, 0.0f};
        TexCoord t2 = {0.0f, 1.0f};
        TexCoord t3 = {1.0f, 1.0f};

        TexCoord t4 = {0.0f, 0.0f};
        TexCoord t5 = {1.0f, 0.0f};
        TexCoord t6 = {1.0f, 1.0f};

        // Scan the bounding box of the triangle 1
        for (int x = std::min({static_cast<int>(v1.x), static_cast<int>(v2.x), static_cast<int>(v3.x)});
            x <= std::max({static_cast<int>(v1.x), static_cast<int>(v2.x), static_cast<int>(v3.x)});
            ++x) {
            for (int y = std::min({static_cast<int>(v1.y), static_cast<int>(v2.y), static_cast<int>(v3.y)});
                y <= std::max({static_cast<int>(v1.y), static_cast<int>(v2.y), static_cast<int>(v3.y)});
                ++y) {
                Point p = {static_cast<float>(x), static_cast<float>(y)};

                // Check if the point is inside the triangle
                float alpha, beta, gamma;
                calculateBarycentricCoordinates(p, v1, v2, v3, alpha, beta, gamma);

                if (alpha >= 0.0f && alpha <= 1.0f && beta >= 0.0f && beta <= 1.0f && gamma >= 0.0f && gamma <= 1.0f) {
                    TexCoord texCoord;
                    if (isAffine)
                        texCoord = affineTextureMapping(p, v1, v2, v3, t1, t2, t3);
                    else
                        texCoord = perspectivelyCorrectTextureMapping(p, v1, v2, v3, t1, t2, t3, v1z, v2z, v3z);

                    int texX = static_cast<int>(texCoord.s * textureSurface->w);
                    int texY = static_cast<int>(texCoord.t * textureSurface->h);

                    if (texX >= 0 && texX < textureSurface->w && texY >= 0 && texY < textureSurface->h) {
                        // Get pixel color from the texture image at the mapped coordinates
                        Uint32 pixel = static_cast<Uint32*>(textureSurface->pixels)[texY * textureSurface->w + texX];

                        SDL_SetRenderDrawColor(renderer, (pixel >> 16) & 0xFF, (pixel >> 8) & 0xFF, pixel & 0xFF, 255);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }
            }
        }
        // Scan the bounding box of the triangle 2
        for (int x = std::min({static_cast<int>(v4.x), static_cast<int>(v5.x), static_cast<int>(v6.x)});
            x <= std::max({static_cast<int>(v4.x), static_cast<int>(v5.x), static_cast<int>(v6.x)});
            ++x) {
            for (int y = std::min({static_cast<int>(v4.y), static_cast<int>(v5.y), static_cast<int>(v6.y)});
                y <= std::max({static_cast<int>(v4.y), static_cast<int>(v5.y), static_cast<int>(v6.y)});
                ++y) {
                Point p = {static_cast<float>(x), static_cast<float>(y)};
                // Check if the point is inside the triangle
                float alpha, beta, gamma;
                calculateBarycentricCoordinates(p, v4, v5, v6, alpha, beta, gamma);

                if (alpha >= 0.0f && alpha <= 1.0f && beta >= 0.0f && beta <= 1.0f && gamma >= 0.0f && gamma <= 1.0f) {
                    TexCoord texCoord;
                    if (isAffine)
                        texCoord = affineTextureMapping(p, v4, v5, v6, t4, t5, t6);
                    else
                        texCoord = perspectivelyCorrectTextureMapping(p, v4, v5, v6, t4, t5, t6, v4z, v5z, v6z);

                    int texX = static_cast<int>(texCoord.s * textureSurface->w);
                    int texY = static_cast<int>(texCoord.t * textureSurface->h);

                    if (texX >= 0 && texX < textureSurface->w && texY >= 0 && texY < textureSurface->h) {
                        Uint32 pixel = static_cast<Uint32*>(textureSurface->pixels)[texY * textureSurface->w + texX];

                        SDL_SetRenderDrawColor(renderer, (pixel >> 16) & 0xFF, (pixel >> 8) & 0xFF, pixel & 0xFF, 255);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);

    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(textureSurface);
    IMG_Quit();
    SDL_Quit();

    return 0;
}