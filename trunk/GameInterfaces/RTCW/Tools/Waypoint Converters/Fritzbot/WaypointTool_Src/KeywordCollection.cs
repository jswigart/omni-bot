using System;
using System.Collections;

namespace WaypointTool
{
	public class KeywordCollection : CollectionBase
	{
		// for a hashed lookup based on the keyword command
		// probably doesn't matter with so few keywords, but
		// this isn't a 1 lookup per keyword either...
		private Hashtable _lookupTable = new Hashtable();

		private void AddLookupWord(Keyword keyword) 
		{
			if (!_lookupTable.ContainsKey(keyword.Command.ToLower())) 
			{
				_lookupTable.Add(keyword.Command.ToLower(), keyword);
			}
		}

		private void RemoveLookupWord(Keyword keyword) 
		{
			if (_lookupTable.ContainsKey(keyword.Command.ToLower())) 
			{
				_lookupTable.Remove(keyword.Command);
			}
		}
			
		public int Add(Keyword keyword) 
		{
			AddLookupWord(keyword);
			return base.InnerList.Add(keyword);
		}

		public void Insert(int index, Keyword keyword) 
		{
			AddLookupWord(keyword);
			base.InnerList.Insert(index, keyword);
		}

		public void Remove(Keyword keyword) 
		{
			RemoveLookupWord(keyword);
			base.InnerList.Remove(keyword);
		}

		public bool Contains(Keyword keyword) 
		{
			return base.InnerList.Contains(keyword);
		}
		
		public int IndexOf(Keyword keyword) 
		{
			return base.InnerList.IndexOf(keyword);
		}

		public void CopyTo(Keyword[] array, int index) 
		{
			base.InnerList.CopyTo(array, index);
		}

		public Keyword this[int index] 
		{
			get { return (Keyword)base.InnerList[index]; }
			set { base.InnerList[index] = value; }
		}

		public void Sort(KeywordComparer.SortBy sortBy) 
		{
			Sort(new KeywordComparer(sortBy));
		}

		public void Sort(IComparer comparer) 
		{
			base.InnerList.Sort(comparer);
		}

		public Keyword GetWord(string command) 
		{
			return _lookupTable[command.ToLower()] as Keyword;
		}
	}
}
