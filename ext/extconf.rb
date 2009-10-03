require 'mkmf'

unless RUBY_PLATFORM =~ /darwin/ || have_header('QuickTime/QuickTime.h')
  abort("The rmov gem is not compatible with this platform : #{RUBY_PLATFORM}")
end

$CFLAGS = CONFIG["CFLAGS"].sub!("x86_64", "i386")
$LDSHARED = CONFIG["LDSHARED"].sub!("x86_64", "i386")
$LIBRUBY_LDSHARED = CONFIG["LIBRUBY_LDSHARED"].sub!("x86_64", "i386")
CONFIG["LDFLAGS"] = $LDFLAGS = CONFIG["LDFLAGS"].sub("x86_64", "i386") + " -framework QuickTime"

create_makefile('rmov_ext')
