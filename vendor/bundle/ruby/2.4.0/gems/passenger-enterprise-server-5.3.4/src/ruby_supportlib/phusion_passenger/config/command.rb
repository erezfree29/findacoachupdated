#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

module PhusionPassenger
  module Config

    class Command
      def initialize(argv)
        @argv = argv.dup
        @options = self.class.create_default_options
      end

      def run_and_get_exit_code
        begin
          run
          exit(0)
        rescue SystemExit => e
          e.status
        end
      end

    private
      def self.create_default_options
        return {}
      end

      def parse_options
        @parser = self.class.create_option_parser(@options)
        begin
          @parser.parse!(@argv)
        rescue OptionParser::ParseError => e
          STDERR.puts "*** ERROR: #{e}"
          abort @parser.to_s
        end
        if @options[:help]
          puts @parser
          exit
        end
      end
    end

  end # module Config
end # module PhusionPassenger
