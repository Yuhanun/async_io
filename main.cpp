#define ASYNC_IO_HEADER_ONLY
#include "async_io.hpp"

int main() {
    std::cout << " Here" << std::endl;
    async_io::async_io io_buffer{ };
    std::cout << "here2" << std::endl;
    io_buffer.write("Hello world!");
    std::cout << "here3" << std::endl;
}