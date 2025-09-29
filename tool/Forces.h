// ===================== Forces.h =====================
#pragma once
#include "VerletCore.h"

// 重力 F = m g
struct GravityForce : IForce {
    Vector3 g{ 0,-9.81f,0 };
    explicit GravityForce(const Vector3& g_) : g(g_) {}
    void apply(VerletBody& b, float) override {
        if (b.invMass == 0.0f) return;
        b.force += g * b.mass;
    }
};

// 線形ドラッグ（空気抵抗の近似, v比例）: F = -k v
struct LinearDragForce : IForce {
    float k{ 0.5f }; // 係数
    explicit LinearDragForce(float k_) : k(k_) {}
    void apply(VerletBody& b, float dt) override {
        (void)dt;
        // v ≈ (x - x_prev)/dt だが、直前フレームの b.velocity を使う簡易版
        b.force += -k * b.velocity;
    }
};

// 入力推進力（プレイヤ）：関数で毎フレーム方向を供給
struct ThrustForce : IForce {
    std::function<Vector3()> dirProvider; // 正規化方向×スカラーでもOK
    float thrustN{ 30.0f };                 // ニュートン
    ThrustForce(std::function<Vector3()> f, float n) : dirProvider(std::move(f)), thrustN(n) {}
    void apply(VerletBody& b, float) override {
        if (!dirProvider) return;
        Vector3 d = dirProvider();
        if (d.LengthSquared() > 0) d.Normalize();
        b.force += d * thrustN;
    }
};
