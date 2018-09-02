#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2014-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

require 'optparse'
PhusionPassenger.require_passenger_lib 'constants'
PhusionPassenger.require_passenger_lib 'admin_tools/instance_registry'
PhusionPassenger.require_passenger_lib 'config/command'
PhusionPassenger.require_passenger_lib 'config/utils'
PhusionPassenger.require_passenger_lib 'utils/json'

module PhusionPassenger
  module Config

    class ListInstancesCommand < Command
      include PhusionPassenger::Config::Utils

      def run
        parse_options
        instances = AdminTools::InstanceRegistry.new.list
        if @options[:json]
          print_json(instances)
        elsif instances.empty?
          print_no_instances_running
        else
          print_instances(instances)
        end
      end

    private
      def self.create_option_parser(options)
        OptionParser.new do |opts|
          nl = "\n" + ' ' * 37
          opts.banner = "Usage: passenger-config list-instances [OPTIONS] <PID>\n"
          opts.separator ""
          opts.separator "  List all running #{PROGRAM_NAME} instances."
          opts.separator ""

          opts.on("--json", "Print output in JSON format") do
            options[:json] = true
          end
          opts.on("-q", "--quiet", "Don't print anything if there are no #{PROGRAM_NAME} instances running") do
            options[:quiet] = true
          end
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
        if !@argv.empty?
          help
          abort
        end
      end

      def print_no_instances_running
        if !@options[:quiet]
          puts "There are no #{PROGRAM_NAME} instances running."
        end
      end

      def print_json(instances)
        result = []
        instances.each do |instance|
          result << instance.as_json
        end
        puts PhusionPassenger::Utils::JSON.generate(result)
      end

      def print_instances(instances)
        list_all_passenger_instances(instances, false)
      end
    end

  end # module Config
end # module PhusionPassenger
