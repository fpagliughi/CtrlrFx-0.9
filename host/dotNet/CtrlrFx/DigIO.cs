// DigIO.cs

using System;
using System.Collections.Generic;

namespace CtrlrFx
{
	public interface IDigIn
	{
		bool Value(int bit);
	}

	public interface IDigOut
	{
		void Set(int bit);
		void Set(int bit, bool on);
		void Clear(int bit);
		void Toggle(int bit);
	}
}
