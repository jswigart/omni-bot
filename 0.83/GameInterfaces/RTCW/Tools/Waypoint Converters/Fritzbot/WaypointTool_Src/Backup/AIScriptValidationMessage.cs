using System;

using ICSharpCode.TextEditor.Document;

namespace WaypointTool
{
	public class AIScriptValidationMessage
	{
		private bool _isError = false;
		private int _lineNumber = Int32.MinValue;
		private string _message;
		private TextWord _errorWord;

		public bool IsError
		{
			get { return _isError; }
			set { _isError = value; } 
		}

		public int LineNumber
		{
			get { return _lineNumber; }
			set { _lineNumber = value; } 
		}

		public string Message
		{
			get { return _message; }
			set { _message = value; } 
		}

		public TextWord ErrorWord
		{
			get { return _errorWord; }
			set { _errorWord = value; } 
		}
	}
}
