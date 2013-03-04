using System;
using System.Collections;

namespace WaypointTool
{
	// TODO: should really be AIScriptKeyword :/
	public class Keyword
	{
		private string _friendlyName;
		private string _command;
		private bool _isHeader;
		private ArrayList _inputs = new ArrayList();
		private string _helpText;
		private bool _allowConditional = false;

		public Keyword() {}

		public Keyword (string command, string friendlyName, string helpText, bool allowConditional) 
		{
			_command = command;
			_friendlyName = friendlyName;
			_helpText = helpText;
			_allowConditional = allowConditional;
		}

		public string FriendlyName
		{
			get { return _friendlyName; }
			set { _friendlyName = value; } 
		}

		public string Command
		{
			get { return _command; }
			set { _command = value; } 
		}

		public bool IsHeader
		{
			get { return _isHeader; }
			set { _isHeader = value; } 
		}

		public ArrayList Inputs
		{
			get { return _inputs; }
			set { _inputs = value; } 
		}

		public string HelpText
		{
			get { return _helpText; }
			set { _helpText = value; } 
		}

		public bool AllowConditional
		{
			get { return _allowConditional; }
			set { _allowConditional = value; } 
		}

		public override string ToString()
		{
			return _friendlyName;
		}
	}
}
