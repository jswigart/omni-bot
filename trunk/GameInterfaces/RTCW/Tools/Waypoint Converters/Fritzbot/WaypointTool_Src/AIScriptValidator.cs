using System;
using System.Collections;

using ICSharpCode.TextEditor;
using ICSharpCode.TextEditor.Document;

namespace WaypointTool
{
	public class AIScriptValidator
	{
		private Hashtable GetHeaderKeywords() 
		{
			Hashtable hash = new Hashtable();

			hash.Add("bot_sightdist", "bot_SightDist");
			hash.Add("spawnflag_is_priority", "spawnflag_is_priority");
			hash.Add("cmdpost_is_priority", "cmdpost_is_priority");
			hash.Add("construct_is_priority", "construct_is_priority");
			hash.Add("map_has_vehicle", "map_has_vehicle");
			hash.Add("vehicle_entity_number", "vehicle_entity_number");
			hash.Add("vehicle_team_owner", "vehicle_team_owner");

			return hash;
		}

		private Hashtable GetKeywords() 
		{
			Hashtable hash = new Hashtable();

			KeywordCollection keywords = KeywordFactory.GetKeywords();

			foreach (Keyword keyword in keywords) 
			{
				hash.Add(keyword.Command.ToLower(), keyword);
			}

			return hash;
		}

