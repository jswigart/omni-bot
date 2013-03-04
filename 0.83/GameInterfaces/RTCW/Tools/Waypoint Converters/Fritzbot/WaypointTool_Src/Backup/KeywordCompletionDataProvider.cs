using System;
using System.Collections;

using ICSharpCode.TextEditor;
using ICSharpCode.TextEditor.Document;
using ICSharpCode.TextEditor.Gui.CompletionWindow;

namespace WaypointTool
{
	public class KeywordCompletionDataProvider : ICompletionDataProvider
	{
		private KeywordCollection _keywords;
		private char _typedChar;
		private TextArea _textArea  = null;
		private System.Windows.Forms.ImageList _imageList;
		
		public ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped)
		{
			_keywords = KeywordFactory.GetKeywords();

			// the keyword factory just has the non-required words
			_keywords.Add(new Keyword("bot_SightDist", "", "How far the highest skill bot can see on this map", false));
			
			ArrayList allowable = new ArrayList();
			allowable.Add("0");
			allowable.Add("1");

			Keyword flag = new Keyword("spawnflag_is_priority", "",  "Whether or no bots focus on spawnflags.", false);
			KeywordInput flagInput = new KeywordInput(KeywordInputType.PredefinedList, "spawnflag_is_priority", "Whether or no bots focus on spawnflags.");
			flagInput.AllowableValues = allowable;
			flag.Inputs.Add(flagInput);
			_keywords.Add(flag);

			Keyword cmdpost = new Keyword("cmdpost_is_priority", "",  "Whether or not command posts critical on this map.", false);
			KeywordInput cmdpostInput = new KeywordInput(KeywordInputType.PredefinedList, "cmdpost_is_priority", "Whether or not command posts critical on this map.");
			cmdpostInput.AllowableValues = allowable;
			cmdpost.Inputs.Add(cmdpostInput);
			_keywords.Add(cmdpost);

			Keyword construct = new Keyword("construct_is_priority", "",  "Whether or not engineers focus more on constructibles.", false);
			KeywordInput constructInput = new KeywordInput(KeywordInputType.PredefinedList, "construct_is_priority", "Whether or not engineers focus more on constructibles.");
			constructInput.AllowableValues = allowable;
			construct.Inputs.Add(constructInput);
			_keywords.Add(construct);

			Keyword vehicle = new Keyword("map_has_vehicle", "",  "Whether or not this map has a tank or a train.", false);
			KeywordInput vehicleInput = new KeywordInput(KeywordInputType.PredefinedList, "map_has_vehicle", "Type of vehicle 0 = none, 1 = tank, 2 = train");

			ArrayList allowableVehicle = new ArrayList();
			allowable.Add("0");
			allowable.Add("1");
			allowable.Add("2");
			vehicleInput.AllowableValues = allowableVehicle;
			vehicle.Inputs.Add(vehicleInput);
			_keywords.Add(vehicle);

			//vehicle_entity_number 429 //the entity number of the tank
			Keyword vehicleEntity = new Keyword("vehicle_entity_number", "",  "The entity number of the vehicle.", false);
			vehicleEntity.Inputs.Add(new KeywordInput(KeywordInputType.Action, "Vehicle Entity Number", "The entity number of the vehicle."));
			_keywords.Add(vehicleEntity);

			//vehicle_team_owner 2 //ALLIES own this tank! 1 = AXIS, 2 = ALLIES
			Keyword vehicleOwner = new Keyword("vehicle_team_owner", "",  "The owner of the vehicle.", false);
			KeywordInput vehicleOwnerInput = new KeywordInput(KeywordInputType.PredefinedList, "vehicle_team_owner", "1 = AXIS, 2 = ALLIES.");
			vehicleOwnerInput.AllowableValues.Add("1");;
			vehicleOwnerInput.AllowableValues.Add("2");;
			vehicleOwner.Inputs.Add(vehicleOwnerInput);
			_keywords.Add(vehicleOwner);

			_keywords.Sort(KeywordComparer.SortBy.Command);

			_textArea = textArea;
			_typedChar = charTyped;

			// see if a word on the line the input was in is a keyword that we have,
			// then figure out what input they are at
			LineSegment line = textArea.Document.GetLineSegmentForOffset(textArea.Caret.Offset);

			bool keywordFound = false;
			if (line != null) 
			{
				TextWord first = GetWord(line, 1);

				if (first != null) 
				{
					Keyword keyword = _keywords.GetWord(first.Word);

					if (keyword != null) 
					{
						keywordFound = true;
						if (keyword.Inputs.Count > 0) 
						{
							for (int x = 0; x < keyword.Inputs.Count; x++) 
							{
								TextWord param = GetWord(line, x + 2);

								if (param == null) 
								{
									KeywordInput inputParam = (KeywordInput)keyword.Inputs[x];

									if (inputParam.InputType == KeywordInputType.PredefinedList) 
									{
										ICompletionData[] data = new ICompletionData[inputParam.AllowableValues.Count];

										for (int y = 0; y < inputParam.AllowableValues.Count; y++) 
										{
											Keyword dummyKeyword = new Keyword((string)inputParam.AllowableValues[y], (string)inputParam.AllowableValues[y], inputParam.HelpText, false);
											data[y] = new KeywordCompletionData(dummyKeyword);
										}

										return data;
									}
								}
							}
						}
					}
				}
			}

			if (keywordFound) 
			{
				return null;
			}

			// fall through, return all keywords
			ICompletionData[] all = new ICompletionData[_keywords.Count];

			for (int x = 0; x < _keywords.Count; x++) 
			{
				all[x] = new KeywordCompletionData(_keywords[x]);
			}

			return all;
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

		public System.Windows.Forms.ImageList ImageList
		{
			get
			{
				return _imageList;
			}
			set 
			{
				_imageList = value;
			}
		}

		public string PreSelection
		{
			get
			{
				// we don't care about this
				return null;
			}
		}
	}
}
