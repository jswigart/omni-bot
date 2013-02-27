using System;
using System.Collections;

namespace WaypointTool
{
	public class KeywordInput
	{
		private KeywordInputType _inputType;
		private string _label;
		private string _helpText;
		private ArrayList _allowableValues = new ArrayList();
		private ArrayList _allowableActions = new ArrayList();

		public KeywordInput(KeywordInputType type, string label, string helpText) 
		{
			_inputType = type;
			_label = label;
			_helpText = helpText;
		}

		public KeywordInputType InputType
		{
			get { return _inputType; }
			set { _inputType = value; } 
		}

		public string Label
		{
			get { return _label; }
			set { _label = value; } 
		}

		public string HelpText
		{
			get { return _helpText; }
			set { _helpText = value; } 
		}

		public ArrayList AllowableValues
		{
			get { return _allowableValues; }
			set { _allowableValues = value; } 
		}

		public ArrayList AllowableActions
		{
			get { return _allowableActions; }
			set { _allowableActions = value; } 
		}
	}
}
