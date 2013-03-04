using System;

namespace WaypointTool
{
	/// -----------------------------------------------------------------------------
	/// Project	 : Bin
	/// Class	 : Conversion
	/// 
	/// Use of this software is privilege of CodeProject members. Feel free to modify
	/// and report any bugs to Codeproject.com.
	/// 
	/// -----------------------------------------------------------------------------
	/// <summary>
	/// This class simplifies the manipulation for the most common conversions required
	/// when using binary, hex, and decimal numbers.
	/// </summary>
	/// <remarks>
	/// This class lists all the possible conversions that could be required from Binary
	/// to Hex and vice versa, Binary to Decimal and vice versa, Hex to Decimal and vice versa,
	/// Decimal signed to unsigned and vice versa.
	/// </remarks>
	/// <history>
	/// 	[Rudy Guzman]	4/10/2005	Created
	/// </history>
	/// -----------------------------------------------------------------------------
	public class Conversion
	{
		public Conversion()
		{
		}

		/// <summary>
		/// Convert integer number to binary.
		/// </summary>
		/// <param name="number">Integer number to be converted.</param>
		/// <param name="numBits">Since an integer number is 32 bits, we might need only
		/// only a few bits converted to binary so we don't get a 32 bit binary string.</param>
		/// <returns>String representation of the binary number.</returns>
		public static string IntToBin(int number, int numBits)
		{
			return new Bits(number, numBits).ToString();
		}

		/// <summary>
		/// Convert unsigned integer to binary.
		/// </summary>
		/// <param name="number">Integer number to be converted.</param>
		/// <param name="numBits">Since an integer number is 32 bits, we might need only
		/// only a few bits converted to binary so we don't get a 32 bit binary string.</param>
		/// <returns>String representation of the binary number.</returns>
		public static string UIntToBin(uint number, int numBits)
		{
			return new Bits(number, numBits).ToString();
		}

		/// <summary>
		/// Convert hexadecimal string to binary string.
		/// </summary>
		/// <param name="hexData">String representation of a hexadecimal value.</param>
		/// <returns>String representation of a binary value.</returns>
		public static string HexToBin(string hexData)
		{
			return new Bits(new Hex(hexData).ToBytes()).ToString();
		}

		/// <summary>
		/// Convert binary string to integer.
		/// </summary>
		/// <param name="binData">String representation of a binary number.</param>
		/// <returns>Integer number converted from binary string.</returns>
		public static int BinToInt(string binData)
		{
			return new Bits(binData).ToInt();
		}

		/// <summary>
		/// Convert binary string to unsigned integer number.
		/// </summary>
		/// <param name="binData">String representation of a binary number.</param>
		/// <returns>Unsigned integer number converted from binary string.</returns>
		public static uint BinToUInt(string binData)
		{
			return new Bits(binData).ToUInt();
		}

		/// <summary>
		/// Convert Binary string to Hexadecimal number.
		/// </summary>
		/// <param name="binData">String representation of a binary number.</param>
		/// <returns>String representation of a hexadecimal number.</returns>
		public static string BinToHex(string binData)
		{
			return new Hex(new Bits(binData).ToBytes()).ToString();
		}

		/// <summary>
		/// Convert a hexadecimal string to integer number.
		/// </summary>
		/// <param name="hexData">String representation of a hexadecimal number.</param>
		/// <returns>Integer number converted from hexadecimal.</returns>
		public static int HexToInt(string hexData)
		{
			return new Hex(hexData).ToInt();
		}

		/// <summary>
		/// Convert a hexadecimal string to unsigned integer number.
		/// </summary>
		/// <param name="hexData">String representation of a hexadecimal number.</param>
		/// <returns>Unsigned integer number converted from hexadecimal.</returns>
		public static uint HexToUInt(string hexData)
		{
			return new Hex(hexData).ToUInt();
		}

		/// <summary>
		/// Convert integer to hexadecimal string.
		/// </summary>
		/// <param name="data">Number to be converted.</param>
		/// <param name="numBytes">Since integer number is 4 bytes long, we may need to convert
		/// less than 4 bytes, so this parameter specifies the number of bytes we want to
		/// convert to a hexadecimal value.</param>
		/// <returns>String representation of a hexadecimal value.</returns>
		public static string IntToHex(int data, int numBytes)
		{
			return new Hex(data, numBytes).ToString();
		}

		/// <summary>
		/// Convert unsigned integer to hexadecimal string.
		/// </summary>
		/// <param name="data">Number to be converted.</param>
		/// <param name="numBytes">Since unsigned integer number is 4 bytes long, we may need to convert
		/// less than 4 bytes, so this parameter specifies the number of bytes we want to
		/// convert to a hexadecimal value.</param>
		/// <returns>String representation of a hexadecimal value.</returns>
		public static string UIntToHex(uint data, int numBytes)
		{
			return new Hex(data, numBytes).ToString();
		}

		/// <summary>
		/// Convert unsigned integer to signed integer.
		/// </summary>
		/// <param name="data">Integer number to be converted.</param>
		/// <param name="numBits">When converting to signed integer number we need
		/// to know the number of bits to determine if number is negative.</param>
		/// <returns>Integer number that was converted from unsigned integer.</returns>
		public static int UIntToInt(uint data, int numBits)
		{
			Int64 result = data;
			Int64 maxValue = Convert.ToInt32(Math.Pow(2, numBits - 1) - 1);
			if (data > maxValue)
				result = result - Convert.ToInt64(Math.Pow(2,numBits));
			return Convert.ToInt32(result);
		}

		/// <summary>
		/// Convert integer to unsigned signed integer.
		/// </summary>
		/// <param name="data">Unsigned integer number to be converted.</param>
		/// <param name="numBits">When converting from signed integer, we need
		/// to know the number of bits if the value is negative so we can do
		/// the proper conversion to unsigned integer.
		/// <returns>Unsigned integer number that was converted from unsigned integer.</returns>
		public static uint IntToUInt(int data, int numBits)
		{
			uint result = 0;
			uint maxValue = Convert.ToUInt32(Math.Pow(2, numBits) - 1);
			if (data < 0)
				result = Convert.ToUInt32(data + maxValue + 1);
			else 
				result = Convert.ToUInt32(data);
			return result;
		}


	}
}
