// Buffer.cs
// The CtrlrFx Buffer class for .NET

using System;
using System.Collections.Generic;
using System.Text;

namespace CtrlrFx
{
	public class Buffer<T>
	{
		private T[] arr = null;
		private int pos = 0,
					lim = 0,
					mark = 0;

		public Buffer()
		{
		}

		public Buffer(int n)
		{
			arr = new T[n];
			lim = n;
		}

		public Buffer(T[] arr)
		{
			if (arr == null || arr.Length == 0)
				return;

			lim = arr.Length;

			this.arr = new T[lim];
			Array.Copy(arr, this.arr, lim);
		}

		public Buffer(Buffer<T> buf)
		{
			this.arr = new T[buf.arr.Length];
			Array.Copy(buf.arr, this.arr, buf.arr.Length);

			this.pos = buf.pos;
			this.lim = buf.lim;
			this.mark = buf.mark;
		}

		public T[] Data
		{
			get { return arr; }
		}

		public int Capacity
		{
			get { return (arr == null) ? 0 : arr.Length; }
		}

		public int Size
		{
			get { return lim; }
			set
			{
				if (value <= Capacity) {
					lim = value;
					pos = mark = 0;
				}
				// TODO: Else thow?
			}
		}

		public int Position
		{
			get { return pos; }
			set
			{
				if (value <= lim)
					pos = value;
				// TODO: Else throw?
			}
		}

		public void IncrPosition()
		{
			if (pos < lim)
				pos++;
		}

		public void IncrPosition(int n)
		{
			if (pos+n <= lim)
				pos += n;
		}

		public int Available
		{
			get { return lim - pos; }
		}

		public bool HasAvailable
		{
			get { return pos < lim; }
		}

		public bool Full
		{
			get { return pos == lim; }
		}

		public bool Empty
		{
			get { return pos == lim; }
		}

		public T this[int i]
		{
			get { return arr[i]; }
			set { arr[i] = value; }
		}

		public Buffer<T> Fill(T val)
		{
			while (pos < lim)
				arr[pos++] = val;

			return this;
		}

		public Buffer<T> Mark()
		{
			mark = pos;
			return this;
		}

		public Buffer<T> Reset()
		{
			pos = mark;
			return this;
		}

		public Buffer<T> Rewind()
		{
			pos = mark = 0;
			return this;
		}

		public Buffer<T> Clear()
		{
			pos = mark = 0;
			lim = Capacity;
			return this;
		}

		public Buffer<T> Flip()
		{
			lim = pos;
			pos = mark = 0;
			return this;
		}

		public Buffer<T> Compact()
		{
			int n = lim - pos;

			for (int i=0; i<n; ++i)
				arr[i] = arr[pos+i];

			lim -= pos;
			pos = 0;

			return this;
		}

		public Buffer<T> Align(int n)
		{
			int rem = pos % n;

			if (rem != 0) {
				int p = pos + n - rem;

				if (p <= lim)
					pos = p;
				// TODO: Else throw?
			}
			return this;
		}

		public void Put(T val)
		{
			if (pos < lim)
				arr[pos++] = val;
			// TODO: else throw?
		}

		public void Put(T[] arr)
		{
			int n = arr.Length;

			if (pos+n <= lim) {
				Array.Copy(arr, 0, this.arr, pos, n);
				pos += n;
			}
		}

		public T Get()
		{
			//TODO: Check limit
			return arr[pos++];
		}

		public void Get(T[] arr)
		{
			int n = arr.Length;

			if (pos+n <= lim) {
				Array.Copy(this.arr, pos, arr, 0, n);
				pos += n;
			}
			// TODO: else throw
		}
	}
}
