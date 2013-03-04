using System;
using System.Data;
using System.Drawing;
using System.Drawing.Printing;

namespace WaypointTool
{
	public class ActionPrinter
	{
		private PrintDocument _printDocument;
		private Actions _actions;
		private string _fileName;

		//pageSettings
		private int _pageWidth;
		private int _pageHeight;
		private bool _pageLandscape;
		
		//pageMargins
		private int _pageMarginRight;
		private int _pageMarginLeft;
		private int _pageMarginTop;
		private int _pageMarginBottom;

		private float _currentY = 0;
		private float _currentX = 0;
		private int _currentRow = 0;

		private int _pageCounter = 0;

        //TODO: Font selection menu..for now, just hardcode bindles request -crapshoot
		//private Font _headerFont = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
		//private Font _font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
        private Font _headerFont = new System.Drawing.Font("Ariel", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
        private Font _font = new System.Drawing.Font("Ariel", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));

		private ActionPrinter() {}

		public ActionPrinter(PrintDocument document, Actions actions, string fileName) 
		{
			_fileName = fileName;
			_printDocument = document;
			_actions = actions;

			//init page settings
			_pageWidth = document.DefaultPageSettings.PaperSize.Width;
			_pageHeight = document.DefaultPageSettings.PaperSize.Height;
			_pageLandscape = document.DefaultPageSettings.Landscape;

			//init margin settings
			_pageMarginLeft = document.DefaultPageSettings.Margins.Left;
			_pageMarginTop = document.DefaultPageSettings.Margins.Top;
			_pageMarginRight = document.DefaultPageSettings.Margins.Right;
			_pageMarginBottom = document.DefaultPageSettings.Margins.Bottom;

			_currentY = _pageMarginTop;
			_currentX =  _pageMarginLeft;
		}

		public bool Print(Graphics g) 
		{
			PrintHeaders(g);
			return PrintActions(g);
		}

		private void PrintHeaders(Graphics g) 
		{
			_currentX = _pageMarginLeft;
			int height = 16; //was 20 -crapshoot
            //add date to header -crapshoot
            string dateTime = Convert.ToString(System.DateTime.Now);

			int titleWidth = 0;
			for (int i = 0; i < _actions.Action.Columns.Count; i++)
			{
				DataColumn column = _actions.Action.Columns[i];
				titleWidth += GetColumnWidth(column);
			}

			g.FillRectangle(new SolidBrush(Color.White), _currentX, _currentY, titleWidth, height);
			g.DrawRectangle(new Pen(Color.Black), _currentX, _currentY, titleWidth, height);
			g.DrawString(_fileName + " Actions" + " " + dateTime, _headerFont, new SolidBrush(Color.Black), new RectangleF(_currentX, _currentY, titleWidth, height));
			_currentY = _currentY + height;

			for (int i = 0; i < _actions.Action.Columns.Count; i++)
			{
				DataColumn column = _actions.Action.Columns[i];

				int width = GetColumnWidth(column);

				g.FillRectangle(new SolidBrush(Color.White), _currentX, _currentY, width, height);
				g.DrawRectangle(new Pen(Color.Black), _currentX, _currentY, width, height);
				g.DrawString(column.ColumnName, _headerFont, new SolidBrush(Color.Black), new RectangleF(_currentX, _currentY, width, height));
				
				//next cell
				_currentX += width;
			}

			//left align
			_currentX = _pageMarginLeft;
			_currentY = _currentY + height;
		}

		private bool PrintActions(Graphics g) 
		{
			_pageCounter++;

			_currentX = _pageMarginLeft;
			int height = 16; //was 20 -crapshoot

			for (int i = _currentRow; i < _actions.Action.Rows.Count; i++)
			{
				for (int j = 0; j < _actions.Action.Columns.Count; j++)
				{
					DataColumn column = _actions.Action.Columns[j];
					int width = GetColumnWidth(column);

					g.FillRectangle(new SolidBrush(Color.White), _currentX, _currentY, width, height);
					g.DrawRectangle(new Pen(Color.Black), _currentX, _currentY, width, height);

					Actions.ActionRow row = (Actions.ActionRow)_actions.Action.Rows[_currentRow];
					object obj = row[column];
					string val = obj == null ? "" : obj.ToString();
					
					g.DrawString(GetValueString(column, val), _font, new SolidBrush(Color.Black), new RectangleF(_currentX, _currentY, width, height));
						
					//next cell
					_currentX += width;
				}

				_currentX = _pageMarginLeft;
				
				//advance to next row
				_currentY = _currentY + height;
				_currentRow++;

				//if portrait is selected
				if((_currentY > _pageHeight - _pageMarginBottom) && (!_pageLandscape))
				{
					_currentY = _pageMarginTop;
					_currentX =  _pageMarginLeft;
					return true;
				}

				//if landscape is selected
				if((_currentY > _pageWidth - _pageMarginRight) && (_pageLandscape))
				{
					_currentY = _pageMarginTop;
					_currentX =  _pageMarginLeft;
					return true;
				}
			}
			
			_currentRow = 0;
			_currentY = _pageMarginTop;
			_currentX =  _pageMarginLeft;

			return false;
		}

