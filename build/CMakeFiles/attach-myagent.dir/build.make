# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = /usr/local/cmake-3.21.3-linux-x86_64/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake-3.21.3-linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yuzhaoxi/Profiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yuzhaoxi/Profiler/build

# Utility rule file for attach-myagent.

# Include any custom commands dependencies for this target.
include CMakeFiles/attach-myagent.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/attach-myagent.dir/progress.make

CMakeFiles/attach-myagent: out/attach-myagent.jar

out/attach-myagent.jar: CMakeFiles/attach-myagent.dir/java_class_filelist
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/yuzhaoxi/Profiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Creating Java archive attach-myagent.jar"
	cd /home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir && /home/yuzhaoxi/src/jdk-17.0.2/bin/jar -cfe /home/yuzhaoxi/Profiler/build/out/attach-myagent.jar AttachAgent @java_class_filelist
	cd /home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir && /usr/local/cmake-3.21.3-linux-x86_64/bin/cmake -D_JAVA_TARGET_DIR=/home/yuzhaoxi/Profiler/build/out -D_JAVA_TARGET_OUTPUT_NAME=attach-myagent.jar -D_JAVA_TARGET_OUTPUT_LINK= -P /usr/local/cmake-3.21.3-linux-x86_64/share/cmake-3.21/Modules/UseJava/Symlinks.cmake

CMakeFiles/attach-myagent.dir/java_class_filelist: CMakeFiles/attach-myagent.dir/java_compiled_attach-myagent
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/yuzhaoxi/Profiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating CMakeFiles/attach-myagent.dir/java_class_filelist"
	cd /home/yuzhaoxi/Profiler && /usr/local/cmake-3.21.3-linux-x86_64/bin/cmake -DCMAKE_JAVA_CLASS_OUTPUT_PATH=/home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir -DCMAKE_JAR_CLASSES_PREFIX= -P /usr/local/cmake-3.21.3-linux-x86_64/share/cmake-3.21/Modules/UseJava/ClassFilelist.cmake

CMakeFiles/attach-myagent.dir/java_compiled_attach-myagent: ../src/JProfiler/AttachAgent.java
CMakeFiles/attach-myagent.dir/java_compiled_attach-myagent: CMakeFiles/attach-myagent.dir/java_sources
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/yuzhaoxi/Profiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building Java objects for attach-myagent.jar"
	cd /home/yuzhaoxi/Profiler && /usr/local/cmake-3.21.3-linux-x86_64/bin/cmake -DCMAKE_JAVA_CLASS_OUTPUT_PATH=/home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir -DCMAKE_JAR_CLASSES_PREFIX= -P /usr/local/cmake-3.21.3-linux-x86_64/share/cmake-3.21/Modules/UseJava/ClearClassFiles.cmake
	cd /home/yuzhaoxi/Profiler && /home/yuzhaoxi/src/jdk-17.0.2/bin/javac -classpath :/home/yuzhaoxi/src/jdk-17.0.2/include/../lib/tools.jar:/home/yuzhaoxi/Profiler:/home/yuzhaoxi/Profiler/build/out -d /home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir @/home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir/java_sources
	cd /home/yuzhaoxi/Profiler && /usr/local/cmake-3.21.3-linux-x86_64/bin/cmake -E touch /home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir/java_compiled_attach-myagent

attach-myagent: CMakeFiles/attach-myagent
attach-myagent: CMakeFiles/attach-myagent.dir/java_class_filelist
attach-myagent: CMakeFiles/attach-myagent.dir/java_compiled_attach-myagent
attach-myagent: out/attach-myagent.jar
attach-myagent: CMakeFiles/attach-myagent.dir/build.make
.PHONY : attach-myagent

# Rule to build all files generated by this target.
CMakeFiles/attach-myagent.dir/build: attach-myagent
.PHONY : CMakeFiles/attach-myagent.dir/build

CMakeFiles/attach-myagent.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/attach-myagent.dir/cmake_clean.cmake
.PHONY : CMakeFiles/attach-myagent.dir/clean

CMakeFiles/attach-myagent.dir/depend:
	cd /home/yuzhaoxi/Profiler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yuzhaoxi/Profiler /home/yuzhaoxi/Profiler /home/yuzhaoxi/Profiler/build /home/yuzhaoxi/Profiler/build /home/yuzhaoxi/Profiler/build/CMakeFiles/attach-myagent.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/attach-myagent.dir/depend
