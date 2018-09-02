#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2014 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'config/command'
PhusionPassenger.require_passenger_lib 'constants'

module PhusionPassenger
module Config

class SendCloudUsageCommand < Command
	def run
		exec("#{PhusionPassenger.find_support_binary(AGENT_EXE)}",
			"send-cloud-usage", "--passenger-root", PhusionPassenger.install_spec,
			*@argv)
	end
end

end # module Config
end # module PhusionPassenger
