#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2014-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

module PhusionPassenger
  module Apache2
    module ConfigUtils
      class << self
        def initialize!(options)
          sort(options)
          set_field_defaults(options)
        end

      private
        def sort(options)
          options.sort! do |a, b|
            a[:name] <=> b[:name]
          end
        end

        def set_field_defaults(options)
          options.each do |option|
            option[:context] ||= :application
            if option[:context] == :location
              option[:htaccess_context] ||= ['OR_OPTIONS']
            end
          end
        end
      end
    end
  end
end # module PhusionPassenger
