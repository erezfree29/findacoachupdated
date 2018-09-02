#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

### Ruby components tests ###

dependencies = [NATIVE_SUPPORT_TARGET, AGENT_TARGET,
  "#{TEST_OUTPUT_DIR}allocate_memory"].compact
desc "Run unit tests for the Ruby libraries"
task 'test:ruby' => dependencies do
  if maybe_grep = string_option('E')
    maybe_grep = "-e #{shesc maybe_grep}"
  end
  command = "rspec -c -f s --tty -P 'dont-autoload-anything' #{maybe_grep} ruby/*_spec.rb ruby/*/*_spec.rb"
  sh "cd test && exec bundle exec #{command}"
end
