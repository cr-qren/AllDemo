# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/admini/raintime

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/admini/raintime/build

# Include any dependencies generated for this target.
include third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend.make

# Include the progress variables for this target.
include third_party/plumber_ir/CMakeFiles/plumber_ir.dir/progress.make

# Include the compile flags for this target's objects.
include third_party/plumber_ir/CMakeFiles/plumber_ir.dir/flags.make

third_party/plumber_ir/plumber_ir/op_param_def.pb.cc: ../third_party/plumber_ir/plumber_ir/op_param_def.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/admini/raintime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Running C++ protocol buffer compiler on /home/admini/raintime/third_party/plumber_ir/plumber_ir/op_param_def.proto"
	cd /home/admini/raintime/build/third_party/plumber_ir && protoc --cpp_out=/home/admini/raintime/build/third_party/plumber_ir -I=/home/admini/raintime/third_party/plumber_ir /home/admini/raintime/third_party/plumber_ir/plumber_ir/op_param_def.proto

third_party/plumber_ir/plumber_ir/op_param_def.pb.h: third_party/plumber_ir/plumber_ir/op_param_def.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate third_party/plumber_ir/plumber_ir/op_param_def.pb.h

third_party/plumber_ir/plumber_ir/dfg_def.pb.cc: ../third_party/plumber_ir/plumber_ir/dfg_def.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/admini/raintime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Running C++ protocol buffer compiler on /home/admini/raintime/third_party/plumber_ir/plumber_ir/dfg_def.proto"
	cd /home/admini/raintime/build/third_party/plumber_ir && protoc --cpp_out=/home/admini/raintime/build/third_party/plumber_ir -I=/home/admini/raintime/third_party/plumber_ir /home/admini/raintime/third_party/plumber_ir/plumber_ir/dfg_def.proto

third_party/plumber_ir/plumber_ir/dfg_def.pb.h: third_party/plumber_ir/plumber_ir/dfg_def.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate third_party/plumber_ir/plumber_ir/dfg_def.pb.h

third_party/plumber_ir/plumber_ir/type_def.pb.cc: ../third_party/plumber_ir/plumber_ir/type_def.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/admini/raintime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Running C++ protocol buffer compiler on /home/admini/raintime/third_party/plumber_ir/plumber_ir/type_def.proto"
	cd /home/admini/raintime/build/third_party/plumber_ir && protoc --cpp_out=/home/admini/raintime/build/third_party/plumber_ir -I=/home/admini/raintime/third_party/plumber_ir /home/admini/raintime/third_party/plumber_ir/plumber_ir/type_def.proto

third_party/plumber_ir/plumber_ir/type_def.pb.h: third_party/plumber_ir/plumber_ir/type_def.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate third_party/plumber_ir/plumber_ir/type_def.pb.h

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/flags.make
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o: third_party/plumber_ir/plumber_ir/op_param_def.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/admini/raintime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o -c /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/op_param_def.pb.cc

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.i"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/op_param_def.pb.cc > CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.i

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.s"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/op_param_def.pb.cc -o CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.s

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.requires:

.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.requires

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.provides: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.requires
	$(MAKE) -f third_party/plumber_ir/CMakeFiles/plumber_ir.dir/build.make third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.provides.build
.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.provides

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.provides.build: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o


third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/flags.make
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o: third_party/plumber_ir/plumber_ir/dfg_def.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/admini/raintime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o -c /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/dfg_def.pb.cc

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.i"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/dfg_def.pb.cc > CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.i

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.s"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/dfg_def.pb.cc -o CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.s

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.requires:

.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.requires

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.provides: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.requires
	$(MAKE) -f third_party/plumber_ir/CMakeFiles/plumber_ir.dir/build.make third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.provides.build
.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.provides

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.provides.build: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o


third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/flags.make
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o: third_party/plumber_ir/plumber_ir/type_def.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/admini/raintime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o -c /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/type_def.pb.cc

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.i"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/type_def.pb.cc > CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.i

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.s"
	cd /home/admini/raintime/build/third_party/plumber_ir && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/admini/raintime/build/third_party/plumber_ir/plumber_ir/type_def.pb.cc -o CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.s

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.requires:

.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.requires

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.provides: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.requires
	$(MAKE) -f third_party/plumber_ir/CMakeFiles/plumber_ir.dir/build.make third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.provides.build
.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.provides

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.provides.build: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o


# Object files for target plumber_ir
plumber_ir_OBJECTS = \
"CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o" \
"CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o" \
"CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o"

# External object files for target plumber_ir
plumber_ir_EXTERNAL_OBJECTS =

third_party/plumber_ir/libplumber_ir.so: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o
third_party/plumber_ir/libplumber_ir.so: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o
third_party/plumber_ir/libplumber_ir.so: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o
third_party/plumber_ir/libplumber_ir.so: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/build.make
third_party/plumber_ir/libplumber_ir.so: /usr/lib/x86_64-linux-gnu/libprotobuf.so
third_party/plumber_ir/libplumber_ir.so: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/admini/raintime/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX shared library libplumber_ir.so"
	cd /home/admini/raintime/build/third_party/plumber_ir && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/plumber_ir.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/build: third_party/plumber_ir/libplumber_ir.so

.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/build

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/requires: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/op_param_def.pb.cc.o.requires
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/requires: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/dfg_def.pb.cc.o.requires
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/requires: third_party/plumber_ir/CMakeFiles/plumber_ir.dir/plumber_ir/type_def.pb.cc.o.requires

.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/requires

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/clean:
	cd /home/admini/raintime/build/third_party/plumber_ir && $(CMAKE_COMMAND) -P CMakeFiles/plumber_ir.dir/cmake_clean.cmake
.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/clean

third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend: third_party/plumber_ir/plumber_ir/op_param_def.pb.cc
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend: third_party/plumber_ir/plumber_ir/op_param_def.pb.h
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend: third_party/plumber_ir/plumber_ir/dfg_def.pb.cc
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend: third_party/plumber_ir/plumber_ir/dfg_def.pb.h
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend: third_party/plumber_ir/plumber_ir/type_def.pb.cc
third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend: third_party/plumber_ir/plumber_ir/type_def.pb.h
	cd /home/admini/raintime/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/admini/raintime /home/admini/raintime/third_party/plumber_ir /home/admini/raintime/build /home/admini/raintime/build/third_party/plumber_ir /home/admini/raintime/build/third_party/plumber_ir/CMakeFiles/plumber_ir.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third_party/plumber_ir/CMakeFiles/plumber_ir.dir/depend

