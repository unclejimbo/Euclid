// This is the main entry for Catch.
// All test cases reside in seperate files.
// Do not modify or repeat the macro in this file.
// Just include <catch.hpp> in your test files.
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "config.h"

TEST_CASE("Basic setup goes here")
{
    // Create tmp directory if it doesn't exist
    fs::path tmpdir(TMP_DIR);
    if (!fs::exists(tmpdir)) { fs::create_directory(tmpdir); }
}