		private int GetColumnWidth(DataColumn column) 
		{
			string columnName = column.ColumnName.ToLower();

			switch (columnName) 
			{
				case "id":
				case "radius":
				case "goal":
				case "group":
				case "prone":
				case "active":
				case "entity":
					return 50;
				case "allyaction":
				case "axisaction":
					return 120;
				case "class":
					return 100;
				case "links":
					return 100;
				case "closenode":
					return 85;
				
			}

			return 100;
		}

		private string GetValueString(DataColumn column, string val) 
		{
			if (val == "") return "";

			string columnName = column.ColumnName.ToLower();

			switch (columnName) 
			{
				case "axisaction":
				case "allyaction":
					return GetActionName(val);
				case "class":
					return GetClassNames(val);
                case "prone":
                    return GetProneState(val);
                case "active":
                    return GetActiveState(val);
				default:
					return val;
			}
		}

        //print friendly name for this too..-crapshoot
        private string GetProneState(string val)
        {
            switch (val)
            {
                case "0": return " ";
                case "1": return "Prone";
            }

            return val;
        }

        //print friendly name for this too..-crapshoot
        private string GetActiveState(string val)
        {
            switch (val)
            {
                case "0": return " ";
                case "1": return "Active";
            }

            return val;
        }

		private string GetActionName(string val) 
		{
			switch (val) 
			{
				case "3": return "Aim at";
				case "10": return "Airstrike Cannister";
				case "11": return "Artillery Hint";
				case "29": return "Button";
				case "0": return "Camp";
				case "16": return "Camp Engineer";
				case "18": return "Camp MG42";
				case "28": return "Camp Mobile MG42";
				case "22": return "Command Post";
				case "19": return "Construct Major";
				case "2": return "Construct Minor";
				case "9": return "Defend";
				case "5": return "Defuse";
				case "7": return "Deliver";
				case "4": return "Dynamite";
				case "15": return "Grenade Hint";
				case "1": return "MG42";
				case "25": return "Mine Hunt";
				case "24": return "Mine Plant";
				case "-1": return "None";
				case "14": return "Panzer";
				case "12": return "Roam";
				case "17": return "Roam Alternate";
				case "27": return "Satchel";
				case "23": return "Smoke Cannister Hint";
				case "13": return "Snipe";
				case "6": return "Spawn Flag";
				case "8": return "Steal";
				case "26": return "Subversion";
				case "20": return "Tank";
				case "21": return "Train";
				case "32": return "Construct Major Plant";
				case "33": return "Expire";
				case "34": return "Explode";
			}

			return val;
		}

		private string GetClassNames(string val) 
		{
			switch (val) 
			{
				case "0": return "Everyone";
				case "16": return "C";
				case "17": return "C,S";
				case "18": return "C,M";
				case "19": return "C,M,S";
				case "20": return "C,E";
				case "21": return "C,E,S";
				case "22": return "C,E,M";
				case "23": return "C,E,M,S";
				case "24": return "C,F";
				case "25": return "C,F,S";
				case "26": return "C,F,M";
				case "27": return "C,F,M,S";
				case "28": return "C,F,E";
				case "29": return "C,F,E,S";
				case "30": return "C,F,E,M";
				case "4": return "E";
				case "8": return "F";
				case "12": return "F,E";
				case "10": return "F,M";
				case "9": return "F,S";
				case "13": return "F,E,S";
				case "11": return "F,M,S";
				case "14": return "F,M,E";
				case "15": return "F,M,S,E";
				case "2": return "M";
				case "6": return "M,E";
				case "7": return "M,E,S";
				case "1": return "S";
				case "3": return "S,M";
				case "5": return "S,E";
			}

			return val;
		}
	}
}
