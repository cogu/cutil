# Cutil repo upgrade plan

- [x] Cleanup old CMake constructs (Same as we did in ADT)
- [x] Rename `inc` directory to `include`
- [x] Replace legacy use of bare `0` for pointer values with `NULL`
- [x] Add "cutil_" prefix to function names (third party files excluded)
- [x] Replace all mixed-case and CamelCase naming with snake-case (third party files excluded)
- [x] Modernize file banner in all sources and headers (third party files excluded)
- [x] Add doxygen comments to headers
- [x] Create `docs` directory with Sphinx config (Same as we did in ADT)
- [x] Use the doxygen headers as documentation for new Sphinx html site
- [x] Add more unit tests
- [x] Fix all compiler warnings for all environments (GCC, clang, MSVC)
- [x] Fix all clang-tidy issues
- [x] Delete old eclipse and Visual studio projects
- [x] Rewrite README

## Third party modules

These files are third party and we must not change their copyright, file banner or API names.

- sha256.h, sha256.c
- CMemLeak.h, CMemLeak.c
- CuTest.h, CuTest.c
