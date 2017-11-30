#ifndef CHARACTER_H
#define CHARACTER_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <mmdadapter.h>

#include "bone_geometry.h"
#include "procedure_geometry.h"
#include "render_pass.h"
#include "config.h"
#include "gui.h"

using namespace std;

class Character {
private:

	vector<glm::vec4> skel_vertices;
	vector<glm::uvec2> skel_lines;
	vector<glm::vec4> cyl_vertices;
	vector<glm::uvec2> cyl_lines;
	vector<glm::vec4> norm_vertices;
	vector<glm::uvec2> norm_lines;
	vector<glm::vec4> binorm_vertices;
	vector<glm::uvec2> binorm_lines;

	Bone* right_arm_upper;
	Bone* right_arm_lower;
	Bone* right_leg_upper;
	Bone* right_leg_lower;
	Bone* left_arm_upper;
	Bone* left_arm_lower;
	Bone* left_leg_upper;
	Bone* left_leg_lower;
	Bone* right_foot;
	Bone* left_foot;
	Bone* root_top;
	Bone* root_bottom;

	string model;


	//SHADERS//
	const char* vertex_shader =
	#include "shaders/default.vert"
	;

	const char* obj_vertex_shader =
	#include "shaders/obj.vert"
	;

	const char* floor_vertex_shader =
	#include "shaders/floor.vert"
	;

	const char* geometry_shader =
	#include "shaders/default.geom"
	;

	const char* floor_geometry_shader =
	#include "shaders/floor.geom"
	;

	const char* line_geometry_shader =
	#include "shaders/line.geom"
	;

	const char* fragment_shader =
	#include "shaders/default.frag"
	;

	const char* floor_fragment_shader =
	#include "shaders/floor.frag"
	;

	const char* line_fragment_shader =
	#include "shaders/line.frag"
	;

	const char* cyl_fragment_shader =
	#include "shaders/cyl.frag"
	;

	const char* norm_fragment_shader =
	#include "shaders/norm.frag"
	;

	const char* binorm_fragment_shader =
	#include "shaders/binorm.frag"
	;

public:
	Character();
	~Character();

	Mesh* mesh;
	GUI* gui_;

	void assignGUI(GUI* gui){
		gui_ = gui;
	}

	/******************
	 * Load the Model *
	 ******************/

	void loadCharacter(const string& waifu, Mesh* mesh_in){
		model = waifu;
		mesh = mesh_in;
		if(waifu == "miku" || waifu == "Miku") 
			mesh->loadpmd("../assets/pmd/Miku_Hatsune.pmd");
		else if(waifu == "meiko" || waifu == "Meiko")
			mesh->loadpmd("../assets/pmd/Meiko_Sakine.pmd");

		cout << "Loaded object  with  " << mesh->vertices.size()
		<< " vertices and " << mesh->faces.size() << " faces.\n";

		gui_->assignMesh(&(*mesh));

		create_skel(*mesh, skel_vertices, skel_lines);
		create_cyl(cyl_lines);
		norm_lines.push_back(glm::uvec2(0, 1));
		binorm_lines.push_back(glm::uvec2(0, 1));

		loadData();		

	}


	void loadData(){
		/*****************************************************/
		/***********Create Values like from Main.cc***********/
		/*****************************************************/
		glm::vec4 light_position = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);
		MatrixPointers mats;


		Mesh mesh_ = *mesh;
		GUI gui = *gui_;
		
