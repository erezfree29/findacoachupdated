#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

module PhusionPassenger
  class << self
    @@event_starting_worker_process = []
    @@event_stopping_worker_process = []
    @@event_starting_request_handler_thread = []
    @@event_credentials = []
    @@event_after_installing_signal_handlers = []
    @@event_oob_work = []
    @@advertised_concurrency_level = nil

    def on_event(name, &block)
      callback_list_for_event(name) << block
    end

    def call_event(name, *args)
      callback_list_for_event(name).each do |callback|
        callback.call(*args)
      end
    end

    # For backward compatibility
    def install_framework_extensions!(user_options = {})
    end

    def advertised_concurrency_level
      @@advertised_concurrency_level
    end

    def advertised_concurrency_level=(value)
      @@advertised_concurrency_level = value
    end

  private
    def callback_list_for_event(name)
      return case name
      when :starting_worker_process
        @@event_starting_worker_process
      when :stopping_worker_process
        @@event_stopping_worker_process
      when :starting_request_handler_thread
        @@event_starting_request_handler_thread
      when :credentials
        @@event_credentials
      when :after_installing_signal_handlers
        @@event_after_installing_signal_handlers
      when :oob_work
        @@event_oob_work
      else
        raise ArgumentError, "Unknown event name '#{name}'"
      end
    end
  end
end # module PhusionPassenger
