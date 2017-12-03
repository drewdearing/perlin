#include <GL/glew.h>
#include <dirent.h>

#include "procedure_geometry.h"
#include "render_pass.h"
#include "config.h"
#include "gui.h"
#include "character.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <debuggl.h>

int window_width = 800, window_height = 600;
const std::string window_title = "Perlin";

const char* vertex_shader =
#include "shaders/default.vert"
;

const char* obj_vertex_shader =
#include "shaders/obj.vert"
;

const char* floor_vertex_shader =
#include "shaders/floor.vert"
;

const char* water_vertex_shader =
#include "shaders/water.vert"
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

const char* water_fragment_shader =
#include "shaders/water.frag"
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

// FIXME: Add more shaders here.

void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);
	CHECK_SUCCESS(ret != nullptr);
	glfwMakeContextCurrent(ret);
	glewExperimental = GL_TRUE;
	CHECK_SUCCESS(glewInit() == GLEW_OK);
	glGetError();  // clear GLEW's error for it
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported:" << version << "\n";

	return ret;
}

int main(int argc, char* argv[])
{
	GLFWwindow *window = init_glefw();

	std::vector<glm::vec4> floor_vertices;
	std::vector<glm::uvec3> floor_faces;
	std::vector<glm::vec4> floor_normals;
	std::vector<float> moisture_values;
	std::vector<float> tree_values;
	std::vector<glm::vec4> water_vertices;
	std::vector<glm::uvec3> water_faces;
	std::vector<glm::vec4> water_normals;

	//Perlin Map and Moisture Map
	PerlinMap floorMap = PerlinMap(1000, 1000, 6, 8.0, -350, 100, 5, 25);
	PerlinMap moistureMap = PerlinMap(1000, 1000, 4, 5.0, 0, 1, 5, 25);
	PerlinMap treeMap = PerlinMap(1000, 1000, 4, 8.0, 0, 1, 5, 25);
	PerlinMap waterMap = PerlinMap(1000, 1000, 4, 8.0, 0, 1, 5, 25);

	floorMap.createFloor(floor_vertices, floor_faces, floor_normals);
	moistureMap.createHeights(moisture_values);
	treeMap.createHeights(tree_values);
	waterMap.createFloor(water_vertices, water_faces, water_normals);

	//Create GUI
	GUI gui(window, &floorMap);

	//Create Character
	std::vector<Character*> characters;

	//Meiko
	Character character = Character("../assets/pmd/Meiko_Sakine.pmd", 0.5f);
	character.setBoneID(0, 32);
	character.setBoneID(4, 14);
	character.setBoneID(2, 53);
	character.setBoneID(6, 56);
	character.setBoneID(10, 0);
	character.setBoneID(11, 5);
	character.setArmRotation(-1);

	//Miku
	Character character2 = Character("../assets/pmd/Miku_Hatsune.pmd", 0.5f);
	character2.setBoneID(0, 18);
	character2.setBoneID(4, 48);
	character2.setBoneID(2, 38);
	character2.setBoneID(6, 68);
	character2.setBoneID(10, 0);
	character2.setBoneID(11, 8);
	character2.setArmRotation(1);

	characters.push_back(&character);
	characters.push_back(&character2);

	Character* current_character = characters[0];

	//GUI object needs the mesh object for bone manipulation.
	gui.assignCharacter(characters[0]);
	gui.assignCharacterList(&characters, &current_character);

	glm::vec4 light_position = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);
	MatrixPointers mats; // Define MatrixPointers here for lambda to capture
	/*
	 * In the following we are going to define several lambda functions to bind Uniforms.
	 * 
	 * Introduction about lambda functions:
	 *      http://en.cppreference.com/w/cpp/language/lambda
	 *      http://www.stroustrup.com/C++11FAQ.html#lambda
	 */
	auto matrix_binder = [](int loc, const void* data) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)data);
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
	/*
	 * These lambda functions below are used to retrieve data
	 */
	auto std_model_data = [&mats]() -> const void* {
		return mats.model;
	}; // This returns point to model matrix
	glm::mat4 floor_model_matrix = glm::mat4(1.0f);
	auto floor_model_data = [&floor_model_matrix]() -> const void* {
		return &floor_model_matrix[0][0];
	}; // This return model matrix for the floor.
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
	auto min_height_map = [&floorMap]() -> const void* {
		return floorMap.getMinHeight();
	};
	auto max_height_map = [&floorMap]() -> const void* {
		return floorMap.getMaxHeight();
	};
	auto look_direction_data = [&gui]() -> const void* {
		return gui.getLook();
	};
	
	//Uniforms
	ShaderUniform std_model = { "model", matrix_binder, std_model_data };
	ShaderUniform floor_model = { "model", matrix_binder, floor_model_data };
	ShaderUniform std_view = { "view", matrix_binder, std_view_data };
	ShaderUniform std_camera = { "camera_position", vector3_binder, std_camera_data };
	ShaderUniform std_proj = { "projection", matrix_binder, std_proj_data };
	ShaderUniform std_light = { "light_position", vector_binder, std_light_data };
	ShaderUniform object_alpha = { "alpha", float_binder, alpha_data };
	ShaderUniform floor_max_height = { "max_height", float_binder, max_height_map };
	ShaderUniform floor_min_height = { "min_height", float_binder, min_height_map };
	ShaderUniform camera_look_dir = { "look_dir", vector3_binder, look_direction_data };

	for(unsigned i = 0; i < characters.size(); i++){
		Character * curr_char = characters[i];
		curr_char->buildPass(
			{ obj_vertex_shader, geometry_shader, fragment_shader },
			{ std_model, std_view, std_proj, std_light, std_camera, object_alpha, curr_char->model_height(),
				curr_char->model_scale(), curr_char->model_normal(), camera_look_dir },
			{ "fragment_color" }
			);
	}

	RenderDataInput floor_pass_input;
	floor_pass_input.assign(0, "vertex_position", floor_vertices.data(), floor_vertices.size(), 4, GL_FLOAT);
	floor_pass_input.assign(1, "normal", floor_normals.data(), floor_normals.size(), 4, GL_FLOAT);
	floor_pass_input.assign(2, "moisture", moisture_values.data(), moisture_values.size(), 1, GL_FLOAT);
	floor_pass_input.assign(3, "tree", tree_values.data(), tree_values.size(), 1, GL_FLOAT);
	floor_pass_input.assign_index(floor_faces.data(), floor_faces.size(), 3);
	RenderPass floor_pass(-1,
			floor_pass_input,
			{ floor_vertex_shader, floor_geometry_shader, floor_fragment_shader},
			{ floor_model, std_view, std_proj, std_light, floor_max_height, floor_min_height },
			{ "fragment_color" }
			);

	RenderDataInput water_pass_input;
	water_pass_input.assign(0, "vertex_position", water_vertices.data(), water_vertices.size(), 4, GL_FLOAT);
	water_pass_input.assign(1, "normal", water_normals.data(), water_normals.size(), 4, GL_FLOAT);
	water_pass_input.assign_index(water_faces.data(), water_faces.size(), 3);
	RenderPass water_pass(-1,
			water_pass_input,
			{ water_vertex_shader, geometry_shader, water_fragment_shader},
			{ floor_model, std_view, std_proj, std_light, floor_max_height, floor_min_height },
			{ "fragment_color" }
			);

	float aspect = 0.0f;
	bool draw_floor = true;
	bool draw_water = true;
	bool draw_object = true;
	gui.updateTime();

	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);
		
		gui.updateMatrices();
		mats = gui.getMatrixPointers();

		//draw floor.
		if (draw_floor) {
			if(floorMap.isDirty()){
				floor_vertices.clear();
				floor_normals.clear();
				moisture_values.clear();
				tree_values.clear();
				floorMap.updateFloor(floor_vertices, floor_normals);
				glm::vec2 floorCenter = floorMap.getCenter();
				moistureMap.setCenter(floorCenter.x, floorCenter.y);
				moistureMap.createHeights(moisture_values);
				treeMap.setCenter(floorCenter.x, floorCenter.y);
				treeMap.createHeights(tree_values);
				floor_pass.updateVBO(0, floor_vertices.data(), floor_vertices.size());
				floor_pass.updateVBO(1, floor_normals.data(), floor_normals.size());
				floor_pass.updateVBO(2, moisture_values.data(), moisture_values.size());
				floor_pass.updateVBO(3, tree_values.data(), tree_values.size());
			}
			floor_pass.setup();
			// Draw our triangles.
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, floor_faces.size() * 3, GL_UNSIGNED_INT, 0));
		}
		if(draw_water){
			water_vertices.clear();
			glm::vec2 floorCenter = floorMap.getCenter();
			waterMap.setCenter(floorCenter.x, floorCenter.y);
			waterMap.updateFloor(water_vertices, water_normals);
			water_pass.updateVBO(0, water_vertices.data(), water_vertices.size());
			water_pass.setup();
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, water_faces.size() * 3, GL_UNSIGNED_INT, 0));
			waterMap.updateZ(0.1);
		}
		if (draw_object) {
			if (gui.isPoseDirty()) {
				gui.animation_walk();
				current_character->getMesh()->updateAnimation();
				current_character->pass()->updateVBO(0,
						      current_character->getMesh()->animated_vertices.data(),
						      current_character->getMesh()->animated_vertices.size());
				gui.clearPose();
			}
			current_character->pass()->setup();
			int mid = 0;
			while (current_character->pass()->renderWithMaterial(mid))
				mid++;
		}
		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
		gui.updateTime();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
