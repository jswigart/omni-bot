using System;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace WaypointTool
{
	public class AppData
	{
		private static string USERDATA_FILE = "user.data";

		public static UserData UserDataLoad()
		{
			object obj = DeserializeObject(GetAppDataFile());
			
			if (obj == null) return null;

			if (obj is UserData) 
			{
				UserData userData = (UserData)obj;
				return userData;
			}

			return null;
		}

		public static void UserDataSave(UserData data) 
		{
			SerializeObject(GetAppDataFile(), data);
		}

		private static string GetWorkingDirectory() 
		{
			string path = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().CodeBase);

			if (path.ToLower().StartsWith("file:\\")) 
			{
				path = path.Substring(6);
			}

			return path;
		}

		private static string GetAppDataFile() 
		{
			return GetWorkingDirectory() + Path.DirectorySeparatorChar + USERDATA_FILE;
		}
		private static void SerializeObject(string fileName, object obj) 
		{
			Stream stream = null;
			try 
			{
				stream = File.Open(fileName, FileMode.Create);
				BinaryFormatter formatter = new BinaryFormatter();
				formatter.Serialize(stream, obj);
			} 
			finally 
			{
				if (stream != null) 
				{
					stream.Close();
				}
			}
		}

		private static object DeserializeObject(string fileName) 
		{
			if (!File.Exists(fileName)) 
			{
				return null;
			}
			
			Stream stream = null;
			try 
			{
				stream = File.Open(fileName, FileMode.Open);
				BinaryFormatter formatter = new BinaryFormatter();
				return formatter.Deserialize(stream);
			} 
			finally 
			{
				if (stream != null) 
				{
					stream.Close();
				}
			}
		}
	}
}