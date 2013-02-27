using System;

namespace WaypointTool
{
	public class NodeDifference
	{
		private Node _leftNode;
		private Node _rightNode;

		public NodeDifference(Node leftNode, Node rightNode) 
		{
			_leftNode = leftNode;
			_rightNode = rightNode;
		}

		public Node LeftNode
		{
			get { return _leftNode; }
			set { _leftNode = value; } 
		}

		public Node RightNode
		{
			get { return _rightNode; }
			set { _rightNode = value; } 
		}
	}
}
