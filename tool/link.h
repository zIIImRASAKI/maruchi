#pragma once

#include "../system/commontypes.h"
#include "verletobject.h"

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <cmath>
#include "verletobject.h"

struct Link 
{
	VerletObject* object1;
	VerletObject* object2;
	float targetdistance;

	void apply() {
		const Vector3 axis = object1->currentposition - object2->currentposition;
		const float distance = axis.Length();
		Vector3 normalizedaxis = axis;
		normalizedaxis.Normalize();

		const float delta = targetdistance - distance;		// 加速度を計算（フックの法則）
		
		// 2つのベルレ頂点の現在位置を修正
		object1->currentposition += 0.5f * delta * normalizedaxis;
		object2->currentposition -= 0.5f * delta * normalizedaxis;
	}
};
