#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

module PhusionPassenger

  module AdminTools
    def self.tmpdir
      ["PASSENGER_INSTANCE_REGISTRY_DIR", "TMPDIR"].each do |name|
        if ENV.has_key?(name) && !ENV[name].empty?
          return ENV[name]
        end
      end
      return "/tmp"
    end

    def self.process_is_alive?(pid)
      begin
        Process.kill(0, pid)
        return true
      rescue Errno::ESRCH
        return false
      rescue SystemCallError => e
        return true
      end
    end
  end # module AdminTools

end # module PhusionPassenger
