#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'native_support'

module PhusionPassenger
  module Utils

    # Utility functions that can potentially be accelerated by native_support functions.
    module NativeSupportUtils
      extend self

      if defined?(PhusionPassenger::NativeSupport)
        # Split the given string into an hash. Keys and values are obtained by splitting the
        # string using the null character as the delimitor.
        def split_by_null_into_hash(data)
          return PhusionPassenger::NativeSupport.split_by_null_into_hash(data)
        end

        # Wrapper for getrusage().
        def process_times
          return PhusionPassenger::NativeSupport.process_times
        end
      else
        NULL = "\0".freeze

        class ProcessTimes < Struct.new(:utime, :stime)
        end

        def split_by_null_into_hash(data)
          args = data.split(NULL, -1)
          args.pop
          return Hash[*args]
        end

        def process_times
          times = Process.times
          return ProcessTimes.new((times.utime * 1_000_000).to_i,
            (times.stime * 1_000_000).to_i)
        end
      end
    end

  end # module Utils
end # module PhusionPassenger
