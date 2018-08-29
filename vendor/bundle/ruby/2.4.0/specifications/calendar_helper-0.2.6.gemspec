# -*- encoding: utf-8 -*-
# stub: calendar_helper 0.2.6 ruby lib

Gem::Specification.new do |s|
  s.name = "calendar_helper".freeze
  s.version = "0.2.6"

  s.required_rubygems_version = Gem::Requirement.new(">= 0".freeze) if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib".freeze]
  s.authors = ["Geoffrey Grosenbach".freeze]
  s.date = "2013-08-05"
  s.description = "    A simple helper for creating an HTML calendar. The \"calendar\" method will\n    be automatically available to your Rails view templates, or can be used\n    with Sinatra or other webapps.\n\n    There is also a Rails generator that copies some stylesheets for use alone\n    or alongside existing stylesheets.\n".freeze
  s.email = "boss@topfunky.com".freeze
  s.extra_rdoc_files = ["README.rdoc".freeze, "History.rdoc".freeze]
  s.files = ["History.rdoc".freeze, "README.rdoc".freeze]
  s.rdoc_options = ["--main".freeze, "README.rdoc".freeze]
  s.rubygems_version = "2.6.14.1".freeze
  s.summary = "A simple helper for creating an HTML calendar".freeze

  s.installed_by_version = "2.6.14.1" if s.respond_to? :installed_by_version

  if s.respond_to? :specification_version then
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<open4>.freeze, [">= 0"])
      s.add_development_dependency(%q<rake>.freeze, [">= 0"])
      s.add_development_dependency(%q<rdoc>.freeze, [">= 3.10"])
      s.add_development_dependency(%q<flexmock>.freeze, [">= 0"])
    else
      s.add_dependency(%q<open4>.freeze, [">= 0"])
      s.add_dependency(%q<rake>.freeze, [">= 0"])
      s.add_dependency(%q<rdoc>.freeze, [">= 3.10"])
      s.add_dependency(%q<flexmock>.freeze, [">= 0"])
    end
  else
    s.add_dependency(%q<open4>.freeze, [">= 0"])
    s.add_dependency(%q<rake>.freeze, [">= 0"])
    s.add_dependency(%q<rdoc>.freeze, [">= 3.10"])
    s.add_dependency(%q<flexmock>.freeze, [">= 0"])
  end
end
