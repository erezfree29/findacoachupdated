#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

### OXT library tests ###

TEST_OXT_TARGET = "#{TEST_OUTPUT_DIR}oxt/main"
TEST_OXT_OBJECTS = {
  "#{TEST_OUTPUT_DIR}oxt/oxt_test_main.o" => "test/oxt/oxt_test_main.cpp",
  "#{TEST_OUTPUT_DIR}oxt/backtrace_test.o" => "test/oxt/backtrace_test.cpp",
  "#{TEST_OUTPUT_DIR}oxt/spin_lock_test.o" => "test/oxt/spin_lock_test.cpp",
  "#{TEST_OUTPUT_DIR}oxt/dynamic_thread_group_test.o" => "test/oxt/dynamic_thread_group_test.cpp",
  "#{TEST_OUTPUT_DIR}oxt/syscall_interruption_test.o" => "test/oxt/syscall_interruption_test.cpp"
}

# Define compilation tasks for object files.
TEST_OXT_OBJECTS.each_pair do |object, source|
  define_cxx_object_compilation_task(
    object,
    source,
    lambda { {
      :include_paths => [
        "test/support",
        *CXX_SUPPORTLIB_INCLUDE_PATHS
      ],
      :flags => TEST_COMMON_CFLAGS
    } }
  )
end

# Define compilation task for the test executable.
dependencies = TEST_OXT_OBJECTS.keys + [TEST_BOOST_OXT_LIBRARY]
file(TEST_OXT_TARGET => dependencies) do
  flags = [
    TEST_BOOST_OXT_LIBRARY,
    PlatformInfo.portability_cxx_ldflags
  ]
  create_cxx_executable(
    TEST_OXT_TARGET,
    TEST_OXT_OBJECTS.keys,
    :flags => flags
  )
end

desc "Run unit tests for the OXT library"
task 'test:oxt' => TEST_OXT_TARGET do
  sh "cd test && #{File.expand_path(TEST_OXT_TARGET)}"
end
