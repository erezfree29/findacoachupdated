# encoding: utf-8
#
#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2014-2015 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

require 'optparse'
require 'tmpdir'
require 'fileutils'
PhusionPassenger.require_passenger_lib 'ruby_core_enhancements'
PhusionPassenger.require_passenger_lib 'constants'
PhusionPassenger.require_passenger_lib 'config/command'
PhusionPassenger.require_passenger_lib 'utils/shellwords'

module PhusionPassenger
module Config

class PackageCloudUsageCommand < Command
	def run
		parse_options
		data_dir = "#{PhusionPassenger.home_dir}/#{USER_NAMESPACE_DIRNAME}/usage_data"
		if File.exist?(data_dir)
			create_archive(data_dir)
			clear_data_dir(data_dir)
		else
			Dir.mktmpdir do |tmpdir|
				create_archive(tmpdir)
			end
		end
	end

private
	def self.create_option_parser(options)
		OptionParser.new do |opts|
			nl = "\n" + ' ' * 37
			opts.banner = "Usage: passenger-config package-cloud-usage <OUTPUT.tar.gz> [OPTIONS]\n"
			opts.separator ""
			opts.separator "  Package recorded cloud data usage points into an archive file, so that it can"
			opts.separator "  be sent to Phusion later."
			opts.separator ""

			opts.separator "Options:"
			opts.on("-h", "--help", "Show this help") do
				options[:help] = true
			end
		end
	end

	def parse_options
		super
		if @argv.empty?
			abort "Please specify an output filename. See --help for more information."
		elsif @argv.size > 1
			abort "Please specify only a single output filename. See --help for more information."
		else
			@output_filename = File.absolute_logical_path(@argv[0])
		end
	end

	def create_archive(data_dir)
		Dir.mktmpdir do |tmpdir|
			Dir.mkdir("#{tmpdir}/usage_data")
			FileUtils.cp(Dir["#{data_dir}/*"], "#{tmpdir}/usage_data")
			File.open("#{tmpdir}/hostname", "wb") do |f|
				f.write(detect_hostname)
			end
			File.open("#{tmpdir}/properties.json", "wb") do |f|
				f.write(collect_machine_properties)
			end
			File.open("#{tmpdir}/license_key", "wb") do |f|
				File.open("/etc/passenger-enterprise-license", "rb") do |f2|
					lines = f2.read.split(force_binary("\n"))
					lines.pop
					f.write(lines.join(force_binary("\n")) + force_binary("\n"))
				end
			end
			Dir.chdir(tmpdir) do
				if !system("tar", "-czf", @output_filename, ".")
					abort "Unable to create package: tar failed."
				end
			end
		end
	end

	def clear_data_dir(data_dir)
		files = Dir["#{data_dir}/*"]
		if !files.empty?
			FileUtils.rm(files)
		end
	end

	def detect_hostname
		hostname = convert_utf8(`hostname`.strip)
		if hostname.empty?
			abort "Unable to query the current machine's host name."
		end
		return hostname
	end

	def collect_machine_properties
		agent_exe = "#{PhusionPassenger.support_binaries_dir}/#{AGENT_EXE}"
		command = "#{agent_exe} send-cloud-usage " +
			"--passenger-root #{Shellwords.escape PhusionPassenger.install_spec} " +
			"--dump-machine-properties"
		result = convert_utf8(`#{command}`.strip)
		if result.empty?
			abort "The command '#{command}' failed."
		end
		return result
	end

	if "".respond_to?(:encode)
		def convert_utf8(str)
			return str.encode("utf-8")
		end

		def force_binary(str)
			return str.force_encoding("binary")
		end
	else
		def convert_utf8(str)
			return str
		end

		def force_binary(str)
			return str
		end
	end
end

end # module Config
end # module PhusionPassenger
