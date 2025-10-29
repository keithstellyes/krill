#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertex_color;

uniform float ticks;

out vec3 color;

void main() {
    float rotationAngleY = ticks;
    mat4 rotationMatrixY = mat4(
            cos(rotationAngleY), 0.0, sin(rotationAngleY), 0.0,
            0.0, 1.0, 0.0, 0.0,
            -sin(rotationAngleY), 0.0, cos(rotationAngleY), 0.0,
            0.0, 0.0, 0.0, 1.0
            );

    float rotationAngleX = ticks/2.0;
    mat4 rotationMatrixX = mat4(
            1.0, 0.0, 0.0, 0.0,
            0.0, cos(rotationAngleX), -sin(rotationAngleX), 0.0,
            0.0, sin(rotationAngleX), cos(rotationAngleX), 0.0,
            0.0, 0.0, 0.0, 1.0
            );
    mat4 transform = rotationMatrixX * rotationMatrixY;
    gl_Position = vec4(pos, 1.0) * transform;
    color = vertex_color;
}
