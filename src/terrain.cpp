#include "terrain.hpp"
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//#define TERRAIN_TIME

static void GenerateSlice(Voxel* voxelData, NoiseGenerator* noise, int z, bool mask = false);
void TerrainWork(Voxel* voxelData, NoiseGenerator* noise, int z0, int SLICE_THICKNESS);
float semiRandomFloat(int x, int y, int z);

NoiseTexture noiseTex0;

void NoiseTexture::load(const char* file)
{
	int channels;
	char path[32];
	strcpy(path, "../noise/");
	strcat(path, file);
	data = stbi_load(path, &size, &size, &channels, STBI_grey);
}

float NoiseTexture::sample(float x, float y, float scale) const
{
	return ((float*)data)[(int)(fmodf(x / scale, 1.0f) * size) + (int)(fmodf(y / scale, 1.0f) * size) * size];
}

TerrainGenerator::TerrainGenerator()
{
	noiseTex0.load("craters10.png");
}

glm::ivec3 TerrainGenerator::Generate(Voxel* voxelData)
{
	// Voxel Data
#ifdef TERRAIN_TIME
	auto it = std::chrono::system_clock::now();
#endif

#define TERRAIN_SCALE 4
#define TERRAIN_AREA (1024)

	printf("Generating\n");
	int WORKERS = std::thread::hardware_concurrency() - 2;
	const int SLICE_THICKNESS = TERRAIN_AREA / WORKERS;

	std::thread** threads = new std::thread * [WORKERS];

	for (int i = 0; i < WORKERS; i++)
	{
		if (i < WORKERS - 1) GenerateSlice(voxelData, &noise, (i + 1) * SLICE_THICKNESS - 1, true);
		threads[i] = new std::thread{ &TerrainWork, voxelData, &noise, i * SLICE_THICKNESS, SLICE_THICKNESS };
	}

	puts("");
	for (int i = 0; i < WORKERS; i++)
	{
		threads[i]->join();
		printf("\rProgress: %.1f", (float)i / WORKERS * 100);
	}

	glm::ivec3 spawnPoint{ TERRAIN_AREA / 2 };
	for (int z = 20 + 4; z < TERRAIN_AREA - 4; z++)
	{
		if (!Voxels::hasCollision(vox(spawnPoint.x, spawnPoint.y, z + 20)) && Voxels::hasCollision(vox(spawnPoint.x, spawnPoint.y, z + 20 + 1)))
		{
			spawnPoint.z = z;
			break;
		}
	}

#ifdef TERRAIN_TIME
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it);
	printf("\nTIME: %lli\n\n", ms.count());
#endif
	return -spawnPoint;
}

static void TerrainWork(Voxel* voxelData, NoiseGenerator* noise, int z0, int SLICE_THICKNESS)
{
	for (int z = z0; z < z0 + SLICE_THICKNESS; z++)
	{
		GenerateSlice(voxelData, noise, z);
	}
}

static void GenerateSlice(Voxel* voxelData, NoiseGenerator* noise, int z, bool mask)
{
	for (int y = 0; y < TERRAIN_AREA; y++)
	{
		for (int x = 0; x < TERRAIN_AREA; x++)
		{
			Voxel base = 0b11100000;
			float v = noise->GetNoise(x * 0.1f * TERRAIN_SCALE, y * 0.1f * TERRAIN_SCALE, z * 0.4f * TERRAIN_SCALE);

			Material material = Materials::AIR;
			if (v <= 0.1)
			{
				material = Materials::STONE;
				if (!mask)
					if (z > 0 && (Voxels::getMat(vox(x, y, z - 1)) == Materials::AIR))
					{
						material = Materials::GRASS;
						float v = semiRandomFloat(x, y, z);
						if (v < 0.08)
						{
							vox(x, y, z - 1) |= Materials::TALL_GRASS;

							if (z > 2 && v < 0.03)
							{
								vox(x, y, z - 2) |= Materials::TALL_GRASS;

								if (z > 3 && v < 0.008)
								{
									vox(x, y, z - 3) |= Materials::FLOWER;
								}
							}
						}
					}
					else
					{
						float v = noise->GetNoise(x * 1.5f * TERRAIN_SCALE, y * 1.5f * TERRAIN_SCALE, z * 1.f * TERRAIN_SCALE);
						int32_t i = *((int32_t*)&v);
						int32_t i2 = i | 342;
						float v2 = *((float*)(&i2));
						if (v2 < -0.15)
						{
							material = (i & 15) < 2 ? Materials::STONE2 : Materials::STONE3;
						}
					}
			}
			else if (z > 0 && (Voxels::getMat(vox(x, y, z - 1)) == Materials::STONE) && noiseTex0.sample(x + (z / 3) * 3, y, TERRAIN_AREA * TERRAIN_SCALE / 6) > 0.97 && ((int)(v * 100) & 2))
			{
				material = Materials::STONE;
			}

			vox(x, y, z) = base | material;
		}
	}
}

static float semiRandomFloat(int x, int y, int z) {
	// A simple hashing function using the input vector components
	uint32_t hash = x * 123456789 + y * 987654321 + z * 567890123;

	// A bitwise operation to mix the hash value
	hash = (hash ^ (hash >> 21)) * 2654435761u;
	hash = hash ^ (hash >> 21);
	hash = hash * 668265263;
	hash = hash ^ (hash >> 21);

	// Return a float between 0 and 1 based on the hash
	return (float)(hash & 0xFFFFFFF) / (float)0xFFFFFFF;
}
