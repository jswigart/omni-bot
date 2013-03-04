using System;
using System.IO;

namespace WaypointTool
{
	public class BotReader
	{
		public static Bots ReadBots(string dir) 
		{
			Bots data = new Bots();

			string[] files = Directory.GetFiles(dir, "*.bot");

			for (int x = 0; x < files.Length; x++) 
			{
				AppendBotRowFromFile(data, files[x]);
			}

			return data;
		}

		private static void AppendBotRowFromFile(Bots bots, string file) 
		{
			StreamReader reader = null;

			try 
			{
				reader = new StreamReader(file);

				bool inDef = false;
				string line = null;

				string name = Path.GetFileNameWithoutExtension(file);
				string funname = "";
				string classIndex = "";
				string weaponIndex = "";
				string teamIndex = "";

				while ((line = reader.ReadLine()) != null) 
				{
					/*
					{
					funname		^0Atla^3s
					class 		0
					weapon 		10
					team		1
					}
					*/
					if (inDef) 
					{
						if (line.Trim().StartsWith("funname")) 
						{
							funname = line.Substring(7).Trim();
						} 
						else if (line.Trim().StartsWith("class")) 
						{	
							classIndex = line.Substring(5).Trim();
						} 
						else if (line.Trim().StartsWith("weapon")) 
						{
							weaponIndex = line.Substring(6).Trim();
						} 
						else if (line.Trim().StartsWith("team"))
						{
							teamIndex = line.Substring(4).Trim();
						}
					} 
					else 
					{
						if (line.Trim().StartsWith("{")) 
						{
							inDef = true;
						}

						if (line.Trim().StartsWith("}")) 
						{
							break;
						}
					}
				}

				bots.Bot.AddBotRow(name, funname, classIndex, weaponIndex, teamIndex);
			} 
			catch 
			{
				// just allow for continuing to next file by not raising the exception
			} 
			finally 
			{
				if (reader != null) 
				{
					reader.Close();
				}
			}
		}
	}
}
