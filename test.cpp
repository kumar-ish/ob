#include <iostream>

int main() {
    int y;
    std::tie(std::ignore, y) = std::pair{1, 2};
    std::cout << y << std::endl;
    return 0;
}