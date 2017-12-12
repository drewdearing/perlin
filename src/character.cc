#include <GL/glew.h>
#include "character.h"
#include "PerlinMap.h"
#include "config.h"

Mesh * Character::getMesh(){
	return &mesh;
}

glm::vec3 Character::getCenter(){
	float height = mesh.getCenter().y;
	glm::vec3 center = (height * normal) * scale;
	center.y += height_offset;
	return center;
}

void Character::setArmRotation(float r){
	arm_rotation = r;
}

void Character::setScale(float s){
	scale = s;
}

void Character::setBones(bool b){
	has_bones = b;
}

void Character::setFile(std::string f){
	file = f;
}

void Character::updateNormal(glm::vec4 n){
	normal = glm::vec3(n);
	binormal = glm::normalize(glm::vec3(binormal.x, 0.0, binormal.z));;
	tangent = glm::normalize(glm::cross(normal, binormal));
	binormal = glm::cross(normal, tangent);
}

void Character::updateLook(glm::vec3 l){
	binormal = glm::normalize(glm::vec3(binormal.x, 0.0, binormal.z));;
	tangent = glm::normalize(glm::cross(normal, binormal));
	binormal = glm::cross(normal, tangent);
}

void Character::build(){
	mesh.loadpmd(file);
	std::cout << "Loaded object  with  " << mesh.vertices.size()
		<< " vertices and " << mesh.faces.size() << " faces.\n";

	glm::vec4 mesh_center = glm::vec4(0.0f);
	for (size_t i = 0; i < mesh.vertices.size(); ++i) {
		mesh_center += mesh.vertices[i];
	}
	mesh_center /= mesh.vertices.size();
	uv_coordinates = mesh.uv_coordinates;
}

void Character::buildObj(){
	mesh.loadobj(file);
	std::cout << "OBJ: Loaded object  with  " << mesh.vertices.size()
		<< " vertices and " << mesh.faces.size() << " faces.\n";

	glm::vec4 mesh_center = glm::vec4(0.0f);
	for (size_t i = 0; i < mesh.vertices.size(); ++i) {
		mesh_center += mesh.vertices[i];
	}
	mesh_center /= mesh.vertices.size();
	uv_coordinates = mesh.uv_coordinates;
}

ShaderUniform Character::model_normal(){
	auto vector3_binder = [](int loc, const void* data) {
		glUniform3fv(loc, 1, (const GLfloat*)data);
	};
	auto tilt_normal_data = [this]() -> const void* {
		return &this->normal;
	};
	return { "model_normal", vector3_binder, tilt_normal_data };
}

ShaderUniform Character::model_height(){
	auto float_binder = [](int loc, const void* data) {
		glUniform1fv(loc, 1, (const GLfloat*)data);
	};
	auto height_offset_data = [this]() -> const void* {
		return &this->height_offset;
	};
	return { "height_offset", float_binder, height_offset_data };
}

ShaderUniform Character::model_scale(){
	auto float_binder = [](int loc, const void* data) {
		glUniform1fv(loc, 1, (const GLfloat*)data);
	};
	auto model_scale_data = [this]() -> const void* {
		return &this->scale;
	};
	return { "scale", float_binder, model_scale_data };
}

void Character::buildPass(
		const std::vector<const char*> shaders, // Order: VS, GS, FS 
		const std::vector<ShaderUniform> uniforms,
   		const std::vector<const char*> output // Order: 0, 1, 2...
	)
{
	object_pass_input.assign(0, "vertex_position", nullptr, mesh.vertices.size(), 4, GL_FLOAT);
	object_pass_input.assign(1, "normal", mesh.vertex_normals.data(), mesh.vertex_normals.size(), 4, GL_FLOAT);
	object_pass_input.assign(2, "uv", uv_coordinates.data(), uv_coordinates.size(), 2, GL_FLOAT);
	object_pass_input.assign_index(mesh.faces.data(), mesh.faces.size(), 3);
	object_pass_input.useMaterials(mesh.materials);
	character_pass = new RenderPass(-1, object_pass_input, shaders, uniforms, output);
}

RenderPass * Character::pass(){
	return character_pass;
}

void Character::setBoneID(unsigned b, int id){
	if(b <= 11 && has_bones){
		switch(b){
			case 0:
				right_arm_upper = mesh.skeleton.getBone(id);
				break;
			case 1:
				right_arm_lower = mesh.skeleton.getBone(id);
				break;
			case 2:
				right_leg_upper = mesh.skeleton.getBone(id);
				break;
			case 3:
				right_leg_lower = mesh.skeleton.getBone(id);
				break;
			case 4:
				left_arm_upper = mesh.skeleton.getBone(id);
				break;
			case 5:
				left_arm_lower = mesh.skeleton.getBone(id);
				break;
			case 6:
				left_leg_upper = mesh.skeleton.getBone(id);
				break;
			case 7:
				left_leg_lower = mesh.skeleton.getBone(id);
				break;
			case 8:
				right_foot = mesh.skeleton.getBone(id);
				break;
			case 9:
				left_foot = mesh.skeleton.getBone(id);
				break;
			case 10:
				root_top = mesh.skeleton.getBone(id);
				break;
			case 11:
				root_bottom = mesh.skeleton.getBone(id);
				break;
		}
	}
}

void Character::walk(){
	if(has_bones){
		current_rotation += rotation_speed;
		if(current_rotation >= walking_speed || current_rotation <= -walking_speed)
			rotation_speed *= -1.0;
		//Arms
		right_arm_upper->rotate(rotation_speed, glm::normalize(right_arm_upper->getBinormal()));
		left_arm_upper->rotate(-rotation_speed, glm::normalize(left_arm_upper->getBinormal()));

		//Legs
		right_leg_upper->rotate(rotation_speed, glm::normalize(right_leg_upper->getBinormal()));
		left_leg_upper->rotate(rotation_speed, glm::normalize(left_leg_upper->getBinormal()));
	}
}

void Character::walk_reverse(){
	if(has_bones){
		current_rotation -= rotation_speed;
		if(current_rotation >= walking_speed || current_rotation <= -walking_speed)
			rotation_speed *= -1.0;
		right_arm_upper->rotate(-rotation_speed, glm::normalize(right_arm_upper->getBinormal()));
		left_arm_upper->rotate(rotation_speed, glm::normalize(left_arm_upper->getBinormal()));
		right_leg_upper->rotate(-rotation_speed, glm::normalize(right_leg_upper->getBinormal()));
		left_leg_upper->rotate(-rotation_speed, glm::normalize(left_leg_upper->getBinormal()));
	}
}

void Character::strafe(bool isRight){
	if(has_bones){
		current_rotation -= rotation_speed;
		if(current_rotation >= (walking_speed) || current_rotation <= -(walking_speed))
			rotation_speed *= -1.0;
		right_leg_upper->rotate(-rotation_speed, glm::normalize(right_leg_upper->getNormal()));
		left_leg_upper->rotate(-rotation_speed, glm::normalize(left_leg_upper->getNormal()));
	}
}

void Character::rest(){
	if(has_bones){
		right_arm_upper->revert();
		left_arm_upper->revert();
		left_leg_upper->revert();
		right_leg_upper->revert();
		right_arm_upper->rotate(-0.7 * arm_rotation, right_arm_upper->getOriginalNormal());
		left_arm_upper->rotate(0.7 * arm_rotation, left_arm_upper->getOriginalNormal());
		current_rotation = 0;
	}
}