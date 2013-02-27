using System;

using ICSharpCode.TextEditor;
using ICSharpCode.TextEditor.Document;
using ICSharpCode.TextEditor.Gui.InsightWindow;

namespace WaypointTool
{
	public class KeywordInsightDataProvider : IInsightDataProvider
	{
		private KeywordCollection _keywords;
		private KeywordCollection _insightKeywords = new KeywordCollection();
		private IDocument _document = null;
		private TextArea _textArea  = null;
		
		public bool HasData 
		{
			get { return _insightKeywords.Count > 0; }
		}

		public void SetupDataProvider(string fileName, ICSharpCode.TextEditor.TextArea textArea)
		{
			_textArea = textArea;
			_document = textArea.Document;
			_keywords = KeywordFactory.GetKeywords();

			//TODO: just put these keywords in the factory and add a flag for "IsRequired" or something.
			//      it may just require a KeywordType (Header, Action, etc). This needs cleaned up 
			//      in the code completion data provider as well.

			// the keyword factory just has the non-required words
			Keyword sightDist = new Keyword("bot_SightDist", "", "", false);
			sightDist.Inputs.Add(new KeywordInput(KeywordInputType.Action, "Sight Distance", "How far the highest skill bot can see on this map. 1500 is good for non-foggy maps, 700 is good for foggy maps."));
			_keywords.Add(sightDist);

			_keywords.Add(new Keyword("spawnflag_is_priority", "",  "Whether or no bots focus on spawnflags.", false));
			_keywords.Add(new Keyword("cmdpost_is_priority", "",  "Whether or not command posts critical on this map.", false));
			_keywords.Add(new Keyword("construct_is_priority", "",  "Whether or not engineers focus more on constructibles.", false));
			_keywords.Add(new Keyword("map_has_vehicle", "",  "Type of vehicle 0 = none, 1 = tank, 2 = train", false));
			_keywords.Add(new Keyword("vehicle_entity_number", "",  "The entity number of the vehicle.", false));
			_keywords.Add(new Keyword("vehicle_team_owner", "",  "The owner of the vehicle.", false));

			LineSegment line = textArea.Document.GetLineSegmentForOffset(textArea.Caret.Offset);

			if (line != null) 
			{
				TextWord first = GetWord(line, 1);

				if (first != null) 
				{
					Keyword keyword = _keywords.GetWord(first.Word);

					if (keyword != null) 
					{
						if (keyword.Inputs.Count > 0) 
						{
							for (int x = 0; x < keyword.Inputs.Count; x++) 
							{
								TextWord param = GetWord(line, x + 2);

								if (param == null) 
								{
									KeywordInput inputParam = (KeywordInput)keyword.Inputs[x];

									if (inputParam.InputType != KeywordInputType.PredefinedList) 
									{
										_insightKeywords.Add(new Keyword(inputParam.Label, inputParam.Label, inputParam.HelpText, false));
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		private TextWord GetWord(LineSegment segment, int ordinal) 
		{
			if (segment.Words == null) return null;

			int count = 0;
			foreach (TextWord word in segment.Words) 
			{
				if (!word.IsWhiteSpace) 
				{
					if (word.Word.StartsWith("//")) 
					{
						return null;
					}

					count++;

					if (count == ordinal) 
					{
						return word;
					}
				}
			}
			return null;
		}

		public string GetInsightData(int number)
		{
			if (_insightKeywords.Count == 0) return "";
			return _insightKeywords[number].Command + "\n" + _insightKeywords[number].HelpText;
		}

		public bool CharTyped()
		{
			return false;
		}

		public bool CaretOffsetChanged()
		{
			int brackets = 0;
			int curlyBrackets = 0;
			int initialOffset = 0;
			bool closeDataProvider = _textArea.Caret.Offset <= initialOffset;

			if (!closeDataProvider) 
			{
				bool insideChar   = false;
				bool insideString = false;
				for (int offset = initialOffset; offset < Math.Min(_textArea.Caret.Offset, _document.TextLength); ++offset) 
				{
					char ch = _document.GetCharAt(offset);
					switch (ch) 
					{
						case '\'':
							insideChar = !insideChar;
							break;
						case '(':
							if (!(insideChar || insideString)) 
							{
								++brackets;
							}
							break;
						case ')':
							if (!(insideChar || insideString)) 
							{
								--brackets;
							}
							if (brackets <= 0) 
							{
								return true;
							}
							break;
						case '"':
							insideString = !insideString;
							break;
						case '}':
							if (!(insideChar || insideString)) 
							{
								--curlyBrackets;
							}
							if (curlyBrackets < 0) 
							{
								return true;
							}
							break;
						case '{':
							if (!(insideChar || insideString)) 
							{
								++curlyBrackets;
							}
							break;
						case ';':
							if (!(insideChar || insideString)) 
							{
								return true;
							}
							break;
					}
				}
			}
			
			return closeDataProvider;
		}

		public int InsightDataCount
		{
			get
			{
				return _insightKeywords.Count;
			}
		}
	}
}
