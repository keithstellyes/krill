#version 450

layout(location = 0) in vec3 pos;

out vec3 color;
uniform float ticks;

void main() {
    float r = (sin(pos.x * 2.0) + 1.0) / 2.0;
    float g = (sin(pos.y * 2.0) + 1.0) / 2.0;
    float b = (sin(pos.z * 2.0) + 1.0) / 2.0;
    // 45 deg radians
    //float rotationAngleY = 0.7853981634;
    float rotationAngleY = ticks;
    mat4 rotationMatrixY = mat4(
            cos(rotationAngleY), 0.0, sin(rotationAngleY), 0.0,
            0.0, 1.0, 0.0, 0.0,
            -sin(rotationAngleY), 0.0, cos(rotationAngleY), 0.0,
            0.0, 0.0, 0.0, 1.0
            );

  //  float rotationAngleX = -0.7853981634;
    float rotationAngleX = -ticks;
    mat4 rotationMatrixX = mat4(
            1.0, 0.0, 0.0, 0.0,
            0.0, cos(rotationAngleX), -sin(rotationAngleX), 0.0,
            0.0, sin(rotationAngleX), cos(rotationAngleX), 0.0,
            0.0, 0.0, 0.0, 1.0
            );
    mat4 transform = rotationMatrixX * rotationMatrixY;
    color = vec3(r, g, b);
    gl_Position = vec4(pos, 1.0) * transform;

}
