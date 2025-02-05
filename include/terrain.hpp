#pragma once
#include <cstdint>
#include <glm/vec3.hpp>
#define FNL_IMPL
#include "FastNoiseLite.hpp"

#include "materials.hpp"

typedef FastNoiseLite NoiseGenerator;
struct NoiseTexture
{
	int size;
	uint8_t* data;

	void load(const char* file);
	float sample(float x, float y, float scale) const;
};

#define vox(x,y,z) voxelData[(z) * TerrainGenerator::CHUNK_SIZE * TerrainGenerator::CHUNK_SIZE + (y) * TerrainGenerator::CHUNK_SIZE + (x)]

class TerrainGenerator
{
public:
	static const int CHUNK_SIZE = 1 << 10;

	glm::ivec3 Generate(Voxel* voxelData);
	TerrainGenerator();

	class VoxelStructure
	{
		// x
		int width;
		// y
		int length;
		// z
		int height;
		Voxel** voxels;
		Voxel getVoxel(int x, int y);
	};

private:
	NoiseGenerator noise;
};
