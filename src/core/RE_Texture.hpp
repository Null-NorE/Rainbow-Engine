#pragma once
#include "RE_includes.h"
#include "RE_math.h"
#include "RE_debug.h"
#include "RE_RawImage.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/fwd.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <random>
#include <tuple>
#include <vector>

namespace RE {
    using rgb = glm::u8vec3;
    using rgba = glm::u8vec4;
    using hrgb = glm::f32vec3;
    using hrgba = glm::f32vec4;

    template <typename T>
    class TextureBase : public RawImage<T> {
    public:
        TextureBase() : RawImage<T>(0, 0, 0){};
        TextureBase(size_t width, size_t height, size_t channel) : RawImage<T>(width, height, channel){};
        TextureBase(T* arr, size_t width, size_t height, size_t channel) : RawImage<T>(arr, width, height, channel){};

        ~TextureBase(){};
    };

    template <>
    class TextureBase<uint8_t> : public RawImage<uint8_t> {
    public:
        TextureBase() : RawImage<uint8_t>(0, 0, 0){};
        TextureBase(size_t width, size_t height, size_t channel) : RawImage<uint8_t>(width, height, channel) {}
        TextureBase(uint8_t* arr, size_t width, size_t height, size_t channel) : RawImage<uint8_t>(arr, width, height, channel) {}

        ~TextureBase() {}

        void init() {
            this->setZero();
        }

        rgb getRGB(size_t x, size_t y) {
            const size_t index = getIndex(x, y);
            return rgb(_vec[index], _vec[index + 1], _vec[index + 2]);
        }

        rgb getRGB(size_t index) {
            return rgb(_vec[index], _vec[index + 1], _vec[index + 2]);
        }

        rgba getRGBA(size_t x, size_t y) {
            const size_t index = getIndex(x, y);
            return rgba(_vec[index], _vec[index + 1], _vec[index + 2], _vec[index + 3]);
        }

        rgba getRGBA(size_t index) {
            return rgba(_vec[index], _vec[index + 1], _vec[index + 2], _vec[index + 3]);
        }

        void writePicture(const char* filename) {
            int width, height, channel;
            stbi_uc* pixels = stbi_load(filename, &width, &height, &channel, 0);
            setSize(width, height, channel);
            memcpy(_vec.data(), pixels, sizeof(uint8_t) * width * height * channel);
            stbi_image_free(pixels);
        }

        static TextureBase<uint8_t>* loadPicture(const char* filename) {
            int width, height, channel;
            stbi_uc* pixels = stbi_load(filename, &width, &height, &channel, 0);
            auto out = new TextureBase<uint8_t>(pixels, width, height, channel);
            stbi_image_free(pixels);
            return out;
        }
    };

    template <>
    class TextureBase<float> : public RawImage<float> {
    public:
        TextureBase() : RawImage<float>(0, 0, 0){};
        TextureBase(size_t width, size_t height, size_t channel) : RawImage<float>(width, height, channel) {}
        TextureBase(float* arr, size_t width, size_t height, size_t channel) : RawImage<float>(arr, width, height, channel) {}

        ~TextureBase() {}

        hrgb getHRGB(size_t x, size_t y) {
            const size_t index = getIndex(x, y);
            return hrgb(_vec[index], _vec[index + 1], _vec[index + 2]);
        }
        hrgba getHRGBA(size_t x, size_t y) {
            const size_t index = getIndex(x, y);
            return hrgba(_vec[index], _vec[index + 1], _vec[index + 2], _vec[index + 3]);
        }

        void setPixel(size_t x, size_t y, hrgb color) {
            const size_t index = getIndex(x, y);
            _vec[index] = color.r;
            _vec[index + 1] = color.g;
            _vec[index + 2] = color.b;
        }

        void setPixel(size_t index, hrgb color) {
            _vec[index] = color.r;
            _vec[index + 1] = color.g;
            _vec[index + 2] = color.b;
        }

        void setPixel(size_t x, size_t y, hrgba color) {
            const size_t index = getIndex(x, y);
            _vec[index] = color.r;
            _vec[index + 1] = color.g;
            _vec[index + 2] = color.b;
            _vec[index + 3] = color.a;
        }

        void setPixel(size_t index, hrgba color) {
            _vec[index] = color.r;
            _vec[index + 1] = color.g;
            _vec[index + 2] = color.b;
            _vec[index + 3] = color.a;
        }

        void init() {
            this->setZero();
        }
    };

    using Texture = TextureBase<uint8_t>;
    using HDRTexture = TextureBase<float>;

    enum UndersamplingFix {
        none = 0,
        mipmap,
        anisotropy,
    };
    enum TextureWrap {
        repeat = 0,
        clamp,
        mirror,
    };
    enum TextureFilter {
        nearest = 0,
        bilinear,
        bicubic,
    };

