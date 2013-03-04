using System;

namespace WaypointTool
{
	// these are broken up for knowing what type of input
	// to render and will also be used for validating that
	// input against a .nav
	public enum KeywordInputType : int
	{
		Action = 1,
		Group = 2,
		Goal = 3,
		Node = 4,
		Flag = 5,
		Boolean = 6,
		PredefinedList = 7,
		Integer = 8
	}
}
