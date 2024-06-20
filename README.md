# pathmaster

pathmaster is a header-only, cross-platform library that returns the absolute path to the current executable.

![Retrofuturistic laptop](/assets/hero.jpeg)

Image generated using OpenAI's [DALL-E 3](https://openai.com/dall-e-3).


## Motivation

I often need to refer to resources relative to the current executable, such as `vocabulary.json` in my [shogun](https://github.com/ryouze/shogun) project.

```bash
[~/shogun/build] $ tree
.
├── shogun
└── vocabulary.json
```

In Python, I can easily do this using the `__file__` attribute.

```python
from pathlib import Path

vocabulary: Path = Path(__file__).resolve().parent / "vocabulary.json"
```

However, there is no easy way to do this in C++. The `argv[0]` argument to `main` is not guaranteed to be the absolute path to the executable. It could be a relative path, or just the executable name. Referring to the current working directory isn't reliable either, as the executable could be run from anywhere, especially if installed in the system path (e.g., `/usr/local/bin`).

There are multiple libraries that provide this functionality, like [Boost.Filesystem](https://www.boost.org/doc/libs/1_34_0/libs/filesystem/doc/index.htm), [Qt](https://doc.qt.io/qt-5/qcoreapplication.html#applicationFilePath), or [whereami](https://github.com/gpakosz/whereami). However, they are either too heavy or difficult to set up with just FetchContent. My library can be easily added to any CMake project using FetchContent with minimal effort.

```cmake
include(FetchContent)

FetchContent_Declare(pathmaster
    GIT_REPOSITORY https://github.com/ryouze/pathmaster.git
    GIT_TAG        main
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(pathmaster)

target_link_libraries(<your-target> PRIVATE pathmaster)
```

```cpp
#include <pathmaster/pathmaster.hpp>

const std::filesystem::path vocabulary = pathmaster::get_executable_path().parent_path() / "vocabulary.json";
```

Under the hood, pathmaster uses platform-specific code (e.g., `_NSGetExecutablePath`). If a platform is not supported, an exception will be thrown. I aim to support the big three: MacOS, GNU/Linux, and Windows.


## Features

- Written in modern C++ (C++17).
- Header-only.
- Support for CMake's [FetchContent](https://www.foonathan.net/2022/06/cmake-fetchcontent/).
- Comprehensive documentation with doxygen-style comments.
- No third-party dependencies.
- No missing STL headers thanks to [header-warden](https://github.com/ryouze/header-warden).


## Tested Systems

This library has been tested on the following systems:

- MacOS 14.5 (Sonoma)
- Manjaro 24.0 (Wynsdey)

**Note:** Although the library includes Windows-specific code, it has not been tested as I don't have access to a Windows machine. If you encounter any issues, please let me know.


## Requirements

To build this library, you'll need:

- C++17 or higher
- CMake


## Usage

Follow these steps to use the library in your project:

1. **Add the following lines to your CMakeLists.txt**:
    ```cmake
    include(FetchContent)

    FetchContent_Declare(pathmaster
      GIT_REPOSITORY https://github.com/ryouze/pathmaster.git
      GIT_TAG        main
      EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(pathmaster)

    target_link_libraries(<your-target> PRIVATE pathmaster)
    ```

2. **Include the library in your source code**:
    ```cpp
    #include <filesystem>  // for std::filesystem
    #include <iostream>    // for std::cout

    #include <pathmaster/pathmaster.hpp>

    int main()
    {
        const std::filesystem::path executable_path = pathmaster::get_executable_path();
        std::cout << "Executable path: " << executable_path << '\n';
        std::cout << "Directory path: " << executable_path.parent_path() << '\n';
        return 0;
    }
    ```

That's it.

If your platform is supported, this should print the absolute path to the current executable.

```bash
Executable path: "/Users/hikari/Github/example/build/example"
Directory path: "/Users/hikari/Github/example/build"
```

On failure, a `PathMasterError` exception will be thrown. You can catch it and print the error message.

```cpp
#include <cstdlib>     // for EXIT_FAILURE
#include <filesystem>  // for std::filesystem
#include <iostream>    // for std::cout, std::cerr
#include <string>      // for std::string

#include <pathmaster/pathmaster.hpp>

int main()
{
    try {
        const std::filesystem::path executable_path = pathmaster::get_executable_path();
        std::cout << "Executable path: " << executable_path << '\n';
        std::cout << "Directory path: " << executable_path.parent_path() << '\n';
    }
    catch (const pathmaster::PathMasterError &e) {
        std::cerr << std::string(e.what()) + '\n';
        return EXIT_FAILURE;
    }
    return 0;
}
```


## Build

Follow these steps to build the test executable:

1. **Clone the repository**:
    ```bash
    git clone https://github.com/ryouze/pathmaster.git
    ```

2. **Generate the build system**:
    ```bash
    cd pathmaster
    mkdir build && cd build
    cmake ..
    ```

3. **Compile the test executable**:
    ```bash
    make -j
    ```

After successful compilation, you can run the test executable using `./pathmaster_test`.

If your platform is supported, it will print the absolute path to the current executable.

The mode is set to `Release` by default. To build in `Debug` mode, use `cmake -DCMAKE_BUILD_TYPE=Debug ..`.


## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
