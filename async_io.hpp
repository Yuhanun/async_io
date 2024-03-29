#pragma once

#ifdef ASYNC_IO_HEADER_ONLY
#define ASYNC_IO_FUNC inline
#else
#define ASYNC_IO_FUNC
#endif

#include <atomic>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <type_traits>

namespace async_io {
    class _writeable {
    public:
        std::unique_ptr<std::ostream> _o_stream;
        std::ostream* _o_stream_ptr;
        std::string write_str;

        ASYNC_IO_FUNC _writeable(std::unique_ptr<std::ostream> _u_ptr, std::string str)
            : _o_stream{ std::move(_u_ptr) }, _o_stream_ptr{ _o_stream.get() }, write_str{ std::move(str) } {
        }

        ASYNC_IO_FUNC _writeable(std::ostream* _u_ptr, std::string str)
            : _o_stream_ptr{ _u_ptr }, write_str{ std::move(str) } {
        }

        ASYNC_IO_FUNC std::ostream& get() {
            return *_o_stream_ptr;
        }

        void write() {
            get() << write_str << std::flush;
        }

        _writeable& operator=(_writeable const&) = delete;
        _writeable& operator=(_writeable&&) = default;

        _writeable(_writeable const&) = delete;
        _writeable(_writeable&& other) noexcept {
            _o_stream = std::move(other._o_stream);
            _o_stream_ptr = other._o_stream_ptr;
            other._o_stream_ptr = nullptr;
            write_str = std::move(other.write_str);
        }

        ~_writeable() = default;
    };

    class async_io {
    public:
        bool _is_active{ true };
        std::unique_ptr<std::thread> _write_thread;
        std::queue<_writeable> _write_queue;
        std::unique_ptr<std::ostream> _o_stream;
        std::ostream* _o_stream_ptr;

        ASYNC_IO_FUNC async_io(std::ostream* stream)
            : _o_stream_ptr{ stream } {
            start_thread();
        }

        ASYNC_IO_FUNC async_io(std::unique_ptr<std::ostream> _ostream_ptr)
            : _o_stream{ std::move(_ostream_ptr) }, _o_stream_ptr{ _ostream_ptr.get() } {
            start_thread();
        }

        ASYNC_IO_FUNC async_io() {
            _o_stream_ptr = &std::cout;
            start_thread();
        }

        ASYNC_IO_FUNC void start_thread() {
            _write_thread = std::make_unique<std::thread>(
                std::bind(
                    &async_io::_write_impl, this));
        }

        ASYNC_IO_FUNC ~async_io() {
            cleanup();
        }

        ASYNC_IO_FUNC void cleanup() {
            _is_active = false;
            _write_thread->join();
        }

        ASYNC_IO_FUNC void write(std::string to_write) {
            if (_o_stream) {
                _write_queue.emplace(_o_stream.get(), std::move(to_write));
            } else if (_o_stream_ptr) {
                _write_queue.emplace(_o_stream_ptr, std::move(to_write));
            } else {
                _write_queue.emplace(&std::cout, std::move(to_write));
            }
        }

        ASYNC_IO_FUNC void write(std::unique_ptr<std::ostream> stream, std::string to_write) {
            _write_queue.emplace(std::move(stream), std::move(to_write));
        }

        ASYNC_IO_FUNC void write(std::ostream* stream, std::string to_write) {
            _write_queue.emplace(stream, std::move(to_write));
        }

    private:
        ASYNC_IO_FUNC void _write_impl() {
            while (_is_active) {
                if (!_write_queue.empty()) {
                    auto element = std::move(_write_queue.front());
                    _write_queue.pop();
                    element.write();
                }
            }
            while (!_write_queue.empty()) {
                auto element = std::move(_write_queue.front());
                _write_queue.pop();
                element.write();
            }
        }
    };
}  // namespace async_io