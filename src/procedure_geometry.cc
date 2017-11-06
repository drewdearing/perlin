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
				 std::vector<glm::uvec2>& skel_lines,
				 std::vector<glm::vec4>& cyl_vertices,
				 std::vector<glm::uvec2>& cyl_lines)
{
	int index = 0;
	Bone * temp_bone = mesh.skeleton.getBone(index);
	while(temp_bone != NULL){
		glm::vec2 line;
		skel_vertices.push_back(temp_bone->firstEndPoint());
		line[0] = skel_vertices.size()-1;
		skel_vertices.push_back(temp_bone->secondEndPoint());
		line[1] = skel_vertices.size()-1;
		skel_lines.push_back(line);
		//std::cout<< "Bone "<<index<<": "<<glm::to_string(skel_vertices.at(line[0]))<<" -> "<<glm::to_string(skel_vertices.at(line[1]))<<std::endl;
		index++;
		temp_bone = mesh.skeleton.getBone(index);
	}

	//cyl_vertices = (mesh.skeleton.getBone(0))->cylVertices();

	cyl_lines.push_back(glm::vec2(0, 2));
	cyl_lines.push_back(glm::vec2(0, 3));
	cyl_lines.push_back(glm::vec2(1, 2));
	cyl_lines.push_back(glm::vec2(1, 3));

	cyl_lines.push_back(glm::vec2(4, 6));
	cyl_lines.push_back(glm::vec2(4, 7));
	cyl_lines.push_back(glm::vec2(5, 6));
	cyl_lines.push_back(glm::vec2(5, 7));

	cyl_lines.push_back(glm::vec2(0, 4));
	cyl_lines.push_back(glm::vec2(1, 5));
	cyl_lines.push_back(glm::vec2(2, 6));
	cyl_lines.push_back(glm::vec2(3, 7));
}
