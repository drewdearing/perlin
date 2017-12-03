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
  float dot_nl = dot(normalize(light_direction), normalize(vertex_normal));
  dot_nl = clamp(dot_nl, 0.0, 1.0);
  float t1;
  float t2;
  vec3 min_color;
  vec3 max_color;
  
  color = vec3(68, 68, 122);

  color /= 255.0f;

  fragment_color = vec4(dot_nl * color, 1.0);
}
)zzz"