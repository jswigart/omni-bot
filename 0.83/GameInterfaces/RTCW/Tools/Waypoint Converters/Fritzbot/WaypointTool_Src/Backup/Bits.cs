using System;
using System.Collections;
using System.Text;

namespace WaypointTool
{
	/// -----------------------------------------------------------------------------
	/// Project	 : Bin
	/// Class	 : Bits
	/// 
	/// Use of this software is privilege of CodeProject members. Feel free to modify
	/// and report any bugs to Codeproject.com.
	/// 
	/// -----------------------------------------------------------------------------
	/// <summary>
	/// This class handles the bit manipulations.
	/// </summary>
	/// <remarks>
	/// This creates an array of booleans to store the binary representation of this class.
	/// </remarks>
	/// <history>
	/// 	[Rudy Guzman]	4/10/2005	Created
	/// </history>
	/// -----------------------------------------------------------------------------
	public class Bits
	{
		bool [] _bitArray;

		public int Count
		{
			get
			{ 
				if (_bitArray == null)
					return 0;
				else
					return _bitArray.Length;
			}
		}

		//----------------------------------------------------------------------------------------------------------------------------------------
		//			CONSTRUCTOR METHODS
		//----------------------------------------------------------------------------------------------------------------------------------------
		private Bits()
		{
		}
		/// <summary>
		/// Initialize a new instance of this class from an array that is ordered in a human readable format.  
		/// For example, if the input is:0x0B, 0x23, 0x84
		/// 0x0B = 0000 1011
		/// 0x23 = 0010 0011
		/// 0x84 = 1000 0100
		/// Then the array of bits will contain 0000 1011 0010 0011 1000 0100
		/// Note that the MSbit is actually at position 0 in the array (outer left)
		/// </summary>
		/// <param name="byteArray"></param>
		public Bits(byte [] byteArray)
		{
			if (byteArray.Length > 0)
				createArray(byteArray);
			else
				throw new ApplicationException("Can't convert to bits a zero length array of bytes");
		}

		/// <summary>
		/// Initialize a new instance of this class from an unsigned integer number.  
		/// Since we may need only a few bits of the 32 bit integer number, the second 
		/// parameter helps by specifying the number of useful bits to construct this class.
		/// </summary>
		/// <param name="data">Number that will be converted to bit array.</param>
		/// <param name="numBits">Number of bits required to build an array of bits.</param>
		public Bits(uint data, int numBits)
		{
			if (numBits == 0)
				throw new ApplicationException("Can't initialize bits when number of bits is 0");
			_bitArray = new bool[numBits];

			int bitIndex = 0;
			uint mask = Convert.ToUInt32(Math.Pow(2, numBits-1)); 
			for (int maskIndex = numBits-1; maskIndex >= 0; maskIndex--)
			{
				if ((data & mask) > 0)
					_bitArray[bitIndex] = true;
				else
					_bitArray[bitIndex] = false;
				bitIndex++;
				mask >>=1;
			}
		}

		/// <summary>
		/// Initialize a new instance of this class from an integer number.  
		/// Since we may need only a few bits of the 32 bit integer number, the second 
		/// parameter helps by specifying the number of useful bits to construct this class.
		/// </summary>
		/// <param name="data">Number that will be converted to bit array.</param>
		/// <param name="numBits">Number of bits required to build an array of bits.</param>
		public Bits(int data, int numBits)
		{
			if (numBits == 0)
				throw new ApplicationException("Can't initialize bits when number of bits is 0");

			if (data < 0)
				data += Convert.ToInt32(System.Math.Pow(2, numBits));

			_bitArray = new bool[numBits];

			int bitIndex = 0;
			int mask = Convert.ToInt32(Math.Pow(2, numBits-1)); 
			for (int maskIndex = numBits-1; maskIndex >= 0; maskIndex--)
			{
				if ((data & mask) > 0)
					_bitArray[bitIndex] = true;
				else
					_bitArray[bitIndex] = false;
				bitIndex++;
				mask >>=1;
			}
		}

		/// <summary>
		/// Initialize a new instance of this class from an instance of "Hex" data.
		/// </summary>
		/// <param name="hexData">Hexadecimal data that will be used to create a bit array.</param>
		public Bits(Hex hexData)
		{
			createArray(hexData.ToBytes());
		}

