// DigIO_stub.cs

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace CtrlrFx
{
	/////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// CtrlrFx remoting stub for a digital input port.
	/// </summary>

	public class DigIn_stub
	{
		private Stream strm;

		public DigIn_stub(Stream strm)
		{
			this.strm = strm;
		}

		public virtual bool Value(int bit)
		{
			return false;
		}
	}

	/////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// CtrlrFx remoting stub for a digital output port. 
	/// </summary>
	public class DigOut_stub : IDigOut
	{
		private const int CMD_LEN = 256;
		private const uint TARGET = 0x0002000;

		private DistObjPort port;

		private enum Cmds : int {
			Set,
			Clear,
			Toggle
		};

		public DigOut_stub(DistObjPort port)
		{
			this.port = port;
		}

		protected void SetHeader(BinNativeEncoder enc, uint cmd)
		{
		}

		public virtual void Set(int bit)
		{
			Set(bit, true);
		}

		public virtual void Set(int bit, bool on)
		{
			Buffer<byte> param = new Buffer<byte>(CMD_LEN), rsp;
			BinNativeEncoder enc = new BinNativeEncoder(param);


			enc.PutInt32(bit);
			enc.PutBool(on);

			port.SendCommand(TARGET, (uint) Cmds.Set, param, out rsp);
		}

		public virtual void Clear(int bit)
		{
			Buffer<byte> param = new Buffer<byte>(CMD_LEN), rsp;
			BinNativeEncoder enc = new BinNativeEncoder(param);

			enc.PutInt32(bit);
			port.SendCommand(TARGET, (uint) Cmds.Clear, param, out rsp);
		}

		public virtual void Toggle(int bit)
		{
			Buffer<byte> param = new Buffer<byte>(CMD_LEN), rsp;
			BinNativeEncoder enc = new BinNativeEncoder(param);

			enc.PutInt32(bit);
			port.SendCommand(TARGET, (uint) Cmds.Toggle, param, out rsp);
		}
	}
}
