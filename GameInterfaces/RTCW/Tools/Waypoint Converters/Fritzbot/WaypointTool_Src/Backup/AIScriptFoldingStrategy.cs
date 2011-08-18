using System;
using System.Collections;
using ICSharpCode.TextEditor.Document;

namespace WaypointTool
{
	public class AIScriptFoldingStrategy : IFoldingStrategy
	{
		public ArrayList GenerateFoldMarkers(IDocument document, string fileName, object parseInformation)
		{
			ArrayList foldMarkers = new ArrayList();

			int startLine = Int32.MinValue;
			string foldWord = "";

			for (int x = 0; x < document.LineSegmentCollection.Count; x++) 
			{
				LineSegment segment = (LineSegment)document.LineSegmentCollection[x];

				if (segment.Words.Count > 0) 
				{
					TextWord word = (TextWord)segment.Words[0];

					if (startLine == Int32.MinValue) 
					{
						if (word.Word.ToLower() == "action" && segment.Words.Count > 2) 
						{
							startLine = x;

							for (int y = 2; y < segment.Words.Count; y++) 
							{
								TextWord comments = (TextWord)segment.Words[y];

								if (comments.Word != "//") 
								{
									foldWord += " " + comments.Word;
								}
							}
						}
					} 
					else 
					{
						if (word.Word == "}") 
						{
							FoldMarker actionMarker = new FoldMarker(document, startLine, 1, x, 1, FoldType.Region, foldWord, false);
							foldMarkers.Add(actionMarker);
							startLine = Int32.MinValue;
							foldWord = "";
						}
					}
				}
			}
			return foldMarkers;
		}
	}
}
