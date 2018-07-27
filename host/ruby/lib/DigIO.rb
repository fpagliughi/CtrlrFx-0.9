# DigIO.rb
#
# Distributed Digital I/O objects for the Controller Framework

require 'CtrlrFx'

#############################################################################
#                                   DigIn
#############################################################################

class DigIn < DistObj
  TGT_CLASS = 0x1

  WIDTH_CMD = 0
  VALUE_CMD = 1

  def initialize(orb)
    super(orb)
  end

end

#############################################################################

class DigIn_stub < DigIn

  def initialize(orb)
    super(orb)
  end

  def width()
    cmd = BinCmdHdr.new(0, tgt_obj(TGT_CLASS, 0), WIDTH_CMD).to_pkt
    cmd = BinPktHdr.new(0, cmd.size).to_pkt + cmd

    rsp = @orb.send(cmd)
    if (rsp[0] == 0)
      return rsp[1].unpack("i_")[0]
    end

    return 0
  end

  def value()
    cmd = BinCmdHdr.new(0, tgt_obj(TGT_CLASS, 0), VALUE_CMD).to_pkt
    cmd = BinPktHdr.new(0, cmd.size).to_pkt + cmd

    rsp = @orb.send(cmd)
    if (rsp[0] == 0)
      return (rsp[1].unpack("C")[0] == 0) ? nil : 1
    end

    return nil
  end
end

#############################################################################
#                                   DigOut
#############################################################################

class DigOut < DistObj
  TGT_CLASS = 0x2

  WIDTH_CMD = 0
  SET_CMD = 1
  CLEAR_CMD = 2
  TOGGLE_CMD = 3

  def initialize(orb)
    super(orb)
  end

end

#############################################################################

class DigOut_stub < DigOut

  def initialize(orb)
    super(orb)
  end

  def width()
    cmd = BinCmdHdr.new(0, tgt_obj(TGT_CLASS, 0), WIDTH_CMD).to_pkt
    cmd = BinPktHdr.new(0, cmd.size).to_pkt + cmd

    rsp = @orb.send(cmd)
    if (rsp[0] == 0)
      return rsp[1].unpack("i_")[0]
    end

    return 0
  end

  def set(*args)
    bit = args[0]
    on = (args[1] || 1).to_int

    cmd = BinCmdHdr.new(0, tgt_obj(TGT_CLASS, 0), SET_CMD).to_pkt
    cmd += [ bit, on ].pack("i_C")
    cmd = BinPktHdr.new(0, cmd.size).to_pkt + cmd

    @orb.send(cmd)
  end

  def clear(bit)
    cmd = BinCmdHdr.new(0, tgt_obj(TGT_CLASS, 0), CLEAR_CMD).to_pkt
    cmd += [ bit ].pack("i_")
    cmd = BinPktHdr.new(0, cmd.size).to_pkt + cmd

    @orb.send(cmd)
  end

  def toggle(bit)
    cmd = BinCmdHdr.new(0, tgt_obj(TGT_CLASS, 0), TOGGLE_CMD).to_pkt
    cmd += [ bit ].pack("i_")
    cmd = BinPktHdr.new(0, cmd.size).to_pkt + cmd

    @orb.send(cmd)
  end
end

#############################################################################

class DigOut_skel < DigOut

  @@OPS = {
    0 => :width_cmd,
    1 => :set_cmd,
    2 => :clear_cmd,
    3 => :toggle_cmd
  }
  #
  # Invokes the specified operation (method
  #
  def invoke(op, param)
    m = @@OPS[op]
    self.send(m, param)
  end

  def width_cmd(param)
    wd = width;
    [ wd ].pack("i_")
  end

end


