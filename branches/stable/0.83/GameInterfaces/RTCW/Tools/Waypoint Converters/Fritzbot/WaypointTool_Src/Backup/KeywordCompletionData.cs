using System;

using ICSharpCode.TextEditor.Gui.CompletionWindow;

namespace WaypointTool
{
	public class KeywordCompletionData : ICompletionData
	{
		private Keyword _keyword;

		private KeywordCompletionData() {}

		public KeywordCompletionData(Keyword keyword) 
		{
			_keyword = keyword;
		}

		public int ImageIndex
		{
			get
			{
				return 0;
			}
		}

		public void InsertAction(ICSharpCode.TextEditor.TextEditorControl control)
		{
			control.ActiveTextAreaControl.TextArea.InsertString(_keyword.Command);
		}

		public string Description
		{
			get
			{
				return _keyword.HelpText;
			}
		}

		public string[] Text
		{
			get
			{
				return new string[] { _keyword.Command };
			}
		}

		public int CompareTo(object obj)
		{
			Keyword keyword = obj as Keyword;

			if (keyword == null) return -1;

			return _keyword.Command.CompareTo(keyword.Command);
		}
	}
}
