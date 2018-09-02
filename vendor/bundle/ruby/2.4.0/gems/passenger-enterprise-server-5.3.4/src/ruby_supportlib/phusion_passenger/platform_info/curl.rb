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
    def self.curl_flags
      result = `(curl-config --cflags) 2>/dev/null`.strip
      if result.empty?
        return nil
      else
        version = `curl-config --vernum`.strip
        if version >= '070c01'
          # Curl >= 7.12.1 supports curl_easy_reset()
          result << " -DHAS_CURL_EASY_RESET"
        end
        return result
      end
    end
    memoize :curl_flags

    def self.curl_libs
      result = `(curl-config --libs) 2>/dev/null`.strip
      if result.empty?
        return nil
      else
        return result
      end
    end
    memoize :curl_libs

    def self.curl_supports_ssl?
      features = `(curl-config --feature) 2>/dev/null`
      return features =~ /SSL/
    end
    memoize :curl_supports_ssl?
  end

end # module PhusionPassenger
