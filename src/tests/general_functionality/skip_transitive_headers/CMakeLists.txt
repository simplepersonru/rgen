set(TEST_PROJECT_NAME skip_transitive_headers_test)

set(GEN_HEADERS enum_notgen.h enum_gen.h)

set(MODULES Enum)

rgen_test_generate(${TEST_PROJECT_NAME} GEN_HEADERS MODULES)
