BD = '..'
require "#{BD}/build_rules/defaults.rb"

SUBDIRS = ["MAStd", "MAUtil", "MTXml", "MAFS", "MAUI"] #, "MinUI"
RAKEFILES = ["rakefile.pipe.rb"]#, "rakefile.native.rb"]

require "#{BD}/build_rules/subdirs.rb"