#include "procedure_geometry.h"
#include "config.h"

void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces)
{
	floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMax, 1.0f));
	floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMax, 1.0f));
	floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMin, 1.0f));
	floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMin, 1.0f));
	floor_faces.push_back(glm::uvec3(0, 1, 2));
	floor_faces.push_back(glm::uvec3(2, 3, 0));
}

// FIXME: create cylinders and lines for the bones
// Hints: Generate a lattice in [-0.5, 0, 0] x [0.5, 1, 0] We wrap this
// around in the vertex shader to produce a very smooth cylinder.  We only
// need to send a small number of points.  Controlling the grid size gives a
// nice wireframe.
void create_skel(Mesh& mesh, std::vector<glm::vec4>& skel_vertices,
				 std::vector<glm::uvec2>& skel_lines)
{
	int index = 0;
	Bone * temp_bone = mesh.skeleton.getBone(index);
	while(temp_bone != NULL){
		glm::vec2 line;
		glm::vec4 fep = temp_bone->firstEndPoint();
		glm::vec4 sep = temp_bone->secondEndPoint();
		fep.y += mesh.height_offset;
		sep.y += mesh.height_offset;
		skel_vertices.push_back(fep);
		line[0] = skel_vertices.size()-1;
		skel_vertices.push_back(sep);
		line[1] = skel_vertices.size()-1;
		skel_lines.push_back(line);
		//std::cout<< "Bone "<<index<<": "<<glm::to_string(skel_vertices.at(line[0]))<<" -> "<<glm::to_string(skel_vertices.at(line[1]))<<std::endl;
		index++;
		temp_bone = mesh.skeleton.getBone(index);
	}
}
