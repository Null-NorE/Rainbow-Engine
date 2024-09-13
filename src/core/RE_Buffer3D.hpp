#pragma once
#include "RE_includes.h"

namespace RE {
    template <typename T>
    class Buffer3D {
    public:
        Buffer3D(size_t width, size_t height, size_t channel);
        Buffer3D(T* arr, size_t width, size_t height, size_t channel);
        ~Buffer3D(){};
        Buffer3D(Buffer3D<T>& other);
        Buffer3D(Buffer3D<T>&& other) noexcept;
        Buffer3D<T>& operator=(Buffer3D<T>& other);
        Buffer3D<T>&& operator=(Buffer3D<T>&& other) noexcept;

        template <typename FN_T>
        static auto mix(Buffer3D<T>* buffer1, Buffer3D<T>* buffer2, Buffer3D<T>* result, FN_T func) {
            return std::transform(buffer1->_data.begin(), buffer1->_data.end(), buffer2->_data.begin(), result->_data.begin(), func);
        }

        void copyFrom(T* arr);
        void copyFrom(Buffer3D<T>* dst);

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
        std::vector<T> _data;

    private:
        size_t _width, _height, _channel;
        size_t _length;
        size_t _area;

        void attributeCopy(Buffer3D<T>& other);
    };
}

namespace RE {

    template <typename T>
    Buffer3D<T>::Buffer3D(Buffer3D<T>& other) {
        attributeCopy(other);
        memcpy(other.data(), data(), sizeof(T) * _length);
    }

    template <typename T>
    Buffer3D<T>::Buffer3D(Buffer3D<T>&& other) noexcept {
        attributeCopy(other);
        other._data = _data;
    }

    template <typename T>
    Buffer3D<T>& Buffer3D<T>::operator=(Buffer3D<T>& other) {
        attributeCopy(other);
        memcpy(other.data(), data(), sizeof(T) * _length);
    }

    template <typename T>
    Buffer3D<T>&& Buffer3D<T>::operator=(Buffer3D<T>&& other) noexcept {
        attributeCopy(other);
        other._data = _data;
    }

    template <typename T>
    void Buffer3D<T>::copyFrom(T* arr) {
        memcpy(arr, _data.data(), sizeof(T) * _length);
    }

    template <typename T>
    void Buffer3D<T>::copyFrom(Buffer3D<T>* dst) {
        memcpy(dst->data(), _data.data(), sizeof(T) * _length);
    }

    template <typename T>
    T* Buffer3D<T>::data() {
        return _data.data();
    }

    template <typename T>
    void Buffer3D<T>::clear() {
        _data.clear();
    }

    template <typename T>
    void Buffer3D<T>::setZero() {
        memset(_data.data(), 0, sizeof(T) * _width * _height * _channel);
    }

    template <typename T>
    void Buffer3D<T>::setSize(size_t width, size_t height, size_t channel) {
        _width = width;
        _height = height;
        _channel = channel;
        _length = width * height * channel;
        _area = width * height;
        _data.resize(_length);
    }

    template <typename T>
    size_t Buffer3D<T>::getRow(size_t index) const {
        return (index / _channel) / _width;
    }

    template <typename T>
    size_t Buffer3D<T>::getCol(size_t index) const {
        return (index / _channel) % _width;
    }

    template <typename T>
    size_t Buffer3D<T>::getIndex(size_t col, size_t row) const {
        return row * (_width * _channel) + col * _channel;
    }

    template <typename T>
    T& Buffer3D<T>::operator[](size_t index) {
        return _data[index];
    }

    template <typename T>
    size_t Buffer3D<T>::width() const { return _width; }
    template <typename T>
    size_t Buffer3D<T>::height() const { return _height; }
    template <typename T>
    size_t Buffer3D<T>::channel() const { return _channel; }
    template <typename T>
    size_t Buffer3D<T>::length() const { return _length; }
    template <typename T>
    size_t Buffer3D<T>::area() const { return _area; }

    template <typename T>
    void Buffer3D<T>::attributeCopy(Buffer3D<T>& other) {
        _width = other._width;
        _height = other._height;
        _channel = other._channel;
        _length = other._length;
        _area = other._area;
    }

    template <typename T>
    inline Buffer3D<T>::Buffer3D(size_t w, size_t h, size_t c) : _width(w), _height(h), _channel(c) {
        _data.resize(w * h * c);
        _length = _width * _height * _channel;
        _area = _width * _height;
    }

    template <typename T>
    inline Buffer3D<T>::Buffer3D(T* arr, size_t w, size_t h, size_t c) : _width(w), _height(h), _channel(c) {
        _data.resize(w * h * c);
        this->copyFrom(arr);
        _length = _width * _height * _channel;
        _area = _width * _height;
    }
}