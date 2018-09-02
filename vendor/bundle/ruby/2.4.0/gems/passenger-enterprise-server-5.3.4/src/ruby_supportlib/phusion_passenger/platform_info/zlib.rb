#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'platform_info'

module PhusionPassenger

  module PlatformInfo
    def self.zlib_flags
      return nil
    end

    def self.zlib_libs
      return '-lz'
    end
  end

end # module PhusionPassenger
