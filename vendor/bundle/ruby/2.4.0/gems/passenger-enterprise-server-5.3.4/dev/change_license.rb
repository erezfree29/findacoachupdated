#!/usr/bin/env ruby
# encoding: utf-8

FILES = Dir[
  "bin/*",
  "build/*.rb",
  "src/**/*.{c,cpp,h,hpp,erb,rb,py,js,cxxcodebuilder}",
  "src/helper-scripts/**/*"
] -
Dir["src/cxx_supportlib/vendor*/**/*"] -
Dir["src/ruby_supportlib/phusion_passenger/vendor/**/*"]
LICENSE_START_SIGNATURE = 'trademarks of Phusion Holding B.V.'
NEW_LICENSE = "See LICENSE file for license information."

def change_license!(filename)
  result = false
  contents = File.open(filename, 'r:utf-8') do |f|
    f.read
  end
  if contents =~ /\A((#.*?\n)+)/m
    header  = $1
    license_start = contents.index(LICENSE_START_SIGNATURE)
    if license_start
      license_start += LICENSE_START_SIGNATURE.size
      license_end = header.size
      contents[license_start .. license_end - 1] = "\n#\n#  #{NEW_LICENSE}\n"
      result = true
    end
  elsif contents =~ %r(\A(/\*.+?\*/))m
    header = $1
    license_start = contents.index(LICENSE_START_SIGNATURE)
    if license_start
      license_start += LICENSE_START_SIGNATURE.size
      license_end = header.size
      contents[license_start .. license_end - 1] = "\n *\n *  #{NEW_LICENSE}\n */"
      result = true
    end
  end
  File.open(filename, 'w:utf-8') do |f|
    f.write(contents)
  end
  return result
rescue EOFError
end

FILES.each do |filename|
  next if File.directory?(filename)
  printf("%-65s", filename)
  STDOUT.flush
  if change_license!(filename)
    puts "  ok!"
  else
    puts "  skipped"
  end
end
