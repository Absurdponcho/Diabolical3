#include "MeshPrimitives.h"

//DSharedPtr<DMesh> MeshPrimitives::Cube = std::make_shared<DMesh>(
//	DVector<SVector3f>({{-.5, -.5,  0.5}, { .5, -.5,  0.5}, {-.5,  .5,  0.5}, { .5,  .5,  0.5}, {-.5, -.5, -0.5}, {.5, -.5, -0.5}, {-.5,  .5, -0.5}, {.5,  .5, -0.5}}), // vertex
//	DVector<SVector3f>(), // normals
//	DVector<int>({ 2, 6, 7,	2, 3, 7, 0, 4, 5, 0, 1, 5, 0, 2, 6, 0, 4, 6, 1, 3, 7, 1, 5, 7, 0, 2, 3,	0, 1, 3, 4, 6, 7, 4, 5, 7 }), // indices
//	DVector<SVector2f>() // UVs
//	);

DSharedPtr<DMesh> MeshPrimitives::Cube = std::make_shared<DMesh>(
	DVector<SVector3f>({
		{ -0.5, 0.5, -0.5 }, { -0.5, 0.5, 0.5 }, { 0.5, 0.5, 0.5 }, { 0.5, 0.5, -0.5 },
		{ -0.5, -0.5, -0.5 }, { -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 }, { 0.5, -0.5, -0.5 },
		{ -0.5, -0.5, 0.5 }, { -0.5, 0.5, 0.5 }, { -0.5, 0.5, -0.5 }, { -0.5, -0.5, -0.5 },
		{ 0.5, -0.5, -0.5 }, { 0.5, 0.5, -0.5 }, { 0.5, 0.5, 0.5 }, { 0.5, -0.5, 0.5 },
		{ -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 }, { 0.5, 0.5, 0.5 }, { -0.5, 0.5, 0.5 },
		{ -0.5, -0.5, -0.5 }, { 0.5, -0.5, -0.5 }, { 0.5, 0.5, -0.5 }, { -0.5, 0.5, -0.5 },
		}), // vertex
	DVector<SVector3f>({
		{ 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 0 },
		{ 0, -1, 0 }, { 0, -1, 0 }, { 0, -1, 0 }, { 0, -1, 0 },
		{ -1, 0, 0 }, { -1, 0, 0 }, { -1, 0, 0 }, { -1, 0, 0 },
		{ 1, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0 },
		{ 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 },
		{ 0, 0, -1 }, { 0, 0, -1 }, { 0, 0, -1 }, { 0, 0, -1 },
		}), // normals
	DVector<int>({
		0, 1, 2, 2, 3, 0,
		6, 5, 4, 4, 7, 6,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 22, 21, 20, 23, 22,
		}), // indices
	DVector<SVector2f>() // UVs
	);

DSharedPtr<DMesh> MeshPrimitives::Triangle = std::make_shared<DMesh>(
	DVector<SVector3f>({{-.5, -.5, 0}, {.5, -.5, 0}, {0, .5, 0}}),
	DVector<SVector3f>(),
	DVector<int>({0, 1, 2}),
	DVector<SVector2f>()
	);

DSharedPtr<DMesh> MeshPrimitives::Quad = std::make_shared<DMesh>(
	DVector<SVector3f>({ {-1, 1, 0}, {1, 1, 0}, {1, -1, 0}, {-1, -1, 0} }),
	DVector<SVector3f>(),
	DVector<int>({ 2, 1, 0, 3, 2, 0 }),
	DVector<SVector2f>({ { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })
	);