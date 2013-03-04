using System;

namespace WaypointTool
{
	public class ActionDifference
	{
		private Action _leftAction;
		private Action _rightAction;

		public ActionDifference(Action leftAction, Action rightAction) 
		{
			_leftAction = leftAction;
			_rightAction = rightAction;
		}

		public Action LeftAction
		{
			get { return _leftAction; }
			set { _leftAction = value; } 
		}

		public Action RightAction
		{
			get { return _rightAction; }
			set { _rightAction = value; } 
		}
	}
}
