// translated to krill from https://github.com/c2d7fa/opengl-cube
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>

#include "krill.hpp"

struct context {
    ShaderProgram *shaderProgram;
    unsigned int vao;
    GLFWwindow* window;

    unsigned int uniform_transform;
    ElementArrayBuffer ebo;
};

void render(struct context*);
void initialize(struct context*);


// a buffer for target, rather go slightly too fast than too slow
double tgtFps = 165.0 + (165 * .05);

double secPerFrame = 1.0/tgtFps;
int main() {
    if (!glfwInit()) return 1;

    GLFWmonitor* fullscreen_monitor = NULL; // Windowed
    GLFWwindow* share = NULL;
    GLFWwindow* window = glfwCreateWindow(800, 800, "Cube", fullscreen_monitor, share);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    glewInit();

    struct context context;
    context.window = window;
    initialize(&context);
    double last = 0;

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        if(now - last < secPerFrame) {
            double tgt = last + secPerFrame-.01;
            sleep(tgt - now);
            continue;
        }
        last = now;

        render(&context);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

const unsigned int triangles = 6 * 2;   // Number of triangles rendered

const unsigned int verticies_index = 0;
const unsigned int colors_index = 1;

void initialize(struct context* context) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {
        // Front face
        0.5,  0.5,  0.5,
        -0.5,  0.5,  0.5,
        -0.5, -0.5,  0.5,
        0.5, -0.5,  0.5,

        // Back face
        0.5,  0.5, -0.5,
        -0.5,  0.5, -0.5,
        -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5,
    };

    float vertex_colors[] = {
        1.0, 0.4, 0.6,
        1.0, 0.9, 0.2,
        0.7, 0.3, 0.8,
        0.5, 0.3, 1.0,

        0.2, 0.6, 1.0,
        0.6, 1.0, 0.4,
        0.6, 0.8, 0.8,
        0.4, 0.8, 0.8,
    };

    std::vector<unsigned short> triangle_indices = {
        // Front
        0, 1, 2,
        2, 3, 0,

        // Right
        0, 3, 7,
        7, 4, 0,

        // Bottom
        2, 6, 7,
        7, 3, 2,

        // Left
        1, 5, 6,
        6, 2, 1,

        // Back
        4, 7, 6,
        6, 5, 4,

        // Top
        5, 1, 0,
        0, 4, 5,
    };

    glGenVertexArrays(1, &context->vao);
    glBindVertexArray(context->vao);
/*
    unsigned int triangles_ebo;
    glGenBuffers(1, &triangles_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangles_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof triangle_indices, triangle_indices, GL_STATIC_DRAW);
*/
    context->ebo.bind();
    context->ebo.setData(triangle_indices, GL_STATIC_DRAW);
    unsigned int verticies_vbo;
    glGenBuffers(1, &verticies_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, verticies_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(verticies_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(verticies_index);

    unsigned int colors_vbo;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex_colors, vertex_colors, GL_STATIC_DRAW);

    glVertexAttribPointer(colors_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(colors_index);

    // Unbind to prevent accidental modification
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    VertexShader vert(std::filesystem::path("vertex.glsl"));
    FragmentShader frag(std::filesystem::path("fragment.glsl"));
    context->shaderProgram = new ShaderProgram(vert, frag);
}

// Based on https://antongerdelan.net/opengl/glcontext2.html
void update_fps(struct context* context) {
    // All times in seconds

    static double last_update_time = 0;
    static int frames_since_last_update = 0;

    double now = glfwGetTime();
    frames_since_last_update++;

    if (now - last_update_time > 0.25) {
        double fps = frames_since_last_update / (now - last_update_time);

        char title_buffer[128];
        sprintf(title_buffer, "Cube (%.1f FPS)", fps);
        glfwSetWindowTitle(context->window, title_buffer);

        last_update_time = now;
        frames_since_last_update = 0;
    }
}

float animation(float duration) {
    unsigned long int ms_time = glfwGetTime() * 1000;
    unsigned int ms_duration = duration * 1000;
    float ms_position = ms_time % ms_duration;

    return ms_position / ms_duration;
}

void render(struct context* context) {
    update_fps(context);

    // Clear
    {
        glClearColor(0.1, 0.12, 0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    context->shaderProgram->use();
    context->shaderProgram->set("ticks", (float)glfwGetTime());
    glBindVertexArray(context->vao);
    glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_SHORT, NULL);
}
