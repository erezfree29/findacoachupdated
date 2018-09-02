# encoding: binary
#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

require 'socket'
PhusionPassenger.require_passenger_lib 'message_channel'
PhusionPassenger.require_passenger_lib 'utils'

module PhusionPassenger

  # A convenience class for communicating with MessageServer servers,
  # for example the ApplicationPool server.
  class MessageClient
    include Utils

    class ProtocolError < StandardError
    end

    # Connect to the given server. By default it connects to the current
    # generation's core.
    def initialize(username, password, address)
      @socket = connect_to_server(address)
      begin
        @channel = MessageChannel.new(@socket)

        result = @channel.read
        if result.nil?
          raise EOFError
        elsif result.size != 2 || result[0] != "version"
          raise IOError, "The message server didn't sent a valid version identifier"
        elsif result[1] != "1"
          raise IOError, "Unsupported message server protocol version #{result[1]}"
        end

        @channel.write_scalar(username)
        @channel.write_scalar(password)

        result = @channel.read
        if result.nil?
          raise EOFError
        elsif result[0] != "status"
          raise ProtocolError, "Invalid authentication response: expected \"status\", got #{result[0].inspect}"
        elsif result[1] == "ok"
          # Do nothing
        elsif result[1] == "error"
          if result[2]
            raise SecurityError, "Authentication error: #{result[2]}"
          else
            raise SecurityError, "Authentication error (no server message given)"
          end
        else
          raise ProtocolError, "Invalid authentication response: #{result.inspect}"
        end
      rescue Exception
        @socket.close
        raise
      end
    end

    def close
      @socket.close if @socket
      @channel = @socket = nil
    end

    def connected?
      return !!@channel
    end

    ### Low level I/O methods ###

    def read
      return @channel.read
    rescue
      auto_disconnect
      raise
    end

    def write(*args)
      @channel.write(*args)
    rescue
      auto_disconnect
      raise
    end

    def write_scalar(*args)
      @channel.write_scalar(*args)
    rescue
      auto_disconnect
      raise
    end

    def read_scalar
      return @channel.read_scalar
    rescue
      auto_disconnect
      raise
    end

    def recv_io(klass = IO, negotiate = true)
      return @channel.recv_io(klass, negotiate)
    rescue
      auto_disconnect
      raise
    end

  private
    def auto_disconnect
      if @channel
        @socket.close rescue nil
        @socket = @channel = nil
      end
    end
  end

end # module PhusionPassenger
