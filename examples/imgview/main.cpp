#include "stb_image.h"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "krill.hpp"
#include <iostream>
#include <optional>

struct vertex {
    float x, y;
    float tx, ty;
};

ShaderProgram createProgram()
{
    std::cout << "About to start vert" << std::endl;
    VertexShader basicVert(std::filesystem::path("shaders/passthru.vert"));
    std::cout << "About to start redTri" << std::endl;
    FragmentShader redTri(std::filesystem::path("shaders/red.frag"));
    return ShaderProgram(basicVert, redTri);
}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <png filepath>" << std::endl;
        return 1;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 4);
    std::cout << "Channels:" << channels << std::endl;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisplayID *displayIds = SDL_GetDisplays(nullptr);
    const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(displayIds[0]);
    float target_heightf = dm->h / 1.1;
    float imgRatio = static_cast<float>(width) / static_cast<float>(height);
    int target_width = static_cast<int>(target_heightf * imgRatio);
    char title[50];
    snprintf(title, 50, "imgview: %s", argv[1]);
    SDL_Window *window = SDL_CreateWindow(title, target_width, static_cast<int>(target_heightf), SDL_WINDOW_OPENGL);

    if(window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    glewInit();

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    ArrayBuffer vbo;
    ElementArrayBuffer ebo;
    VAO vao;
    vao.bind();

    vbo.bind();
    std::vector<vertex> vertices = {{-1.0f, 1.0, 0.0, 1.0}, //topleft
        {1.0, 1.0, 1.0, 1.0}, //topright
        {-1.0, -1.0, 0.0, 0.0}, //botleft
        {1.0, -1.0, 1.0, 0.0}}; //botright
    vbo.setData(vertices, GL_STATIC_DRAW);

    std::vector<unsigned int> indices = {0, 1, 2, 3};
    ebo.bind();
    ebo.setData(indices, GL_STATIC_DRAW);

    VertexDefinition vd;
    vd.addVec2();
    vd.addVec2();
    vbo.useVertexDefinition(vd);

    unsigned int texture;

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(img);

    ShaderProgram program = createProgram();
    program.use();
    glUniform1i(glGetUniformLocation(program.getProgramId(), "tex"), 0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
    bool shouldExit = false;

    while(!shouldExit) {
        SDL_Event event;

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                shouldExit = true;
            }
        }
    }
    return 0;
}
