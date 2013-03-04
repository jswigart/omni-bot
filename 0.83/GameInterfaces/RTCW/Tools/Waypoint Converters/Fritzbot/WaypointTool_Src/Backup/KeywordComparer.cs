using System;
using System.Collections;

namespace WaypointTool
{
	// sort by Friendly Name
	public class KeywordComparer : IComparer 
	{
		public enum SortBy : int
		{
			FriendlyName = 1,
			Command = 2
		}

		private SortBy _sortBy = SortBy.FriendlyName;

		private KeywordComparer() {}

		public KeywordComparer(SortBy sortBy) 
		{
			_sortBy = sortBy;
		}

		public int Compare(object x, object y)
		{
			Keyword first = x as Keyword;
			Keyword second = y as Keyword;

			if (first == null || second == null) return 0;

			if (_sortBy == SortBy.FriendlyName) 
			{
				return first.FriendlyName.CompareTo(second.FriendlyName);
			} 
			else 
			{
				return first.Command.CompareTo(second.Command);
			}
		}
	}
}
