R"zzz(#version 330 core
layout (lines) in;
layout (line_strip, max_vertices = 2) out;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec4 light_position;
out vec4 world_position;
void main() {
	int n = 0;
	for (n = 0; n < gl_in.length(); n++) {
		world_position = gl_in[n].gl_Position;
		gl_Position = projection * view * model * gl_in[n].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
)zzz"
