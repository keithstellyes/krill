#version 430
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// examples showed using std430... why?
layout(std430, binding = 0) buffer layoutName
{
    float data[];
};

void main()
{
    data[gl_GlobalInvocationID.x] *= 2;
}
