#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

module PhusionPassenger
  module Standalone

    class Command
      def initialize(argv)
        @argv = argv.dup
        @options = self.class.create_default_options
      end

    private
      def self.create_default_options
        return {}
      end

      def parse_options
        load_and_merge_global_options(@options)
        @parsed_options = self.class.create_default_options
        @parser = self.class.create_option_parser(@parsed_options)
        begin
          @original_argv = @argv.dup
          @parser.parse!(@argv)
          @options.merge!(@parsed_options)
        rescue OptionParser::ParseError => e
          STDERR.puts "*** ERROR: #{e}"
          abort @parser.to_s
        end
        if @options[:help]
          puts @parser
          exit
        end
      end

      def load_and_merge_global_options(options)
        path = ConfigUtils.global_config_file_path
        if File.exist?(path)
          begin
            @global_options = ConfigUtils.load_config_file(path)
          rescue ConfigUtils::ConfigLoadError => e
            STDERR.puts "*** Warning: #{e.message}"
            return
          end
          @options.merge!(@global_options)
        else
          @global_options = {}
        end
      end
    end

  end # module Standalone
end # module PhusionPassenger
