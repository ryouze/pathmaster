/**
 * @file pathmaster.hpp
 *
 * @brief Return the absolute path to the current executable.
 */

#pragma once

#include <exception>   // for std::exception
#include <filesystem>  // for std::filesystem
#include <stdexcept>   // for std::runtime_error
#include <string>      // for std::string
#include <vector>      // for std::vector

#if defined(__APPLE__)
#include <cstdint>          // for std::uint32_t
#include <mach-o/dyld.h>    // for _NSGetExecutablePath
#include <sys/syslimits.h>  // for PATH_MAX
#elif defined(_WIN32)
#include <windows.h>  // for GetModuleFileNameW, DWORD, MAX_PATH
#endif

/**
 * @namespace pathmaster
 *
 * @brief Contains functions and classes related to retrieving the absolute path of the current executable.
 */
namespace pathmaster {

/**
 * @brief Exception thrown when the executable path cannot be retrieved.
 *
 * @note This is derived from std::runtime_error.
 */
class PathMasterError : public std::runtime_error {
  public:
    explicit PathMasterError(const std::string &message)
        : std::runtime_error("PathMasterError: " + message) {}
};

/**
 * @brief Get the absolute path to the current executable.
 *
 * @return Absolute path to the executable (e.g., "/Users/hikari/Github/example/build/example").
 *
 * @throws PathMasterError If failed to get the executable path.
 *
 * @note This supports GNU/Linux, MacOS, and Windows. Other platforms are not supported.
 */
[[nodiscard]] std::filesystem::path get_executable_path()
{
#if defined(__linux__)
    // Get the path using read_symlink, skip creating a buffer
    try {
        return std::filesystem::canonical(std::filesystem::read_symlink("/proc/self/exe"));
    }
    catch (const std::exception &e) {
        throw PathMasterError("Failed to get the executable path on GNU/Linux: " + std::string(e.what()));
    }
#elif defined(__APPLE__)
    try {
        // Initialize the buffer with PATH_MAX, resize if needed
        std::vector<char> buffer(PATH_MAX);
        std::uint32_t size = static_cast<std::uint32_t>(buffer.size());
        if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
            buffer.resize(size);
            if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
                throw PathMasterError("Failed to get the executable path on MacOS after resizing the buffer");
            }
        }
        return std::filesystem::canonical(buffer.data());
    }
    catch (const std::exception &e) {  // This will also catch bad_alloc
        throw PathMasterError("Failed to canonicalize the path on MacOS: " + std::string(e.what()));
    }
#elif defined(_WIN32)
    try {
        // Initialize the buffer with the maximum possible path length on Windows
        std::vector<wchar_t> buffer(32767);
        const DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (size == 0) {
            throw PathMasterError("Failed to get the executable path on Windows");
        }
        return std::filesystem::canonical(std::filesystem::path(buffer.data(), size));
    }
    catch (const std::exception &e) {  // This will also catch bad_alloc
        throw PathMasterError("Failed to canonicalize the path on Windows: " + std::string(e.what()));
    }
#else
    throw PathMasterError("Unsupported platform");
#endif
}

}  // namespace pathmaster
