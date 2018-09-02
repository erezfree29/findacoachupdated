#!/usr/bin/env ruby
# encoding: binary
#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

# When Passenger Standalone is started with the --defer-port-binding
# parameter, Passenger core will call this script after having
# pre-started web apps. This script will then start an Nginx proxy
# that proxies traffic to the Passenger Standalone Unix domain socket.
#
# The parameter file passed to ARGV[0] is generated in standalone/nginx_engine.rb,
# method #dump_port_binding_proxy_params.

## Magic comment: begin bootstrap ##
source_root = File.expand_path("../..", File.dirname(__FILE__))
$LOAD_PATH.unshift("#{source_root}/src/ruby_supportlib")
begin
  require 'rubygems'
rescue LoadError
end
require 'phusion_passenger'
## Magic comment: end bootstrap ##

PhusionPassenger.locate_directories
PhusionPassenger.require_passenger_lib 'debug_logging'
PhusionPassenger.require_passenger_lib 'standalone/main'
PhusionPassenger.require_passenger_lib 'standalone/control_utils'

module PhusionPassenger
  module Standalone
    STDOUT.sync = true
    STDERR.sync = true
    Standalone::ControlUtils.require_daemon_controller

    controller_options = File.open(ARGV[0], 'rb') do |f|
      Marshal.load(f.read)
    end
    controller = DaemonController.new(controller_options)
    begin
      DebugLogging.notice 'Application started; initiating port binding'
      controller.start
    rescue DaemonController::AlreadyStarted
      begin
        pid = controller.pid
      rescue SystemCallError, IOError
        pid = nil
      end
      if pid
        abort "#{PROGRAM_NAME} port binding proxy is already running on PID #{pid}."
      else
        abort "#{PROGRAM_NAME} port binding proxy is already running."
      end
    rescue DaemonController::StartError => e
      abort "Could not start the port binding proxy:\n#{e}"
    end
  end
end
