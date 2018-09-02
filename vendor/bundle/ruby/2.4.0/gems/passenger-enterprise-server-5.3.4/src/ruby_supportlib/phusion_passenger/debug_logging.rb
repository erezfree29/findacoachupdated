#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

PhusionPassenger.require_passenger_lib 'constants'

module PhusionPassenger

  module DebugLogging
    # We don't refer to STDERR directly because STDERR's reference might
    # change during runtime.
    @@log_level = DEFAULT_LOG_LEVEL
    @@log_device = nil
    @@log_filename = nil
    @@stderr_evaluator = lambda { STDERR }

    def self.included(klass)
      klass.class_eval do
        private :debug
        private :trace
      end
    end

    def self.log_level
      return @@log_level
    end

    def self.log_level=(level)
      @@log_level = level
    end

    def self.log_file=(filename)
      if filename && filename.empty?
        @@log_filename = nil
      else
        @@log_filename = filename
      end
      @@log_device.close if @@log_device && !@@log_device.closed?
      @@log_device = nil
    end

    def self._log_device
      return @@log_device
    end

    def self.stderr_evaluator=(block)
      if block
        @@stderr_evaluator = block
      else
        @@stderr_evaluator = lambda { STDERR }
      end
    end

    def error(message)
      _output(1, message, 1)
    end
    module_function :error

    def warn(message)
      _output(2, message, 1)
    end
    module_function :warn

    def notice(message)
      _output(3, message, 1)
    end
    module_function :notice

    def info(message)
      _output(4, message, 1)
    end
    module_function :info

    def debug(message)
      _output(5, message, 1)
    end
    module_function :debug

    def trace(level, message)
      _output(4 + level, message, 1)
    end
    module_function :trace

    def _output(level, message, nesting_level = 0)
      if @@log_level >= level
        if @@log_filename
          if !@@log_device || @@log_device.closed?
            @@log_device = File.open(@@log_filename, "a")
          end
          output = @@log_device
        else
          output = @@stderr_evaluator.call
        end
        location = caller[nesting_level].sub(/.*phusion_passenger\//, '')
        location.sub!(/(.*?):.*/, '\1')
        now = Time.now
        time_str = now.strftime("%Y-%m-%d %H:%M:%S.")
        time_str << sprintf("%04d", now.usec / 100)

        current_thread = Thread.current
        if !(thread_id = current_thread[:id])
          current_thread.to_s =~ /:(0x[0-9a-f]+)/i
          thread_id = current_thread[:id] = $1 || '?'
        end
        if thread_name = current_thread[:name]
          thread_name = "(#{thread_name})"
        end

        output.write("[ #{time_str} #{$$}/#{thread_id}#{thread_name} #{location} ]: #{message}\n")
        output.flush
      end
    end
    module_function :_output
  end

end
