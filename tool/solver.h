#pragma once

#include "../system/commontypes.h"
#include "verletobject.h"
#include "link.h"

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <cmath>


struct solver {

	Vector3 gravity = { 0,-9.81f,0 };

	void applyGravity(std::vector<std::unique_ptr<VerletObject>>& verleobjects)
	{
		for (auto& vo : verleobjects) {
			vo->accerlate(gravity);
		}
	}

	void applySpring(std::vector<std::unique_ptr<Link>>& verlelinks)
	{
		for (auto& vl : verlelinks) {
			vl->apply();
		}
	}

	void updatePosition(std::vector<std::unique_ptr<VerletObject>>& verleobjects, float dt)
	{
		for (auto& vo : verleobjects) {
			vo->update(dt);
		}
	}

	void applyConstrain(std::vector<std::unique_ptr<VerletObject>>& objs) {
		// 例：中心(100,0,0)、大球半径400
		applyInnerSphereConstraintAndBounce(
			objs,
			Vector3(100, 0, 0),
			400.0f,
			/*restitution*/0.9f,
			/*friction   */0.0001f);
	}

	// 大球の内側で小球をバウンドさせる（Verlet）
	// center: 大球中心
	// bigRadius: 大球半径
	// restitution: 反発係数(0=吸収, 1=完全弾性)
	// friction: 接線減衰(0=なし, 0.02〜0.2で摩擦風)

	void applyInnerSphereConstraintAndBounce(
		std::vector<std::unique_ptr<VerletObject>>& objects,
		const Vector3& center,
		float bigRadius,
		float restitution = 0.6f,
		float friction = 0.02f)
	{
		const float EPS = 1e-6f;
		const float SLOP = 1e-4f;   // 浮動小数誤差で再侵入しないための微小押し込み

		for (auto& u : objects) {
			auto& o = *u;

			// 中心球の“有効半径”（小球の中心が存在できる半径）
			const float allowR = bigRadius - o.radius;
			if (allowR <= 0.0f) continue; // 小球が大きすぎる場合は何もしない

			// 中心→小球中心ベクトル
			Vector3 d = o.currentposition - center;
			float   dist2 = d.LengthSquared();

			// まだ内側なら何もしない（境界は dist <= allowR）
			if (dist2 <= allowR * allowR) continue;

			// ---- 衝突（境界を越えて外に出ている）: 位置を内側へ補正 ----
			float   dist = std::sqrt(std::max(dist2, EPS));
			Vector3 n = (dist > EPS) ? (d / dist) : Vector3(1, 0, 0); // 外向き法線（中心→外）

			float penetration = dist - allowR; // 超過量（正）
			Vector3 corr = n * (penetration + SLOP);

			// 位置を内側へ戻す
			o.currentposition -= corr;

			// ---- 反射（Verlet速度の更新）----
			// Verletの"速度"は v ≒ current - previous
			Vector3 v = o.currentposition - o.prevposition;

			// もし“外向き”に動いていたら（壁に向かって突っ込んでいたら）反射させる
			float vn_s = v.Dot(n); // 法線成分スカラー
			if (vn_s > 0.0f) {
				Vector3 v_n = n * vn_s;
				Vector3 v_t = v - v_n;

				// 反射：法線は反転＆反発係数、接線は摩擦で減衰
				Vector3 v_reflected = v_t * (1.0f - friction) - v_n * restitution;

				// Verletは previous を調整して v' を実現する
				o.prevposition = o.currentposition - v_reflected;
			}

		}
	}

	void solveCollisions(std::vector<std::unique_ptr<VerletObject>>& objects)
	{
		for (int idx1 = 0; idx1 < objects.size(); idx1++)
		{
			for (int idx2 = idx1 + 1; idx2 < objects.size(); idx2++)
			{

				// 中心を結ぶベクトル
				const Vector3 collisionaxis = objects[idx1]->currentposition - objects[idx2]->currentposition;
				float distance = collisionaxis.Length();

				if (distance < objects[idx1]->radius + objects[idx2]->radius)
				{
					Vector3 normalizedcollisionaxis = collisionaxis;
					normalizedcollisionaxis.Normalize();
					const float delta = (objects[idx1]->radius + objects[idx2]->radius) - distance;
					objects[idx1]->currentposition += normalizedcollisionaxis * delta * 0.5f;
					objects[idx2]->currentposition -= normalizedcollisionaxis * delta * 0.5f;
				}
			}
		}

	}
};
