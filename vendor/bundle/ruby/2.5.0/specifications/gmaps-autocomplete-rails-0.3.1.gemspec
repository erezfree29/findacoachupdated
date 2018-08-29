# -*- encoding: utf-8 -*-
# stub: gmaps-autocomplete-rails 0.3.1 ruby lib

Gem::Specification.new do |s|
  s.name = "gmaps-autocomplete-rails".freeze
  s.version = "0.3.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0".freeze) if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib".freeze]
  s.authors = ["Kristian Mandrup".freeze, "Rinas Muhammed".freeze]
  s.date = "2017-01-01"
  s.description = "Easily add autocomplete geo-functionality to your Rails app :)".freeze
  s.email = ["kmandrup@gmail.com".freeze, "onerinas@gmail.com".freeze]
  s.extra_rdoc_files = ["LICENSE.txt".freeze, "README.md".freeze]
  s.files = ["LICENSE.txt".freeze, "README.md".freeze]
  s.homepage = "http://github.com/kristianmandrup/gmaps-autocomplete-rails".freeze
  s.licenses = ["MIT".freeze]
  s.rubygems_version = "2.7.6".freeze
  s.summary = "Google Maps v3 search with jQuery UI Autocomplete, ready for use with Rails asset pipeline".freeze

  s.installed_by_version = "2.7.6" if s.respond_to? :installed_by_version

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<rails>.freeze, [">= 3.1.0"])
      s.add_development_dependency(%q<rspec>.freeze, [">= 2.9.0"])
      s.add_development_dependency(%q<bundler>.freeze, [">= 1.1.0"])
      s.add_development_dependency(%q<jeweler>.freeze, [">= 1.8.4"])
    else
      s.add_dependency(%q<rails>.freeze, [">= 3.1.0"])
      s.add_dependency(%q<rspec>.freeze, [">= 2.9.0"])
      s.add_dependency(%q<bundler>.freeze, [">= 1.1.0"])
      s.add_dependency(%q<jeweler>.freeze, [">= 1.8.4"])
    end
  else
    s.add_dependency(%q<rails>.freeze, [">= 3.1.0"])
    s.add_dependency(%q<rspec>.freeze, [">= 2.9.0"])
    s.add_dependency(%q<bundler>.freeze, [">= 1.1.0"])
    s.add_dependency(%q<jeweler>.freeze, [">= 1.8.4"])
  end
end
