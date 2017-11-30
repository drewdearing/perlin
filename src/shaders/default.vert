R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform float height_offset;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;
void main() {
	gl_Position = vec4(vertex_position.x, vertex_position.y + height_offset, vertex_position.z, 1);
	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
