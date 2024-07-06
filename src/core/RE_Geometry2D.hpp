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
}