#version 330

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform vec2 resolution;

void main()
{
    fragColor = texture(tex, gl_FragCoord.xy/resolution );
}