		private TextWord GetWord(LineSegment segment, int ordinal) 
		{
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

		private int GetWordInt(TextWord word) 
		{
			try 
			{
				return Convert.ToInt32(word.Word);
			} 
			catch 
			{
				return Int32.MinValue;
			}
		}

		public ArrayList Validate(TextEditorControl editor, NavigationReader reader) 
		{
			bool isValid = true;
			bool inActionBracket = false;
			bool actionsDefined = false;
			IDocument document = editor.Document;
			Hashtable required = GetHeaderKeywords();
			Hashtable keywords = GetKeywords();

			ArrayList messages = new ArrayList();

			if (reader == null) 
			{
				AIScriptValidationMessage msg = new AIScriptValidationMessage();
				msg.IsError = false;
				msg.Message = "Script validation will be more complete if you open a navigation file.";
				messages.Add(msg);
			}

			for (int x = 0; x < document.LineSegmentCollection.Count; x++) 
			{
				LineSegment segment = (LineSegment)document.LineSegmentCollection[x];

				TextWord word = GetWord(segment, 1);

				if (word != null) 
				{
					if (required.ContainsKey(word.Word.ToLower()))
					{
						if (actionsDefined) 
						{
							isValid = false;
	
							AIScriptValidationMessage msg = new AIScriptValidationMessage();
							msg.IsError = true;
							msg.LineNumber = x + 1;
							msg.Message = "Header parameter " + word.Word + " must be defined before actions.";
							msg.ErrorWord = word;
							messages.Add(msg);
						}

						required.Remove(word.Word.ToLower());
						TextWord val = GetWord(segment, 2);
						TextWord bogusRequiredParam = GetWord(segment, 3);

						if (val == null) 
						{
							isValid = false;
	
							AIScriptValidationMessage msg = new AIScriptValidationMessage();
							msg.IsError = true;
							msg.LineNumber = x + 1;
							msg.Message = word.Word + " does not have a value defined.";
							msg.ErrorWord = word;
							messages.Add(msg);
						} 
						else 
						{
							if (bogusRequiredParam != null) 
							{
								isValid = false;
	
								AIScriptValidationMessage msg = new AIScriptValidationMessage();
								msg.IsError = true;
								msg.LineNumber = x + 1;
								msg.Message = word.Word + " has too many parameters defined.";
								msg.ErrorWord = word;
								messages.Add(msg);
							}

							int param = GetWordInt(val);

							if (param == Int32.MinValue) 
							{
								isValid = false;
	
								AIScriptValidationMessage msg = new AIScriptValidationMessage();
								msg.IsError = true;
								msg.LineNumber = x + 1;
								msg.Message = word.Word + " has a non-numeric value defined.";
								msg.ErrorWord = word;
								messages.Add(msg);
							} 
							else 
							{
								switch (word.Word.ToLower()) 
								{
									case "vehicle_entity_number":
									case "bot_sightdist":
										if (param <= 0) 
										{
											isValid = false;
	
											AIScriptValidationMessage msg = new AIScriptValidationMessage();
											msg.IsError = true;
											msg.Message = word.Word + " should be greater than zero.";
											msg.LineNumber = x + 1;
											msg.ErrorWord = word;
											messages.Add(msg);
										}
										break;
									case "spawnflag_is_priority":
									case "cmdpost_is_priority":
									case "construct_is_priority":
										if (param != 0 && param != 1) 
										{
											isValid = false;
	
											AIScriptValidationMessage msg = new AIScriptValidationMessage();
											msg.IsError = true;
											msg.Message = word.Word + " should be 0 = false, 1 = true";
											msg.LineNumber = x + 1;
											msg.ErrorWord = word;
											messages.Add(msg);
										}
										break;
									case "map_has_vehicle":
										if (param != 0 && param != 1 && param != 2) 
										{
											isValid = false;
	
											AIScriptValidationMessage msg = new AIScriptValidationMessage();
											msg.IsError = true;
											msg.Message = word.Word + " should be 0 = none, 1 = tank, 2 = train";
											msg.LineNumber = x + 1;
											msg.ErrorWord = word;
											messages.Add(msg);
										} 
										else if (param == 0) 
										{
											// not required if no vehicle
											required.Remove("vehicle_entity_number");
											required.Remove("vehicle_team_owner");
										}
										break;
									case "vehicle_team_owner":
										if (param != 1 && param != 2) 
										{
											isValid = false;
	
											AIScriptValidationMessage msg = new AIScriptValidationMessage();
											msg.IsError = true;
											msg.Message = word.Word + " 1 = axis, 2 = allies";
											msg.LineNumber = x + 1;
											msg.ErrorWord = word;
											messages.Add(msg);
										} 
										break;
								}
							}
						}
					} 
					else if (keywords.ContainsKey(word.Word.ToLower())) 
					{
						if (!inActionBracket) 
						{
							isValid = false;

							AIScriptValidationMessage msg = new AIScriptValidationMessage();
							msg.IsError = true;
							msg.Message = word.Word + " can only be defined inside an action.";
							msg.LineNumber = x + 1;
							msg.ErrorWord = word;
							messages.Add(msg);
						} 
						else 
						{
							Keyword keyword = (Keyword)keywords[word.Word.ToLower()];

							int inputParamWord = 2; // second word is first input param

							foreach (KeywordInput input in keyword.Inputs) 
							{
								TextWord inputParam = GetWord(segment, inputParamWord);

								if (inputParam == null) 
								{
									isValid = false;

									AIScriptValidationMessage msg = new AIScriptValidationMessage();
									msg.IsError = true;
									msg.Message = word.Word + " expects an " + input.Label + " parameter. See script help for more information.";
									msg.LineNumber = x + 1;
									msg.ErrorWord = word;
									messages.Add(msg);
								} 
								else 
								{
									// validate based on input type :)
									switch (input.InputType) 
									{
										case KeywordInputType.Integer:
											int inputInteger = GetWordInt(inputParam);

											if (inputInteger < 0) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = word.Word + " has an invalid " + input.Label + " value. See script help for more information.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											} 
											break;
										case KeywordInputType.Action:
											int inputActionID = GetWordInt(inputParam);

											if (inputActionID < 0) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = word.Word + " has an invalid " + input.Label + " value. See script help for more information.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											} 
											else 
											{
												if (reader != null) 
												{
													if (inputActionID > reader.Actions.Count - 1) 
													{
														isValid = false;

														AIScriptValidationMessage msg = new AIScriptValidationMessage();
														msg.IsError = true;
														msg.Message = word.Word + " has an " + input.Label + " value that is not in the opened navigation file.";
														msg.LineNumber = x + 1;
														msg.ErrorWord = word;
														messages.Add(msg);
													} 
													else 
													{
														if (input.AllowableActions.Count > 0) 
														{
															Action action = (Action)reader.Actions[inputActionID];

															bool actionIsValid = false;

															foreach (int validActionType in input.AllowableActions) 
															{
																if (validActionType == action.AllyAction ||
																	validActionType == action.AxisAction) 
																{
																	actionIsValid = true;
																	break;
																}
															}

															if (!actionIsValid) 
															{
																isValid = false;

																AIScriptValidationMessage msg = new AIScriptValidationMessage();
																msg.IsError = true;
																msg.Message = word.Word + " has an " + input.Label + " value that isn't valid. Verify the correct Allies and/or Axis action.";
																msg.LineNumber = x + 1;
																msg.ErrorWord = word;
																messages.Add(msg);
															}
														}
													}
												}
											}
											break;
										case KeywordInputType.Boolean:
											if (inputParam.Word != "true" && inputParam.Word != "false") 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = word.Word + " " + input.Label + " should be either 'true' or 'false'.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											}
											break;
										case KeywordInputType.Flag:
											int inputFlag = GetWordInt(inputParam);

											if (inputFlag < 0) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = word.Word + " has an invalid " + input.Label + " value. Flags must be numeric and greater than or equal to zero.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											}
											break;
										case KeywordInputType.Goal:
											int inputGoal = GetWordInt(inputParam);

											if (inputGoal < 0) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = word.Word + " has an invalid " + input.Label + " value. Goals must be numeric and greater than or equal to zero.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											}
											break;
										case KeywordInputType.Group:
											int inputGroup = GetWordInt(inputParam);

											if (inputGroup < 0) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = word.Word + " has an invalid " + input.Label + " value. Groups must be numeric and greater than or equal to zero.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											}
											break;
										case KeywordInputType.Node:
											int inputNode = GetWordInt(inputParam);

											if (inputNode < 0) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = word.Word + " has an invalid " + input.Label + " value. Nodes must be numeric and greater than or equal to zero.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											} 
											else 
											{
												if (reader != null) 
												{
													if (inputNode > reader.Nodes.Count - 1) 
													{
														isValid = false;

														AIScriptValidationMessage msg = new AIScriptValidationMessage();
														msg.IsError = true;
														msg.Message = word.Word + " has an invalid " + input.Label + " value. The node defined does not exist in opened navigation file.";
														msg.LineNumber = x + 1;
														msg.ErrorWord = word;
														messages.Add(msg);
													} 
												}
											}
											break;
										case KeywordInputType.PredefinedList:
											bool isAllowedValue = false;

											foreach (string allowed in input.AllowableValues) 
											{
												if (allowed.ToLower() == inputParam.Word.ToLower()) 
												{
													isAllowedValue = true;
													break;
												}
											}

											if (!isAllowedValue) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = inputParam.Word + " is not an allowed parameter for " + word.Word + ". See script help for more information.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											}
											break;
									}
								}

								inputParamWord++;
							}

							TextWord bogusKeywordParam = GetWord(segment, inputParamWord);

							if (bogusKeywordParam != null) 
							{
								if (bogusKeywordParam.Word.ToLower() != "and" &&
									bogusKeywordParam.Word.ToLower() != "or") 
								{
									isValid = false;

									AIScriptValidationMessage msg = new AIScriptValidationMessage();
									msg.IsError = true;
									msg.Message = word.Word + " has too many parameters defined. See script help for more information.";
									msg.LineNumber = x + 1;
									msg.ErrorWord = word;
									messages.Add(msg);
								} 
								else 
								{
									if (!keyword.AllowConditional) 
									{
										isValid = false;

										AIScriptValidationMessage msg = new AIScriptValidationMessage();
										msg.IsError = true;
										msg.Message = keyword.Command + " does not allow conditional statements.";
										msg.LineNumber = x + 1;
										msg.ErrorWord = word;
										messages.Add(msg);
									} 
									else 
									{
										inputParamWord++;

										TextWord conditionalWord = GetWord(segment, inputParamWord);

										if (conditionalWord != null) 
										{
											Keyword conditionalKeyword = keywords[conditionalWord.Word.ToLower()]
												as Keyword;

											if (conditionalKeyword == null) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = keyword.Command + " conditional statement requires additional command.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											} 
											else 
											{
												if (!conditionalKeyword.AllowConditional) 
												{
													isValid = false;

													AIScriptValidationMessage msg = new AIScriptValidationMessage();
													msg.IsError = true;
													msg.Message = conditionalKeyword.Command + " is not a valid conditional statement.";
													msg.LineNumber = x + 1;
													msg.ErrorWord = word;
													messages.Add(msg);
												} 
												else 
												{
													// TODO: validate the params
												}
											}
										} 
										else 
										{
											isValid = false;

											AIScriptValidationMessage msg = new AIScriptValidationMessage();
											msg.IsError = true;
											msg.Message = keyword.Command + " conditional statement requires additional command.";
											msg.LineNumber = x + 1;
											msg.ErrorWord = word;
											messages.Add(msg);
										}
									}
								}
							}
						}
					}
                    // crapshoot: add support for the default action test
                    else if (word.Word.ToLower() == "default")
                    {
                        actionsDefined = true;
                        if (inActionBracket)
                        {
                            isValid = false;

                            AIScriptValidationMessage msg = new AIScriptValidationMessage();
                            msg.IsError = true;
                            msg.Message = "Actions cannot be defined inside of other actions.";
                            msg.LineNumber = x + 1;
                            msg.ErrorWord = word;
                            messages.Add(msg);
                        }
                        else
                        {
                            // validate int for second word
                            TextWord actionParam = GetWord(segment, 2);
                            TextWord bogusActionParam = GetWord(segment, 3);

                            if (bogusActionParam != null)
                            {
                                isValid = false;

                                AIScriptValidationMessage msg = new AIScriptValidationMessage();
                                msg.IsError = true;
                                msg.Message = "Action has too many parameters defined.";
                                msg.LineNumber = x + 1;
                                msg.ErrorWord = word;
                                messages.Add(msg);
                            }

                            if (actionParam != null)
                            {
                                isValid = false;

                                AIScriptValidationMessage msg = new AIScriptValidationMessage();
                                msg.IsError = true;
                                msg.Message = "Too Many Parameters Defined For Default Action Test";
                                msg.LineNumber = x + 1;
                                msg.ErrorWord = word;
                                messages.Add(msg);
                            }
                        }
                    }
					else if (word.Word.ToLower() == "action") 
					{
						actionsDefined = true;
						if (inActionBracket) 
						{
							isValid = false;

							AIScriptValidationMessage msg = new AIScriptValidationMessage();
							msg.IsError = true;
							msg.Message = "Actions cannot be defined inside of other actions.";
							msg.LineNumber = x + 1;
							msg.ErrorWord = word;
							messages.Add(msg);
						} 
						else 
						{
							// validate int for second word
							TextWord actionParam = GetWord(segment, 2);
							TextWord bogusActionParam = GetWord(segment, 3);

							if (bogusActionParam != null) 
							{
								isValid = false;

								AIScriptValidationMessage msg = new AIScriptValidationMessage();
								msg.IsError = true;
								msg.Message = "Action has too many parameters defined.";
								msg.LineNumber = x + 1;
								msg.ErrorWord = word;
								messages.Add(msg);
							}

							if (actionParam == null) 
							{
								isValid = false;

								AIScriptValidationMessage msg = new AIScriptValidationMessage();
								msg.IsError = true;
								msg.Message = "Action does not have an ID";
								msg.LineNumber = x + 1;
								msg.ErrorWord = word;
								messages.Add(msg);
							} 
							else 
							{
								int actionNumber = GetWordInt(actionParam);
								
								if (actionNumber < 0) 
								{
									isValid = false;

									AIScriptValidationMessage msg = new AIScriptValidationMessage();
									msg.IsError = true;
									msg.Message = "Action has an invalid value.";
									msg.LineNumber = x + 1;
									msg.ErrorWord = word;
									messages.Add(msg);
								} 
								else 
								{
									// if reader avail, validate action exists and that it
									// is a scriptable action
									if (reader != null) 
									{
										if (actionNumber > reader.Actions.Count - 1) 
										{
											isValid = false;

											AIScriptValidationMessage msg = new AIScriptValidationMessage();
											msg.IsError = true;
											msg.Message = "Action ID " + actionNumber + " does not exist in opened navigation file.";
											msg.LineNumber = x + 1;
											msg.ErrorWord = word;
											messages.Add(msg);
										} 
										else 
										{
											Action action = (Action)reader.Actions[actionNumber];

                                            //crapshoot: added event_explode and event_expire
											if (action.AllyAction != 4 && action.AxisAction != 4
												&& action.AllyAction != 8 && action.AxisAction != 8
												&& action.AllyAction != 19 && action.AxisAction != 19
												&& action.AllyAction != 2 && action.AxisAction != 2
												&& action.AllyAction != 22 && action.AxisAction != 22
												&& action.AllyAction != 6 && action.AxisAction != 6
                                                && action.AllyAction != 33 && action.AxisAction != 33
                                                && action.AllyAction != 34 && action.AxisAction != 34) 
											{
												isValid = false;

												AIScriptValidationMessage msg = new AIScriptValidationMessage();
												msg.IsError = true;
												msg.Message = "Action ID " + actionNumber + " is not a scriptable action.";
												msg.LineNumber = x + 1;
												msg.ErrorWord = word;
												messages.Add(msg);
											}
										}
									}
								}
							}
						}
					} 
					else if (word.Word == "{") 
					{
						inActionBracket = true;
					} 
					else if (word.Word == "}") 
					{
						inActionBracket = false;
					} 
					else 
					{
						if (!word.Word.StartsWith("#") && !word.Word.StartsWith("//")) 
						{
							isValid = false;

							AIScriptValidationMessage msg = new AIScriptValidationMessage();
							msg.IsError = true;
							msg.Message = "Unknown keyword defined [" + word.Word + "]";
							msg.LineNumber = x + 1;
							msg.ErrorWord = word;
							messages.Add(msg);
						}
					}
				}
			}

			foreach (string key in required.Keys) 
			{
				isValid = false;

				AIScriptValidationMessage msg = new AIScriptValidationMessage();
				msg.IsError = true;
				msg.Message = key + " is not defined.";
				messages.Add(msg);
			}

			if (isValid) 
			{
				AIScriptValidationMessage msg = new AIScriptValidationMessage();
				msg.IsError = false;
				msg.Message = "Script is valid.";
				messages.Add(msg);
			}

			return messages;
		}
	}
}
