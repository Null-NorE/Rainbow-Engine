#pragma once
#include "RE_includes.h"
#include "RE_Texture.hpp"
namespace RE {
    using Range2D = glm::u64vec4;
    using Point2D = glm::u64vec2;
    struct Line2D {
        Line2D() = default;
        Line2D(glm::u64vec2 p1, glm::u64vec2 p2) : p1(p1), p2(p2){};
        glm::u64vec2 p1;
        glm::u64vec2 p2;
    };

    class Polygon2D {
    public:
        Polygon2D(std::initializer_list<Point2D> pl) : points(pl){};
        Point2D& operator[](size_t index) {
            return points[index];
        }
        size_t size() {
            return points.size();
        }
        Line2D getLine(size_t index) {
            const size_t ip1 = (index + 1 == this->size()) ? 0 : (index + 1);
            Line2D line;
            line.p1 = (*this)[index];
            line.p2 = (*this)[ip1];
            return line;
        }
        std::vector<Line2D> getLineList() {
            std::vector<Line2D> lineList;
            for (size_t i = 0; i < this->size(); i++) {
                lineList.push_back(this->getLine(i));
            }
            return lineList;
        }
        // 计算y=int处的交点坐标
        std::vector<Point2D> intersection(int y) {
            std::vector<Point2D> pointList;
            pointList.reserve(this->size() * 2); // Assuming each line can intersect at most twice

            auto addIntersectionPoint = [&](const Point2D& p1, const Point2D& p2) {
                const float f = (y == 0) ? 0 : ((static_cast<float>(y) - p2.y) / (p1.y - p2.y));
                const int x = RE::lerp(p2.x, p1.x, f);
                pointList.emplace_back(x, y);
            };

            for (size_t i = 0; i < this->size(); i++) {
                Line2D line = this->getLine(i);
                if (line.p1.y >= y && line.p2.y < y) {
                    addIntersectionPoint(line.p1, line.p2);
                }
                if (line.p1.y < y && line.p2.y >= y) {
                    addIntersectionPoint(line.p2, line.p1);
                }
                if (line.p1.y == y && line.p2.y == y) {
                    pointList.push_back(line.p1);
                    pointList.push_back(line.p2);
                }
            }

            std::sort(pointList.begin(), pointList.end(), [](const Point2D& p1, const Point2D& p2) -> bool { return p1.x < p2.x; });
            return pointList;
        }

        Range2D range() {
            Range2D out{(*this)[0].x, (*this)[0].y, (*this)[0].x, (*this)[0].y};
            for (auto& i : points) {
                out[0] = std::min(out[0], i.x);
                out[1] = std::min(out[1], i.y);
                out[2] = std::max(out[2], i.x);
                out[3] = std::max(out[3], i.y);
            }
            return out;
        }

    private:
        std::vector<Point2D> points;
    };

    template <typename T>
    class Painter {
    public:
        Painter(ImageView<T>* iv) : imageView(iv), texture(iv->texture){};
        ~Painter() {
            imageView->update();
            imageView->changed = false;
        };
        Painter(const Painter&) = delete;
        Painter(Painter&&) = delete;
        Painter& operator=(const Painter&) = delete;
        Painter& operator=(Painter&&) = delete;

        void drawPixel(size_t x, size_t y, rgba color) {
            paintStart();
            const size_t index = texture.getIndex(x, y);
            color = alphaMix(texture.getRGBA(x, y), color);

            texture.data()[index] = color.r;
            texture.data()[index + 1] = color.g;
            texture.data()[index + 2] = color.b;
            texture.data()[index + 3] = color.a;
        }

        void drawPixel(size_t index, rgba color) {
            paintStart();
            color = alphaMix(texture.getRGBA(index), color);

            texture.data()[index] = color.r;
            texture.data()[index + 1] = color.g;
            texture.data()[index + 2] = color.b;
            texture.data()[index + 3] = color.a;
        }

        void drawPixelSafe(size_t x, size_t y, rgba color) {
            paintStart();
            if (x < texture.width() && x > 0 && y > 0 && y < texture.height()) {
                const size_t index = texture.getIndex(x, y);
                color = alphaMix(texture.getRGBA(x, y), color);

                texture.data()[index] = color.r;
                texture.data()[index + 1] = color.g;
                texture.data()[index + 2] = color.b;
                texture.data()[index + 3] = color.a;
            }
        }

        void drawPixel(size_t x, size_t y, const rgb& color) {
            paintStart();
            const size_t&& index = texture.getIndex(x, y);

            texture.data()[index] = color.r;
            texture.data()[index + 1] = color.g;
            texture.data()[index + 2] = color.b;
        }

