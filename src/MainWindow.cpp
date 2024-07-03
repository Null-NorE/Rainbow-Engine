#include "core/MainWindow.h"
#include "core/RE_Init.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <omp.h>

int main(int argc, char* argv[]) {
    // 初始化SDL
    initSDLVideo();
    int refreshRate = GetRefreshRate();
    std::cout << "Current display refresh rate: " << refreshRate << " Hz" << std::endl;

    const size_t width = 800;
    const size_t height = 600;
    // 创建窗口
    RE::MSWindow window(width, height);

    // #pragma omp parallel for num_threads(20)
    //     for (long long int i = 0; i < 10; i++) {
    //         REDebug << i << i << i << i << i << i << i;
    //     }

    const size_t IMAGE_CHANNELS = 3;
    const size_t IMAGE_SIZE = width * height;
    auto imageView = new RE::ImageView<uint8_t>(RE::UndersamplingFix::none, RE::TextureWrap::clamp, RE::TextureFilter::nearest);
    RE::Painter<uint8_t> pt(imageView);
    pt.setSize(width, height, 3);
    RE::Polygon2D poly({
        {80, 100},
        {150, 50},
        {280, 100},
        {200, 200},
        {100, 200},
    });

    // 在image数组中填充示例数据，这里用一些简单的颜色来模拟图像数据
    for (size_t i = 0; i < IMAGE_SIZE * IMAGE_CHANNELS; i += IMAGE_CHANNELS) {
        pt.drawPixel(i, RE::rgb(0xff, 0xff, 0xff));
    }

    // 主循环
    bool quit = false;
    SDL_Event e;
    const Uint32 frameDelay = 1000 / refreshRate;
    Uint32 frameStart;
    int frameTime;

    while (!quit) {
        frameStart = SDL_GetTicks64();

        // 处理事件
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        pt.generateFractalPerlinNoise();

        // 在这里进行渲染
        // 在image数组中填充示例数据，这里用一些简单的颜色来模拟图像数据
        auto t = SDL_GetTicks64();
        int zq = 2000;

        int ts = 5;
        int rg = IMAGE_SIZE * IMAGE_CHANNELS / ts;
        for (size_t k = 0; k < ts; k++) {
            for (size_t i = k * rg; i < (k + 1) * rg - 400; i += IMAGE_CHANNELS) {
                RE::rgb color(0.0f);
                if (static_cast<int>(t / zq) % 2) {
                    color.r = RE::lerp(0, 256, (t % zq) / static_cast<float>(zq)) * imageView->getTexture().data()[i] / 256; // 红色
                } else {
                    color.r = RE::lerp(256, 0, (t % zq) / static_cast<float>(zq)) * imageView->getTexture().data()[i] / 256; // 红色
                }
                color.g = imageView->getTexture().getCol(i) % 256 * imageView->getTexture().data()[i + 1] / 256;
                color.b = imageView->getTexture().getRow(i) % 256 * imageView->getTexture().data()[i + 2] / 256;

                pt.drawPixel(i, color);
            }
        }

        const float delta = t % 3600 / 10;
        for (float i = delta; i <= 90 + delta; i += 10) {
            pt.drawLine(400, 300, 200 * sin(i / 180 * 3.14159) + 400, 200 * cos(i / 180 * 3.14159) + 300, RE::rgb(255, 255, 255));
        }

        pt.drawRect(30, 30, 80, 60, RE::rgb(255, 255, 255));
        pt.drawRectEmpty(200, 200, 80, 60, RE::rgb(255, 255, 255));
        pt.drawCircle(400, 300, 100, RE::rgb(255, 255, 255));
        pt.drawCircleEmpty(400, 300, 200, RE::rgb(255, 255, 255));

        // pt.drawPolygon(poly, RE::rgb(255, 255, 255));
        pt.drawPolygonEmpty(poly, RE::rgb(255, 255, 255));

        // 写入缓冲区
        window.drawToBuffer(nullptr, imageView->getTexture().data(), width * IMAGE_CHANNELS);

        // 更新窗口
        window.present();

        // 控制帧率
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay((frameDelay - frameTime) * 10);
        }
    }

    // 清理SDL
    quitSDL();
    return 0;
}