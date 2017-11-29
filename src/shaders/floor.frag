R"zzz(
#version 330 core
in vec4 face_normal;
in vec4 vertex_normal;
in vec4 light_direction;
in vec4 world_position;
uniform float min_height;
uniform float max_height;
out vec4 fragment_color;
void main() {
	vec4 pos = world_position;
	float distance = max_height - min_height;
	float e = (pos.y - (min_height))/(distance);
	vec3 color;
	float dot_nl = dot(normalize(light_direction), normalize(face_normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);

	vec3 min_color;
	vec3 max_color;
	float t;
  
	vec3 water = vec3(68/255.0, 68/255.0, 122/255.0);
	vec3 beach = vec3(148/255.0, 134/255.0, 119/255.0);
	vec3 forest = vec3(51/255.0, 119/255.0, 85/255.0);
	vec3 jungle = vec3(68/255.0, 136/255.0, 85/255.0);
	vec3 savannah = vec3(153/255.0, 170/255.0, 119/255.0);
	vec3 desert = vec3(221/255.0, 221/255.0, 228/255.0);
	vec3 snow = vec3(1.0, 1.0, 1.0);

	if (e < 0.1){
		color = water;
	}
  	else if (e < 0.2){
  		color = beach;
  	}
  	else if (e < 0.3){
  		float t = (e-0.2)/(0.3 - 0.2);
  		min_color = beach;
  		max_color = forest;
  		color.x = min_color.x + (max_color.x - min_color.x) * t;
  		color.y = min_color.y + (max_color.y - min_color.y) * t;
  		color.z = min_color.z + (max_color.z - min_color.z) * t;
  	}
  	else if (e < 0.5){
		float t = (e-0.3)/(0.5 - 0.3);
  		min_color = forest;
  		max_color = jungle;
  		color.x = min_color.x + (max_color.x - min_color.x) * t;
  		color.y = min_color.y + (max_color.y - min_color.y) * t;
  		color.z = min_color.z + (max_color.z - min_color.z) * t;
  	}
  	else if (e < 0.7){
		float t = (e-0.5)/(0.7 - 0.5);
  		min_color = jungle;
  		max_color = savannah;
  		color.x = min_color.x + (max_color.x - min_color.x) * t;
  		color.y = min_color.y + (max_color.y - min_color.y) * t;
  		color.z = min_color.z + (max_color.z - min_color.z) * t;
  	}
  	else if (e < 0.9){
		float t = (e-0.7)/(0.9 - 0.7);
  		min_color = savannah;
  		max_color = snow;
  		color.x = min_color.x + (max_color.x - min_color.x) * t;
  		color.y = min_color.y + (max_color.y - min_color.y) * t;
  		color.z = min_color.z + (max_color.z - min_color.z) * t;
	}
	else{
		color = snow;
	}

	fragment_color = vec4(dot_nl * color, 1.0);
}
)zzz"