		/// <summary>
		/// Initialize a new instance of this class from a string that represents a binary number.
		/// </summary>
		/// <param name="data">String that contains a valid binary value.</param>
		public Bits(string data)
		{
			if (ValidBitString(data))
			{
				_bitArray = new bool[data.Length];
				for (int i = 0; i < data.Length; i++)
				{
					if (data[i] == '1')
						_bitArray[i] = true;
					else
						_bitArray[i] = false;
				}
			}
			else 
				throw new ApplicationException("Invalid binary number: " + data);
		}

		/// <summary>
		/// Constructs this class by using another array of bits.
		/// </summary>
		/// <param name="boolBits"></param>
		public Bits(bool [] boolBits)
		{
			if (boolBits.Length > 0)
			{
				_bitArray = new bool[boolBits.Length];
				for (int i = 0; i < _bitArray.Length; i++)
					_bitArray[i] = boolBits[i];
			}
			else
				throw new ApplicationException("Can't initialize Bits because the input arry of boolBits can't be zero length");
		}

		//----------------------------------------------------------------------------------------------------------------------------------------
		//			CONVERSION METHODS
		//----------------------------------------------------------------------------------------------------------------------------------------

		/// <summary>
		/// Convert to unsigned integer.
		/// </summary>
		/// <returns>Unsigned integer that represents the binary value of this instance.</returns>
		public uint ToUInt()
		{
			uint result = 0;
			if (_bitArray != null)
			{
				for (int i = 0; i < _bitArray.Length; i++)
				{
					//bit at position 0 is the MSBit so left shift bits from _bitArray
					result <<= 1;
					if (_bitArray[i] == true)
						result |= 1;
				}
			}
			return result;
		}

		/// <summary>
		/// Convert to SIGNED integer.
		/// </summary>
		/// <returns>
		/// SIGNED integer that represents the binary value of this instance.</returns>
		/// <remarks>
		/// This method uses two's complement method to convert convert from binary to signed integer number.
		/// </remarks>
		public int ToInt()
		{
			int result = Convert.ToInt32(this.ToUInt());
			int numBits = _bitArray.Length;
			int maxPositiveValue = Convert.ToInt32(System.Math.Pow(2, numBits-1)-1);
			if (result > maxPositiveValue)
				result -= Convert.ToInt32(System.Math.Pow(2, numBits));

			return result;
		}

		/// <summary>
		/// Convert to bytes
		/// </summary>
		/// <returns>Byte array that represents the binary value of this instance.</returns>
		/// <remarks>
		/// The byte array contains Least Significant Bit first.
		/// </remarks>
		public byte[] ToBytes()
		{
			byte [] bytes = null;
			if (_bitArray != null)
			{
				byte currByte = 0;
				int numBytes = (int)(_bitArray.Length-1) / 8 + 1;
				bytes = new byte[numBytes];
				int bitCnt = 0;
				int byteIndex = 0;
				for (int bitIndex = _bitArray.Length-1; bitIndex >= 0; bitIndex--)
				{
					currByte >>= 1;
					if (_bitArray[bitIndex] == true)
						currByte |= 0x80;
					//store byte every 8 bits
					bitCnt++;
					if ( bitCnt % 8 == 0)
					{
						bytes[byteIndex] = currByte;
						currByte = 0;
						byteIndex++;
					}
				}
				if (bitCnt % 8 != 0)
					currByte >>= (8 - (bitCnt % 8));  //right shift remaining bits
				//if last set of bits is less than 8 bits then it didn't get stored yet, so make sure to store it
				if (byteIndex < numBytes)
				{
					bytes[byteIndex] = currByte;
				}
				System.Array.Reverse(bytes);
			}
			return bytes;
		}

		/// <summary>
		/// Convert to Hex number.
		/// </summary>
		/// <returns>Hex number that represents the binary value of this instance.</returns>
		public Hex ToHex()
		{
			Hex hexData = new Hex(this.ToBytes());
			return hexData;
		}


		/// <summary>
		/// Converts the binary value of this instance to a string representation that
		/// follows a human readable convention (Least Significant Bit to farthest right).
		/// </summary>
		/// <returns>String that represents the binary value of this instance.</returns>
		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();
			if (_bitArray.Length > 0)
			{
				for (int i = 0; i < _bitArray.Length; i++)
				{
					if (_bitArray[i] == true)
						sb.Append('1');
					else
						sb.Append('0');
				}
			}
			return sb.ToString();
		}

