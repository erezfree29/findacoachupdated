#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2013-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

### Node.js components tests ###

desc "Run unit tests for the Node.js libraries"
task 'test:node' do
  command = "cd test && env NODE_PATH=#{PhusionPassenger.node_libdir} NODE_ENV=test " +
    "../node_modules/.bin/mocha -R spec node/*_spec.js"
  if grep = string_option('G')
    command << " -g #{shesc grep}"
  end
  sh(command)
end