    template <typename T = uint8_t>
    class ImageView {
    public:
        ImageView(UndersamplingFix uf = RE::UndersamplingFix::mipmap, TextureWrap tw = clamp, TextureFilter tf = nearest) : ufx(uf) {
            sampler = new Sampler(this, tw, tf);
        };
        ImageView(const ImageView&) = delete;
        ImageView(ImageView&&) = delete;
        ImageView& operator=(const ImageView&) = delete;
        ImageView& operator=(ImageView&&) = delete;
        ~ImageView() {
            delete sampler;
        };

        void update() {
            switch (ufx) {
            case RE::UndersamplingFix::none:
                break;
            case RE::UndersamplingFix::mipmap:
                updateMipmap();
                break;
            case RE::UndersamplingFix::anisotropy:
                updateAnisotropy();
                break;
            }
        }

        TextureBase<T>& getTexture() {
            return this->texture;
        }

        rgb getPixel(float u, float v) {
            return sampler->getPixel(u, v);
        }

    private:
        class Sampler {
        public:
            TextureWrap wrap;
            TextureFilter filter;

            Sampler(ImageView<T>* iv, TextureWrap tw = clamp, TextureFilter tf = nearest) : imageView(iv), wrap(tw), filter(tf){};
            ~Sampler(){};
            Sampler(const Sampler&) = delete;
            Sampler(Sampler&&) = delete;
            Sampler& operator=(const Sampler&) = delete;
            Sampler& operator=(Sampler&&) = delete;

            rgb getPixel(float u, float v) {
                if (imageView->changed) {
                    imageView->update();
                }
                switch (filter) {
                case nearest:
                    return nearestFilter(u, v);
                    break;
                case bilinear:
                    return bilinearFilter(u, v);
                    break;
                case bicubic:
                    return bicubicFilter(u, v);
                    break;
                }
            }

        private:
            ImageView<T>* imageView;
            glm::u64vec2 uv2xy(glm::vec2 uv) {
                const size_t w = imageView->texture.width();
                const size_t h = imageView->texture.height();
                size_t x = uv.x * w;
                size_t y = uv.y * h;
                return glm::u64vec2(x, y);
            }

            glm::vec2 wrapUV(float u, float v) {
                switch (wrap) {
                case repeat:
                    return glm::vec2(std::fmod(u, 1.0f), std::fmod(v, 1.0f));
                case clamp:
                    return glm::vec2(RE::camp(u, 0.0f, 1.0f), RE::camp(v, 0.0f, 1.0f));
                case mirror:
                    return glm::vec2(1 - std::fmod(u, 1.0f), 1 - std::fmod(v, 1.0f));
                }
                return glm::vec2(std::fmod(u, 1.0f), std::fmod(v, 1.0f));
            }

            std::function<rgb(float, float)> nearestFilter = [this](float u, float v) -> rgb {
                const glm::u64vec2 xy = uv2xy(wrapUV(u, v));
                return imageView->texture.getRGB(xy.x, xy.y);
            };

            std::function<rgb(float, float)> bilinearFilter = [this](float u, float v) -> rgb {
                const glm::vec2 uv = wrapUV(u, v);
                const size_t w = imageView->texture.width();
                const size_t h = imageView->texture.height();
                const glm::vec2 dxy{std::fmod(uv.x * w, 1), std::fmod(uv.y * h, 1)};
                const glm::u64vec2 xy = uv2xy(uv);

                const size_t clampXAdd = std::min(xy.x + 1, w - 1);
                const size_t clampYAdd = std::min(xy.y + 1, h - 1);

                const rgb nearestGrid[4] = {
                    imageView->texture.getRGB(xy.x, xy.y),
                    imageView->texture.getRGB(clampXAdd, xy.y),
                    imageView->texture.getRGB(xy.x, clampYAdd),
                    imageView->texture.getRGB(clampXAdd, clampYAdd),
                };

                const rgb insertGrid[2] = {
                    RE::lerp(nearestGrid[0], nearestGrid[1], dxy.x),
                    RE::lerp(nearestGrid[2], nearestGrid[3], dxy.x)};

                return RE::lerp(insertGrid[0], insertGrid[1], dxy.y);
            };

            std::function<rgb(float, float)> bicubicFilter = [this](float u, float v) -> rgb {
                return rgb(0, 0, 0);
            };
        };

        template <typename TN>
        friend class Painter;

        TextureBase<T> texture;
        TextureBase<T> mipmap;
        TextureBase<T> anisotropy;
        Sampler* sampler;
        UndersamplingFix ufx;

        bool changed = false;

        void updateMipmap() {
        }
        void updateAnisotropy() {
        }
    };
}