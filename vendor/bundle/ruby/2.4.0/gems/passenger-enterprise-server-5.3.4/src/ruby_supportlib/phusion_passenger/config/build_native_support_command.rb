#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2014-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

require 'optparse'
PhusionPassenger.require_passenger_lib 'constants'
PhusionPassenger.require_passenger_lib 'config/command'

module PhusionPassenger
  module Config

    class BuildNativeSupportCommand < Command
      def run
        parse_options
        PhusionPassenger.require_passenger_lib 'native_support'
      end

    private
      def self.create_option_parser(options)
        PhusionPassenger.require_passenger_lib 'platform_info/ruby'
        OptionParser.new do |opts|
          nl = "\n" + ' ' * 37
          opts.banner = "Usage: passenger-config build-native-support [OPTIONS]\n"
          opts.separator ""
          opts.separator "  #{PROGRAM_NAME} utilizes a Ruby native extension, called native_support,"
          opts.separator "  for improving Ruby performance. The extension depends on the"
          opts.separator "  #{PROGRAM_NAME} version and the Ruby version. Normally, every time you run"
          opts.separator "  a #{PROGRAM_NAME} version with a Ruby version that it hasn't encountered"
          opts.separator "  before, it will rebuild the native_support library for that Ruby version."
          opts.separator "  By running this command, you can force the native_support to be built for"
          opts.separator "  the current Ruby interpreter."
          opts.separator ""
          opts.separator "  The current Ruby interpreter is:"
          opts.separator "    Path: #{PlatformInfo.ruby_command}"
          opts.separator "    Version: #{RUBY_VERSION}"
          opts.separator ""

          opts.separator "Options:"
          opts.on("-h", "--help", "Show this help") do
            options[:help] = true
          end
        end
      end

      def help
        puts @parser
      end

      def parse_options
        super
        if @argv.size > 0
          help
          abort
        end
      end
    end

  end # module Config
end # module PhusionPassenger
