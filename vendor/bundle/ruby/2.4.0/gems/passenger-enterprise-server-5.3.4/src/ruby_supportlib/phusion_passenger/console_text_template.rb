# encoding: utf-8
#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

require 'erb'
PhusionPassenger.require_passenger_lib 'utils/ansi_colors'

module PhusionPassenger

  class ConsoleTextTemplate
    def initialize(input, options = {})
      @buffer = ''
      if input[:file]
        filename = "#{PhusionPassenger.resources_dir}/templates/#{input[:file]}.txt.erb"
        data = File.open(filename, 'r:utf-8') do |f|
          f.read
        end
      else
        data = input[:text]
      end
      @colors = options[:colors] || AnsiColors.new
      @template = ERB.new(@colors.ansi_colorize(data),
        nil, '-', '@buffer')
      @template.filename = filename if filename
      options.each_pair do |name, value|
        self[name] = value
      end
    end

    def []=(name, value)
      instance_variable_set("@#{name}".to_sym, value)
      return self
    end

    def result
      return @template.result(binding)
    end
  end

end # module PhusionPassenger
