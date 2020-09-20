#pragma once
#include <array>
#include <cmath>
#include <functional>
#include <tuple>

template <class T, std::size_t S, class = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class Vector final {
    using Arr = std::array<T, S>;
    Arr m_arr{};

public:
    constexpr Vector() noexcept = default;
    ~Vector() noexcept = default;

    template <class... Args>
    constexpr explicit Vector(Args &&... args) noexcept
    : m_arr({std::forward<Args>(args)...}) {}

    template <class... Args>
    constexpr Vector(std::tuple<Args ...> &&args) noexcept
    : m_arr(std::apply([](auto &&... x) constexpr {
        return std::array{std::forward<decltype(x)>(x)...};
    }, std::forward<std::tuple<Args ...>>(args))) {}

// ================================================================================================================== //

    constexpr Vector(const Vector &v) noexcept
    : m_arr(v.m_arr) {}

    constexpr Vector(Vector &&v) noexcept
    : m_arr(v.m_arr) {}

    constexpr Vector &operator=(const Vector &v) noexcept {
        m_arr = v.m_arr;
        return *this;
    }

    constexpr Vector &operator=(Vector &&v) noexcept {
        m_arr = v.m_arr;
        return *this;
    }

// ================================================================================================================== //

    [[nodiscard]] constexpr T &operator[](std::size_t index) noexcept {
        return m_arr[index];
    }

    [[nodiscard]] constexpr T operator[](std::size_t index) const noexcept {
        return m_arr[index];
    }

// ================================================================================================================== //

    [[nodiscard]] constexpr std::array<T, S> &vector() noexcept {
        return m_arr;
    }

    [[nodiscard]] constexpr const std::array<T, S> &vector() const noexcept {
        return m_arr;
    }

// ================================================================================================================== //

    [[nodiscard]] constexpr Vector operator+(const Vector &v) const noexcept {
        return Vector{operation(m_arr, v.m_arr, std::plus<>{}, std::make_index_sequence<S>{})};
    }

    [[nodiscard]] constexpr Vector operator-(const Vector &v) const noexcept {
        return Vector{operation(m_arr, v.m_arr, std::minus<>{}, std::make_index_sequence<S>{})};
    }

// ================================================================================================================== //

    constexpr Vector &operator+=(const Vector &v) noexcept {
        operation(m_arr, v.m_arr, [](T &lhs, T rhs) constexpr {
            return lhs += rhs;
        }, std::make_index_sequence<S>{});
        return *this;
    }

    constexpr Vector &operator-=(const Vector &v) noexcept {
        operation(m_arr, v.m_arr, [](T &lhs, T rhs) constexpr {
            return lhs -= rhs;
        }, std::make_index_sequence<S>{});
        return *this;
    }

// ================================================================================================================== //

    [[nodiscard]] constexpr Vector operator+(T s) const noexcept {
        return Vector{operation(m_arr, s, std::plus<>{}, std::make_index_sequence<S>{})};
    }

    [[nodiscard]] constexpr Vector operator-(T s) const noexcept {
        return Vector{operation(m_arr, s, std::minus<>{}, std::make_index_sequence<S>{})};
    }

    [[nodiscard]] constexpr Vector operator*(T s) const noexcept {
        return Vector{operation(m_arr, s, std::multiplies<>{}, std::make_index_sequence<S>{})};
    }

    [[nodiscard]] constexpr Vector operator/(T s) const noexcept {
        return Vector{operation(m_arr, s, std::divides<>{}, std::make_index_sequence<S>{})};
    }

// ================================================================================================================== //

    constexpr Vector &operator+=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs += rhs;
        }, std::make_index_sequence<S>{});
        return *this;
    }

    constexpr Vector &operator-=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs -= rhs;
        }, std::make_index_sequence<S>{});
        return *this;
    }

    constexpr Vector &operator*=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs *= rhs;
        }, std::make_index_sequence<S>{});
        return *this;
    }

    constexpr Vector &operator/=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs /= rhs;
        }, std::make_index_sequence<S>{});
        return *this;
    }

// ================================================================================================================== //

    constexpr Vector &normalize() noexcept {
        auto len = length_sqr();
        return len == 0 ? *this : *this *= (T{1} / length(len));
    }

    [[nodiscard]] static constexpr Vector normalize(const Vector &v) noexcept {
        auto len = v.length_sqr();
        return len == 0 ? v : v * (T{1} / length(len));
    }

// ================================================================================================================== //

    constexpr Vector &negate() noexcept {
        operation(m_arr, std::negate<>{}, std::make_index_sequence<S>{});
        return *this;
    }

    [[nodiscard]] constexpr Vector operator-() const noexcept {
        return Vector{operation(m_arr, std::negate<>{}, std::make_index_sequence<S>{})};
    }

