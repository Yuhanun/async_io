#define ASYNC_IO_HEADER_ONLY
#include "async_io.hpp"

int main() {
    async_io::async_io io_buffer{};
    io_buffer.write("Test\n");
    io_buffer.write(
        std::make_unique<std::ofstream>("test_stream.txt", std::ios::app),
        "Hello World\n");
}