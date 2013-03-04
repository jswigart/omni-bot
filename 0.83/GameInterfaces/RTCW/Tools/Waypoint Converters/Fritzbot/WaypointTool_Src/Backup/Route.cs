using System;

namespace WaypointTool
{
	public class Route : NavigationElementBase
	{
		private int _iD;
		private int _team;
		private int _radius;
		private string _actions = "";
		private string _pathActions = "";

		public void FromRouteRow(Routes.RouteRow routeRow) 
		{
			if (!routeRow.IsTeamNull()) Team = Convert.ToInt32(routeRow.Team);
			if (!routeRow.IsRadiusNull()) Radius = Convert.ToInt32(routeRow.Radius);
			if (!routeRow.IsActionsNull()) Actions = routeRow.Actions;
			if (!routeRow.IsPathActionsNull()) PathActions = routeRow.PathActions;
		}

		public override bool Equals(object obj)
		{
			Route route = obj as Route;

			if (route == null) return false;

			return route.Team == Team
				&& route.Radius == Radius
				&& route.Actions == Actions
				&& route.PathActions == PathActions;
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

		public int Team
		{
			get { return _team; }
			set { _team = value; } 
		}

		public int Radius
		{
			get { return _radius; }
			set { _radius = value; } 
		}

		public string Actions
		{
			get { return _actions; }
			set { _actions = value; } 
		}

		public string PathActions
		{
			get { return _pathActions; }
			set { _pathActions = value; } 
		}

		private byte[] _bytes;
		public byte[] Bytes
		{
			get { return _bytes; }
			set { _bytes = value; } 
		}

		public byte[] ToByteArray() 
		{
			UpdateInt(Team, 14, ref _bytes);
			UpdateInt(Radius, 12, ref _bytes);

			// clear all actions
			UpdateInt(65535, 16, ref _bytes);
			UpdateInt(65535, 18, ref _bytes);
			UpdateInt(65535, 20, ref _bytes);
			UpdateInt(65535, 22, ref _bytes);
			UpdateInt(65535, 24, ref _bytes);
			UpdateInt(65535, 26, ref _bytes);

			// clear all path actions
			UpdateInt(65535, 28, ref _bytes);
			UpdateInt(65535, 30, ref _bytes);
			UpdateInt(65535, 32, ref _bytes);
			UpdateInt(65535, 34, ref _bytes);
			UpdateInt(65535, 36, ref _bytes);
			UpdateInt(65535, 38, ref _bytes);

			if (Actions.Trim().Length != 0) 
			{
				string[] actions = Actions.Split(new char[] { ' ' });

				int pos = 16;

				for (int x = 0; x < actions.Length; x++) 
				{
					try 
					{
						int action = Convert.ToInt32(actions[x]);
						UpdateInt(action, pos, ref _bytes);
						pos += 2;
					} 
					catch {}
				}
			} 
			
			if (PathActions.Trim().Length != 0) 
			{
				string[] actions = PathActions.Split(new char[] { ' ' });

				int pos = 28;

				for (int x = 0; x < actions.Length; x++) 
				{
					try 
					{
						int action = Convert.ToInt32(actions[x]);
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