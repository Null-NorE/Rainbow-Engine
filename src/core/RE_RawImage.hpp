#pragma once
#include "RE_includes.h"

namespace RE {
    template <typename T>
    class RawImage {
    public:
        RawImage(size_t width, size_t height, size_t channel);
        RawImage(T* arr, size_t width, size_t height, size_t channel);
        ~RawImage(){};
        RawImage(RawImage<T>& other);
        RawImage(RawImage<T>&& other) noexcept;
        RawImage<T>& operator=(RawImage<T>& other);
        RawImage<T>&& operator=(RawImage<T>&& other) noexcept;

        template <typename FN_T>
        static auto mix(RawImage<T>* vec1, RawImage<T>* vec2, RawImage<T>* result, FN_T func) {
            return std::transform(vec1->_vec.begin(), vec1->_vec.end(), vec2->_vec.begin(), result->_vec.begin(), func);
        }

        void copyFrom(T* arr);
        void copyFrom(RawImage<T>* dst);

        T* data();
        void clear();
        void setZero();
        void setSize(size_t width, size_t height, size_t channel);

        // start from 0 , 相当于 y
        size_t getRow(size_t index) const;

        // start from 0 , 相当于 x
        size_t getCol(size_t index) const;

        // start from 0
        size_t getIndex(size_t col, size_t row) const;

        T& operator[](size_t index);

        size_t width() const;
        size_t height() const;
        size_t channel() const;

        // length = with * height * channel
        size_t length() const;

        // area = with * height
        size_t area() const;

    protected:
        std::vector<T> _vec;

    private:
        size_t _width, _height, _channel;
        size_t _length;
        size_t _area;

        void attributeCopy(RawImage<T>& other);
    };
}

namespace RE {

    template <typename T>
    RawImage<T>::RawImage(RawImage<T>& other) {
        attributeCopy(other);
        memcpy(other.data(), data(), sizeof(T) * _length);
    }

    template <typename T>
    RawImage<T>::RawImage(RawImage<T>&& other) noexcept {
        attributeCopy(other);
        other._vec = _vec;
    }

    template <typename T>
    RawImage<T>& RawImage<T>::operator=(RawImage<T>& other) {
        attributeCopy(other);
        memcpy(other.data(), data(), sizeof(T) * _length);
    }

    template <typename T>
    RawImage<T>&& RawImage<T>::operator=(RawImage<T>&& other) noexcept {
        attributeCopy(other);
        other._vec = _vec;
    }

    template <typename T>
    void RawImage<T>::copyFrom(T* arr) {
        memcpy(arr, _vec.data(), sizeof(T) * _length);
    }

    template <typename T>
    void RawImage<T>::copyFrom(RawImage<T>* dst) {
        memcpy(dst->data(), _vec.data(), sizeof(T) * _length);
    }

    template <typename T>
    T* RawImage<T>::data() {
        return _vec.data();
    }

    template <typename T>
    void RawImage<T>::clear() {
        _vec.clear();
    }

    template <typename T>
    void RawImage<T>::setZero() {
        memset(_vec.data(), 0, sizeof(T) * _width * _height * _channel);
    }

    template <typename T>
    void RawImage<T>::setSize(size_t width, size_t height, size_t channel) {
        _width = width;
        _height = height;
        _channel = channel;
        _length = width * height * channel;
        _area = width * height;
        _vec.resize(_length);
    }

    template <typename T>
    size_t RawImage<T>::getRow(size_t index) const {
        return (index / _channel) / _width;
    }

    template <typename T>
    size_t RawImage<T>::getCol(size_t index) const {
        return (index / _channel) % _width;
    }

    template <typename T>
    size_t RawImage<T>::getIndex(size_t col, size_t row) const {
        return row * (_width * _channel) + col * _channel;
    }

    template <typename T>
    T& RawImage<T>::operator[](size_t index) {
        return _vec[index];
    }

    template <typename T>
    size_t RawImage<T>::width() const { return _width; }
    template <typename T>
    size_t RawImage<T>::height() const { return _height; }
    template <typename T>
    size_t RawImage<T>::channel() const { return _channel; }
    template <typename T>
    size_t RawImage<T>::length() const { return _length; }
    template <typename T>
    size_t RawImage<T>::area() const { return _area; }

    template <typename T>
    void RawImage<T>::attributeCopy(RawImage<T>& other) {
        _width = other._width;
        _height = other._height;
        _channel = other._channel;
        _length = other._length;
        _area = other._area;
    }

    template <typename T>
    inline RawImage<T>::RawImage(size_t w, size_t h, size_t c) : _width(w), _height(h), _channel(c) {
        _vec.resize(w * h * c);
        _length = _width * _height * _channel;
        _area = _width * _height;
    }

    template <typename T>
    inline RawImage<T>::RawImage(T* arr, size_t w, size_t h, size_t c) : _width(w), _height(h), _channel(c) {
        _vec.resize(w * h * c);
        this->copyFrom(arr);
        _length = _width * _height * _channel;
        _area = _width * _height;
    }
}