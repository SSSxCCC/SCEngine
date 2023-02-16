#version 450

layout(push_constant) uniform PVM {
    mat4 m;
} pvm;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = pvm.m * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}
