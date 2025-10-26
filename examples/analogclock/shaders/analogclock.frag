#version 330 core
uniform sampler2D clockTexture;
uniform sampler2D minutesHand;
uniform sampler2D hoursHand;

in vec2 clockTexCoord;
// whole 0 <= hours < 12
uniform float hours;
// fractional 0 <= mins < 60
uniform float mins;
uniform float secs;

mat2 rotMatrix(float rads) {
    return mat2(cos(rads), -sin(rads), sin(rads), cos(rads));
}

vec4 rotTex(float rads, vec2 coord, sampler2D tex) {
    vec2 centeredUV = coord - vec2(0.5);
    vec2 rotatedUV = rotMatrix(rads) * centeredUV;
    return texture(tex, rotatedUV + vec2(.5));
}

void main() {

    float hourRotRads = -radians(360.0) * ((hours + (mins * (1/60.0))) / 12);
    float minRotRads = -radians(360.0) * ((mins + (secs * (1/60.0))) / 60);
    vec4 clockPix = texture(clockTexture, clockTexCoord);
    vec4 hoursPix = rotTex(hourRotRads, clockTexCoord, hoursHand);
    vec4 minsPix = rotTex(minRotRads, clockTexCoord, minutesHand);

    gl_FragColor = mix(mix(clockPix, hoursPix, hoursPix.a), minsPix, minsPix.a);
    //gl_FragColor = texture(clockTexture, clockTexCoord) + texture(hoursHand, rotatedUV+vec2(.5));
}
