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
#include "config.h"

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
	unsigned id;
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
	Bone(Joint* start, Joint* end, unsigned i, Bone * parent){
		setID(i);
		setParent(parent);
		setEndpoints(start, end);
	}

	void setID(unsigned i){
		id = i;
	}

	void setParent(Bone* p){
		parent = p;
	}

	void setChild(Bone* c){
		children.push_back(c);
	}

	unsigned getID(){
		return id;
	}

	Joint* getStartPoint(){
		return startPoint;
	}

	Joint* getEndPoint(){
		return endPoint;
	}

	glm::vec3 getNormal(){
		return normal;
	}

	glm::vec3 getBinormal(){
		return binormal;
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
		glm::vec4 origin;
		if(parent == NULL){
			origin = glm::vec4(startPoint->offset, 1);
		}
		else{
			origin = parent->secondEndPoint();
		}
		return origin;
	}

	glm::vec4 secondEndPoint(){
		glm::vec4 end;
		if(parent == NULL){
			end = glm::vec4(startPoint->offset+endPoint->offset, 1);
		}
		else{
			end = parent->secondEndPoint() + glm::vec4(tangent, 0);
		}
		return end;
	}

	std::vector<glm::vec4> cylVertices(){

		std::vector<glm::vec4> cyl_vertices;
		glm::vec3 n = (kCylinderRadius/glm::length(normal)) * normal;
		glm::vec3 b = (kCylinderRadius/glm::length(binormal)) * binormal;

		glm::vec4 fep = firstEndPoint();
		glm::vec4 sep = secondEndPoint();

		cyl_vertices.push_back(glm::vec4(n, 0) + fep);
		cyl_vertices.push_back(glm::vec4(-n, 0) + fep);
		cyl_vertices.push_back(glm::vec4(b, 0) + fep);
		cyl_vertices.push_back(glm::vec4(-b, 0) + fep);

		cyl_vertices.push_back(glm::vec4(n, 0) + sep);
		cyl_vertices.push_back(glm::vec4(-n, 0) + sep);
		cyl_vertices.push_back(glm::vec4(b, 0) + sep);
		cyl_vertices.push_back(glm::vec4(-b, 0) + sep);

		return cyl_vertices;
	}

	bool intersect(glm::vec3 ray_world, glm::vec3 eye_, float& t){
		bool found = false;
		glm::vec3 normals[4];
		glm::vec3 points[4][4];

		glm::vec3 n = (kCylinderRadius/glm::length(normal)) * normal;
		glm::vec3 b = (kCylinderRadius/glm::length(binormal)) * binormal;

		normals[0] = ((kCylinderRadius/sqrtf(2.0f))/glm::length(n+b))*(n+b);
		normals[1] = ((kCylinderRadius/sqrtf(2.0f))/glm::length(n-b))*(n-b);
		normals[2] = ((kCylinderRadius/sqrtf(2.0f))/glm::length(-n+b))*(-n-b);
		normals[3] = ((kCylinderRadius/sqrtf(2.0f))/glm::length(-n-b))*(-n+b);

		std::vector<glm::vec4> cyl_vertices = cylVertices();

		points[0][0] = glm::vec3(cyl_vertices.at(0));
		points[0][1] = glm::vec3(cyl_vertices.at(2));
		points[0][2] = glm::vec3(cyl_vertices.at(4));
		points[0][3] = glm::vec3(cyl_vertices.at(6));

		points[1][0] = glm::vec3(cyl_vertices.at(0));
		points[1][1] = glm::vec3(cyl_vertices.at(3));
		points[1][2] = glm::vec3(cyl_vertices.at(4));
		points[1][3] = glm::vec3(cyl_vertices.at(7));

		points[2][0] = glm::vec3(cyl_vertices.at(1));
		points[2][1] = glm::vec3(cyl_vertices.at(2));
		points[2][2] = glm::vec3(cyl_vertices.at(5));
		points[2][3] = glm::vec3(cyl_vertices.at(6));

		points[3][0] = glm::vec3(cyl_vertices.at(1));
		points[3][1] = glm::vec3(cyl_vertices.at(3));
		points[3][2] = glm::vec3(cyl_vertices.at(5));
		points[3][3] = glm::vec3(cyl_vertices.at(7));

		for(int i = 0; i < 4; i++){
			if(glm::dot(ray_world, normals[i]) != 0){
				float d = -glm::dot(points[i][0], normals[i]);
				float temp_t = -(glm::dot(eye_, normals[i]) + d)/glm::dot(ray_world, normals[i]);
				glm::vec3 q = eye_ + temp_t * ray_world;
				if(    glm::length(q-points[i][0]) <= glm::length(points[i][0]-points[i][3])
					&& glm::length(q-points[i][3]) <= glm::length(points[i][0]-points[i][3])
					&& glm::length(q-points[i][1]) <= glm::length(points[i][1]-points[i][2])
					&& glm::length(q-points[i][2]) <= glm::length(points[i][1]-points[i][2]))
				{
					found = true;
					t = temp_t;
				}
				if(found)
					break;
			}
		}

		return found;
	}
};


class Skeleton {
private:
	std::vector<Joint *> roots;
	std::vector<Bone *> bones;
public:
	Skeleton(){};

	Joint* parentIsRoot(int parent_id){
		Joint* root = NULL;
		for(unsigned i = 0; i < roots.size(); i++){
			if(roots.at(i)->id == parent_id){
				root = roots.at(i);
				break;
			}
		}
		return root;
	}

	unsigned numBones () const{
		return bones.size();
	}

	bool addJoint(Joint* j, int parent_id){
		bool found = false;
		
		if(parent_id == -1){
			roots.push_back(j);
			found = true;
		}
		else{
			Joint * parentRoot = parentIsRoot(parent_id);
			if(parentRoot != NULL){
				Bone* b = new Bone(parentRoot, j, bones.size(), NULL);
				bones.push_back(b);
				found = true;
			}
			else{
				int size = bones.size();
				for(int i = 0; i < size; i++){
					Bone * current = bones.at(i);
					if(current->getEndPoint()->id == parent_id){
						Bone* b = new Bone(current->getEndPoint(), j, bones.size(), current);
						current->setChild(b);
						bones.push_back(b);
						found = true;
						break;
					}
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
		std::cout<<"Skeleton"<<std::endl;
		std::cout<<"Root Joints: "<<roots.size()<<std::endl;
		std::cout<<"Bones: "<<bones.size()<<std::endl;
		int size = bones.size();
		for(int i = 0; i < size; i++){
			Bone * current = bones.at(i);
			std::cout<<"\nBone "<<i<<": "<<current->getStartPoint()->id<<"->"<<current->getEndPoint()->id<<std::endl;
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
		return skeleton.numBones();
		// FIXME: return number of bones in skeleton
	}
	glm::vec3 getCenter() const { return 0.5f * glm::vec3(bounds.min + bounds.max); }
private:
	void computeBounds();
	void computeNormals();
};

#endif
