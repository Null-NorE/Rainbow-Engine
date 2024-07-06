#pragma once
#include "RE_Geometry2D.hpp"
#include "RE_Texture.hpp"
#include "RE_includes.h"

namespace RE {
    template <typename T>
    class Painter {
    public:
        Painter(ImageView<T>* iv);
        ~Painter();
        Painter(const Painter&) = delete;
        Painter(Painter&&) = delete;
        Painter& operator=(const Painter&) = delete;
        Painter& operator=(Painter&&) = delete;

        void drawPixel(size_t x, size_t y, rgba color);
        void drawPixel(size_t index, rgba color);
        void drawPixelSafe(size_t x, size_t y, rgba color);
        void drawPixel(size_t x, size_t y, const rgb& color);
        void drawPixel(size_t index, const rgb& color);
        void drawPixelSafe(size_t x, size_t y, const rgb& color);

        void clearImage();

        template <typename Color_T>
        void drawScanline(size_t x, size_t y, size_t width, Color_T color);

        void setSize(size_t width, size_t height, size_t channel);

        template <typename Color_T>
        void drawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Color_T color);

        template <typename Color_T>
        void drawLineSafe(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Color_T color);

        template <typename Color_T>
        void drawRectEmpty(size_t x, size_t y, size_t width, size_t height, Color_T color);

        template <typename Color_T>
        void drawRect(size_t x, size_t y, size_t width, size_t height, Color_T color);

        template <typename Color_T>
        void drawPolygon(Polygon2D& p, Color_T color);

        template <typename Color_T>
        void drawPolygonEmpty(Polygon2D& p, Color_T color);

        template <typename Color_T>
        void drawCircle(int originX, int originY, int radius, Color_T color);

        template <typename Color_T>
        void drawCircleEmpty(int originX, int originY, int radius, Color_T color);

        rgba alphaMix(const rgba& src, const rgba& dst);

#ifdef RE_EXTEND_NOISE_GENERATOR
        template <typename TF>
        friend void generateCommonNoise(RE::Painter<TF>* painter);
        template <typename TF>
        friend void generatePerlinNoise(RE::Painter<TF>* painter, size_t freq);
        template <typename TF>
        friend void generateFractalPerlinNoise(RE::Painter<TF>* painter);
        template <typename TF>
        friend float perlinNoise(RE::Painter<TF>* painter, size_t x, size_t y, size_t freq);
#endif
    private:
        ImageView<T>* imageView;
        TextureBase<T>& texture;

        inline void paintStart();
    };
}

namespace RE {
    template <typename T>
    Painter<T>::Painter(ImageView<T>* iv) : imageView(iv), texture(iv->texture){};
    template <typename T>
    Painter<T>::~Painter() {
        imageView->update();
        imageView->changed = false;
    };
    template <typename T>
    void Painter<T>::drawPixel(size_t x, size_t y, rgba color) {
        paintStart();
        const size_t index = texture.getIndex(x, y);
        color = alphaMix(texture.getRGBA(x, y), color);

        texture.data()[index] = color.r;
        texture.data()[index + 1] = color.g;
        texture.data()[index + 2] = color.b;
        texture.data()[index + 3] = color.a;
    }
    template <typename T>
    void Painter<T>::drawPixel(size_t index, rgba color) {
        paintStart();
        color = alphaMix(texture.getRGBA(index), color);

        texture.data()[index] = color.r;
        texture.data()[index + 1] = color.g;
        texture.data()[index + 2] = color.b;
        texture.data()[index + 3] = color.a;
    }
    template <typename T>
    void Painter<T>::drawPixelSafe(size_t x, size_t y, rgba color) {
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

    template <typename T>
    void Painter<T>::drawPixel(size_t x, size_t y, const rgb& color) {
        paintStart();
        const size_t&& index = texture.getIndex(x, y);

        texture.data()[index] = color.r;
        texture.data()[index + 1] = color.g;
        texture.data()[index + 2] = color.b;
    }

    template <typename T>
    void Painter<T>::drawPixel(size_t index, const rgb& color) {
        paintStart();
        texture.data()[index] = color.r;
        texture.data()[index + 1] = color.g;
        texture.data()[index + 2] = color.b;
    }

    template <typename T>
    void Painter<T>::drawPixelSafe(size_t x, size_t y, const rgb& color) {
        paintStart();
        if (x < texture.width() && x >= 0 && y >= 0 && y < texture.height()) {
            const size_t&& index = texture.getIndex(x, y);

            texture.data()[index] = color.r;
            texture.data()[index + 1] = color.g;
            texture.data()[index + 2] = color.b;
        }
    }

    template <typename T>
    void Painter<T>::clearImage() {
        paintStart();
        texture.setZero();
    }

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawScanline(size_t x, size_t y, size_t width, Color_T color) {
        paintStart();
        for (size_t i = 0; i < width; i++) {
            drawPixel(x + i, y, color);
        }
    }

    template <typename T>
    void Painter<T>::setSize(size_t width, size_t height, size_t channel) {
        texture.setSize(width, height, channel);
    }

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Color_T color) {
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

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawLineSafe(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Color_T color) {
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

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawRectEmpty(size_t x, size_t y, size_t width, size_t height, Color_T color) {
        paintStart();
        drawScanline(x, y, width - 1, color);
        drawScanline(x, y + height - 1, width, color);
        for (size_t i = 0; i < height; i++) {
            drawPixel(x, y + i, color);
            drawPixel(x + width - 1, y + i, color);
        }
    }

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawRect(size_t x, size_t y, size_t width, size_t height, Color_T color) {
        paintStart();
        for (size_t i = 0; i < width; i++) {
            for (size_t j = 0; j < height; j++) {
                drawPixel(x + i, y + j, color);
            }
        }
    }

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawPolygon(Polygon2D& p, Color_T color) {
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

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawPolygonEmpty(Polygon2D& p, Color_T color) {
        paintStart();
        const std::vector<Line2D> lineList = p.getLineList();
        for (auto& l : lineList) {
            drawLine(l.p1.x, l.p1.y, l.p2.x, l.p2.y, color);
        }
    }

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawCircle(int originX, int originY, int radius, Color_T color) {
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

    template <typename T>
    template <typename Color_T>
    void Painter<T>::drawCircleEmpty(int originX, int originY, int radius, Color_T color) {
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

    template <typename T>
    rgba Painter<T>::alphaMix(const rgba& src, const rgba& dst) {
        const float alpha = src.a / 255.0f;
        const float invAlpha = 1.0f - alpha;

        const uint8_t r = static_cast<uint8_t>(src.r * alpha + dst.r * invAlpha);
        const uint8_t g = static_cast<uint8_t>(src.g * alpha + dst.g * invAlpha);
        const uint8_t b = static_cast<uint8_t>(src.b * alpha + dst.b * invAlpha);
        const uint8_t a = static_cast<uint8_t>(src.a * alpha + dst.a * invAlpha);

        return {r, g, b, a};
    }

    template <typename T>
    inline void Painter<T>::paintStart() {
        imageView->changed = true;
    }
}