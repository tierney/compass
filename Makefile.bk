CFLAGS := -Wall -ggdb2 -pthread -fPIC -Wno-sign-compare -Wno-format
CXXFLAGS := $(CFLAGS) -std=gnu++11

prop_logic_main:
	$(CXX) $(CXXFLAGS) -o $@ prop_logic_main.cc

# $(shell mkdir -p build)

# SRC := $(wildcard src/*.cc)
# HEADERS := $(wildcard */*.h) Makefile
# INCLUDE := -I. -Isrc
# OBJ := $(patsubst src/%.cc,build/%.o,$(SRC))

# TEST_SRC := $(wildcard test/*.cc)
# TESTS := $(patsubst test/%.cc,build/%,$(TEST_SRC))

# CFLAGS := -Wall -ggdb2 -pthread -fPIC -Wno-sign-compare -Wno-format
# CXXFLAGS := $(CFLAGS) -std=gnu++11

# # OMPI_CXX=clang++
# # export OMPI_CXX

# # CXX := mpic++

# CXX := clang++

# LDFLAGS :=

# build/% : test/%.cc build/libprop.a $(HEADERS)
# 	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $@ -Lbuild/ $(LDFLAGS) -lprop -lpth -lboost_thread

# build/%.o : src/%.cc $(HEADERS)
# 	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@

# all: build/libprop.a $(TESTS)

# test: $(TESTS)
# 	for t in $(TESTS); do echo Running $$t; $$t; done

# clean:
# 	rm -rf build/*

# build/libprop.a : $(OBJ) $(HEADERS)
# 	ar rcs $@ $(OBJ)
# 	ranlib $@
