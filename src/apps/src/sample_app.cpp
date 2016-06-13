#include <iostream>

#include <linal/vec.hpp>


int main()
{
    Linal::Vec<int> v{ 1, 2 }, w{ 1, 2 }, u{ 3, 4 };
    std::cout << (v == w) << std::endl;
    std::cout << (v == u) << std::endl;

    std::cout << v << " " << u;
    return 0;
}