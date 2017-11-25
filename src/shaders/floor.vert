R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform samplerBuffer texture0;
uniform samplerBuffer texture1;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;
void main() {
	gl_Position = vertex_position;
	vec2 distance[4];
	float displace[4];
	float total;
	float weight[4];

	distance[0][0] = vertex_position[0]/5.0 - floor(vertex_position[0]/5.0);
	distance[0][1] = vertex_position[2]/5.0 - floor(vertex_position[2]/5.0);

	distance[1][0] = vertex_position[0]/5.0 - floor(vertex_position[0]/5.0);
	distance[1][1] = vertex_position[2]/5.0 - ceil(vertex_position[2]/5.0);

	distance[2][0] = vertex_position[0]/5.0 - ceil(vertex_position[0]/5.0);
	distance[2][1] = vertex_position[2]/5.0 - floor(vertex_position[2]/5.0);

	distance[3][0] = vertex_position[0]/5.0 - ceil(vertex_position[0]/5.0);
	distance[3][1] = vertex_position[2]/5.0 - ceil(vertex_position[2]/5.0);

	displace[0] = length(distance[0]);
	displace[1] = length(distance[1]);
	displace[2] = length(distance[2]);
	displace[3] = length(distance[3]);

	total = displace[0] + displace[1] + displace[2] + displace[3];
	weight[0] = displace[3]/total;
	weight[1] = displace[2]/total;
	weight[2] = displace[1]/total;
	weight[3] = displace[0]/total;

	int width = 1000;

	int pos = int(floor(vertex_position[2]/5.0)) * width + int(floor(vertex_position[0]/5.0));
	vec3 n0 = vec3(texelFetch(texture1, pos));
	float e0 = texelFetch(texture0, pos).y;

	pos = int(floor(vertex_position[2]/5.0)) * width + int(ceil(vertex_position[0]/5.0));
	vec3 n1 = vec3(texelFetch(texture1, pos));
	float e1 = texelFetch(texture0, pos).y;

	pos = int(ceil(vertex_position[2]/5.0)) * width + int(floor(vertex_position[0]/5.0));
	vec3 n2 = vec3(texelFetch(texture1, pos));
	float e2 = texelFetch(texture0, pos).y;

	pos = int(ceil(vertex_position[2]/5.0)) * width + int(ceil(vertex_position[0]/5.0));
	vec3 n3 = vec3(texelFetch(texture1, pos));
	float e3 = texelFetch(texture0, pos).y;

	vs_normal = normal;
	gl_Position = vertex_position;
	gl_Position[1] = weight[0] * e0 + weight[1] + e1 + weight[2] * e2 + weight[3] * e3;

	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_uv = uv;
}
)zzz"
