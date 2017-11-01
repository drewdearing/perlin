#ifndef BONE_GEOMETRY_H
#define BONE_GEOMETRY_H

#include <ostream>
#include <iostream>
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
public:
	glm::vec3 offset;
	int parent_id;
	int id;
	Joint(){};
	Joint(int i) : id(i){}
};

class Bone {
private:
	std::vector<Bone*> children;
	Bone* parent = NULL;
	Joint* startPoint;
	Joint* endPoint;
	glm::vec3 tangent;
	glm::vec3 normal;
	glm::vec3 binormal;
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 translation = glm::mat4(1.0f);
	float length;
public:
	Bone(){};
	Bone(Joint* start, Joint* end){
		setEndpoints(start, end);
	}

	void setParent(Bone* p){
		parent = p;
	}

	void setChild(Bone* c){
		children.push_back(c);
	}

	Joint* getStartPoint(){
		return startPoint;
	}

	Joint* getEndPoint(){
		return endPoint;
	}

	void setEndpoints(Joint* start, Joint* end){
		startPoint = start;
		endPoint = end;
		build();
	}

	void build(){
		tangent = endPoint->offset;
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
		else{
			for(int i = 0; i < 3; i++){
				rotation[i][0] = tangent[i];
				rotation[i][1] = binormal[i];
				rotation[i][2] = normal[i];
			}
		}
	}
	
	glm::vec4 firstEndPoint(){
		glm::vec4 origin = translation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Bone * temp = parent;
		while(temp != NULL){
			origin = temp->rotation * origin;
			origin = temp->translation * origin;
			temp = temp->parent;
		}
		return origin;
	}

	glm::vec4 secondEndPoint(){
		glm::vec4 end = translation * rotation * glm::vec4(0.0f, 0.0f, length, 1.0f);
		Bone * temp = parent;
		while(temp != NULL){
			end = temp->rotation * end;
			end = temp->translation * end;
			temp = temp->parent;
		}
		return end;
	}
};


class Skeleton {
private:
	Joint* root = NULL;
	std::vector<Bone *> bones;
public:
	Skeleton(){};
	Skeleton(Joint* r): root(r){};
	void setRoot(Joint* r){
		root = r;
	}

	bool addJoint(Joint* j, int parent_id){
		bool found = false;
		int size = bones.size();

		if(parent_id == 0){
			Bone* b = new Bone(root, j);
			bones.push_back(b);
			found = true;
		}
		else{
			for(int i = 0; i < size; i++){
				Bone * current = bones.at(i);
				if(current->getEndPoint()->id == parent_id){
					Bone* b = new Bone(current->getEndPoint(), j);
					current->setChild(b);
					b->setParent(current);
					bones.push_back(b);
					found = true;
					break;
				}
			}
		}

		return found;
	}

	Bone * getBone(unsigned i){
		if(i >= 0 && i < bones.size())
			return bones.at(i);
		else
			return NULL;
	}

	void printSkeleton(){
		int size = bones.size();
		for(int i = 0; i < size; i++){
			Bone * current = bones.at(i);
			std::cout<<"Bone "<<i<<": "<<current->getStartPoint()->id<<"->"<<current->getEndPoint()->id<<std::endl;
		}
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