        void drawPixel(size_t index, const rgb& color) {
            paintStart();
            texture.data()[index] = color.r;
            texture.data()[index + 1] = color.g;
            texture.data()[index + 2] = color.b;
        }

        void drawPixelSafe(size_t x, size_t y, const rgb& color) {
            paintStart();
            if (x < texture.width() && x >= 0 && y >= 0 && y < texture.height()) {
                const size_t&& index = texture.getIndex(x, y);

                texture.data()[index] = color.r;
                texture.data()[index + 1] = color.g;
                texture.data()[index + 2] = color.b;
            }
        }

        void clearImage() {
            paintStart();
            texture.setZero();
        }

        template <typename Color_T>
        void drawScanline(size_t x, size_t y, size_t width, Color_T color) {
            paintStart();
            for (size_t i = 0; i < width; i++) {
                drawPixel(x + i, y, color);
            }
        }

        void setSize(size_t width, size_t height, size_t channel) {
            texture.setSize(width, height, channel);
        }

        template <typename Color_T>
        void drawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Color_T color) {
            paintStart();
            const int64_t dx = llabs(x2 - x1);
            const int64_t dy = llabs(y2 - y1);

            const int64_t stepX = (x2 == x1) ? 0 : (x2 - x1) / dx;
            const int64_t stepY = (y2 == y1) ? 0 : (y2 - y1) / dy;

            int64_t xi = x1;
            int64_t yi = y1;

            if (dx > dy) {
                int64_t isDelta = 2 * dy - dx;

                while (xi != x2 + stepX) {
                    drawPixel(xi, yi, color);
                    if (isDelta < 0) {
                        isDelta += 2 * dy;
                    } else {
                        yi += stepY;
                        isDelta = isDelta + 2 * dy - 2 * dx;
                    }
                    xi += stepX;
                }
            } else {
                int64_t isDelta = 2 * dx - dy;

                while (yi != y2 + stepY) {
                    drawPixel(xi, yi, color);
                    if (isDelta < 0) {
                        isDelta += 2 * dx;
                    } else {
                        xi += stepX;
                        isDelta = isDelta + 2 * dx - 2 * dy;
                    }
                    yi += stepY;
                }
            }
        }

        template <typename Color_T>
        void drawLineSafe(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Color_T color) {
            paintStart();
            const int64_t dx = llabs(x2 - x1);
            const int64_t dy = llabs(y2 - y1);

            const int64_t stepX = (x2 == x1) ? 0 : (x2 - x1) / dx;
            const int64_t stepY = (y2 == y1) ? 0 : (y2 - y1) / dy;

            int64_t xi = x1;
            int64_t yi = y1;

            if (dx > dy) {
                int64_t isDelta = 2 * dy - dx;

                while (xi != x2 + stepX) {
                    drawPixelSafe(xi, yi, color);
                    if (isDelta < 0) {
                        isDelta += 2 * dy;
                    } else {
                        yi += stepY;
                        isDelta = isDelta + 2 * dy - 2 * dx;
                    }
                    xi += stepX;
                }
            } else {
                int64_t isDelta = 2 * dx - dy;

                while (yi != y2 + stepY) {
                    drawPixelSafe(xi, yi, color);
                    if (isDelta < 0) {
                        isDelta += 2 * dx;
                    } else {
                        xi += stepX;
                        isDelta = isDelta + 2 * dx - 2 * dy;
                    }
                    yi += stepY;
                }
            }
        }

        template <typename Color_T>
        void drawRectEmpty(size_t x, size_t y, size_t width, size_t height, Color_T color) {
            paintStart();
            drawScanline(x, y, width - 1, color);
            drawScanline(x, y + height - 1, width, color);
            for (size_t i = 0; i < height; i++) {
                drawPixel(x, y + i, color);
                drawPixel(x + width - 1, y + i, color);
            }
        }

        template <typename Color_T>
        void drawRect(size_t x, size_t y, size_t width, size_t height, Color_T color) {
            paintStart();
            for (size_t i = 0; i < width; i++) {
                for (size_t j = 0; j < height; j++) {
                    drawPixel(x + i, y + j, color);
                }
            }
        }

