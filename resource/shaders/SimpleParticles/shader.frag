#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    //gl_PointCoord — contains the coordinate of a fragment within a point
    //gl_PointCoord.s ranges from 0.0 to 1.0 across the point horizontally from left to right. 
    //gl_PointCoord.t varies from 0.0 to 1.0 vertically from top to bottom
    vec2 coord = gl_PointCoord - vec2(0.5); //(-0.5,-0.5) to (0.5,0.5)
    //length(coord): sqrt(s^2+t^2), 0.707~0~0.707
    //outColor = vec4(fragColor, 0.5 - length(coord)); //alpha = -0.207~0.5~-0.207
    outColor = vec4(fragColor, 1.0 - 5*length(coord)); //alpha = -2.535~1.0~-2.535
}