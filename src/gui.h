#ifndef SKINNING_GUI_H
#define SKINNING_GUI_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "PerlinMap.h"
#include <chrono>

class Character;
struct Mesh;

/*
 * Hint: call glUniformMatrix4fv on thest pointers
 */
struct MatrixPointers {
	const float *projection, *model, *view;
};

class GUI {
public:
	GUI();
	GUI(GLFWwindow*, PerlinMap*);
	~GUI();
	void assignCharacter(Character * c);
	void assignModel(const std::string& model);
	void assignFloorMap(PerlinMap *);
	void assignCharacterList(std::vector<Character *>* list, Character ** current);
	void updateTime();
	void updateFrameRate();

	void keyCallback(int key, int scancode, int action, int mods);
	void mousePosCallback(double mouse_x, double mouse_y);
	void mouseButtonCallback(int button, int action, int mods);
	void updateMatrices();
	MatrixPointers getMatrixPointers() const;

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	glm::vec3 getCenter() const { return center_; }
	const glm::vec3& getCamera() const { return eye_; }
	bool isPoseDirty() const { return pose_changed_; }
	void clearPose() { if(!is_animating)pose_changed_ = false; }
	const float* getLightPositionPtr() const { return &light_position_[0]; }
	
	int getCurrentBone() const { return current_bone_; }

	glm::vec4 getMouseNear() { return glm::vec4(center_, 1); }

	glm::vec4 getMouseFar() { return glm::vec4(intersect, 1); }

	glm::vec3 * getLook() { return &look_; }

	bool setCurrentBone(int i);

	void finishAnimation();

	void animation_walk();

	bool isTransparent() const { return transparent_; }

private:
	GLFWwindow* window_;
	Character* character;
	PerlinMap* floorMap;
	Character** curr_char;
	std::vector<Character *>* char_list;

	int window_width_, window_height_;

	unsigned char_id = 0;

	bool drag_state_ = false;
	bool fps_mode_ = false;
	bool pose_changed_ = true;
	bool transparent_ = false;
	bool display_fr = false;
	int current_bone_ = -1;
	int current_button_ = -1;
	int frame_rate;
	float roll_speed_ = 0.1;
	float last_x_ = 0.0f, last_y_ = 0.0f, current_x_ = 0.0f, current_y_ = 0.0f;
	float camera_distance_ = 35.0;
	float pan_speed_ = 0.1f;
	float rotation_speed_ = 0.02f;
	float zoom_speed_ = 0.1f;
	float aspect_;
	float frame = 1000.0f/60.0f;
	bool running_animation = false;
	bool is_running = false;
	float walking_speed = 0.3f;
	float current_rotation = 0.0f;

	bool waifuMiku = true;
	bool is_animating = false;
	bool two_step = false;
	//std::string model_;

	//BONES FOR MODEL//
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

	std::chrono::high_resolution_clock::time_point current_time;
	std::chrono::duration<float, std::milli> delta_time;


	glm::vec3 intersect;

	glm::vec3 eye_ = glm::vec3(0.0f, 0.1f, camera_distance_);
	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 look_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 tangent_ = glm::cross(look_, up_);
	glm::vec3 center_ = eye_ - camera_distance_ * look_;
	glm::mat3 orientation_ = glm::mat3(tangent_, up_, look_);
	glm::vec4 light_position_;

	glm::mat4 view_matrix_ = glm::lookAt(eye_, center_, up_);
	glm::mat4 projection_matrix_;
	glm::mat4 model_matrix_ = glm::mat4(1.0f);

	void revertBoneRotation(Bone* rotated_bone);
	bool captureWASDUPDOWN(int key, int action);

};

#endif
