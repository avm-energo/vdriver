include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/avm-gen.git
  GIT_TAG        main
)

set(GEN_STATIC ON)
set(GEN_USING_SANITIZERS OFF)
set(GEN_BUILD_TESTS OFF)

FetchContent_MakeAvailable(gen)
