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
		skel_vertices.push_back(fep);
		line[0] = skel_vertices.size()-1;
		skel_vertices.push_back(sep);
		line[1] = skel_vertices.size()-1;
		skel_lines.push_back(line);
		index++;
		temp_bone = mesh.skeleton.getBone(index);
	}
}

void create_cyl(std::vector<glm::uvec2>& cyl_lines){
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

void create_tree(std::vector<glm::vec4>& vertices, std::vector<glm::uvec3>& faces){
	vertices.push_back(glm::vec4(-1, 0, 1, 1));
	vertices.push_back(glm::vec4(-1, 0, -1, 1));
	vertices.push_back(glm::vec4(1, 0, -1, 1));
	vertices.push_back(glm::vec4(1, 0, -1, 1));
	vertices.push_back(glm::vec4(-1, 2, 1, 1));
	vertices.push_back(glm::vec4(-1, 2, -1, 1));
	vertices.push_back(glm::vec4(1, 2, -1, 1));
	vertices.push_back(glm::vec4(1, 2, -1, 1));

	faces.push_back(glm::uvec3(0, 3, 2));
	faces.push_back(glm::uvec3(0, 2, 1));

	faces.push_back(glm::uvec3(3, 0, 4));
	faces.push_back(glm::uvec3(3, 4, 7));

	faces.push_back(glm::uvec3(2, 3, 7));
	faces.push_back(glm::uvec3(2, 7, 6));

	faces.push_back(glm::uvec3(1, 2, 6));
	faces.push_back(glm::uvec3(1, 6, 5));

	faces.push_back(glm::uvec3(0, 1, 5));
	faces.push_back(glm::uvec3(0, 5, 4));

	faces.push_back(glm::uvec3(7, 4, 5));
	faces.push_back(glm::uvec3(7, 5, 6));

}
