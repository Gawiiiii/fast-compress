# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gawi/fast-compress

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gawi/fast-compress

# Include any dependencies generated for this target.
include CMakeFiles/FastCompress.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/FastCompress.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/FastCompress.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/FastCompress.dir/flags.make

CMakeFiles/FastCompress.dir/main.cpp.o: CMakeFiles/FastCompress.dir/flags.make
CMakeFiles/FastCompress.dir/main.cpp.o: main.cpp
CMakeFiles/FastCompress.dir/main.cpp.o: CMakeFiles/FastCompress.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gawi/fast-compress/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/FastCompress.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/FastCompress.dir/main.cpp.o -MF CMakeFiles/FastCompress.dir/main.cpp.o.d -o CMakeFiles/FastCompress.dir/main.cpp.o -c /home/gawi/fast-compress/main.cpp

CMakeFiles/FastCompress.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FastCompress.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gawi/fast-compress/main.cpp > CMakeFiles/FastCompress.dir/main.cpp.i

CMakeFiles/FastCompress.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FastCompress.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gawi/fast-compress/main.cpp -o CMakeFiles/FastCompress.dir/main.cpp.s

# Object files for target FastCompress
FastCompress_OBJECTS = \
"CMakeFiles/FastCompress.dir/main.cpp.o"

# External object files for target FastCompress
FastCompress_EXTERNAL_OBJECTS =

FastCompress: CMakeFiles/FastCompress.dir/main.cpp.o
FastCompress: CMakeFiles/FastCompress.dir/build.make
FastCompress: /usr/lib/x86_64-linux-gnu/libzstd.so
FastCompress: /usr/lib/x86_64-linux-gnu/liblz4.so
FastCompress: /usr/lib/x86_64-linux-gnu/liblzo2.so
FastCompress: /usr/lib/x86_64-linux-gnu/libz.so
FastCompress: CMakeFiles/FastCompress.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gawi/fast-compress/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable FastCompress"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FastCompress.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/FastCompress.dir/build: FastCompress
.PHONY : CMakeFiles/FastCompress.dir/build

CMakeFiles/FastCompress.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/FastCompress.dir/cmake_clean.cmake
.PHONY : CMakeFiles/FastCompress.dir/clean

CMakeFiles/FastCompress.dir/depend:
	cd /home/gawi/fast-compress && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gawi/fast-compress /home/gawi/fast-compress /home/gawi/fast-compress /home/gawi/fast-compress /home/gawi/fast-compress/CMakeFiles/FastCompress.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/FastCompress.dir/depend

