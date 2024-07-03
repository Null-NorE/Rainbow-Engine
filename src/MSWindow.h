#pragma once
#include "REInit.h"

namespace RE {
    class MSWindow {
    public:
        MSWindow(size_t w, size_t h, const char* title = "RainbowEngine") : _width(w), _height(h) {
            _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
            if (_window == nullptr) {
                std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
                SDL_Quit();
            }

            _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
            if (_renderer == NULL) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
                SDL_DestroyWindow(_window);
                SDL_Quit();
            }

            _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, w, h);
            if (_texture == nullptr) {
                std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
                SDL_DestroyRenderer(_renderer);
                SDL_DestroyWindow(_window);
                SDL_Quit();
            }
        };
        ~MSWindow() {
            SDL_DestroyTexture(_texture);
            SDL_DestroyRenderer(_renderer);
            SDL_DestroyWindow(_window);
        };

        void drawToBuffer(const SDL_Rect* rect, const void* pixels, int pitch) {
            SDL_UpdateTexture(_texture, rect, pixels, pitch);

            SDL_RenderClear(_renderer);
            SDL_RenderCopy(_renderer, _texture, NULL, NULL);
        }

        void present() {
            SDL_RenderPresent(_renderer);
        }

        size_t width() const { return _width; }
        size_t height() const { return _height; }

    private:
        size_t _width, _height;
        SDL_Window* _window;
        SDL_Renderer* _renderer;
        SDL_Texture* _texture;
    };

    // TODO 交换链
    class ExchangeChain {
    public:
        ExchangeChain(){};
        ~ExchangeChain(){};

    private:
    };
}