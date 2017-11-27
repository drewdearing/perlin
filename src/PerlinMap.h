#include "bone_geometry.h"
#include "PerlinNoise.h"
#include <iostream>

class PerlinMap {
private:
	siv::PerlinNoise perlin;
	int height;
	int width;
	int octaves;
	int diameter_x;
	int diameter_y;
	int radius;
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
	bool dirty;
	bool seedSet;
public:
	PerlinMap(int h, int w, int o, double f, float min, float max, float d, int r):
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

	PerlinMap(int h, int w, int o, double f, float min, float max, float d, int r, std::uint32_t s):
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

		diameter_x = radius * 2;
		diameter_y = radius * 2;

		if(diameter_x > width)
			diameter_x = width;
		if(diameter_y > height)
			diameter_y = height;

		originX = (width - 1)/2.0f;
		originY = (height - 1)/2.0f;

		setCenter(0, 0);

		seed = perlin.getSeed();
		seedSet = true;
		dirty = true;
	}

	void createFloor(std::vector<glm::vec4>& vertices, std::vector<glm::uvec3>& faces, std::vector<glm::vec4>& normals){
		int currentX;
		int currentY;
		float distanceX;
		float distanceY;
		float elevation;

		int min_x = std::max((int)ceil(centerX - float(radius)), 0);
		int max_x = std::min((int)floor(centerX + float(radius)), width-1);
		int min_y = std::max((int)ceil(centerY - float(radius)), 0);
		int max_y = std::min((int)floor(centerY + float(radius)), height-1);

		if(min_x == 0)
			max_x = diameter_x - 1;
		else if(max_x == width-1)
			min_x = width - diameter_x;
		if(min_y == 0)
			max_y = diameter_y - 1;
		else if(max_y == height-1)
			min_y = height - diameter_y;

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
				elevation = getVertexElevation(currentX, currentY);
				vertices.push_back(glm::vec4(distanceX, elevation, distanceY, 1));
				normals.push_back(getNormal(currentX, currentY));

				
				if(x == current_width - 1){
					currentX++;
					distanceX = (float(currentX) - centerX) * vert_distance;
					elevation = getVertexElevation(currentX, currentY);
					vertices.push_back(glm::vec4(distanceX, elevation, distanceY, 1));
					normals.push_back(getNormal(currentX, currentY));
				}

				vertex[0] = index;
				vertex[1] = index + 1;
				vertex[2] = index + current_width + 1;
				vertex[3] = vertex[2] + 1;

				faces.push_back(glm::uvec3(vertex[2], vertex[0], vertex[1]));
				faces.push_back(glm::uvec3(vertex[3], vertex[2], vertex[1]));
			}
		}

		//add the last row of vertices
		currentY = min_y + current_height;
		distanceY = (float(currentY) - centerY) * vert_distance;

		for(int x = 0; x <= current_width; x++){
			currentX = min_x + x;
			distanceX = (float(currentX) - centerX) * vert_distance;
			elevation = getVertexElevation(currentX, currentY);
			vertices.push_back(glm::vec4(distanceY, elevation, distanceX, 1));
			normals.push_back(getNormal(currentX, currentY));
		}

		dirty = false;
	}

	glm::vec4 getNormal(int x, int y){ //not accurate (working on more efficient solution)

		float left = perlin.octaveNoise0_1(float(x - 1) / fx, float(y) / fy, fz, octaves);
		float right = perlin.octaveNoise0_1(float(x + 1) / fx, float(y) / fy, fz, octaves);
		float up = perlin.octaveNoise0_1(float(x) / fx, float(y + 1) / fy, fz, octaves);
		float down = perlin.octaveNoise0_1(float(x) / fx, float(y - 1) / fy, fz, octaves);

		
		glm::vec3 normal = glm::vec3(left-right, 2.0f, down-up);

		return glm::vec4(glm::normalize(normal), 0);
	}

	void updateFloor(std::vector<glm::vec4>& vertices, std::vector<glm::vec4>& normals){
		int currentX;
		int currentY;
		float distanceX;
		float distanceY;
		float elevation;

		int min_x = std::max((int)ceil(centerX - float(radius)), 0);
		int max_x = std::min((int)floor(centerX + float(radius)), width-1);
		int min_y = std::max((int)ceil(centerY - float(radius)), 0);
		int max_y = std::min((int)floor(centerY + float(radius)), height-1);

		if(min_x == 0)
			max_x = diameter_x - 1;
		else if(max_x == width-1)
			min_x = width - diameter_x;
		if(min_y == 0)
			max_y = diameter_y - 1;
		else if(max_y == height-1)
			min_y = height - diameter_y;

		for(int y = min_y; y <= max_y; y++){
			distanceY = (float(y) - centerY) * vert_distance;
			for(int x = min_x; x <= max_x; x++){
				distanceX = (float(x) - centerX) * vert_distance;
				elevation = getVertexElevation(x, y);
				vertices.push_back(glm::vec4(distanceY, elevation, distanceX, 1));
				normals.push_back(getNormal(x, y));
			}
		}

		dirty = false;
	}

	glm::vec4 getVertexPoint(int x, int y){
		float distanceX = (float(x) - centerX) * vert_distance;
		float distanceY = (float(y) - centerY) * vert_distance;
		float elevation = getVertexElevation(x, y);
		return glm::vec4(distanceY, elevation, distanceX, 1);
	}

	void updateZ(float offset){
		fz += offset;
		dirty = true;
	}

	bool isDirty(){
		return dirty;
	}

	float getVertexElevation(int x, int y){
		return min_height + (perlin.octaveNoise0_1(float(x) / fx, float(y) / fy, fz, octaves) * (max_height-min_height));
	}

	float getElevation(float x, float y){
		float newX = centerX + x/vert_distance;
		float newY = centerY + y/vert_distance;
		float elevation;

		if(floor(newX) == newX && floor(newY) == newY){
			elevation = getVertexElevation(newX, newY);
		}
		else{
			int square_x = floor(newX);
			int square_y = floor(newY);
			int vertex[4][2];
			float v_elevation[4];
			float distance[4];
			float weight[4];
			float total_distance = 0;

			vertex[0][0] = square_x;
			vertex[0][1] = square_y;

			vertex[1][0] = square_x + 1;
			vertex[1][1] = square_y;

			vertex[2][0] = square_x;
			vertex[2][1] = square_y + 1;

			vertex[3][0] = square_x + 1;
			vertex[3][1] = square_y + 1;

			for(int i = 0; i < 4; i++){
				glm::vec2 local_distance;
				v_elevation[i] = getVertexElevation(vertex[i][0], vertex[i][1]);
				local_distance[0] = newX - float(vertex[i][0]);
				local_distance[1] = newY - float(vertex[i][1]);
				distance[i] = glm::length(local_distance);
				total_distance += distance[i];
			}
			weight[0] = distance[3]/total_distance;
			weight[1] = distance[2]/total_distance;
			weight[2] = distance[1]/total_distance;
			weight[3] = distance[0]/total_distance;

			elevation = weight[0] * v_elevation[0] 
					  + weight[1] * v_elevation[1]
					  + weight[2] * v_elevation[2]
					  + weight[3] * v_elevation[3];

		}

		return elevation;
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