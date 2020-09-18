#include "math_vector.h"
#include <iostream>

int main() {
    VectorF2 v0(9.f, 15.f);
    VectorF2 v1(3.f, 5.f);
    auto v2 = v0 - v1;

    std::cout << "v0: " << v0 << std::endl;
    std::cout << "v1: " << v1 << std::endl;
    std::cout << "v2: " << v2 << std::endl;

    v2 += v0;
    std::cout << "v2: " << v2 << std::endl;

    v2 -= v2;
    std::cout << "v2: " << v2 << std::endl;

    auto v3 = v2 + 10;
    std::cout << "v3: " << v3 << std::endl;

    (v3 -= 5) /= 5;
    std::cout << "v3: " << v3 << std::endl;

    std::cout << "v3.length_sqr(): " << v3.length_sqr() << std::endl;
    std::cout << "v3.dot(v1): " << v3.dot(v1) << std::endl;

    std::cout << "v3.dist(v1): " << v3.dist(v1) << std::endl;

    VectorF3 v4(std::make_tuple(1.f, 2.f, 3.f));
    std::cout << v4 << std::endl;
}
