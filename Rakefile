require 'rubygems'
require 'rake'
require 'echoe'

Echoe.new('rmov', '0.2.5') do |p|
  p.summary        = "Ruby wrapper for the QuickTime C API."
  p.description    = "Ruby wrapper for the QuickTime C API.  Updates by 1K include exposing some movie properties such as codec and audio channel descriptions"
  p.url            = "http://github.com/one-k/rmov"
  p.author         = 'Ryan Bates, with updates by 1K Studios, LLC'
  p.email          = "ryan (at) railscasts (dot) com"
  p.ignore_pattern = ["script/*", "tmp/*", "spec/output/*", "**/*.o", "**/*.bundle", "**/*.mov"]
  p.extensions     = ["ext/extconf.rb"]
  p.development_dependencies = []
end

Dir["#{File.dirname(__FILE__)}/tasks/*.rake"].sort.each { |ext| load ext }
