#include "gui.h"
#include "config.h"
#include <jpegio.h>
#include "bone_geometry.h"
#include <iostream>
#include <debuggl.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <ctime>
#include <limits>

GUI::GUI(GLFWwindow* window)
	:window_(window)
{
	glfwSetWindowUserPointer(window_, this);
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window_, KeyCallback);
	glfwSetCursorPosCallback(window_, MousePosCallback);
	glfwSetMouseButtonCallback(window_, MouseButtonCallback);

	glfwGetWindowSize(window_, &window_width_, &window_height_);
	float aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
}

GUI::~GUI()
{
}

void GUI::assignMesh(Mesh* mesh)
{
	mesh_ = mesh;
	center_ = mesh_->getCenter() * scale;
	center_.y += mesh_->height_offset;
	eye_ = center_ - look_ * camera_distance_;
}

void GUI::assignFloorMap(PerlinMap * map){
	floorMap = map;
}

void GUI::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window_, GL_TRUE);
		return ;
	}
	if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
		//FIXME save out a screenshot using SaveJPEG
		unsigned char * buffer = (unsigned char *) malloc(3 * sizeof(unsigned char) * window_height_ * window_width_);
		std::string filename("Screenshot ");
		time_t rawtime;
  		time (&rawtime);
		filename.append(ctime (&rawtime));
		glReadPixels(0, 0, window_width_, window_height_, GL_RGB, GL_UNSIGNED_BYTE, buffer);
		SaveJPEG(filename, window_width_ , window_height_, buffer);
		free(buffer);
	}

	if (captureWASDUPDOWN(key, action))
		return ;
	if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
		fps_mode_ = !fps_mode_;
		if(!fps_mode_){
			mesh_->height_offset = floorMap->getElevation(0,0);
			center_ = mesh_->getCenter() * scale;
			center_.y += mesh_->height_offset;
			eye_ = center_ - look_ * camera_distance_;
		}
	} else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE) {
		current_bone_--;
		current_bone_ += mesh_->getNumberOfBones();
		current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE) {
		current_bone_++;
		current_bone_ += mesh_->getNumberOfBones();
		current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_T && action != GLFW_RELEASE) {
		transparent_ = !transparent_;
	} else if (key == GLFW_KEY_F && action != GLFW_RELEASE) {
		display_fr = !display_fr;
	}
}

void GUI::updateTime(){
	std::chrono::high_resolution_clock::time_point old_time = current_time;
	current_time = std::chrono::high_resolution_clock::now();
	delta_time = current_time-old_time;
	updateFrameRate();
}

void GUI::updateFrameRate(){
	if(display_fr){
		frame_rate = floor(1000.0f/delta_time.count());
		std::cout<<frame_rate<<std::endl;
	}
}

void GUI::mousePosCallback(double mouse_x, double mouse_y)
{
	last_x_ = current_x_;
	last_y_ = current_y_;
	current_x_ = mouse_x;
	current_y_ = window_height_ - mouse_y;
	float delta_x = current_x_ - last_x_;
	float delta_y = current_y_ - last_y_;

	if(delta_x != 0 || delta_y != 0){
		glm::uvec4 viewport = glm::uvec4(0, 0, window_width_, window_height_);
		glm::vec3 lastPlane = glm::unProject(glm::vec3(last_x_, last_y_, 0.0), view_matrix_, projection_matrix_, viewport);
		glm::vec3 nearPlane = glm::unProject(glm::vec3(current_x_, current_y_, 0.0), view_matrix_, projection_matrix_, viewport);

		glm::vec3 mouse_world = nearPlane-lastPlane;
		glm::vec3 axis = glm::normalize(glm::cross(look_, mouse_world));

		float r = M_PI/50.0f * (delta_time.count()/frame);

		glm::vec3 old_look_ = look_;
		eye_ = center_ + glm::rotate(eye_-center_, r, axis);
		look_ = glm::normalize(center_ - eye_);
		tangent_ = glm::rotate(tangent_, r, axis);
		tangent_.y = 0;
		tangent_ = glm::normalize(tangent_);
		up_ = -glm::normalize(glm::cross(look_, tangent_));

	}
}

void GUI::mouseButtonCallback(int button, int action, int mods)
{
	drag_state_ = (action == GLFW_PRESS);
	current_button_ = button;
}

