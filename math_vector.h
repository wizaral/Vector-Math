#pragma once
#include <array>
#include <cmath>
#include <functional>
#include <tuple>

template <class T, std::size_t S, class = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class Vector final {
    std::array<T, S> m_arr{};

public:
    constexpr Vector() noexcept = default;
    ~Vector() noexcept = default;

    template <class... Args>
    constexpr explicit Vector(Args &&... args) noexcept
    : m_arr({args...}) {}

    template <class... Args>
    constexpr explicit Vector(std::tuple<Args ...> &&args) noexcept
    : m_arr(std::apply([](auto &&... x) constexpr {
        return std::array{std::forward<decltype(x)>(x)...};
    }, std::forward<std::tuple<Args ...>>(args))) {}

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

    [[nodiscard]] constexpr T &operator[](std::size_t index) noexcept {
        return m_arr[index];
    }

    [[nodiscard]] constexpr const T &operator[](std::size_t index) const noexcept {
        return m_arr[index];
    }

    [[nodiscard]] constexpr std::array<T, S> &vector() noexcept {
        return m_arr;
    }

    [[nodiscard]] constexpr const std::array<T, S> &vector() const noexcept {
        return m_arr;
    }

    [[nodiscard]] constexpr Vector operator+(const Vector &v) const noexcept {
        return Vector{operation(m_arr, v.m_arr, std::plus<>{})};
    }

    [[nodiscard]] constexpr Vector operator-(const Vector &v) const noexcept {
        return Vector{operation(m_arr, v.m_arr, std::minus<>{})};
    }

    constexpr Vector &operator+=(const Vector &v) noexcept {
        operation(m_arr, v.m_arr, [](T &lhs, T rhs) constexpr {
            return lhs += rhs;
        });
        return *this;
    }

    constexpr Vector &operator-=(const Vector &v) noexcept {
        operation(m_arr, v.m_arr, [](T &lhs, T rhs) constexpr {
            return lhs -= rhs;
        });
        return *this;
    }

    [[nodiscard]] constexpr Vector operator+(T s) const noexcept {
        return Vector{operation(m_arr, s, std::plus<>{})};
    }

    [[nodiscard]] constexpr Vector operator-(T s) const noexcept {
        return Vector{operation(m_arr, s, std::minus<>{})};
    }

    [[nodiscard]] constexpr Vector operator*(T s) const noexcept {
        return Vector{operation(m_arr, s, std::multiplies<>{})};
    }

    [[nodiscard]] constexpr Vector operator/(T s) const noexcept {
        return Vector{operation(m_arr, s, std::divides<>{})};
    }

    constexpr Vector &operator+=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs += rhs;
        });
        return *this;
    }

    constexpr Vector &operator-=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs -= rhs;
        });
        return *this;
    }

    constexpr Vector &operator*=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs *= rhs;
        });
        return *this;
    }

    constexpr Vector &operator/=(T s) noexcept {
        operation(m_arr, s, [](T &lhs, T rhs) constexpr {
            return lhs /= rhs;
        });
        return *this;
    }

    constexpr Vector &normalize() noexcept {
        auto len = length_sqr();
        return len == 0 ? *this : *this *= (1.l / length(len));
    }

    [[nodiscard]] constexpr T dist(const Vector &v) const {
        return Vector{*this - v}.length();
    }

    [[nodiscard]] static constexpr T dist(const Vector &v1, const Vector &v2) {
        return Vector{v1 - v2}.length();
    }

    [[nodiscard]] constexpr T length() const noexcept {
        return length(length_sqr());
    }

    [[nodiscard]] constexpr T length_sqr() const noexcept {
        return length_sqr(m_arr);
    }

    [[nodiscard]] constexpr T dot(const Vector &v) const noexcept {
        return dot(m_arr, v.m_arr);
    }

    [[nodiscard]] static constexpr T dot(const Vector &v1, const Vector &v2) noexcept {
        return dot(v1.m_arr, v2.m_arr);
    }

    friend inline constexpr std::ostream &operator<<(std::ostream &os, const Vector &v) noexcept {
        return print(os, as_tuple(v.m_arr), std::make_index_sequence<S>{});
    }

private:
    template <class... Args, std::size_t... I>
    inline static constexpr std::ostream &print(std::ostream &os, const std::tuple<Args ...> &t, std::index_sequence<I...>) noexcept {
        return ((os << std::get<I>(t) << ' '), ...);
    }

