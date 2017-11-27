R"zzz(
#version 330 core
in vec4 face_normal;
in vec4 vertex_normal;
in vec4 light_direction;
in vec4 world_position;
out vec4 fragment_color;
void main() {
	vec4 pos = world_position;
	vec3 color;
	float dot_nl = dot(normalize(light_direction), normalize(vertex_normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);

	color = vec3(0.26, 0.96, 0.27);

	fragment_color = vec4(dot_nl * color, 1.0);
}
)zzz"