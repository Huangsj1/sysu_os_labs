#include <iostream>

extern "C" void function_from_cpp() {
    std::cout << "This is a function from c++!" << std::endl;
}
