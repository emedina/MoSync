#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_exe.rb')

work = PipeExeWork.new
work.instance_eval do 
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = " -Wno-shadow"
	@LSTFILES = ["Res/res.lst"]
	@LIBRARIES = ["mautil", "map", "maui"]
	@EXTRA_LINKFLAGS = ' -datasize=1024000 -heapsize=386000 -stacksize=64000'
	@NAME = "MapDemo"
end

work.invoke
