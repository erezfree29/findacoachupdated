#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2014-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

require 'optparse'
require 'net/http'
require 'socket'
PhusionPassenger.require_passenger_lib 'constants'
PhusionPassenger.require_passenger_lib 'admin_tools/instance_registry'
PhusionPassenger.require_passenger_lib 'config/command'
PhusionPassenger.require_passenger_lib 'config/utils'
PhusionPassenger.require_passenger_lib 'utils/json'

module PhusionPassenger
  module Config

    class DetachProcessCommand < Command
      include PhusionPassenger::Config::Utils

      def run
        parse_options
        select_passenger_instance
        perform_detach
      end

    private
      def self.create_option_parser(options)
        OptionParser.new do |opts|
          nl = "\n" + ' ' * 37
          opts.banner = "Usage: passenger-config detach-process [OPTIONS] <PID>\n"
          opts.separator ""
          opts.separator "  Remove an application process from the #{PROGRAM_NAME} process pool, and"
          opts.separator "  shut it down. Has a similar effect to killing the application process"
          opts.separator "  directly with `kill <PID>`. But `kill` aborts any active requests, while"
          opts.separator "  this command shuts down the process after active requests are finished."
          opts.separator ""
          opts.separator "  If you want to force abort a process and its active requests, just use `kill`."
          opts.separator ""

          opts.separator "Options:"
          opts.on("--instance NAME", String, "The #{PROGRAM_NAME} instance to select") do |value|
            options[:instance] = value
          end
          opts.on("--ignore-nonexistant-pid", "Exit successfully even if the specified#{nl}" +
            "PID is not in the process pool.") do
            options[:ignore_nonexistant_pid] = true
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
        if @argv.empty?
          abort "Please pass a PID. " +
            "See --help for more information."
        elsif @argv.size == 1
          @pid = @argv[0].to_i
        elsif @argv.size > 1
          help
          abort
        end
      end

      def perform_detach
        request = Net::HTTP::Post.new("/pool/detach_process.json")
        try_performing_full_admin_basic_auth(request, @instance)
        request.content_type = "application/json"
        request.body = PhusionPassenger::Utils::JSON.generate(:pid => @pid)
        response = @instance.http_request("agents.s/core_api", request)
        if response.code.to_i / 100 == 2
          body = PhusionPassenger::Utils::JSON.parse(response.body)
          if body['detached']
            puts "Process #{@pid} detached."
          elsif @options[:ignore_nonexistant_pid]
            puts "Could not detach process #{@pid}."
          else
            abort "Could not detach process #{@pid}."
          end
        elsif response.code.to_i == 401
          print_full_admin_command_permission_error
          abort
        else
          STDERR.puts "*** An error occured while communicating with the #{PROGRAM_NAME} server:"
          STDERR.puts response.body
          abort
        end
      end
    end

  end # module Config
end # module PhusionPassenger
