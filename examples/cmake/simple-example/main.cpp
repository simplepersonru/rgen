#include <iostream>

#include "mylib/enum.h"
#include <rgen/enum.h>

int main()
{
    std::cout << rgen::Enum<Color>::toString(Color::Red) << std::endl;
    auto color = rgen::Enum<Color>::fromString("blue");
    std::cout << rgen::Enum<Color>::toString(color) << std::endl;
}