void GUI::updateMatrices()
{

	view_matrix_ = glm::lookAt(eye_, center_, up_);
	light_position_ = glm::vec4(eye_, 1.0f);

	aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ =
		glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
	model_matrix_ = glm::mat4(1.0f);
}

MatrixPointers GUI::getMatrixPointers() const
{
	MatrixPointers ret;
	ret.projection = &projection_matrix_[0][0];
	ret.model= &model_matrix_[0][0];
	ret.view = &view_matrix_[0][0];
	return ret;
}

bool GUI::setCurrentBone(int i)
{
	if (i < 0 || i >= mesh_->getNumberOfBones())
		return false;
	current_bone_ = i;
	return true;
}

bool GUI::captureWASDUPDOWN(int key, int action)
{
	glm::vec2 c = floorMap->getCenter();
	glm::vec3 dir_f;
	glm::vec3 dir_s;
	float speed = walk_speed * scale * floorMap->getVertDistance();

	if(fps_mode_){
		dir_f = speed * glm::normalize(glm::vec3(look_.z, look_.y, look_.x));
		dir_s = speed * glm::normalize(glm::vec3(tangent_.z, tangent_.y, tangent_.x));
	}
	else{
		dir_f = speed * glm::normalize(glm::vec3(look_.z, 0, look_.x));
		dir_s = speed * glm::normalize(glm::vec3(tangent_.z, 0, tangent_.x));
	}

	if (key == GLFW_KEY_W) {
		floorMap->setCenter(c.x+dir_f.x, c.y+dir_f.z);
		if(fps_mode_){
			mesh_->height_offset += dir_f.y;
			center_.y += dir_f.y;
		}
		else{
			mesh_->height_offset = floorMap->getElevation(0,0);
			mesh_->tilt_normal = floorMap->getNormal(0,0);
			center_ = mesh_->getCenter() * scale;
			center_.y += mesh_->height_offset;
		}
		eye_ = center_ - look_ * camera_distance_;
		return true;
	} else if (key == GLFW_KEY_S) {
		floorMap->setCenter(c.x-dir_f.x, c.y-dir_f.z);
		if(fps_mode_){
			mesh_->height_offset -= dir_f.y;
			center_.y -= dir_f.y;
		}
		else{
			mesh_->height_offset = floorMap->getElevation(0,0);
			mesh_->tilt_normal = floorMap->getNormal(0,0);
			center_ = mesh_->getCenter() * scale;
			center_.y += mesh_->height_offset;
		}
		eye_ = center_ - look_ * camera_distance_;
		return true;
	} else if (key == GLFW_KEY_A) {
		floorMap->setCenter(c.x-dir_s.x, c.y-dir_s.z);
		if(!fps_mode_){
			mesh_->height_offset = floorMap->getElevation(0,0);
			mesh_->tilt_normal = floorMap->getNormal(0,0);
			center_ = mesh_->getCenter() * scale;
			center_.y += mesh_->height_offset;
			eye_ = center_ - look_ * camera_distance_;
		}
		return true;
	} else if (key == GLFW_KEY_D) {
		floorMap->setCenter(c.x+dir_s.x, c.y+dir_s.z);
		if(!fps_mode_){
			mesh_->height_offset = floorMap->getElevation(0,0);
			mesh_->tilt_normal = floorMap->getNormal(0,0);
			center_ = mesh_->getCenter() * scale;
			center_.y += mesh_->height_offset;
			eye_ = center_ - look_ * camera_distance_;
		}
		return true;
	} else if (key == GLFW_KEY_SPACE) {
		if(fps_mode_){
			mesh_->height_offset += 1;
			center_.y += 1;
			eye_ = center_ - look_ * camera_distance_;
		}
		return true;
	} else if (key == GLFW_KEY_LEFT_SHIFT) {
		if(fps_mode_){
			mesh_->height_offset -= 1;
			center_.y -= 1;
			eye_ = center_ - look_ * camera_distance_;
		}
		return true;
	}
	return false;
}


// Delegrate to the actual GUI object.
void GUI::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->keyCallback(key, scancode, action, mods);
}

void GUI::MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mousePosCallback(mouse_x, mouse_y);
}

void GUI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseButtonCallback(button, action, mods);
}
