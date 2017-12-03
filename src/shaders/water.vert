R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform float min_height;
uniform float max_height;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;
void main() {
	gl_Position = vertex_position;
	float water_level = min_height + 0.1 * (max_height - min_height);
	gl_Position.y = water_level + gl_Position.y;
	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
