#!/usr/bin/ruby
# CfxPktDump.rb
# Acts as a CtrlrFx object server, but just dumps the contents of the 
# packets to stdout.

require 'socket'
require 'CtrlrFx'

# ---------------------------------------------------------------------------
#                                 Main
# ---------------------------------------------------------------------------

Signal.trap("INT") do
  Process.exit
end

port = (ARGV[0] || 12345).to_i

server = TCPServer.new('localhost', port)

while (sock = server.accept)
  while s = sock.read(8)
    pkt_hdr = BinPktHdr.new(s)
    pkt_hdr.dump

    n = pkt_hdr.msg_size
    s = sock.read(n)

    if (pkt_hdr.msg_type == 0)
      cmd = BinCmdHdr.new(s)
      cmd.dump
      rsp = BinCmdRsp.new(cmd.msg_id)
      sock.write(rsp.to_pkt)
    end
  end

  sock.close
end


