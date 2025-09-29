#pragma once
#include	"../system/commontypes.h"

struct VerletObject {

	Vector3		currentposition{};
	Vector3		prevposition{};		

	Vector3		acceleration{};		// 加速度

	float		radius;				// 表示用半径

	Color		color = {1,0,0,1};	// 表示用

	bool		pinned = false;		// 固定されているか

	void update(float dt) {
		const Vector3 velocity = currentposition - prevposition;
		prevposition = currentposition;

		currentposition = currentposition + velocity + acceleration * dt * dt;
	
		acceleration = {0,0,0};
	}

	void accerlate(Vector3 a) {
		acceleration += a;
	}

};