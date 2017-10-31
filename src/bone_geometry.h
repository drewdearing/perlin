#ifndef BONE_GEOMETRY_H
#define BONE_GEOMETRY_H

#include <ostream>
#include <vector>
#include <map>
#include <limits>
#include <glm/glm.hpp>
#include <mmdadapter.h>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h> 

struct BoundingBox {
	BoundingBox()
		: min(glm::vec3(-std::numeric_limits<float>::max())),
		max(glm::vec3(std::numeric_limits<float>::max())) {}
	glm::vec3 min;
	glm::vec3 max;
};

class Joint {
private:
	std::vector<Joint*> children;
	Joint* parent = NULL;
	glm::vec3 tangent;
	glm::vec3 normal;
	glm::vec3 binormal;
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 translation = glm::mat4(1.0f);
	float length;
	int id;
public:
	Joint(){};
	void setParent(Joint* p){
		parent = p;
	}
	void setChild(Joint* c){
		children.push_back(c);
	}
	void setTangent(glm::vec3 o){
		tangent = o;
		glm::vec3 v = glm::vec3(0, 0, 0);
		if(tangent[0] < tangent[1]){
			if(tangent[0] < tangent[2])
				v[0] = 1;
			else
				v[2] = 1;
		}
		else if(tangent[1] < tangent[2])
			v[1] = 1;
		else
			v[2] = 1;

		normal = glm::cross(tangent, v);
		normal /= glm::length(normal);
		binormal = glm::cross(tangent, normal);
		length = glm::length(tangent);
		translation = glm::translate(translation, tangent);
		if(parent != NULL){
			glm::vec3 axis = glm::cross(parent->tangent, tangent);
			float angle = glm::dot(parent->tangent, tangent)/(parent->length*length);
			angle = (M_PI/180)*acos(angle);
			rotation = glm::rotate(rotation, angle, axis);
		}
	}
	void setID(int i){
		id = i;
	}
	int getID(){
		return id;
	}
};

class Skeleton {
private:
	Joint* root = NULL;
	std::vector<Joint *> joints;
public:
	Skeleton(){};
	Skeleton(Joint* r): root(r){};
	void setRoot(Joint* r){
		root = r;
		joints.push_back(r);
	}

	bool addJoint(Joint* j, int parent_id){
		bool found = false;
		int size = joints.size();
		for(int i = 0; i < size; i++){
			if(joints.at(i)->getID() == parent_id){
				joints.at(i)->setChild(j);
				j->setParent(joints.at(i));
				joints.push_back(j);
				found = true;
				break;
			}
		}
		return found;
	}
};

struct Mesh {
	Mesh();
	~Mesh();
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> animated_vertices;
	std::vector<glm::uvec3> faces;
	std::vector<glm::vec4> vertex_normals;
	std::vector<glm::vec4> face_normals;
	std::vector<glm::vec2> uv_coordinates;
	std::vector<Material> materials;
	BoundingBox bounds;
	Skeleton skeleton;

	void loadpmd(const std::string& fn);
	void updateAnimation();
	int getNumberOfBones() const 
	{ 
		return 0;
		// FIXME: return number of bones in skeleton
	}
	glm::vec3 getCenter() const { return 0.5f * glm::vec3(bounds.min + bounds.max); }
private:
	void computeBounds();
	void computeNormals();
};

#endif
