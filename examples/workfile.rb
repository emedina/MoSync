#!/usr/bin/ruby

require File.expand_path('../rules/task.rb')

SUBDIRS = [
	"3dLines", 
	"AdvGraphics",
	"debugging",
	"HelloMap",
	"location",
	"MapDemo",
	"MAStx",
	"MAUI/MAUIex",
	"MinUI",
	"Moblet/Connection",
	"Moblet/HelloMoblet",
	"Moblet/MoTooth",
	"Moblet/btServer",
	"Moblet/simple",
	"Moblet/Soap",
	"Moblet/Stylus",
	"Moblet/timers",
	"MoSketch",
	"MoSound",
	"MoTris",
	"MoTrix",
	"OtaLoad",
]

target :default do
	Work.invoke_subdirs(SUBDIRS)
end

target :clean do
	Work.invoke_subdirs(SUBDIRS, 'clean')
end

Targets.invoke
