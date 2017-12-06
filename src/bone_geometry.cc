#include "config.h"
#include "bone_geometry.h"
#include "OBJ_Loader.h"
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

void Mesh::loadobj(const std::string& fn)
{
	objl::Loader loader;
	bool loadout = loader.LoadFile(fn);
	if(loadout){
		for (int i = 0; i < loader.LoadedMeshes.size(); i++){
			unsigned vertex_start = vertices.size();
			objl::Mesh curMesh = loader.LoadedMeshes[i];
			for (int j = 0; j < curMesh.Vertices.size(); j++){
				glm::vec4 v = glm::vec4(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z, 1);
				glm::vec4 n = glm::vec4(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z, 1);
				glm::vec2 uv = glm::vec2(curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y);
				vertices.push_back(v);
				vertex_normals.push_back(n);
				uv_coordinates.push_back(uv);
			}
			unsigned face_start = faces.size();
			for (int j = 0; j < curMesh.Indices.size(); j += 3){
				glm::uvec3 t = glm::uvec3(vertex_start + curMesh.Indices[j], vertex_start + curMesh.Indices[j+1], vertex_start + curMesh.Indices[j+2]);
				faces.push_back(t);
			}
			Material m;
			m.ambient = glm::vec4(curMesh.MeshMaterial.Ka.X, curMesh.MeshMaterial.Ka.Y, curMesh.MeshMaterial.Ka.Z, 1);
			m.diffuse = glm::vec4(curMesh.MeshMaterial.Kd.X, curMesh.MeshMaterial.Kd.Y, curMesh.MeshMaterial.Kd.Z, 1);
			m.specular = glm::vec4(curMesh.MeshMaterial.Ks.X, curMesh.MeshMaterial.Ks.Y, curMesh.MeshMaterial.Ks.Z, 1);
			m.shininess = curMesh.MeshMaterial.Ns;
			m.offset = face_start;
			m.nfaces = faces.size() - face_start;
			materials.push_back(m);
		}
		computeBounds();
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
			new_vertices.at(vid) += weight * newWorldCoords;
		}
	}

	animated_vertices = new_vertices;
}

glm::vec3 Mesh::getCenter(){
	return 0.7f * glm::vec3(bounds.min + bounds.max);
}


void Mesh::computeBounds()
{
	bounds.min = glm::vec3(std::numeric_limits<float>::max());
	bounds.max = glm::vec3(-std::numeric_limits<float>::max());
	for (const auto& vert : vertices) {
		bounds.min = glm::min(glm::vec3(vert), bounds.min);
		bounds.max = glm::max(glm::vec3(vert), bounds.max);
	}
	std::cout<<"BOUND MIN:"<<glm::to_string(bounds.min)<<std::endl;
	std::cout<<"BOUND MAX:"<<glm::to_string(bounds.max)<<std::endl;
}

