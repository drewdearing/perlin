R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform float tree_pos_x;
uniform float tree_pos_y;
uniform float tree_pos_z;
uniform vec4 tree_normal;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;
void main() {
	gl_Position = vertex_position;
	gl_Position.x += tree_pos_x;
	gl_Position.y += tree_pos_y;
	gl_Position.z += tree_pos_z;
	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
