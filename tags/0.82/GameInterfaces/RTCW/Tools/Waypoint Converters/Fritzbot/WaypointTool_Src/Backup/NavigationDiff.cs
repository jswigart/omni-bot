using System;
using System.Collections;

namespace WaypointTool
{
	public class NavigationDiff
	{
		private ArrayList _differentNodes = new ArrayList();
		private ArrayList _differentActions = new ArrayList();
		private ArrayList _differentRoutes = new ArrayList();

		public ArrayList DifferentNodes
		{
			get { return _differentNodes; }
			set { _differentNodes = value; } 
		}

		public ArrayList DifferentActions
		{
			get { return _differentActions; }
			set { _differentActions = value; } 
		}

		public ArrayList DifferentRoutes 
		{
			get { return _differentRoutes; }
			set { _differentRoutes = value; }
		}

		public void DoDiff(NavigationReader left, NavigationReader right) 
		{
			int nodeIndex = 0;
			int rightNodeIndex = right.Nodes.Count - 1;
			int rightActionIndex = right.Actions.Count - 1;
			int rightRouteIndex = right.Routes.Count - 1;

			foreach (Node leftNode in left.Nodes) 
			{
				if (nodeIndex <= rightNodeIndex) 
				{
					Node rightNode = (Node)right.Nodes[nodeIndex];

					if (!leftNode.Equals(rightNode)) 
					{
						_differentNodes.Add(new NodeDifference(leftNode, rightNode));
					}
				} 
				else 
				{
					_differentNodes.Add(new NodeDifference(leftNode, null));
				}

				nodeIndex++;
			}

			if (nodeIndex <= rightNodeIndex) 
			{
				for (int x = nodeIndex; x < right.Nodes.Count; x++) 
				{
					_differentNodes.Add(new NodeDifference(null, (Node)right.Nodes[x]));
				}
			}

			int actionIndex = 0;

			foreach (Action leftAction in left.Actions) 
			{
				if (actionIndex <= rightActionIndex) 
				{
					Action rightAction = (Action)right.Actions[actionIndex];

					if (!leftAction.Equals(rightAction)) 
					{
						_differentActions.Add(new ActionDifference(leftAction, rightAction));
					}
				} 
				else 
				{
					_differentActions.Add(new ActionDifference(leftAction, null));
				}

				actionIndex++;
			}

			if (actionIndex <= rightActionIndex) 
			{
				for (int x = actionIndex; x < right.Actions.Count; x++) 
				{
					_differentActions.Add(new ActionDifference(null, (Action)right.Actions[x]));
				}
			}

			int routeIndex = 0;
			foreach (Route leftRoute in left.Routes) 
			{
				if (routeIndex <= rightRouteIndex) 
				{
					Route rightRoute = (Route)right.Routes[routeIndex];

					if (!leftRoute.Equals(rightRoute)) 
					{
						_differentRoutes.Add(new RouteDifference(leftRoute, rightRoute));
					}
				} 
				else 
				{
					_differentRoutes.Add(new RouteDifference(leftRoute, null));
				}

				routeIndex++;
			}

			if (routeIndex <= rightRouteIndex) 
			{
				for (int x = routeIndex; x < right.Routes.Count; x++) 
				{
					_differentRoutes.Add(new RouteDifference(null, (Route)right.Routes[x]));
				}
			}
		}
	}
}
