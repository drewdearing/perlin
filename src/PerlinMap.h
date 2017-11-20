#include "bone_geometry.h"
#include "PerlinNoise.h"
#include <iostream>

class Map {
private:
	std::vector<float> values;
	int width;
	int height;
public:
	Map(){};

	Map(int w, int h) : width(w), height(h){
		setSize(w*h);
	}

	void setSize(int size){
		values.resize(size);
	}

	bool inBounds(int x, int y){
		return (x >= 0 && x < width) && (y >= 0 && y < height);
	}

	bool setValue(int x, int y, float value){
		if(inBounds(x, y)){
			values.at(y * width + x) = value;
			return true;
		}
		else
			return false;
	}

	float getValue(int x, int y){
		if(inBounds(x, y)){
			return values.at(y * width + x);
		}
		else
			return -1;
	}
};

class PerlinMap {
private:
	
	Map map;
	int height;
	int width;
	int octaves;
	double frequency;
	std::uint32_t seed;
	float max_height;
	float min_height;
	float vert_distance;
	bool seedSet = false;
public:
	PerlinMap(int h, int w, int o, double f, float min, float max, float d):
	height(h),
	width(w),
	octaves(o),
	frequency(f),
	max_height(max),
	min_height(min),
	vert_distance(d)
	{
		build();
	};

	PerlinMap(int h, int w, int o, double f, float min, float max, float d, std::uint32_t s):
	height(h),
	width(w),
	octaves(o),
	frequency(f),
	seed(s),
	seedSet(true),
	max_height(max),
	min_height(min),
	vert_distance(d)
	{
		build();
	};

	void build(){
		map = Map(width, height);
		siv::PerlinNoise perlin;
		if(seedSet)
			perlin = siv::PerlinNoise(seed);
		else
			perlin = siv::PerlinNoise();

		double fx = width / frequency;
		double fy = height / frequency;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				double value = perlin.octaveNoise0_1(x / fx, y / fy, octaves);
				if(!map.setValue(x, y, value))
					std::cout<<"ERROR"<<std::endl;
			}
		}

		seed = perlin.getSeed();
		seedSet = true;
	}

	void createFloor(std::vector<glm::vec4>& vertices, std::vector<glm::uvec3>& faces){

		float centerX = (width - 1)/2.0f;
		float centerY = (height - 1)/2.0f;

		for(int x = 0; x < width - 1; x++){
			for(int y = 0; y < height - 1; y++){
				unsigned index = vertices.size();
				int values[4][2];

				bool xEven = x % 2 == 0;
				bool yEven = y % 2 == 0;
				bool defaultDiag = (xEven && yEven) || (!xEven && !yEven);

				values[0][0] = x;
				values[0][1] = y;

				values[1][0] = x + 1;
				values[1][1] = y;

				values[2][0] = x;
				values[2][1] = y + 1;

				values[3][0] = x + 1;
				values[3][1] = y + 1;

				for(int i = 0; i < 4; i++){
					int currentX = values[i][0];
					int currentY = values[i][1];
					float distanceX = (float(currentX) - centerX) * vert_distance;
					float distanceY = (float(currentY) - centerY) * vert_distance;
					float elevation = min_height + (map.getValue(currentX, currentY) * (max_height-min_height));
					vertices.push_back(glm::vec4(distanceY, elevation, distanceX, 1));
				}

				if(defaultDiag){
					faces.push_back(glm::uvec3(index + 2, index, index + 1));
					faces.push_back(glm::uvec3(index + 3, index + 2, index + 1));
				}
				else{
					faces.push_back(glm::uvec3(index + 3, index + 2, index));
					faces.push_back(glm::uvec3(index + 3, index, index + 1));
				}
			}
		}
	}

};