#version 330


out vec4 fragColor;
uniform vec2 resolution;

void main() {
    if (gl_FragCoord.y>resolution.y/2) fragColor = vec4(0.4,0.75,1.0,1.0);
    else {
        float linearFactor = gl_FragCoord.y/resolution.y;
        float factor = 0.475*log(1-linearFactor);
        fragColor = vec4(0.1,0.6,0.05,1.0)+vec4(factor,factor,factor,0.0);
    }
}