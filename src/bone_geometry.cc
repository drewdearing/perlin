#include "config.h"
#include "bone_geometry.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>

/*
 * For debugging purpose.
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	size_t count = std::min(v.size(), static_cast<size_t>(10));
	for (size_t i = 0; i < count; ++i) os << i << " " << v[i] << "\n";
	os << "size = " << v.size() << "\n";
	return os;
}

std::ostream& operator<<(std::ostream& os, const BoundingBox& bounds)
{
	os << "min = " << bounds.min << " max = " << bounds.max;
	return os;
}



// FIXME: Implement bone animation.


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::loadpmd(const std::string& fn)
{
	MMDReader mr;
	mr.open(fn);
	mr.getMesh(vertices, faces, vertex_normals, uv_coordinates);
	computeBounds();
	mr.getMaterial(materials);

	// FIXME: load skeleton and blend weights from PMD file
	//        also initialize the skeleton as needed
	int joint_id = 0;
	glm::vec3 offset;
	int parent;

	mr.getJointWeights(vst);

	while(mr.getJoint(joint_id, offset, parent)){
		Joint* j = new Joint(joint_id);
		j->offset = offset;
		j->parent_id = parent;

		if(!skeleton.addJoint(j, parent))
			std::cout<<"WARNING: joint "<< joint_id <<" not added to skeleton."<<std::endl;
		
		joint_id++;
	}

	for(unsigned i = 0; i < vst.size(); i++){
		SparseTuple current = vst.at(i);
		Joint * j = skeleton.getJoint(current.jid);
		for(unsigned b = 0; b < j->bones.size(); b++){
			j->bones.at(b)->addWeight(current.vid, current.weight/(j->bones).size());
		}
	}
}

void Mesh::updateAnimation()
{
	std::vector<glm::vec4> new_vertices(vertices.size());

	for(unsigned i = 0; i < skeleton.numBones(); i++){
		Bone * current = skeleton.getBone(i);
		for(unsigned j = 0; j < current->vertex_weights.size(); j++){
			glm::vec2 current_set = current->vertex_weights.at(j);
			int vid = int(current_set[0]);
			float weight = current_set[1];
			glm::vec4 newWorldCoords;

			if(current->isDirty()){
				glm::vec4 localCoords = current->worldToLocal(vertices.at(vid));
				newWorldCoords = current->localToWorld(localCoords);
			}
			else{
				newWorldCoords = vertices.at(vid);
			}
			newWorldCoords.y += height_offset;
			new_vertices.at(vid) += weight * newWorldCoords;
		}
	}

	animated_vertices = new_vertices;
}


void Mesh::computeBounds()
{
	bounds.min = glm::vec3(std::numeric_limits<float>::max());
	bounds.max = glm::vec3(-std::numeric_limits<float>::max());
	for (const auto& vert : vertices) {
		bounds.min = glm::min(glm::vec3(vert), bounds.min);
		bounds.max = glm::max(glm::vec3(vert), bounds.max);
	}
}

