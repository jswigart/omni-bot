using System;

namespace WaypointTool
{
	public class RouteDifference
	{
		private Route _leftRoute;
		private Route _rightRoute;

		public RouteDifference(Route left, Route right) 
		{
			_leftRoute = left;
			_rightRoute = right;
		}

		public Route LeftRoute
		{
			get { return _leftRoute; }
			set { _leftRoute = value; } 
		}

		public Route RightRoute
		{
			get { return _rightRoute; }
			set { _rightRoute = value; } 
		}
	}
}
