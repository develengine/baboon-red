#version 460 core

uniform vec2 u_points[2];

void main()
{
    gl_Position = vec4(u_points[gl_VertexID], 0.0, 1.0);
}

