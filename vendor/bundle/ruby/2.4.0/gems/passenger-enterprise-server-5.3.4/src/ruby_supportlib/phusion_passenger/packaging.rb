#  Phusion Passenger - https://www.phusionpassenger.com/
#  Copyright (c) 2010-2017 Phusion Holding B.V.
#
#  "Passenger", "Phusion Passenger" and "Union Station" are registered
#  trademarks of Phusion Holding B.V.
#
#  See LICENSE file for license information.

module PhusionPassenger

  module Packaging
    # Files that must be generated before packaging.
    PREGENERATED_FILES = [
      'src/cxx_supportlib/Constants.h'
    ]

    USER_EXECUTABLES = [
      'passenger',
      'passenger-install-apache2-module',
      'passenger-install-nginx-module',
      'passenger-config',
      'flying-passenger'
    ]

    SUPER_USER_EXECUTABLES = [
      'passenger-status',
      'passenger-memory-stats',
      'passenger-irb'
    ]

    # Used during native packaging. Specifies executables for
    # which the shebang should NOT be set to #!/usr/bin/ruby,
    # so that these executables can be run with any Ruby interpreter
    # the user desires.
    EXECUTABLES_WITH_FREE_RUBY = [
      'passenger',
      'passenger-config',
      'passenger-install-apache2-module',
      'passenger-install-nginx-module',
      'passenger-irb',
      'flying-passenger'
    ]

    # A list of globs which match all files that should be packaged
    # in the Phusion Passenger gem or tarball.
    GLOB = [
      '.editorconfig',
      'configure',
      'Rakefile',
      'README.md',
      'CODE_OF_CONDUCT.md',
      'CONTRIBUTORS',
      'CONTRIBUTING.md',
      'LICENSE',
      'CHANGELOG',
      'INSTALL.md',
      'NEWS',
      'package.json',
      'passenger-enterprise-server.gemspec',
      'build/**/*',
      'bin/*',
      'doc/**/*',
      'images/*',
      'man/*',
      'dev/**/*',
      'src/**/*',
      'resources/**/*',
      'resources/templates/error_renderer/.editorconfig'
    ]

    # Files that should be excluded from the gem or tarball. Overrides GLOB.
    EXCLUDE_GLOB = [
      '**/.DS_Store',
      '.gitignore',
      '.gitattributes',
      '.gitmodules',
      '.github/*',
      '.settings/*',
      '.externalToolBuilders/*',
      '.vscode/*',
      '.cproject',
      '.project',
      'Gemfile',
      'Gemfile.lock',
      'yarn.lock',
      'Vagrantfile',
      'Jenkinsfile',
      'Passenger.sublime-project',
      'Passenger.xcodeproj/**/*',
      'build/support/vendor/*/.*',
      'build/support/vendor/*/spec/**/*',
      'src/ruby_supportlib/phusion_passenger/vendor/*/.*',
      'src/ruby_supportlib/phusion_passenger/vendor/*/hacking/**/*',
      'src/ruby_supportlib/phusion_passenger/vendor/*/spec/**/*',
      'src/cxx_supportlib/vendor-copy/*/.*',
      'packaging/**/*',
      'test/**/*'
    ]

    # Files and directories that should be excluded from the Homebrew installation.
    HOMEBREW_EXCLUDE = [
      "package.json", "npm-shrinkwrap.json"
    ]

    def self.files
      result = Dir[*GLOB] - Dir[*EXCLUDE_GLOB]
      result.reject! { |path| path =~ %r{/\.\.?$} }
      result
    end
  end

end # module PhusionPassenger
