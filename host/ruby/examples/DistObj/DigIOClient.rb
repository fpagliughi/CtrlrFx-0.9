#!/usr/bin/ruby -w

require 'socket'
require 'DigIO'

# ---------------------------------------------------------------------------
#                                     Main
# ---------------------------------------------------------------------------

host = 'localhost'
port = 12345

if (ARGV.size == 2)
  host = ARGV[0]
  port = ARGV[1].to_i
elsif (ARGV.size == 1)
  port = ARGV[0].to_i
end

tgt = TCPSocket.new(host, port)
orb = DistObjSrvr.new(tgt)
dig_out = DigOut_stub.new(orb)
bit = 1

puts "Getting width..."
wd = dig_out.width()
puts "\tDigOut port has #{wd} bits"

puts "Setting bit #{bit}..."
dig_out.set(bit)

puts "Setting bit #{bit} low..."
dig_out.set(bit, 0)

puts "Toggling bit #{bit}..."
dig_out.toggle(bit)

puts "Clearing bit #{bit}..."
dig_out.clear(bit)

puts "Ok"

