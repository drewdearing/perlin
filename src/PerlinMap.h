#include "bone_geometry.h"
#include "PerlinNoise.h"
#include <iostream>

class PerlinMap {
private:
	siv::PerlinNoise perlin;
	int height;
	int width;
	int octaves;
	double frequency;
	double fx;
	double fy;
	double fz;
	std::uint32_t seed;
	float max_height;
	float min_height;
	float originX;
	float originY;
	float centerX;
	float centerY;
	float vert_distance;
	float radius;
	bool dirty;
	bool seedSet;
public:
	PerlinMap(int h, int w, int o, double f, float min, float max, float d, float r):
	height(h),
	width(w),
	octaves(o),
	frequency(f),
	seedSet(false),
	max_height(max),
	min_height(min),
	vert_distance(d),
	radius(r)
	{
		build();
	};

	PerlinMap(int h, int w, int o, double f, float min, float max, float d, float r, std::uint32_t s):
	height(h),
	width(w),
	octaves(o),
	frequency(f),
	seed(s),
	seedSet(true),
	max_height(max),
	min_height(min),
	vert_distance(d),
	radius(r)
	{
		build();
	};

	void build(){
		if(seedSet)
			perlin = siv::PerlinNoise(seed);
		else
			perlin = siv::PerlinNoise();

		fx = width / frequency;
		fy = height / frequency;
		fz = 0;

		originX = (width - 1)/2.0f;
		originY = (height - 1)/2.0f;

		setCenter(0, 0);

		seed = perlin.getSeed();
		seedSet = true;
		dirty = true;
	}

	void createFloor(std::vector<glm::vec4>& vertices, std::vector<glm::uvec3>& faces){
		int currentX;
		int currentY;
		float distanceX;
		float distanceY;
		float elevation;

		int min_x = std::max((int)ceil(centerX - radius), 0);
		int max_x = std::min((int)floor(centerX + radius), width-1);
		int min_y = std::max((int)ceil(centerY - radius), 0);
		int max_y = std::min((int)floor(centerY + radius), height-1);
		int current_width = max_x - min_x;
		int current_height = max_y - min_y;

		for(int y = 0; y < current_height; y++){
			for(int x = 0; x < current_width; x++){
				unsigned index = y * (current_width + 1) + x;
				int vertex[4];
				currentX = min_x + x;
				currentY = min_y + y;

				bool xEven = currentX % 2 == 0;
				bool yEven = currentY % 2 == 0;
				bool defaultDiag = (xEven && yEven) || (!xEven && !yEven);

				distanceX = (float(currentX) - centerX) * vert_distance;
				distanceY = (float(currentY) - centerY) * vert_distance;
				elevation = min_height + (perlin.octaveNoise0_1(currentX / fx, currentY / fy, fz, octaves) * (max_height-min_height));
				vertices.push_back(glm::vec4(distanceY, elevation, distanceX, 1));
				
				if(x == current_width - 1){
					currentX++;
					distanceX = (float(currentX) - centerX) * vert_distance;
					elevation = min_height + (perlin.octaveNoise0_1(currentX / fx, currentY / fy, fz, octaves) * (max_height-min_height));
					vertices.push_back(glm::vec4(distanceY, elevation, distanceX, 1));
				}

				vertex[0] = index;
				vertex[1] = index + 1;
				vertex[2] = index + current_width + 1;
				vertex[3] = vertex[2] + 1;

				if(defaultDiag){
					faces.push_back(glm::uvec3(vertex[2], vertex[0], vertex[1]));
					faces.push_back(glm::uvec3(vertex[3], vertex[2], vertex[1]));
				}
				else{
					faces.push_back(glm::uvec3(vertex[3], vertex[2], vertex[0]));
					faces.push_back(glm::uvec3(vertex[3], vertex[0], vertex[1]));
				}
			}
		}

		//add the last row of vertices
		currentY = min_y + current_height;
		distanceY = (float(currentY) - centerY) * vert_distance;

		for(int x = 0; x <= current_width; x++){
			currentX = min_x + x;
			distanceX = (float(currentX) - centerX) * vert_distance;
			elevation = min_height + (perlin.octaveNoise0_1(currentX / fx, currentY / fy, fz, octaves) * (max_height-min_height));
			vertices.push_back(glm::vec4(distanceY, elevation, distanceX, 1));
		}

		dirty = false;
	}

	void updateZ(float offset){
		fz += offset;
		dirty = true;
	}

	bool isDirty(){
		return dirty;
	}

	glm::vec4 getCoordinate(int x, int y){
		float distanceX = (float(x) - centerX) * vert_distance;
		float distanceY = (float(y) - centerY) * vert_distance;
		float elevation = min_height + (perlin.octaveNoise0_1(float(x) / fx, float(y) / fy, fz, octaves) * (max_height-min_height));
		return glm::vec4(distanceY, elevation, distanceX, 1);
	}

	bool setCenter(float x, float y){
		float newX = originX + x/vert_distance;
		float newY = originY + y/vert_distance;

		if(newX < 0)
			newX = 0;
		if(newX >= width)
			newX = width - 1;
		if(newY < 0)
			newY = 0;
		if(newY >= height)
			newY = height - 1;

		bool change = newX != centerX || newY != centerY;

		if(change){
			centerX = newX;
			centerY = newY;
			dirty = true;
		}

		return change;
	}

	glm::vec2 getCenter(){
		float wX = (centerX - originX) * vert_distance;
		float wY = (centerY - originX) * vert_distance;

		return glm::vec2(wX, wY);
	}

	float getVertDistance() {
		return vert_distance;
	}
	
};