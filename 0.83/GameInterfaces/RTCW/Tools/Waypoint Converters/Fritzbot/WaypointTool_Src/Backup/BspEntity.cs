using System;

namespace WaypointTool
{
	public class BspEntity
	{
		private int _number;
		private string _text;

		public BspEntity(int number, string text) 
		{
			_number = number;
			_text = text;
		}

		public int Number
		{
			get { return _number; }
			set { _number = value; } 
		}

		public string Text
		{
			get { return _text; }
			set { _text = value; } 
		}

		public string ClassName 
		{
			get 
			{
				return GetProperty("classname");
			}
		}

		public string Target 
		{
			get 
			{
				return GetProperty("target");
			}
		}

		public string TargetName 
		{
			get 
			{
				return GetProperty("targetname");
			}
		}

		public string ScriptName 
		{
			get 
			{
				return GetProperty("scriptname");
			}
		}

		public string SpawnFlags
		{
			get 
			{
				return GetProperty("spawnflags");
			}
		}

		public string GetProperty(string name) 
		{
			name = "\"" + name + "\"";
			if (_text.IndexOf(name) != -1) 
			{
				int quote1 = _text.IndexOf("\"", _text.IndexOf(name) + name.Length + 1);
				int quote2 = _text.IndexOf("\"", quote1 + 1);

				return _text.Substring(quote1 + 1, quote2 - quote1 - 1);
			} 
			else 
			{
				return "UNDEFINED";
			}
		}
	}
}
