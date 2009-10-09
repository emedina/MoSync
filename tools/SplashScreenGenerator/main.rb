# Copyright (C) 2009 Mobile Sorcery AB
# 
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License, version 2, as published by
# the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to the Free
# Software Foundation, 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# This script generates a splash screen for the IDE, using RMagick operatations

require 'rubygems'
require 'RMagick'

$KCODE = "UTF8"

HEADER_TEXT = 'MoSync mobile development SDK'

version = ['Developer build', 'Unknown']

PLATFORMS = [
	'Java ME MIDP 2',
	'Symbian S60 2nd edition',
	'Symbian S60 3rd edition',
	'Smartphone 2003',
	'Pocket PC 2003',
	'Windows Mobile 5.0',
	'Windows Mobile 6.0'
]

index = 0
File.open('\mb\revision', 'r') do |f|
	while (line = f.gets)
		version[index] = line
		index = index + 1
	end
end

COPYRIGHT = "Copyright © 2004-#{Time.new.year.to_s}. All rights reserved. " + 
            "Mobile Sorcery, the Mobile Sorcery logo, MoSync and the MoSync " + 
			"logo are trademarks of Mobile Sorcery AB."

img = Magick::Image.read('template.png').first
img2 = Magick::Image.read('template_installer.png').first

header = Magick::Draw.new

header.annotate(img, 271, 200, 275, 110, HEADER_TEXT) do
	self.font = 'MyriadPro-Bold'
	self.pointsize = 19
	self.font_weight = Magick::BoldWeight
	self.fill = 'white'
	self.gravity = Magick::NorthWestGravity
end

header.annotate(img, 271, 340, 275, 130, "Version #{version[0].strip} ( Revision #{version[1].strip} )" ) do
	self.font = 'Verdana'
	self.pointsize = 14
	self.font_weight = Magick::LighterWeight
	self.fill = 'white'
	self.gravity = Magick::NorthWestGravity
end

header.annotate(img2, 271, 340, 180, 260, "Version #{version[0].strip} ( Revision #{version[1].strip} )" ) do
	self.font = 'Verdana'
	self.pointsize = 12
	self.font_weight = Magick::LighterWeight
	self.fill = 'white'
	self.gravity = Magick::NorthWestGravity
end

platforms = Magick::Image.read("caption:#{PLATFORMS.join(', ')}") do 
	self.size = "300x200"
	self.pointsize = 12
	self.fill = '#b0b0b0'
	self.background_color = '#00000000'
end

copyright = Magick::Image.read("caption:#{COPYRIGHT}") do 
	self.size = "450x100"
	self.pointsize = 11
	self.fill = '#b0b0b0'
	self.background_color = '#00000000'
end

img = img.composite(platforms.first, 275, 160, Magick::ScreenCompositeOp)
img = img.composite(copyright.first, 134, 350, Magick::ScreenCompositeOp)

img.write('splash.bmp')
img2.write('installer_splash.bmp')
