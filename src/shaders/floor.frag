R"zzz(
#version 330 core
in vec4 face_normal;
in vec4 vertex_normal;
in vec4 light_direction;
in vec4 world_position;
out vec4 fragment_color;
void main() {
	vec4 pos = world_position;
	float e = (pos.y - (-350.0))/(450.0);
	vec3 color;
	float dot_nl = dot(normalize(light_direction), normalize(vertex_normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);

	if (e < 0.1)
		color = vec3(68/255.0, 68/255.0, 122/255.0);
  	else if (e < 0.2)
  		color = vec3(148/255.0, 134/255.0, 119/255.0);
  	else if (e < 0.3)
		color = vec3(51/255.0, 119/255.0, 85/255.0);
  	else if (e < 0.5)
		color = vec3(68/255.0, 136/255.0, 85/255.0);
  	else if (e < 0.7)
		color = vec3(153/255.0, 170/255.0, 119/255.0);
  	else if (e < 0.9)
		color = vec3(221/255.0, 221/255.0, 228/255.0);

	fragment_color = vec4(dot_nl * color, 1.0);
}
)zzz"