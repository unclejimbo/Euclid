// This is the main entry for Catch.
// All test cases reside in seperate files.
// Do not modify or repeat the macro in this file.
// Just include <catch.hpp> in your test files.
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// Only define for once
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// #ifdef CXX_STANDARD
#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include "config.h"

TEST_CASE("Basic setup goes here")
{
    // Create tmp directory if it doesn't exist
    fs::path tmpdir(TMP_DIR);
    if (!fs::exists(tmpdir)) {
        fs::create_directory(tmpdir);
    }
}