		/// <summary>
		/// Convert to array of boolean.
		/// </summary>
		/// <returns>Array of boolean that represents the binary number of this instance.</returns>
		/// <remarks>The order of the bits in the array is: Least Significant Bit starts at 0 index position.</remarks>
		public bool[] ToBoolArray()
		{
			bool [] array = null;
			if (_bitArray.Length > 0)
			{
				array = new bool[_bitArray.Length];
				for (int i = 0; i < _bitArray.Length; i++)
					array[i] = _bitArray[i];
			}
			return array;
		}

		/// <summary>
		/// Extract bits from the specified location.  The position is calculated based on byte position,
		/// bit position within that byte position, and number of bits.
		/// Exact formula is as follows:
		///					From: (bytePosition + 1) * 8 - 1;
		///					  To: (bytePosition + 1) * 8 - (numBits + bitPosition);
		/// </summary>
		/// <param name="bytePosition">Byte position is indicated based on a human readable format.  
		/// For example, if this instance contains 32 bits such as: 0xAABBCCDD then byte byte 2 will be
		/// position of CC.</param>
		/// <param name="bitPosition">Indicates the bit position with respect to the byte position.
		/// For example, given 0xAABBCCDD and given byte 2 then byte 2 is 0xCC or 11001100 therefore,
		/// a bit position of 0 is the least significant bit which is a zero in this case.</param>
		/// <param name="numBits">The number of bits to get given a byte position, and bit position.
		/// For example, given 0xAABBCCDD, byte position 2, bit position 3 and number of bits=4 then
		/// the value returned will be 1001 (byte 2 = 11001100, then to get 4 bits from bit position
		/// 3 is: -1001--- the hyphens are the other bits that get removed.</param>
		/// <returns></returns>
		public Bits GetBits(int bytePosition, int bitPosition, int numBits)
		{
			int byteIndex = (bytePosition + 1) * 8 - 1;
			int from = byteIndex - (bitPosition + numBits) + 1;
			int to = byteIndex - bitPosition;
			if (from < 0 || to > (_bitArray.Length - 1) )
			{
				throw new ApplicationException("Unable to extract bits because the combination of byte position, bit position, and number of bits is not valid");
			}

			bool [] extractedBits = new bool[numBits];
			int index = 0;
			for (int i = from; i <= to; i++)
			{
				extractedBits[index] = _bitArray[i];
				index++;
			}
			
			return new Bits(extractedBits);	
		}



		#region UTILITY METHODS
		/// <summary>
		/// Verify that the string contains a valid sequence of bits.
		/// </summary>
		/// <param name="data">String representation of a binary number.</param>
		/// <returns>True if all digits are binary.</returns>
		public bool ValidBitString(string data)
		{
			bool validString = true;
			if (data != null)
			{
				for (int i = 0; i < data.Length; i++)
				{
					if (data[i] != '0' && data[i] != '1')
					{
						validString = false;
						break;
					}
				}
			}

			return validString;
		}

		/// <summary>
		/// Create a sequence of binary bits from an array of bytes.
		/// </summary>
		/// <param name="byteArray">Array of bytes that will be converted to bits.</param>
		/// <remarks>
		/// The it array is created in a human readable format.  For example,
		/// If we have an array such as AA BB CC DD, where AA is index 0 then the
		/// bit array will contain 10101010 10111011 11001100 11011101, where left most bit is the zero index.
		/// The representation of bits in this format makes it easy for string conversions.
		/// </remarks>
		private  void createArray(byte[] byteArray)
		{
			byte currByte, mask;
			_bitArray = new bool[byteArray.Length * 8];
		
			int bitIndex = 0;
			for (int i = 0; i < byteArray.Length; i++)
			{
				currByte = byteArray[i];
				//the mask initially is: 1000 0000 then we can test the 7th bit
				//then it becomes       : 0100 0000 so we can test the 6th bit
				mask = 0x80;
				for (int maskIndex = 7; maskIndex >= 0; maskIndex--)
				{
					if ((currByte & mask) > 0)
						_bitArray[bitIndex] = true;
					else
						_bitArray[bitIndex] = false;
					bitIndex++;
					mask >>=1;
				}
			}
		}
		#endregion
	}
}
