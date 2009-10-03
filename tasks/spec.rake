require 'spec/rake/spectask'

spec_files = Rake::FileList["spec/**/*_spec.rb"]

desc "Run specs"
Spec::Rake::SpecTask.new do |t|
  t.spec_files = spec_files
  t.spec_opts = ["-c"]
end


# hack for running 32bit tests
task :spec32 do 
  
  sh %{ arch -i386 /System/Library/Frameworks/Ruby.framework/Versions/1.8/usr/bin/ruby -I"/Library/Ruby/Gems/1.8/gems/rspec-1.2.7/lib" "/Library/Ruby/Gems/1.8/gems/rspec-1.2.7/bin/spec" "spec/quicktime/exporter_spec.rb" "spec/quicktime/hd_track_spec.rb" "spec/quicktime/movie_spec.rb" "spec/quicktime/track_spec.rb"}
  
end