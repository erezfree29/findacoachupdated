ENV['BUNDLE_GEMFILE'] ||= File.expand_path('../Gemfile', __dir__)


 require 'bundler/setup' # Set up gems listed in the Gemfile.
 require 'bootsnap/setup' # Speed up boot time by caching expensive operations.

# module Rails
#   class Server
#     alias :_default_options :default_options
#     def default_options
#       _default_options.merge!(Host:'0.0.0.0')
#     end
#   end
# end
