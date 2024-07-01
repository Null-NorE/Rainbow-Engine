#pragma once
#include <mutex>
#include <iostream>

class REDebugStream {
public:
    REDebugStream() {
        isWriting.lock();
    }

    ~REDebugStream() {
        std::cout << std::endl;
        isWriting.unlock();
    }

    template<typename T>
    REDebugStream& operator<<(const T& arg) {
        //if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, std::string>) {
        //    std::cout << arg; // 输入类型为文本时不加逗号
        //} else 
        if (_outputCount) {
            std::cout << ", " << arg;
        } else {
            std::cout << arg;
        }
        _outputCount++;
        return *this;
    }

    REDebugStream& operator<<(std::ostream& (*manipulator)(std::ostream&)) {
        manipulator(std::cout);
        return *this;
    }
private:
    static std::mutex isWriting;
    size_t _outputCount = 0;
};

std::mutex REDebugStream::isWriting;

void reBlank() {
    std::cout << std::endl;
}

#define REDebug REDebugStream()
#define REBlank reBlank()

#ifdef GLM_ENABLE_EXPERIMENTAL

// 通用的输出运算符<<重载模板
template<typename T, glm::length_t L, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<L, T, Q>& vec) {
    os << "[";
    for (glm::length_t i = 0; i < L; ++i) {
        os << vec[i];
        if (i != L - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template<glm::length_t L, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<L, uint8_t, Q>& vec) {
    os << "[";
    for (glm::length_t i = 0; i < L; ++i) {
        os << static_cast<int>(vec[i]);
        if (i != L - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// 重载输出运算符<<以支持将mat4对象输出到ostream
std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            os << m[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

#endif // GLM_ENABLE_EXPERIMENTAL