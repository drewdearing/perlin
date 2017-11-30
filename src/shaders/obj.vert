R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform float height_offset;
uniform vec3 look_dir;
uniform float scale;
uniform vec4 tilt_normal;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;

void main() {
	vec3 v = vertex_position.xyz * scale;
	vec3 default_look = vec3(0.0, 0.0, -1.0);
	vec3 new_look = normalize(vec3(look_dir.x, 0.0, look_dir.z));

	vec3 new_normal = tilt_normal.xyz;
	vec3 new_binormal = new_look;
	vec3 new_tangent = normalize(cross(new_normal, new_binormal));
	new_binormal = cross(new_normal, new_tangent);

	gl_Position = vec4(v, 1);
	vec3 local_coord;
	local_coord[0] = dot(gl_Position.xyz, vec3(-1.0, 0.0, 0.0));
	local_coord[1] = dot(gl_Position.xyz, vec3(0.0, 1.0, 0.0));
	local_coord[2] = dot(gl_Position.xyz, vec3(0.0, 0.0, 1.0));

	gl_Position = vec4(local_coord[0] * new_tangent + local_coord[1] * new_normal + local_coord[2] * new_binormal, 1);

	gl_Position.y += height_offset;

	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
