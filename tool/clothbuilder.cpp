#include    "clothbuilder.h"
#include    <memory>

ClothHandle ClothGridBuilder::build(
    std::vector<std::unique_ptr<VerletObject>>& volist,
    std::vector<std::unique_ptr<Link>>& linklist) 
{
    assert(m_config.rows > 0 && m_config.cols > 0 && "rows/cols must be positive");

    ClothHandle h;
    h.bodyIds.resize(static_cast<size_t>(m_config.rows) * static_cast<size_t>(m_config.cols));

    // 1) 質点生成
    for (int r = 0; r < m_config.rows; ++r) {
        for (int c = 0; c < m_config.cols; ++c) {
            auto b = std::make_unique<VerletObject>();
            b->currentposition =
                m_config.origin + Vector3(
                    static_cast<float>(c) * m_config.spacing,      // X：列方向
                    -static_cast<float>(r) * m_config.spacing,     // Y：行（下向き）
                    0.0f),

            b->prevposition = b->currentposition;
            b->radius = m_config.spacing/2.0f;

            volist.emplace_back(std::move(b));
            h.bodyIds[idx(r, c)] = volist.size()-1;
        }
    }

    // 2) 固定（必要なら）
    if (m_config.pinTopCorners) {
        volist[h.bodyIds[idx(0, 0)]]->pinned = true;
        volist[h.bodyIds[idx(0, m_config.cols - 1)]]->pinned = true;
    }

    // 3) 構造ばね（4近傍）
    for (int r = 0; r < m_config.rows; ++r) {
        for (int c = 0; c < m_config.cols; ++c) {
            addSpringIfValid(
                volist, 
                linklist,
                h, 
                r, c, 
                r, c + 1, 
                m_config.structParams); // 右

            addSpringIfValid(
                volist,
                linklist,
                h,
                r, c,
                r+1, c,
                m_config.structParams); // 下
        }
    }

    // 4) せん断ばね（斜め）
    for (int r = 0; r < m_config.rows; ++r) {
        for (int c = 0; c < m_config.cols; ++c) {
            addSpringIfValid(volist,linklist,h, r, c, r + 1, c + 1, m_config.shearParams); // 右下
            addSpringIfValid(volist,linklist,h, r, c, r + 1, c - 1, m_config.shearParams); // 左下
        }
    }

    // 5) 曲げばね（2マス離れ）
    for (int r = 0; r < m_config.rows; ++r) {
        for (int c = 0; c < m_config.cols; ++c) {
            addSpringIfValid(volist, linklist, h, r, c, r, c + 2, m_config.bendParams); // 横2
            addSpringIfValid(volist, linklist, h, r, c, r + 2, c, m_config.bendParams); // 縦2
        }
    }

    return h;
}

void ClothGridBuilder::addSpringIfValid(
    std::vector<std::unique_ptr<VerletObject>>& volist,
    std::vector<std::unique_ptr<Link>>& linklist,
    ClothHandle& h,
    int r1, int c1, int r2, int c2,
    const ClothSpringParams& p)
{
    const int rows = m_config.rows;
    const int cols = m_config.cols;

    if (r1 < 0 || r1 >= rows || c1 < 0 || c1 >= cols) return;
    if (r2 < 0 || r2 >= rows || c2 < 0 || c2 >= cols) return;

    const size_t i = h.bodyIds[idx(r1, c1)];
    const size_t j = h.bodyIds[idx(r2, c2)];

    auto& A = *volist[i];
    auto& B = *volist[j];

    const float rest = (B.currentposition - A.currentposition).Length();

    auto link = std::make_unique<Link>();
    link->object1 = volist[i].get();
    link->object2 = volist[j].get();
    link->targetdistance = rest;

    linklist.emplace_back(std::move(link));

    h.links.emplace_back(i, j);
}
