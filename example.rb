#!/usr/bin/ruby

require "opendmx"
OpenDMX.init

LED_WASH_1 = 1
LED_WASH_2 = 3
INNO_POCKET_1 = 3
INNO_POCKET_2 = 4

(0..255).each do |i|
  OpenDMX.set_dmx_data [i,i,i]
  sleep 0.01
end

(255..0).each do |i|
  OpenDMX.set_dmx_data [i,i,i]
  sleep 0.01
end