		auto matrix_binder = [](int loc, const void* data) {
			glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)data);
		};
		auto bone_matrix_binder = [&mesh_](int loc, const void* data) {
			auto nelem = mesh_.getNumberOfBones();
			glUniformMatrix4fv(loc, nelem, GL_FALSE, (const GLfloat*)data);
		};
		auto vector_binder = [](int loc, const void* data) {
			glUniform4fv(loc, 1, (const GLfloat*)data);
		};
		auto vector3_binder = [](int loc, const void* data) {
			glUniform3fv(loc, 1, (const GLfloat*)data);
		};
		auto float_binder = [](int loc, const void* data) {
			glUniform1fv(loc, 1, (const GLfloat*)data);
		};


		auto std_model_data = [&mats]() -> const void* {
			return mats.model;
		};
		auto std_view_data = [&mats]() -> const void* {
			return mats.view;
		};
		auto std_camera_data  = [&gui]() -> const void* {
			return &gui.getCamera()[0];
		};
		auto std_proj_data = [&mats]() -> const void* {
			return mats.projection;
		};
		auto std_light_data = [&light_position]() -> const void* {
			return &light_position[0];
		};
		auto alpha_data  = [&gui]() -> const void* {
			static const float transparet = 0.5; // Alpha constant goes here
			static const float non_transparet = 1.0;
			if (gui.isTransparent())
				return &transparet;
			else
				return &non_transparet; 
		};
		glm::mat4 mesh_model_matrix = glm::mat4(1.0f);
		auto line_mesh_data = [&mats]() -> const void* {
			return mats.model;
		};
		auto cyl_mesh_data = [&mats]() -> const void* {
			return mats.model;
		};
		auto norm_mesh_data = [&mats]() -> const void* {
			return mats.model;
		};
		auto binorm_mesh_data = [&mats]() -> const void* {
			return mats.model;
		};
		auto height_offset_data = [&mesh_]() -> const void* {
			return &mesh_.height_offset;
		};
		auto look_direction_data = [&gui]() -> const void* {
			return gui.getLook();
		};
		auto model_scale_data = [&gui]() -> const void* {
			return gui.getScale();
		};

		ShaderUniform std_model = { "model", matrix_binder, std_model_data };
		ShaderUniform height_model = { "height_offset", float_binder, height_offset_data };
		ShaderUniform std_view = { "view", matrix_binder, std_view_data };
		ShaderUniform std_camera = { "camera_position", vector3_binder, std_camera_data };
		ShaderUniform std_proj = { "projection", matrix_binder, std_proj_data };
		ShaderUniform std_light = { "light_position", vector_binder, std_light_data };
		ShaderUniform object_alpha = { "alpha", float_binder, alpha_data };
		ShaderUniform line_mesh = { "line_mesh", bone_matrix_binder, line_mesh_data };
		ShaderUniform cyl_mesh = { "cyl_mesh", bone_matrix_binder, cyl_mesh_data };
		ShaderUniform norm_mesh = { "norm_mesh", bone_matrix_binder, norm_mesh_data };
		ShaderUniform binorm_mesh = { "binorm_mesh", bone_matrix_binder, binorm_mesh_data };
		ShaderUniform look_dir_model = { "look_dir", vector3_binder, look_direction_data };
		ShaderUniform model_scale = { "scale", float_binder, model_scale_data };
	
		std::vector<glm::vec2>& uv_coordinates = mesh->uv_coordinates;
		
		RenderDataInput object_pass_input;
		object_pass_input.assign(0, "vertex_position", nullptr, mesh->vertices.size(), 4, GL_FLOAT);
		object_pass_input.assign(1, "normal", mesh->vertex_normals.data(), mesh->vertex_normals.size(), 4, GL_FLOAT);
		object_pass_input.assign(2, "uv", uv_coordinates.data(), uv_coordinates.size(), 2, GL_FLOAT);
		object_pass_input.assign_index(mesh->faces.data(), mesh->faces.size(), 3);
		object_pass_input.useMaterials(mesh->materials);
		RenderPass object_pass(-1,
				object_pass_input,
				{
				  obj_vertex_shader,
				  geometry_shader,
				  fragment_shader
				},
				{ std_model, std_view, std_proj,
				  std_light,
				  std_camera, object_alpha, height_model, look_dir_model,model_scale },
				{ "fragment_color" }
				);
		RenderDataInput mesh_pass_input;
		mesh_pass_input.assign(0, "vertex_position", skel_vertices.data(), skel_vertices.size(), 4, GL_FLOAT);
		mesh_pass_input.assign_index(skel_lines.data(), skel_lines.size(), 2);
		RenderPass mesh_pass(-1,
				mesh_pass_input,
				{obj_vertex_shader, line_geometry_shader, line_fragment_shader},
				{line_mesh, std_view, std_proj, std_light, std_camera, object_alpha, height_model, look_dir_model},
				{ "fragment_color"}
				);

		RenderDataInput cyl_pass_input;
		cyl_pass_input.assign(0, "vertex_position", cyl_vertices.data(), cyl_vertices.size(), 4, GL_FLOAT);
		cyl_pass_input.assign_index(cyl_lines.data(), cyl_lines.size(), 2);
		RenderPass cyl_pass(-1,
				cyl_pass_input,
				{obj_vertex_shader, line_geometry_shader, cyl_fragment_shader},
				{cyl_mesh, std_view, std_proj, std_light, std_camera, object_alpha, height_model, look_dir_model},
				{ "fragment_color"}
				);

		RenderDataInput norm_pass_input;
		norm_pass_input.assign(0, "vertex_position", norm_vertices.data(), norm_vertices.size(), 4, GL_FLOAT);
		norm_pass_input.assign_index(norm_lines.data(), norm_lines.size(), 2);
		RenderPass norm_pass(-1,
				norm_pass_input,
				{vertex_shader, line_geometry_shader, norm_fragment_shader},
				{norm_mesh, std_view, std_proj, std_light, std_camera, object_alpha},
				{ "fragment_color"}
				);

		RenderDataInput binorm_pass_input;
		binorm_pass_input.assign(0, "vertex_position", binorm_vertices.data(), binorm_vertices.size(), 4, GL_FLOAT);
		binorm_pass_input.assign_index(binorm_lines.data(), binorm_lines.size(), 2);
		RenderPass binorm_pass(-1,
				binorm_pass_input,
				{vertex_shader, line_geometry_shader, binorm_fragment_shader},
				{binorm_mesh, std_view, std_proj, std_light, std_camera, object_alpha},
				{ "fragment_color"}
				);
	}
};

#endif