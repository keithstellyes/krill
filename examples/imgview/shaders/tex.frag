#version 330 core
out vec4 FragColor;
uniform sampler2D tex;

in vec2 texCoord;
void main()
{
    //FragColor = vec4(1.0f, 0.5f, 1.0f, 1.0f);
    FragColor = texture(tex, texCoord);
}
