#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2015-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'platform_info'
PhusionPassenger.require_passenger_lib 'platform_info/operating_system'

module PhusionPassenger

  module PlatformInfo
    def self.openssl_extra_cflags
      if PlatformInfo.os_name_simple == "macosx"
        if File.exist?("/usr/include/openssl")
          ""
          # OS X >= 10.11 El Capitan no longer include
          # OpenSSL development headers.
        elsif File.exist?("/usr/local/opt/openssl/include")
          # Use the one from Homebrew.
          "-I/usr/local/opt/openssl/include"
        else
          # Use the one from Macports.
          "-I/opt/local/include"
        end
      else
        ""
      end
    end
    memoize :openssl_extra_cflags

    def self.openssl_extra_ldflags
      if PlatformInfo.os_name_simple == "macosx"
        if File.exist?("/usr/include/openssl")
          ""
        elsif File.exist?("/usr/local/opt/openssl/include")
          "-L/usr/local/opt/openssl/lib"
        else
          "-L/opt/local/lib"
        end
      else
        ""
      end
    end
    memoize :openssl_extra_ldflags
  end

end # module PhusionPassenger
