#pragma once

template <typename S, typename T>
class CUtlMap {
public:
    auto begin() noexcept {
        return base_;
    }

    auto end() noexcept {
        return base_ + size_;
    }

    [[nodiscard]] auto begin() const {
        return base_;
    }

    [[nodiscard]] auto end() const {
        return base_ + size_;
    }

    T& operator[](int i) {
        return base_[i];
    }

    S& Count() const;
    S& Count();

    T& Element(int i);
    const T& Element(int i) const;
private:
    T* base_ = nullptr;
    S size_ = 0;
};

template <typename S, typename T>
inline S& CUtlMap<S, T>::Count() const {
    return size_;
}

template <typename S, typename T>
inline S& CUtlMap<S, T>::Count() {
    return size_;
}

template <typename S, typename T>
inline T& CUtlMap<S, T>::Element(int i) {
    assert(i < size_);
    return base_[i];
}

template <typename S, typename T>
const T& CUtlMap<S, T>::Element(int i) const {
    assert(i < size_);
    return base_[i];
};
