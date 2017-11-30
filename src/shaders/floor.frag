R"zzz(
#version 330 core
in vec4 face_normal;
in vec4 vertex_normal;
in float vertex_moisture;
in vec4 light_direction;
in vec4 world_position;
uniform float min_height;
uniform float max_height;
out vec4 fragment_color;
void main() {
	vec4 pos = world_position;
	float distance = max_height - min_height;
	float e = (pos.y - (min_height))/(distance);
	float m = vertex_moisture;
	vec3 color;
	float dot_nl = dot(normalize(light_direction), normalize(vertex_normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);
	vec3 min_color;
	vec3 max_color;
	float t;
  
	vec3 ocean = vec3(68, 68, 122);
	vec3 beach = vec3(160, 144, 119);

	vec3 scorched = vec3(85, 85, 85);
	vec3 bare = vec3(136, 136, 136);
	vec3 tundra = vec3(187, 187, 170);
  vec3 snow = vec3(221, 221, 228);

	vec3 temperate_desert = vec3(201, 210, 155);
	vec3 shrubland = vec3(136, 153, 119);
  vec3 taiga = vec3(153, 170, 119);

  vec3 grassland = vec3(136, 170, 85);
  vec3 temperate_deciduous_forest = vec3(103, 148, 89);
  vec3 temperate_rain_forest = vec3(68, 136, 85);

  vec3 subtropical_desert = vec3(210, 185, 139);
  vec3 tropical_seasonal_forest = vec3(85, 153, 68);
  vec3 tropical_rain_forest = vec3(51, 119, 85);

  if (e < 0.1)
    color = ocean;
  else if (e < 0.12)
    color = beach;
  else if (e > 0.8) {
    if (m < 0.1)
      color = scorched;
    else if (m < 0.2)
      color = bare;
    else if (m < 0.5)
      color = tundra;
    else
      color = snow;
  }
  else if (e > 0.6) {
    if (m < 0.33)
      color = temperate_desert;
    else if (m < 0.66)
      color = shrubland;
    else
      color = taiga;
  }
  else if (e > 0.3) {
    if (m < 0.16)
      color = temperate_desert;
    else if (m < 0.50)
      color = grassland;
    else if (m < 0.83)
      color = temperate_deciduous_forest;
    else
      color = temperate_rain_forest;
  }
  else if (m < 0.16)
    color = subtropical_desert;
  else if (m < 0.33)
    color = grassland;
  else if (m < 0.66)
    color = tropical_seasonal_forest;
  else
    color = tropical_rain_forest;

  color /= 255.0;

	fragment_color = vec4(dot_nl * color, 1.0);
}
)zzz"