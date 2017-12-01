R"zzz(
#version 330 core
in vec4 face_normal;
in vec4 vertex_normal;
in float vertex_moisture;
in float vertex_tree;
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
  float t1;
  float t2;
  vec3 min_color;
  vec3 max_color;
  
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

  if (e < 0.1) {
    color = ocean;
  }
  else if (e < 0.12) {
    color = beach;
  }
  else if (e < 0.3) {
    t1 = (e-0.12)/(0.3 - 0.12);
    min_color = beach;
    if (m < 0.16)
      max_color = subtropical_desert;
    else if (m < 0.33) {
      t2 = (m-0.16)/(0.33 - 0.16);
      max_color.x = subtropical_desert.x + (grassland.x - subtropical_desert.x) * t2;
      max_color.y = subtropical_desert.y + (grassland.y - subtropical_desert.y) * t2;
      max_color.z = subtropical_desert.z + (grassland.z - subtropical_desert.z) * t2;
    }
    else if (m < 0.66) {
      t2 = (m-0.33)/(0.66 - 0.33);
      max_color.x = grassland.x + (tropical_seasonal_forest.x - grassland.x) * t2;
      max_color.y = grassland.y + (tropical_seasonal_forest.y - grassland.y) * t2;
      max_color.z = grassland.z + (tropical_seasonal_forest.z - grassland.z) * t2;
    }
    else {
      t2 = (m-0.66)/(1 - 0.66);
      max_color.x = tropical_seasonal_forest.x + (tropical_rain_forest.x - tropical_seasonal_forest.x) * t2;
      max_color.y = tropical_seasonal_forest.y + (tropical_rain_forest.y - tropical_seasonal_forest.y) * t2;
      max_color.z = tropical_seasonal_forest.z + (tropical_rain_forest.z - tropical_seasonal_forest.z) * t2;
    }

    color.x = min_color.x + (max_color.x - min_color.x) * t1;
    color.y = min_color.y + (max_color.y - min_color.y) * t1;
    color.z = min_color.z + (max_color.z - min_color.z) * t1;
  }
  else if (e < 0.6) {
    if (m < 0.16)
      min_color = subtropical_desert;
    else if (m < 0.33) {
      t2 = (m-0.16)/(0.33 - 0.16);
      min_color.x = subtropical_desert.x + (grassland.x - subtropical_desert.x) * t2;
      min_color.y = subtropical_desert.y + (grassland.y - subtropical_desert.y) * t2;
      min_color.z = subtropical_desert.z + (grassland.z - subtropical_desert.z) * t2;
    }
    else if (m < 0.66) {
      t2 = (m-0.33)/(0.66 - 0.33);
      min_color.x = grassland.x + (tropical_seasonal_forest.x - grassland.x) * t2;
      min_color.y = grassland.y + (tropical_seasonal_forest.y - grassland.y) * t2;
      min_color.z = grassland.z + (tropical_seasonal_forest.z - grassland.z) * t2;
    }
    else {
      t2 = (m-0.66)/(1 - 0.66);
      min_color.x = tropical_seasonal_forest.x + (tropical_rain_forest.x - tropical_seasonal_forest.x) * t2;
      min_color.y = tropical_seasonal_forest.y + (tropical_rain_forest.y - tropical_seasonal_forest.y) * t2;
      min_color.z = tropical_seasonal_forest.z + (tropical_rain_forest.z - tropical_seasonal_forest.z) * t2;
    }

    t1 = (e-0.3)/(0.6 - 0.3);
    if (m < 0.16)
      max_color = temperate_desert;
    else if (m < 0.50) {
      t2 = (m-0.16)/(0.5 - 0.16);
      max_color.x = temperate_desert.x + (grassland.x - temperate_desert.x) * t2;
      max_color.y = temperate_desert.y + (grassland.y - temperate_desert.y) * t2;
      max_color.z = temperate_desert.z + (grassland.z - temperate_desert.z) * t2;
    }
    else if (m < 0.83) {
      t2 = (m-0.5)/(0.83 - 0.5);
      max_color.x = grassland.x + (temperate_deciduous_forest.x - grassland.x) * t2;
      max_color.y = grassland.y + (temperate_deciduous_forest.y - grassland.y) * t2;
      max_color.z = grassland.z + (temperate_deciduous_forest.z - grassland.z) * t2;
    }
    else  {
      t2 = (m-0.83)/(1 - 0.83);
      max_color.x = temperate_deciduous_forest.x + (temperate_rain_forest.x - temperate_deciduous_forest.x) * t2;
      max_color.y = temperate_deciduous_forest.y + (temperate_rain_forest.y - temperate_deciduous_forest.y) * t2;
      max_color.z = temperate_deciduous_forest.z + (temperate_rain_forest.z - temperate_deciduous_forest.z) * t2;
    }

    color.x = min_color.x + (max_color.x - min_color.x) * t1;
    color.y = min_color.y + (max_color.y - min_color.y) * t1;
    color.z = min_color.z + (max_color.z - min_color.z) * t1;
  }
  else if (e < 0.8) {
    if (m < 0.16)
      min_color = temperate_desert;
    else if (m < 0.50) {
      t2 = (m-0.16)/(0.5 - 0.16);
      min_color.x = temperate_desert.x + (grassland.x - temperate_desert.x) * t2;
      min_color.y = temperate_desert.y + (grassland.y - temperate_desert.y) * t2;
      min_color.z = temperate_desert.z + (grassland.z - temperate_desert.z) * t2;
    }
    else if (m < 0.83) {
      t2 = (m-0.5)/(0.83 - 0.5);
      min_color.x = grassland.x + (temperate_deciduous_forest.x - grassland.x) * t2;
      min_color.y = grassland.y + (temperate_deciduous_forest.y - grassland.y) * t2;
      min_color.z = grassland.z + (temperate_deciduous_forest.z - grassland.z) * t2;
    }
    else  {
      t2 = (m-0.83)/(1 - 0.83);
      min_color.x = temperate_deciduous_forest.x + (temperate_rain_forest.x - temperate_deciduous_forest.x) * t2;
      min_color.y = temperate_deciduous_forest.y + (temperate_rain_forest.y - temperate_deciduous_forest.y) * t2;
      min_color.z = temperate_deciduous_forest.z + (temperate_rain_forest.z - temperate_deciduous_forest.z) * t2;
    }

    t1 = (e-0.6)/(0.8 - 0.6);
    if (m < 0.33)
      max_color = temperate_desert;
    else if (m < 0.66) {
      t2 = (m-0.33)/(0.66 - 0.33);
      max_color.x = temperate_desert.x + (shrubland.x - temperate_desert.x) * t2;
      max_color.y = temperate_desert.y + (shrubland.y - temperate_desert.y) * t2;
      max_color.z = temperate_desert.z + (shrubland.z - temperate_desert.z) * t2;
    }
    else {
      t2 = (m-0.66)/(1 - 0.66);
      max_color.x = shrubland.x + (taiga.x - shrubland.x) * t2;
      max_color.y = shrubland.y + (taiga.y - shrubland.y) * t2;
      max_color.z = shrubland.z + (taiga.z - shrubland.z) * t2;
    }

    color.x = min_color.x + (max_color.x - min_color.x) * t1;
    color.y = min_color.y + (max_color.y - min_color.y) * t1;
    color.z = min_color.z + (max_color.z - min_color.z) * t1;
  }
  else {
    if (m < 0.1)
      color = scorched;
    else if (m < 0.2) {
      t2 = (m-0.1)/(0.2 - 0.1);
      color.x = scorched.x + (bare.x - scorched.x) * t2;
      color.y = scorched.y + (bare.y - scorched.y) * t2;
      color.z = scorched.z + (bare.z - scorched.z) * t2;
    }
    else if (m < 0.5) {
      t2 = (m-0.2)/(0.5 - 0.2);
      color.x = bare.x + (tundra.x - bare.x) * t2;
      color.y = bare.y + (tundra.y - bare.y) * t2;
      color.z = bare.z + (tundra.z - bare.z) * t2;
    }
    else {
      t2 = (m-0.5)/(1.0 - 0.5);
      color.x = tundra.x + (snow.x - tundra.x) * t2;
      color.y = tundra.y + (snow.y - tundra.y) * t2;
      color.z = tundra.z + (snow.z - tundra.z) * t2;
    }
  }

  color /= 255.0;

  fragment_color = vec4(dot_nl * color, 1.0);
}
)zzz"