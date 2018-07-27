# CtrlrFx.rb


#############################################################################

#
# Creates a target object key reference nominally as a 32-bit int from a 
# 16-bit class and instance.
#
def tgt_obj(tgt_class, tgt_inst)
  (tgt_class << 16) | tgt_inst
end

#############################################################################

#
# Header for a CtrlrFx Binary Packet
#
class BinPktHdr
  attr_reader :magic, :byte_order, :msg_type, :msg_size
  attr_writer :msg_type, :msg_size

  SIZE = 8

  def initialize(*args)
    case args.size
    when 1
      @magic, @byte_order, @msg_type, @msg_size = args[0].unpack("A2CCI4")
    when 2
      @magic = 'CX'
      @byte_order = 0
      @msg_type, @msg_size = args
    else
      raise ArgumentError, "Takes 1 or 2 arguments"
    end
  end

  def to_pkt
    [ @magic, @byte_order, @msg_type, @msg_size ].pack("A2CCI_")
  end

  def dump
    puts "BinPktHdr"
    puts "\tmagic: #{@magic}"
    puts "\tbyte_order: #{@byte_order}"
    puts "\tmsg_type: #{@msg_type}"
    puts "\tmsg_size: #{@msg_size}"
  end
end

#############################################################################

#
# Header for a CtrlrFx binary command
#
class BinCmdHdr
  attr_reader :msg_id, :tgt_obj, :operation

  SIZE = 12

  def initialize(*args)
    case args.size
    when 0
      # do nothing
    when 1
      @msg_id, @tgt_obj, @operation = args[0].unpack("I4I4I4")
    when 3
      @msg_id, @tgt_obj, @operation = args
    end
  end

  def to_pkt
    [ @msg_id, @tgt_obj, @operation ].pack("I_I_I_")
  end

  def dump
    puts "BinCmdHdr"
    puts "\tmsg_id: #{@msg_id}"
    puts "\ttgt_obj: #{@tgt_obj}"
    puts "\toperation: #{@operation}"
  end
end

#############################################################################

#
# Header for a CtrlrFx binary command response
#
class BinCmdRsp
  attr_reader :msg_id, :reply
  attr_writer :msg_id, :reply

  SIZE = 8

  def initialize(*args)
    case args.size
    when 0
      # Do nothing
    when 1
      @msg_id, @reply = args[0].unpack("I4i4")
    when 2
      @msg_id, @reply = args
    end
  end

  #
  # Convert the object into an 8-byte binary packet
  #
  def to_pkt
    [ @msg_id, @reply ].pack("I_i_")
  end
end

#############################################################################

#
# The base class for distributed objects
#
class DistObj
  def initialize(orb)
    @orb = orb
  end

  #
  # Extracts an integer return value from the data portion of a response packet
  #
  def rsp_int(rsp)
    if (rsp[0] == 0)
      return rsp[1].unpack("i_")[0]
    end

    return -1
  end  

end

#############################################################################

#
# Distributed Object Server. Relays messages between object skeletons or 
# stubs and a communications port.
#

class DistObjSrvr
  def initialize(port)
    @port = port
    @quit = false
    @obj_reg = Hash.new
  end

  #
  # Register an object, +obj+ as a target for incoming messages based on
  # the specified +key+
  # 
  def register_obj(key, obj)
    @obj_reg[key] = obj
  end

  #
  # Send a client command, response, or event packet to a remote server.
  #
  def send(pkt)
    @port.write(pkt)

    s = @port.read(BinPktHdr::SIZE)
    hdr = BinPktHdr.new(s)
    n = hdr.msg_size

    s = @port.read(BinCmdRsp::SIZE)
    rsp = BinCmdRsp.new(s)

    n -= BinCmdRsp::SIZE

    if (n > 0)
      s = @port.read(n)
      return [ rsp.reply, s ]
    end

    return [ rsp.reply ]
  end

  #
  # Read data from the underlying port
  # 
  def read(n)
    @port.read(n)
  end

  #
  # Run the server.
  #
  def run()
    until (@quit)
      # Read the packet
      s = @port.read(BinPktHdr::SIZE)

      break if (@quit || s.length == 0)

      hdr = BinPktHdr.new(s)

      # TODO: Determine packet type (not just command) and act accordingly
      s = @port.read(BinCmdHdr::SIZE)
      cmd = BinCmdHdr.new(s);

      n = hdr.msg_size - BinCmdHdr::SIZE
      param = (n > 0) ? @port.read(n) : nil

      # Invoke the command

      obj = @obj_reg[cmd.tgt_obj]

      if obj
        rsp_arr = obj.invoke(cmd.operation, param)
      else
        rsp_arr = [ -1, nil ]
      end

      # Return the response
    end
  end

end


