#pragma once
#include <cstdint>

struct Material
{
	uint8_t value;
	bool hasCollision;

	operator int() const noexcept
	{
		return value;
	}
};

namespace Materials
{
	static Material AIR{ 0, false };
	static Material STONE{ 1, true };
	static Material STONE2{ 2, true };
	static Material STONE3{ 3, true };
	static Material GRASS{ 8, true };
	static Material TALL_GRASS{ 9, false };
	static Material FLOWER{ 10, false };
	static Material WATER{ 19, false };

	static Material LIST[32];

#define _MAT_INIT(mat) LIST[mat.value] = mat;

	static void initMaterials()
	{
		_MAT_INIT(AIR);
		_MAT_INIT(STONE);
		_MAT_INIT(STONE2);
		_MAT_INIT(STONE3);
		_MAT_INIT(GRASS);
		_MAT_INIT(TALL_GRASS);
		_MAT_INIT(FLOWER);
		_MAT_INIT(WATER);
	}
};

typedef uint8_t Voxel;

namespace Voxels
{
	inline Material getMat(Voxel voxel)
	{
		return Materials::LIST[voxel & 0b11111];
	}

	inline bool hasCollision(Voxel voxel)
	{
		return getMat(voxel).hasCollision;
	}
};

