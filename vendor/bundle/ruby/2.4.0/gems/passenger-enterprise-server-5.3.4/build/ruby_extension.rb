#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

if !PlatformInfo.passenger_needs_ruby_dev_header?
  NATIVE_SUPPORT_TARGET = nil

  task :native_support do
    # Do nothing.
  end

  task 'native_support:clean' do
    # Do nothing.
  end
else
  output_dir  = RUBY_EXTENSION_OUTPUT_DIR
  output_name = "passenger_native_support.#{libext}"
  source_dir  = "src/ruby_native_extension"
  NATIVE_SUPPORT_TARGET = File.join(output_dir, output_name)

  task :native_support => NATIVE_SUPPORT_TARGET
  task :clean => 'native_support:clean'

  dependencies = [
    File.join(output_dir, "Makefile"),
    "#{source_dir}/passenger_native_support.c"
  ]
  file(NATIVE_SUPPORT_TARGET => dependencies) do
    sh "mkdir -p '#{output_dir}'" if !File.exist?(output_dir)
    sh "cd '#{output_dir}' && make"
  end

  file(File.join(output_dir, "Makefile") => "#{source_dir}/extconf.rb") do
    sh "mkdir -p '#{output_dir}'" if !File.exist?(output_dir)
    extconf_rb = File.expand_path("#{source_dir}/extconf.rb")
    sh "cd '#{output_dir}' && #{PlatformInfo.ruby_command} '#{extconf_rb}'"
  end

  task 'native_support:clean' do
    sh "rm -rf #{output_dir}"
  end
end
