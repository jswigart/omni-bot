using System;
using System.Data;
using System.Drawing;
using System.Drawing.Printing;

namespace WaypointTool
{
	public class NodePrinter
	{
		private PrintDocument _printDocument;
		private Nodes _nodes;
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

		private Font _headerFont = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
		private Font _font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));

		private NodePrinter() {}

		public NodePrinter(PrintDocument document, Nodes nodes, string fileName) 
		{
			_fileName = fileName;
			_printDocument = document;
			_nodes = nodes;

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
			return PrintNodes(g);
		}

		private void PrintHeaders(Graphics g) 
		{
			_currentX = _pageMarginLeft;
			int height = 20;

			int titleWidth = 0;
			for (int i = 0; i < _nodes.Node.Columns.Count; i++)
			{
				DataColumn column = _nodes.Node.Columns[i];
				titleWidth += GetColumnWidth(column);
			}

			g.FillRectangle(new SolidBrush(Color.White), _currentX, _currentY, titleWidth, height);
			g.DrawRectangle(new Pen(Color.Black), _currentX, _currentY, titleWidth, height);
			g.DrawString(_fileName + " Nodes", _headerFont, new SolidBrush(Color.Black), new RectangleF(_currentX, _currentY, titleWidth, height));
			_currentY = _currentY + height;

			for (int i = 0; i < _nodes.Node.Columns.Count; i++)
			{
				DataColumn column = _nodes.Node.Columns[i];

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

		private bool PrintNodes(Graphics g) 
		{
			_pageCounter++;

			_currentX = _pageMarginLeft;
			int height = 20;

			for (int i = _currentRow; i < _nodes.Node.Rows.Count; i++)
			{
				for (int j = 0; j < _nodes.Node.Columns.Count; j++)
				{
					DataColumn column = _nodes.Node.Columns[j];
					int width = GetColumnWidth(column);

					g.FillRectangle(new SolidBrush(Color.White), _currentX, _currentY, width, height);
					g.DrawRectangle(new Pen(Color.Black), _currentX, _currentY, width, height);

					Nodes.NodeRow row = (Nodes.NodeRow)_nodes.Node.Rows[_currentRow];
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
				case "entity":
				case "team":
				case "group":
					return 50;
				case "flag":
					return 100;
				case "connection1":
				case "connection2":
				case "connection3":
				case "connection4":
					return 95;
				
			}

			return 100;
		}

		private string GetValueString(DataColumn column, string val) 
		{
			if (val == "") return "";

			string columnName = column.ColumnName.ToLower();

			switch (columnName) 
			{
				case "flag":
					return GetFlagName(val);
				case "team":
					return GetTeamName(val);
				default:
					return val;
			}
		}

		private string GetFlagName(string val) 
		{
			switch (val) 
			{
				case "4": return "Button";
				case "7": return "Constructable";
				case "3": return "Flag";
				case "-1": return "Invalid";
				case "5": return "Jump";
				case "2": return "Ladder Bottom";
				case "1": return "Ladder Top";
				case "6": return "Leap";
				case "0": return "Normal";
				case "9": return "Walk";
			}

			return val;
		}

		private string GetTeamName(string val) 
		{
			switch (val) 
			{
				case "0": return "Both";
				case "1": return "Axis";
				case "2": return "Ally";
			}

			return val;
		}
	}
}
