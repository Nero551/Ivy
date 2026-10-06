#version 450 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

layout (std140, binding = 0) uniform Global {
    mat4 VIEW_MATRIX;
    mat4 PROJECTION_MATRIX;
    float TIME;
    vec3 VIEW_POSITION;
};

in VS_OUT {
    vec4 Position;
    vec4 WorldPosition;
    vec4 Color;
    vec3 Normal;
    vec2 UV;
    vec3 UVW;
} GSIn[];

out VS_OUT {
    vec4 Position;
    vec4 WorldPosition;
    vec4 Color;
    vec3 Normal;
    vec2 UV;
    vec3 UVW;
} GSOut;

uniform mat4 MODEL_MATRIX;
uniform mat3 NORMAL_MATRIX;



void GenerateLine(int index)
{
    gl_Position = PROJECTION_MATRIX * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = PROJECTION_MATRIX * (gl_in[index].gl_Position +
    vec4(GSIn[index].Normal, 0.0) * 0.4);
    EmitVertex();
    EndPrimitive();

}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}

out vec3 vertex_color;

void main()
{
    float length = 1.0f;
    vec3 normal = GSIn[0].Normal;
    mat4 mvp = PROJECTION_MATRIX * VIEW_MATRIX * MODEL_MATRIX;

    vertex_color = abs(normal);

    vec4 v0 = gl_in[0].gl_Position;
    gl_Position = mvp * v0;
    EmitVertex();

    vec4 v1 = v0 + vec4(normal * length, 0.0);
    gl_Position = mvp * v1;
    EmitVertex();

    EndPrimitive();
}
