R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform float height_offset;
uniform vec3 look_dir;
uniform float scale;
uniform vec3 tilt_normal;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;

vec4 rotate(vec3 default_v, vec3 new_v, vec3 axis, vec3 v){
	if(new_v.x != default_v.x || new_v.y != default_v.y || new_v.z != default_v.z){
		float angle = acos(dot(default_v, new_v));
		angle = angle*sign(dot(axis, cross(default_v, new_v)));
		
		vec4 q;
		float half_angle = angle/2;
		q.x = axis.x * sin(half_angle);
		q.y = axis.y * sin(half_angle);
		q.z = axis.z * sin(half_angle);
		q.w = cos(half_angle);

		return vec4(v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v), 1);
	}
	else
		return vec4(v, 1);
}

void main() {
	vec3 v = vertex_position.xyz * scale;
	vec3 default_look = vec3(0.0, 0.0, -1.0);
	vec3 new_look = normalize(vec3(look_dir.x, 0.0, look_dir.z));
	vec3 default_tilt = vec3(0.0, 1.0, 0.0);
	vec3 new_norm = tilt_normal;
	float tilt_height = new_norm.y;
	new_norm.y = 0;
	vec3 new_tilt = vec3(0.0, tilt_height, -length(new_norm));
	
	gl_Position = rotate(default_tilt, new_tilt, vec3(1.0, 0.0, 0.0), v);

	gl_Position = rotate(default_look, new_look, vec3(0.0, 1.0, 0.0), gl_Position.xyz);

	gl_Position.y += height_offset;

	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
