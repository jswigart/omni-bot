using System;
using System.Collections;

namespace WaypointTool
{
	// will alternate teams as bots are added. this is to 
	// support the writing of the .bots file
	public class AlternatingBotRowCollection : CollectionBase
	{
		public int Add(Bots.BotRow bot) 
		{
			if (base.InnerList.Count == 0) 
			{
				return base.InnerList.Add(bot);
			} 
			else 
			{
				int count = 0;
				bool positionFound = false;
				string lastTeam = "";

				// loop through, find 2 adjacent with other team
				// and insert between (after enumerating)
				foreach (Bots.BotRow row in this) 
				{
					if (row.Team != bot.Team && lastTeam == row.Team) 
					{
						// insert at this pos
						positionFound = true;
						break;
					} 
					else 
					{
						lastTeam = row.Team;
						count++;
					}
				}

				if (positionFound) 
				{
					base.InnerList.Insert(count, bot);
					return count;
				} 
				else 
				{
					return base.InnerList.Add(bot);
				}
			}
		}

		public Bots.BotRow this[int index] 
		{
			get { return (Bots.BotRow)base.InnerList[index]; }
			set { base.InnerList[index] = value; }
		}
	}
}
