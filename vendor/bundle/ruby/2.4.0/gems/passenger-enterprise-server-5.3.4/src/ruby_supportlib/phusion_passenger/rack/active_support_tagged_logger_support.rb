#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2016 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

module PhusionPassenger
  module Rack
    module ThreadHandlerExtension
    private
      def maybe_tag_logger
        @thread_name ||= create_thread_name
        Rails.logger.tagged(@thread_name) { yield }
      end

      def create_thread_name
        if name = Thread.current[:name]
          name = name.dup
          name.gsub!(/^Worker /, '')
          name.gsub!(' ', '-')
          "Thr-#{name}"
        else
          "Thr-#{Thread.current.object_id.to_s(16)}"
        end
      end
    end
  end
end
