#!/bin/bash

# examples:
# scripts/test-cpp.sh ~/games/cs2/
# scripts/test-cpp.sh ~/games/cs2/ cpp

# this is a rudimentary test script to test the C++ output.
# we should switch to a test framework (like GTest) and test
# individual functions and classes instead for better test
# performance, faster debugging, and without using the file system.

set -Eeuxo pipefail

SCRIPT_DIRECTORY="$(dirname "$(readlink -f "$0")")"
PROJECT_ROOT="${SCRIPT_DIRECTORY}/../"

CS2_DIRECTORY="${1}"
TEST_LANGUAGE="${2:-}"

test_language()
{
    local language="${1}"
    # remove contents of sdk directory instead of the directory itself
    # because the user might have a shell open in the sdk directory.
    rm -rf "${PROJECT_ROOT}/sdk/"*
    "${PROJECT_ROOT}/scripts/run.sh" "${CS2_DIRECTORY}" --emit-language "${language}"

    pushd "${PROJECT_ROOT}/sdk/"
    # conan invokes cmake, which compiles "compile-test" binary to check if the SDK compiles
    conan create .
    popd

    echo "success: ${language}"
}

if [ -z ${TEST_LANGUAGE} ]; then
    test_language "cpp"
    test_language "c"
else
    test_language "${TEST_LANGUAGE}"
fi
