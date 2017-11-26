R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec4 light_position;
in vec4 vs_light_direction[];
in vec4 vs_camera_direction[];
in vec4 vs_normal[];
in vec2 vs_uv[];
out vec4 face_normal;
out vec4 light_direction;
out vec4 camera_direction;
out vec4 world_position;
out vec4 vertex_normal;
out vec2 uv_coords;
void main() {
	int n = 0;
	vec3 a = gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[1].gl_Position.xyz;
	vec3 c = gl_in[2].gl_Position.xyz;
	vec3 na = vs_normal[0].xyz;
	vec3 nb = vs_normal[1].xyz;
	vec3 nc = vs_normal[2].xyz;
	vec3 u = normalize(b - a);
	vec3 v = normalize(c - a);
	face_normal = normalize(vec4(normalize(cross(u, v)), 0.0));
	for (n = 0; n < gl_in.length(); n++) {
		world_position = gl_in[n].gl_Position;
		float j = (v.x * (world_position.x * u.z - world_position.z * u.x))/(u.z + v.x * v.z * u.x);
		float i = (world_position.z - j * v.z)/u.z;
		uv_coords = vec2(i, j);
		vertex_normal = vec4((1 - uv_coords.x - uv_coords.y) * na + uv_coords.x * nb + uv_coords.y * nc, 0);
		light_direction = normalize(vs_light_direction[n]);
		camera_direction = normalize(vs_camera_direction[n]);
		gl_Position = projection * view * model * gl_in[n].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
)zzz"