// ================================================================================================================== //

    // operatorX(Vector, Vector)

    inline static constexpr auto as_tuple(const std::array<T, S> &arr) noexcept {
        return as_tuple(arr, std::make_index_sequence<S>{});
    }

    template <std::size_t... I>
    inline static constexpr auto as_tuple(const std::array<T, S> &arr, std::index_sequence<I...>) noexcept {
        return std::tie(arr[I]...);
    }

    template <class Op>
    inline static constexpr auto operation(const std::array<T, S> &a1, const std::array<T, S> &a2, Op &&op) noexcept {
        return operation(as_tuple(a1), as_tuple(a2), std::forward<Op>(op), std::make_index_sequence<S>{});
    }

    template <class Op, class... Args, std::size_t... I>
    inline static constexpr auto operation(const std::tuple<Args ...> &t1, const std::tuple<Args ...> &t2, Op &&op, std::index_sequence<I...>) noexcept {
        return std::array<T, S>{op(std::get<I>(t1), std::get<I>(t2))...};
    }

// ================================================================================================================== //

    // operatorX=(Vector, Vector)

    inline static constexpr auto as_tuple(std::array<T, S> &arr) noexcept {
        return as_tuple(arr, std::make_index_sequence<S>{});
    }

    template <std::size_t... I>
    inline static constexpr auto as_tuple(std::array<T, S> &arr, std::index_sequence<I...>) noexcept {
        return std::tie(arr[I]...);
    }

    template <class Op>
    inline static constexpr void operation(std::array<T, S> &a1, const std::array<T, S> &a2, Op &&op) noexcept {
        auto tie = as_tuple(a1);
        operation(tie, as_tuple(a2), std::forward<Op>(op), std::make_index_sequence<S>{});
    }

    template <class Op, class... Args, class... Cargs, std::size_t... I>
    inline static constexpr void operation(std::tuple<Args ...> &t1, const std::tuple<Cargs...> &t2, Op &&op, std::index_sequence<I...>) noexcept {
        (op(std::get<I>(t1), std::get<I>(t2)), ...);
    }

// ================================================================================================================== //

    // operatorX(Vector, Scalar)

    template <class Op>
    inline static constexpr auto operation(const std::array<T, S> &a, T s, Op &&op) noexcept {
        return operation(as_tuple(a), s, std::forward<Op>(op), std::make_index_sequence<S>{});
    }

    template <class Op, class... Args, std::size_t... I>
    inline static constexpr auto operation(const std::tuple<Args ...> &t, T s, Op &&op, std::index_sequence<I...>) noexcept {
        return std::array<T, S>{op(std::get<I>(t), s)...};
    }

// ================================================================================================================== //

    // operatorX=(Vector, Scalar)

    template <class Op>
    inline static constexpr void operation(std::array<T, S> &a, T s, Op &&op) noexcept {
        auto tie = as_tuple(a);
        operation(tie, s, std::forward<Op>(op), std::make_index_sequence<S>{});
    }

    template <class Op, class... Args, class... Cargs, std::size_t... I>
    inline static constexpr void operation(std::tuple<Args ...> &t, T s, Op &&op, std::index_sequence<I...>) noexcept {
        (op(std::get<I>(t), s), ...);
    }

// ================================================================================================================== //

    inline static constexpr T length(T len_sqr) noexcept {
        return std::sqrt(len_sqr);
    }

    inline static constexpr T length_sqr(const std::array<T, S> &a) noexcept {
        return length_sqr(as_tuple(a), std::make_index_sequence<S>{});
    }

    template <class... Args, std::size_t... I>
    inline static constexpr T length_sqr(const std::tuple<Args ...> &t, std::index_sequence<I...>) noexcept {
        return ((std::get<I>(t) * std::get<I>(t)) + ...);
    }

// ================================================================================================================== //

    inline static constexpr T dot(const std::array<T, S> &a1, const std::array<T, S> &a2) noexcept {
        return dot(as_tuple(a1), as_tuple(a2), std::make_index_sequence<S>{});
    }

    template <class... Args, std::size_t... I>
    inline static constexpr T dot(const std::tuple<Args ...> &t1, const std::tuple<Args ...> &t2, std::index_sequence<I...>) noexcept {
        return ((std::get<I>(t1) * std::get<I>(t2)) + ...);
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
