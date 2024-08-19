#pragma once
#include "RE_Painter.hpp"

namespace RE {
    float grad(int hash, float x, float y) {
        switch (hash & 7) {
        case 0:
            return x + y;
        case 1:
            return -x + y;
        case 2:
            return x - y;
        case 3:
            return -x - y;
        case 4:
            return x;
        case 5:
            return -x;
        case 6:
            return y;
        case 7:
            return -y;
        default:
            return 0;
        }
    }

    float perlinFade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    };

    std::vector<int> generatePermutationTable() {
        std::vector<int> p(256);
        std::iota(p.begin(), p.end(), 0);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(p.begin(), p.end(), gen);
        p.insert(p.end(), p.begin(), p.end());
        return p;
    }

    template <typename T>
    float perlinNoise(RE::Painter<T>* painter, size_t x, size_t y, size_t freq) {
        static std::vector<int> perm = generatePermutationTable();
        const size_t girdSize = std::max(painter->texture.width(), painter->texture.height()) / freq;

        const size_t x1 = (x / girdSize) & 255;
        const size_t y1 = (y / girdSize) & 255;
        const size_t x2 = x1 + 1;
        const size_t y2 = y1 + 1;

        const float xf = x / static_cast<float>(girdSize) - x1;
        const float yf = y / static_cast<float>(girdSize) - y1;

        const float u = perlinFade(xf);
        const float v = perlinFade(yf);

        const float dotA = grad(perm[x1 + perm[y1]], xf, yf);
        const float dotB = grad(perm[x1 + perm[y2]], xf, yf - 1);
        const float dotC = grad(perm[x2 + perm[y1]], xf - 1, yf);
        const float dotD = grad(perm[x2 + perm[y2]], xf - 1, yf - 1);

        const float temp1 = RE::lerp(dotA, dotB, v);
        const float temp2 = RE::lerp(dotC, dotD, v);
        float noiseValue = RE::lerp(temp1, temp2, u);
        noiseValue = (noiseValue + 1.0f) / 2.0f;
        return noiseValue;
    }

    template <typename T>
    void generateCommonNoise(RE::Painter<T>* painter, size_t seed = 0) {
        painter->paintStart();
        for (size_t i = 0; i < painter->texture.area(); i++) {
            const T value = static_cast<T>(rand() % 256);
            painter->texture.data()[i] = value;
        }
    }

    template <typename T>
    void generatePerlinNoise(RE::Painter<T>* painter, size_t freq) {
        painter->paintStart();
        for (size_t i = 0; i < painter->texture.length(); i += painter->texture.channel()) {
            const size_t x = painter->texture.getCol(i);
            const size_t y = painter->texture.getRow(i);
            const float noiseValue = perlinNoise(painter, x, y, freq);
            painter->drawPixel(x, y, RE::rgb{static_cast<uint8_t>(noiseValue * 255)});
        }
    }

    template <typename T>
    void generateFractalPerlinNoise(RE::Painter<T>* painter) {
        painter->paintStart();
        for (size_t i = 0; i < painter->texture.length(); i += painter->texture.channel()) {
            const size_t x = painter->texture.getCol(i);
            const size_t y = painter->texture.getRow(i);
            float noiseValue = 0;
            for (float freq = 4; freq <= 128; freq *= 2) {
                noiseValue += perlinNoise<T>(painter, x, y, freq) / freq * 2;
            }

            painter->drawPixel(x, y, RE::rgb{static_cast<uint8_t>(noiseValue * 255)});
        }
    }
}