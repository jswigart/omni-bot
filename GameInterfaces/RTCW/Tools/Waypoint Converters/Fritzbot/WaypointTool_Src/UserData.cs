using System;
using System.Collections;
using System.Runtime.Serialization;

namespace WaypointTool
{
	[Serializable]
	public class UserData
	{
		private string _lastNavDirectory;
		private ArrayList _recentNavFiles = new ArrayList();

		private string _lastAIScriptDirectory;
		private ArrayList _recentAIScripts = new ArrayList();

		public string LastNavDirectory
		{
			get { return _lastNavDirectory; }
			set { _lastNavDirectory = value; } 
		}

		public void AddRecentNavFile(string file) 
		{
			if (_recentNavFiles.Count >= 10) 
			{
				_recentNavFiles.RemoveAt(0);
			}
			_recentNavFiles.Add(file);
		}

		public ArrayList RecentNavFiles
		{
			get { return _recentNavFiles; }
		}

		public string LastAIScriptDirectory
		{
			get { return _lastAIScriptDirectory; }
			set { _lastAIScriptDirectory = value; } 
		}

		public void AddRecentAIScript(string file) 
		{
			if (_recentAIScripts.Count >= 10) 
			{
				_recentAIScripts.RemoveAt(0);
			}
			_recentAIScripts.Add(file);
		}

		public ArrayList RecentAIScripts
		{
			get { return _recentAIScripts; }
		}

		public UserData() {}

		public UserData(SerializationInfo info, StreamingContext ctxt)
		{
			_lastNavDirectory = info.GetString("LastNavDirectory");
			_lastAIScriptDirectory = info.GetString("LastAIScriptDirectory");
		}

		public void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			info.AddValue("LastNavDirectory", _lastNavDirectory);
			info.AddValue("LastAIScriptDirectory", _lastAIScriptDirectory);
		}
	}
}
