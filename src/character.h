#ifndef CHARACTER_H
#define CHARACTER_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <mmdadapter.h>
#include <functional>
#include "render_pass.h"
#include "bone_geometry.h"

class PerlinMap;

class Character {
private:
	std::string file;
	Mesh mesh;
	RenderPass * character_pass;
	RenderDataInput object_pass_input;
	PerlinMap * map;
	glm::vec4 mesh_center;
	std::vector<glm::vec2> uv_coordinates;
	
	Bone* right_arm_upper; //0
	Bone* right_arm_lower; //1
	Bone* right_leg_upper; //2
	Bone* right_leg_lower; //3
	Bone* left_arm_upper; //4
	Bone* left_arm_lower; //5
	Bone* left_leg_upper; //6
	Bone* left_leg_lower; //7
	Bone* right_foot; //8
	Bone* left_foot; //9
	Bone* root_top; //10
	Bone* root_bottom; //11

public:
	float scale;
	float height_offset;
	glm::vec3 normal;
	glm::vec3 binormal;
	glm::vec3 tangent;

	Character(){};

	Character(std::string f, PerlinMap * m, float s){
		setFile(f);
		setMap(m);
		setScale(s);
		build();
	}

	Mesh * getMesh();

	glm::vec3 getCenter();

	void setScale(float s);

	void setMap(PerlinMap * m);

	void setFile(std::string f);

	void updateNormal(glm::vec4 n);

	void updateLook(glm::vec3 l);

	void build();

	ShaderUniform tilt_normal();

	ShaderUniform height_model();

	ShaderUniform model_scale();

	void buildPass(
			const std::vector<const char*> shaders, // Order: VS, GS, FS 
			const std::vector<ShaderUniform> uniforms,
	   		const std::vector<const char*> output // Order: 0, 1, 2...
		);

	RenderPass * pass();

	void setBoneID(unsigned b, int id);
};

#endif