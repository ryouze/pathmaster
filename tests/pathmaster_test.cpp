/**
 * @file pathmaster_test.cpp
 */

#include <cstdlib>     // for EXIT_FAILURE
#include <filesystem>  // for std::filesystem
#include <iostream>    // for std::cout

#include "../include/pathmaster/pathmaster.hpp"

/**
 * @brief Entry-point of the application.
 */
int main()
{
    const std::filesystem::path executable_path = pathmaster::get_executable_path();
    std::cout << "Executable path: " << executable_path << '\n';
    std::cout << "Directory path: " << executable_path.parent_path() << '\n';

    // Check if the executable name is "pathmaster_test"
    return (executable_path.filename().string() == "pathmaster_test") ? 0 : EXIT_FAILURE;
}
