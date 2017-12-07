#ifndef PROCEDURE_GEOMETRY_H
#define PROCEDURE_GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>
#include "bone_geometry.h"
#include <glm/gtx/string_cast.hpp>

class LineMesh;

void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces);
// FIXME: Add functions to generate the bone mesh.
void create_skel(Mesh& mesh, std::vector<glm::vec4>& skel_vertices, std::vector<glm::uvec2>& skel_lines);

void create_cyl(std::vector<glm::uvec2>& cyl_lines);

void create_tree(std::vector<glm::vec4>& vertices, std::vector<glm::uvec3>& faces);
#endif
