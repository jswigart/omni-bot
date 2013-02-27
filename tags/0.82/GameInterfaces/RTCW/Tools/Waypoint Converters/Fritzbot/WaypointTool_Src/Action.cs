using System;

namespace WaypointTool
{
	public class Action : NavigationElementBase
	{
		private int _iD = 0;
		private int _allyAction = Int32.MinValue;
		private int _axisAction = Int32.MinValue;
		private int _radius = Int32.MinValue;
		private int _goal = Int32.MinValue;
		private int _group = Int32.MinValue;
		private int _class = Int32.MinValue;
		private int _closeNode = Int32.MinValue;
		private string _links = "";
		private int _prone = Int32.MinValue;
		private int _active = Int32.MinValue;
		private int _entity = Int32.MinValue;

		public void FromActionRow(Actions.ActionRow actionRow) 
		{
            if (!actionRow.IsIDNull()) ID = Convert.ToInt32(actionRow.ID);
			if (!actionRow.IsAllyActionNull()) AllyAction = Convert.ToInt32(actionRow.AllyAction);
			if (!actionRow.IsAxisActionNull()) AxisAction = Convert.ToInt32(actionRow.AxisAction);
			if (!actionRow.IsRadiusNull()) Radius = Convert.ToInt32(actionRow.Radius);
			if (!actionRow.IsGoalNull()) Goal = Convert.ToInt32(actionRow.Goal);
			if (!actionRow.IsGroupNull()) Group = Convert.ToInt32(actionRow.Group);
			if (!actionRow.IsClassNull()) Class = Convert.ToInt32(actionRow.Class);
			if (!actionRow.IsCloseNodeNull()) CloseNode = Convert.ToInt32(actionRow.CloseNode);
			if (!actionRow.IsLinksNull()) Links = actionRow.Links;
			if (!actionRow.IsProneNull()) Prone = Convert.ToInt32(actionRow.Prone);
			if (!actionRow.IsActiveNull()) Active = Convert.ToInt32(actionRow.Active);
			if (!actionRow.IsEntityNull()) Entity = Convert.ToInt32(actionRow.Entity);
		}

		public override bool Equals(object obj)
		{
			Action action = obj as Action;

			if (action == null) return false;

			return action.AllyAction == AllyAction
				&& action.AxisAction == AxisAction
				&& action.Radius == Radius
				&& action.Goal == Goal
				&& action.Group == Group
				&& action.Class == Class
				&& action.CloseNode == CloseNode
				&& action.Links == Links
				&& action.Prone == Prone
				&& action.Active == Active
				&& action.Entity == Entity;
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

		public int AllyAction
		{
			get { return _allyAction; }
			set { _allyAction = value; } 
		}

		public int AxisAction
		{
			get { return _axisAction; }
			set { _axisAction = value; } 
		}

		public int Radius
		{
			get { return _radius; }
			set { _radius = value; } 
		}

		public int Goal
		{
			get { return _goal; }
			set { _goal = value; } 
		}

		public int Group
		{
			get { return _group; }
			set { _group = value; } 
		}

		public int Class
		{
			get { return _class; }
			set { _class = value; } 
		}

		public int CloseNode
		{
			get { return _closeNode; }
			set { _closeNode = value; } 
		}

		public string Links
		{
			get { return _links; }
			set { _links = value; } 
		}

		public int Prone
		{
			get { return _prone; }
			set { _prone = value; } 
		}

		public int Active
		{
			get { return _active; }
			set { _active = value; } 
		}

		public int Entity
		{
			get { return _entity; }
			set { _entity = value; } 
		}

		private byte[] _bytes;
		public byte[] Bytes
		{
			get { return _bytes; }
			set { _bytes = value; } 
		}

		public byte[] ToByteArray() 
		{
			UpdateInt(AllyAction, 8, ref _bytes);
			UpdateInt(AxisAction, 10, ref _bytes);
			UpdateInt(Group, 6, ref _bytes);
			UpdateInt(Active, 0, ref _bytes);
			UpdateInt(CloseNode, 4, ref _bytes);
			UpdateInt(Entity, 26, ref _bytes);
			UpdateInt(Radius, 24, ref _bytes);
			UpdateInt(Class, 28, ref _bytes);
			UpdateInt(Goal, 30, ref _bytes);
			UpdateInt(Prone, 32, ref _bytes);

			// clear all links
			UpdateInt(65535, 44, ref _bytes);
			UpdateInt(65535, 46, ref _bytes);
			UpdateInt(65535, 48, ref _bytes);
			UpdateInt(65535, 50, ref _bytes);

			if (Links.Trim().Length != 0) 
			{
				string[] links = Links.Split(new char[] { ' ' });

				int pos = 44;

				for (int x = 0; x < links.Length; x++) 
				{
					try 
					{
						int action = Convert.ToInt32(links[x]);
						UpdateInt(action, pos, ref _bytes);
						pos += 2;
					} 
					catch {}
				}
			} 

			return _bytes;
		}
	}
}
