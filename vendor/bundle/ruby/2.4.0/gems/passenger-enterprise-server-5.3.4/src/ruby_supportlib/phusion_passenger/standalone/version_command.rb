#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'standalone/command'
PhusionPassenger.require_passenger_lib 'config/about_command'

module PhusionPassenger
  module Standalone

    class VersionCommand < Command
      def run
        command = PhusionPassenger::Config::AboutCommand.new(['version'])
        command.run
      end
    end

  end
end
