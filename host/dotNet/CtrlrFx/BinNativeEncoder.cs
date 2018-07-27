// BinNativeEncoder.cs

using System;
using System.Collections.Generic;
using System.Text;

namespace CtrlrFx
{
	public class BinNativeEncoder
	{
        private Buffer<byte> buf;

        public BinNativeEncoder(Buffer<byte> buf)
        {
            this.buf = buf;
        }

		public unsafe int Put(byte by)
		{
			buf.Put(by);
			return 0;
		}

		public int Put(byte[] arr)
		{
			return Put(arr, 0, arr.Length);
		}

		public int Put(byte[] arr, int offset, int count)
		{
			for (int i=0; i<count; ++i)
				buf.Put(arr[i+offset]);

			return 0;
		}

		public int Put(Buffer<byte> buf)
		{
			while (!buf.Empty)
				this.buf.Put(buf.Get());

			return 0;
		}

		public unsafe int Put(byte* arr, int count)
        {
			for (int i=0; i<count; ++i)
				buf.Put(arr[i]);

            return 0;
        }
        
        public int PutBool(bool b)
		{
            byte by = (byte) (b ? 1 : 0);
            return Put(by);			
		}

		public int PutChar(char c)
		{
            byte by = (byte)(c & 0xFF);
            return Put(by);
		}

		public int PutByte(byte by)
		{
            return Put(by);
		}

		public unsafe int PutInt16(short n)
		{
			buf.Align(sizeof(short));
            return Put((byte*)&n, sizeof(short));
		}

		public unsafe int PutUInt16(ushort n)
		{
			buf.Align(sizeof(ushort));
			return Put((byte*) &n, sizeof(ushort));
		}

		public unsafe int PutInt32(int n)
		{
			buf.Align(sizeof(int));
			return Put((byte*) &n, sizeof(int));
		}

		public unsafe int PutUInt32(uint n)
		{
			buf.Align(sizeof(uint));
			return Put((byte*) &n, sizeof(uint));
		}
	}
}
