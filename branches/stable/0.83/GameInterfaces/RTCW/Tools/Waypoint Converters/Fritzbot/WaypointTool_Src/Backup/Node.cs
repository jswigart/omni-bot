using System;

namespace WaypointTool
{
	public class Node : NavigationElementBase
	{
		private int _iD = 0;
		private int _entity = Int32.MinValue;
		private int _radius = Int32.MinValue;
		private int _flags = Int32.MinValue;
		private int _group = Int32.MinValue;
		private int _team = Int32.MinValue;
		private int _numberOfConnects = Int32.MinValue;
		private int _connect1 = -1;
		private int _connect2 = -1;
		private int _connect3 = -1;
		private int _connect4 = -1;

		public void FromNodeRow(Nodes.NodeRow nodeRow) 
		{
			if (!nodeRow.IsEntityNull()) Entity = Convert.ToInt32(nodeRow.Entity);
			if (!nodeRow.IsRadiusNull()) Radius = Convert.ToInt32(nodeRow.Radius);
			if (!nodeRow.IsFlagNull()) Flags = Convert.ToInt32(nodeRow.Flag);
			if (!nodeRow.IsGroupNull()) Group = Convert.ToInt32(nodeRow.Group);
			if (!nodeRow.IsTeamNull()) Team = Convert.ToInt32(nodeRow.Team);
			if (!nodeRow.IsConnection1Null()) Connect1 = Convert.ToInt32(nodeRow.Connection1);
			if (!nodeRow.IsConnection2Null()) Connect2 = Convert.ToInt32(nodeRow.Connection2);
			if (!nodeRow.IsConnection3Null()) Connect3 = Convert.ToInt32(nodeRow.Connection3);
			if (!nodeRow.IsConnection4Null()) Connect4 = Convert.ToInt32(nodeRow.Connection4);
		}

		public override bool Equals(object obj)
		{
			Node node = obj as Node;

			if (node == null) return false;

			return node.Entity == Entity
				&& node.Radius == Radius
				&& node.Flags == Flags
				&& node.Group == Group
				&& node.Team == Team
				&& node.NumberOfConnects == NumberOfConnects
				&& node.Connect1 == Connect1
				&& node.Connect2 == Connect2
				&& node.Connect3 == Connect3
				&& node.Connect4 == Connect4;
		}

		public override int GetHashCode()
		{
			return base.GetHashCode ();
		}

		public int ID
		{
			get { return _iD; }
			set { _iD = value; } 
		}

		public int Entity
		{
			get { return _entity; }
			set { _entity = value; } 
		}

		public int Radius
		{
			get { return _radius; }
			set { _radius = value; } 
		}

		public int Flags
		{
			get { return _flags; }
			set { _flags = value; } 
		}

		public int Group
		{
			get { return _group; }
			set { _group = value; } 
		}

		public int Team
		{
			get { return _team; }
			set { _team = value; } 
		}

		public int NumberOfConnects
		{
			get { return _numberOfConnects; }
			set { _numberOfConnects = value; } 
		}

		public int Connect1
		{
			get { return _connect1; }
			set { _connect1 = value; } 
		}

		public int Connect2
		{
			get { return _connect2; }
			set { _connect2 = value; }
		} 

		public int Connect3
		{
			get { return _connect3; }
			set { _connect3 = value; }
		}

		public int Connect4
		{
			get { return _connect4; }
			set { _connect4 = value; }
		}

		private byte[] _bytes;
		public byte[] Bytes
		{
			get { return _bytes; }
			set { _bytes = value; } 
		}

		public byte[] ToByteArray() 
		{
			_bytes[12] = (byte)Flags;
			
			UpdateInt(Entity, 14, ref _bytes);

			int numberOfConnects = 0;
			if (_connect1 != -1) numberOfConnects++;
			if (_connect2 != -1) numberOfConnects++;
			if (_connect3 != -1) numberOfConnects++;
			if (_connect4 != -1) numberOfConnects++;

			// make sure connects are sequential
			FixConnects();

			UpdateInt(numberOfConnects, 16, ref _bytes);
			UpdateInt(Radius, 18, ref _bytes);
			UpdateInt(Team, 20, ref _bytes);
			UpdateInt(Group, 22, ref _bytes);

			// create another byte[] at a size big enough for the
			// amount of connects defined
			byte[] newBytes = new byte[_bytes.Length + (numberOfConnects * 2)];

			for (int x = 0; x < _bytes.Length; x++) 
			{
				newBytes[x] = _bytes[x];
			}	

			for (int x = 0; x < numberOfConnects; x++) 
			{
				int start = _bytes.Length + (x * 2);
				
				switch (x) 
				{
					case 0:
						UpdateInt(Connect1, start, ref newBytes);
						break;
					case 1:
						UpdateInt(Connect2, start, ref newBytes);
						break;
					case 2:
						UpdateInt(Connect3, start, ref newBytes);
						break;
					case 3:
						UpdateInt(Connect4, start, ref newBytes);
						break;
				}
			}

			return newBytes;
		}

		private void FixConnects() 
		{
			for (int x = 4; x > 1; x--) 
			{
				for (int y = x; y > 1; y--) 
				{
					switch (y) 
					{
						case 4:
							if (_connect4 != -1 && _connect3 == -1) 
							{
								_connect3 = _connect4;
								_connect4 = -1;
							}
							break;
						case 3:
							if (_connect3 != -1 && _connect2 == -1) 
							{
								_connect2 = _connect3;
								_connect3 = -1;
							}
							break;
						case 2:
							if (_connect2 != -1 && _connect1 == -1) 
							{
								_connect1 = _connect2;
								_connect2 = -1;
							}
							break;
					}
				}
			}
		}
	}
}
