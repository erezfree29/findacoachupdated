#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2014-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'constants'
PhusionPassenger.require_passenger_lib 'config/command'

module PhusionPassenger
  module Config

    class SystemMetricsCommand < Command
      def run
        agent_exe = PhusionPassenger.find_support_binary(AGENT_EXE)
        if agent_exe
          exec(agent_exe, "system-metrics", *@argv)
        else
          abort "This command requires the #{PROGRAM_NAME} agent to be installed. " +
            "Please install it by running `passenger-config install-agent`."
        end
      end
    end

  end # module Config
end # module PhusionPassenger
