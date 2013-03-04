using System;

namespace WaypointTool
{
	public abstract class NavigationElementBase
	{
		protected int GetInt(byte left, byte right) 
		{
			byte[] bytes = new byte[2];
			bytes[0] = left;
			bytes[1] = right;
			Bits bits = new Bits(bytes);
			return bits.ToInt();
		}

		protected byte[] FromInt(int val) 
		{
			Bits bits = new Bits(val, 16);
			return bits.ToBytes();
		}

		protected void UpdateInt(int val, int pos, ref byte[] bytes) 
		{
			byte[] intBytes = FromInt(val);
			bytes[pos] = intBytes[1];
			bytes[pos+1] = intBytes[0];
		}
	}
}
