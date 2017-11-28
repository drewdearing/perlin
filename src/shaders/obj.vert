R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform float height_offset;
uniform vec3 look_dir;
uniform float scale;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;
void main() {
	gl_Position = vertex_position;
	vec3 default_look = vec3(0.0, 0.0, -1.0);
	vec3 new_look = normalize(vec3(look_dir.x, 0.0, look_dir.z));
	vec3 axis = vec3(0.0, 1.0, 0.0);
	float angle = acos(dot(default_look, new_look));
	angle = angle*sign(dot(axis, cross(default_look, new_look)));
		
	vec4 q;
	float half_angle = angle/2;
	q.x = axis.x * sin(half_angle);
	q.y = axis.y * sin(half_angle);
	q.z = axis.z * sin(half_angle);
	q.w = cos(half_angle);

	vec3 v = vertex_position.xyz * scale;

	gl_Position = vec4(v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v), 1);

	gl_Position.y += height_offset;

	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
