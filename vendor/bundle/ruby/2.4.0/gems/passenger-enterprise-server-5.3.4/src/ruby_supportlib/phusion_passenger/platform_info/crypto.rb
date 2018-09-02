#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'platform_info'
PhusionPassenger.require_passenger_lib 'platform_info/operating_system'
PhusionPassenger.require_passenger_lib 'platform_info/openssl'

module PhusionPassenger

  module PlatformInfo
    def self.crypto_libs
      prefix = ' -framework CoreFoundation -framework Security'
      suffix = ' -lcrypto'
      if os_name_simple == "macosx"
        if os_version < '10.13'
          return prefix
        else
          return "#{prefix} #{openssl_extra_ldflags} #{suffix}"
        end
      else
        return suffix
      end
    end
    memoize :crypto_libs

    def self.crypto_extra_cflags
      if os_name_simple == "macosx"
        if os_version < '10.13'
          return ' -Wno-deprecated-declarations'
        else
          return " -Wno-deprecated-declarations #{openssl_extra_cflags}"
        end
      else
        return ''
      end
    end
    memoize :crypto_extra_cflags

  end

end # module PhusionPassenger