// ================================================================================================================== //

    [[nodiscard]] constexpr T dist(const Vector &v) const {
        return dist(*this, v);
    }

    [[nodiscard]] static constexpr T dist(const Vector &v1, const Vector &v2) {
        return Vector{v1 - v2}.length();
    }

// ================================================================================================================== //

    [[nodiscard]] constexpr T length() const noexcept {
        return length(length_sqr());
    }

    [[nodiscard]] constexpr T length_sqr() const noexcept {
        return dot(*this);
    }

// ================================================================================================================== //

    [[nodiscard]] constexpr T operator*(const Vector &v) const noexcept {
        return dot(v);
    }

    [[nodiscard]] constexpr T dot(const Vector &v) const noexcept {
        return dot(*this, v);
    }

    [[nodiscard]] static constexpr T dot(const Vector &v1, const Vector &v2) noexcept {
        return dot(v1.m_arr, v2.m_arr, std::make_index_sequence<S>{});
    }

// ================================================================================================================== //

    friend inline constexpr std::ostream &operator<<(std::ostream &os, const Vector &v) noexcept {
        return print(os, v.m_arr, std::make_index_sequence<S>{});
    }

private:
    template <class... Args, std::size_t... I>
    inline static constexpr std::ostream &print(std::ostream &os, const Arr &a, std::index_sequence<I...>) noexcept {
        return ((os << a[I] << ' '), ...);
    }

// ================================================================================================================== //

    // operatorX(Vector, Vector)
    template <class Op, std::size_t... I>
    inline static constexpr auto operation(const Arr &a1, const Arr &a2, Op &&op, std::index_sequence<I...>) noexcept {
        return Arr{op(a1[I], a2[I])...};
    }

    // operatorX=(Vector, Vector)
    template <class Op, std::size_t... I>
    inline static constexpr void operation(Arr &a1, const Arr &a2, Op &&op, std::index_sequence<I...>) noexcept {
        (op(a1[I], a2[I]), ...);
    }

    // operatorX(Vector, Scalar)
    template <class Op, std::size_t... I>
    inline static constexpr auto operation(const Arr &a, T s, Op &&op, std::index_sequence<I...>) noexcept {
        return Arr{op(a[I], s)...};
    }

    // operatorX=(Vector, Scalar)
    template <class Op, std::size_t... I>
    inline static constexpr void operation(Arr &a, T s, Op &&op, std::index_sequence<I...>) noexcept {
        (op(a[I], s), ...);
    }

    // operator-() const
    template <class Op, std::size_t... I>
    inline static constexpr auto operation(const Arr &a, Op &&op, std::index_sequence<I...>) noexcept {
        return Arr{op(a[I])...};
    }

    // negate
    template <class Op, std::size_t... I>
    inline static constexpr void operation(Arr &a, Op &&op, std::index_sequence<I...>) noexcept {
        ((a[I] = op(a[I])), ...);
    }

// ================================================================================================================== //

    inline static constexpr T length(T len_sqr) noexcept {
        return std::sqrt(len_sqr);
    }

    template <std::size_t... I>
    inline static constexpr T dot(const Arr &a1, const Arr &a2, std::index_sequence<I...>) noexcept {
        return ((a1[I] * a2[I]) + ...);
    }
};

template <class T> using Vector2 = Vector<T, 2>;
template <class T> using Vector3 = Vector<T, 3>;

template <std::size_t S> using VectorC = Vector<int8_t, S>;
template <std::size_t S> using VectorS = Vector<int16_t, S>;
template <std::size_t S> using VectorI = Vector<int32_t, S>;
template <std::size_t S> using VectorL = Vector<int64_t, S>;
template <std::size_t S> using VectorF = Vector<float, S>;
template <std::size_t S> using VectorD = Vector<double, S>;
template <std::size_t S> using VectorR = Vector<long double, S>;

using VectorC2 = VectorC<2>;
using VectorS2 = VectorS<2>;
using VectorI2 = VectorI<2>;
using VectorL2 = VectorL<2>;
using VectorF2 = VectorF<2>;
using VectorD2 = VectorD<2>;
using VectorR2 = VectorR<2>;

using VectorC3 = VectorC<3>;
using VectorS3 = VectorS<3>;
using VectorI3 = VectorI<3>;
using VectorL3 = VectorL<3>;
using VectorF3 = VectorF<3>;
using VectorD3 = VectorD<3>;
using VectorR3 = VectorR<3>;