        template <typename Color_T>
        void drawPolygon(Polygon2D& p, Color_T color) {
            paintStart();
            const Range2D range = p.range();
            for (size_t y = range[1]; y < range[3]; y++) {
                std::vector<Point2D> pointList = p.intersection(y);
                const size_t length = pointList.size();
                for (size_t j = 1; j < length; j += 2) {
                    const int startX = pointList[j - 1].x;
                    const int endX = pointList[j].x;
                    const int width = endX - startX;
                    drawScanline(startX, y, width + 1, color);
                }
            }
            for (size_t i = 0; i < p.size(); i++) {
                const auto& l = p.getLine(i);
                drawLine(l.p1.x, l.p1.y, l.p2.x, l.p2.y, color);
            }
        }

        template <typename Color_T>
        void drawPolygonEmpty(Polygon2D& p, Color_T color) {
            paintStart();
            const std::vector<Line2D> lineList = p.getLineList();
            for (auto& l : lineList) {
                drawLine(l.p1.x, l.p1.y, l.p2.x, l.p2.y, color);
            }
        }

        template <typename Color_T>
        void drawCircle(int originX, int originY, int radius, Color_T color) {
            paintStart();
            const int r2 = radius * radius;
            int x = radius, y = 0;
            int dx = 1 - 2 * radius, dy = 1;
            int err = 0;

            while (x >= y) {
                for (int i = originX - x; i <= originX + x; i++) {
                    drawPixel(i, originY + y, color);
                    drawPixel(i, originY - y, color);
                }
                for (int i = originX - y; i <= originX + y; i++) {
                    drawPixel(i, originY + x, color);
                    drawPixel(i, originY - x, color);
                }

                y++;
                err += dy;
                dy += 2;
                if (2 * err + dx > 0) {
                    x--;
                    err += dx;
                    dx += 2;
                }
            }
        }

        template <typename Color_T>
        void drawCircleEmpty(int originX, int originY, int radius, Color_T color) {
            paintStart();
            const int r2 = radius * radius;
            int x = radius, y = 0;
            int dx = 1 - 2 * radius, dy = 1;
            int err = 0;

            while (x >= y) {
                drawPixel(originX + x, originY + y, color);
                drawPixel(originX - x, originY + y, color);
                drawPixel(originX + x, originY - y, color);
                drawPixel(originX - x, originY - y, color);
                drawPixel(originX + y, originY + x, color);
                drawPixel(originX - y, originY + x, color);
                drawPixel(originX + y, originY - x, color);
                drawPixel(originX - y, originY - x, color);

                y++;
                err += dy;
                dy += 2;
                if (2 * err + dx > 0) {
                    x--;
                    err += dx;
                    dx += 2;
                }
            }
        }

        rgba alphaMix(const rgba& src, const rgba& dst) {
            const float alpha = src.a / 255.0f;
            const float invAlpha = 1.0f - alpha;

            const uint8_t r = static_cast<uint8_t>(src.r * alpha + dst.r * invAlpha);
            const uint8_t g = static_cast<uint8_t>(src.g * alpha + dst.g * invAlpha);
            const uint8_t b = static_cast<uint8_t>(src.b * alpha + dst.b * invAlpha);
            const uint8_t a = static_cast<uint8_t>(src.a * alpha + dst.a * invAlpha);

            return {r, g, b, a};
        }

        void generateCommonNoise() {
            paintStart();
            for (size_t i = 0; i < texture.area(); i++) {
                const T value = static_cast<T>(rand() % 256);
                texture.data()[i] = value;
            }
        }

        void generatePerlinNoise(size_t freq) {
            paintStart();
            for (size_t i = 0; i < texture.length(); i += texture.channel()) {
                const size_t x = texture.getCol(i);
                const size_t y = texture.getRow(i);
                const float noiseValue = perlinNoise(x, y, freq);
                drawPixel(x, y, rgb{static_cast<uint8_t>(noiseValue * 255)});
            }
        }

        void generateFractalPerlinNoise() {
            paintStart();
            for (size_t i = 0; i < texture.length(); i += texture.channel()) {
                const size_t x = texture.getCol(i);
                const size_t y = texture.getRow(i);
                float noiseValue = 0;
                for (float freq = 4; freq <= 128; freq *= 2) {
                    noiseValue += perlinNoise(x, y, freq) / freq * 2;
                }

                drawPixel(x, y, rgb{static_cast<uint8_t>(noiseValue * 255)});
            }
        }

    private:
        ImageView<T>* imageView;
        TextureBase<T>& texture;

        inline void paintStart() {
            imageView->changed = true;
        }

        // 计算梯度点积
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

        float perlinNoise(size_t x, size_t y, size_t freq) {
            static std::vector<int> perm = generatePermutationTable();
            const size_t girdSize = std::max(texture.width(), texture.height()) / freq;

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
    };
}