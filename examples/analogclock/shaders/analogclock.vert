#version 330 core
in vec2 pos;

out vec2 clockTexCoord;

void main() {
    gl_Position = vec4(pos, 1.0, 1.0);

    vec2 baseTexCoord = vec2((pos.x+1.0) / 2.0, (pos.y+1.0)/2.0);

    // background is never rotated, so just send it the plin texture
    clockTexCoord = baseTexCoord;
}
