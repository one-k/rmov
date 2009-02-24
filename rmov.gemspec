# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{rmov}
  s.version = "0.2.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 1.2") if s.respond_to? :required_rubygems_version=
  s.authors = ["Ryan Bates"]
  s.date = %q{2009-02-24}
  s.description = %q{Ruby wrapper for the QuickTime C API.}
  s.email = %q{ryan (at) railscasts (dot) com}
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = ["CHANGELOG", "ext/exporter.c", "ext/extconf.rb", "ext/movie.c", "ext/rmov_ext.c", "ext/rmov_ext.h", "ext/track.c", "lib/quicktime/exporter.rb", "lib/quicktime/movie.rb", "lib/quicktime/track.rb", "lib/rmov.rb", "LICENSE", "README.rdoc", "tasks/setup.rake", "tasks/spec.rake", "TODO"]
  s.files = ["CHANGELOG", "ext/exporter.c", "ext/extconf.rb", "ext/movie.c", "ext/rmov_ext.c", "ext/rmov_ext.h", "ext/track.c", "lib/quicktime/exporter.rb", "lib/quicktime/movie.rb", "lib/quicktime/track.rb", "lib/rmov.rb", "LICENSE", "Manifest", "Rakefile", "README.rdoc", "spec/fixtures/settings.st", "spec/quicktime/exporter_spec.rb", "spec/quicktime/movie_spec.rb", "spec/quicktime/track_spec.rb", "spec/spec.opts", "spec/spec_helper.rb", "tasks/setup.rake", "tasks/spec.rake", "TODO", "rmov.gemspec"]
  s.has_rdoc = true
  s.homepage = %q{http://github.com/ryanb/rmov}
  s.rdoc_options = ["--line-numbers", "--inline-source", "--title", "Rmov", "--main", "README.rdoc"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{rmov}
  s.rubygems_version = %q{1.3.1}
  s.summary = %q{Ruby wrapper for the QuickTime C API.}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 2

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
