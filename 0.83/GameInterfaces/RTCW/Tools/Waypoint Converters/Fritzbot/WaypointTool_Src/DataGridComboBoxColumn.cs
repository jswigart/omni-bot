using System;
using System.Windows.Forms;
using System.Drawing;
using System.Data;
using System.Collections;

namespace WaypointTool
{
	/// <summary>
	/// Implementation of a ComboBox as a column in a DataGrid
	/// </summary>
	public class DataGridComboBoxColumn : DataGridColumnStyle {
		private int _xMargin = 2;
		private int _yMargin = 1;
		private ComboBox _comboBox;
		private string _oldValue="";
		private bool _inEdit= false;
		private DataTable _dataTable;
		private string _displayMember, _valueMember;

		/// <summary>
		/// The only creator ;)
		/// </summary>
		/// <param name="colName">The name of the column</param>
		/// <param name="dataSource">The datasource that contains the lookup table</param>
		/// <param name="displayMember">The member of the lookuptable to display</param>
		/// <param name="valueMember">The member of the lookuptable with the value</param>
		/// <param name="dataGrid">The datagrid parent of this column</param>
		public DataGridComboBoxColumn(string colName, DataTable dataSource, string displayMember, string valueMember, DataGrid dataGrid) {
			_comboBox = new ComboBox();
			_comboBox.Visible = false;
			_comboBox.DataSource = dataSource;
			_dataTable = dataSource;
			_comboBox.DisplayMember = displayMember;
			_displayMember=displayMember;
			_valueMember=valueMember;
			_comboBox.ValueMember = valueMember;
			_comboBox.DropDownStyle = ComboBoxStyle.DropDownList;
			Graphics _graphicsContext=dataGrid.CreateGraphics();
			float _widest=0;
			SizeF _stringSize=new SizeF(0,0);
			foreach (DataRow dr in dataSource.Rows) {
				_stringSize=_graphicsContext.MeasureString(dr[displayMember].ToString(), dataGrid.Font);
				if (_stringSize.Width>_widest) {
					_widest=_stringSize.Width;
				}
			}
			_comboBox.DropDownWidth=(int)Math.Ceiling(_widest);
			this.Width=_comboBox.DropDownWidth+25; // Add the space for the dropdown arrow
			this.MappingName=colName;
			this.HeaderText=colName;
			dataGrid.Controls.Add(_comboBox);
		}

		/// <summary>
		/// Standard override
		/// </summary>
		/// <param name="rowNum"></param>
		protected override void Abort(int rowNum) {
			_inEdit=false;
			_comboBox.Hide();
		}
		/// <summary>
		/// Standard override
		/// </summary>
		/// <param name="dataSource"></param>
		/// <param name="rowNum"></param>
		/// <returns></returns>
		protected override bool Commit(CurrencyManager dataSource,int rowNum) {
			if(!_inEdit) {
				return true;
			}
			try {
				object _value = _comboBox.SelectedValue;
				if(NullText.Equals(_value)) {
					_value = System.Convert.DBNull; 
				}
				this.SetColumnValueAtRow(dataSource, rowNum, _value);
			}
			catch {
				return false;
			}
			finally {
				_inEdit=false;
				_comboBox.Hide();
			}
			return true;
		}
		
		/// <summary>
		/// Standard override
		/// </summary>
		/// <param name="source"></param>
		/// <param name="rowNum"></param>
		/// <param name="bounds"></param>
		/// <param name="readOnly"></param>
		/// <param name="instantText"></param>
		/// <param name="cellIsVisible"></param>
		protected override void Edit(CurrencyManager source, int rowNum, Rectangle bounds, bool readOnly, string instantText, bool cellIsVisible) {
			_comboBox.Text = "";
			Rectangle _originalBounds = bounds;
			_oldValue = _comboBox.Text;
	
			if(!cellIsVisible) return;

			bounds.Offset(_xMargin, _yMargin);
			bounds.Width -= _xMargin * 2;
			bounds.Height -= _yMargin;
			_comboBox.Bounds = bounds;
			
			_comboBox.Visible = !this.ReadOnly;
			//_comboBox.BringToFront();
			//_comboBox.Focus();
			
			_comboBox.SelectedValue = GetText(GetColumnValueAtRow(source, rowNum));
			
			if(instantText!=null) {
				_comboBox.SelectedValue = instantText;
				int End = _comboBox.Text.Length;
				_comboBox.Select(End, 0);
			} else {
				_comboBox.SelectAll();
			}

			//this.DataGridTableStyle.DataGrid.Invalidate(OriginalBounds);
			//_comboBox.BringToFront();
			_inEdit = true;
		}
		
		/// <summary>
		/// Standard override
		/// </summary>
		/// <returns></returns>
		protected override int GetMinimumHeight() {
			return _comboBox.PreferredHeight + _yMargin;
		}
		
		/// <summary>
		/// Standard override
		/// </summary>
		/// <param name="g"></param>
		/// <param name="val"></param>
		/// <returns></returns>
		protected override int GetPreferredHeight(Graphics g ,object val) {
			return FontHeight + _yMargin;
		}
		
		/// <summary>
		/// Standard override
		/// </summary>
		/// <param name="g"></param>
		/// <param name="val"></param>
		/// <returns></returns>
		protected override Size GetPreferredSize(Graphics g, object val) {
			Size _extents = Size.Ceiling(g.MeasureString(GetText(val), this.DataGridTableStyle.DataGrid.Font));
			_extents.Width += _xMargin * 2 ;
			_extents.Height += _yMargin;
			return _extents;
		}
		
		/// <summary>
		/// Standard override
		/// </summary>
		/// <param name="g"></param>
		/// <param name="bounds"></param>
		/// <param name="source"></param>
		/// <param name="rowNum"></param>
		protected override void Paint(Graphics g,Rectangle bounds,CurrencyManager source,int rowNum) {
			Paint(g, bounds, source, rowNum, false);
		}

		/// <summary>
		/// Standard override
		/// </summary>
		/// <param name="g"></param>
		/// <param name="bounds"></param>
		/// <param name="source"></param>
		/// <param name="rowNum"></param>
		/// <param name="alignToRight"></param>
		protected override void Paint(Graphics g,Rectangle bounds,CurrencyManager source,int rowNum, bool alignToRight) {
			string _text = GetText(GetColumnValueAtRow(source, rowNum));
			foreach (DataRow dr in _dataTable.Rows) {
				if (dr[_valueMember].ToString()==_text) {
					_text=dr[_displayMember].ToString();
					break;
				}
			}
			PaintText(g, bounds, _text, alignToRight, rowNum);
		}

		// Helper functions
		private void PaintText(Graphics g ,Rectangle bounds, string text,bool alignToRight, int rowNum) {
			
			Brush _backBrush = new SolidBrush(this.DataGridTableStyle.AlternatingBackColor);
			_comboBox.BackColor = this.DataGridTableStyle.AlternatingBackColor;

			if (rowNum % 2 == 0) 
			{
				_backBrush = new SolidBrush(this.DataGridTableStyle.BackColor);
				_comboBox.BackColor = this.DataGridTableStyle.BackColor;
			}

			Brush _foreBrush= new SolidBrush(this.DataGridTableStyle.ForeColor);
			Rectangle _rect = bounds;
			RectangleF _rectF  = _rect; 
			StringFormat _format = new StringFormat();
			if(alignToRight) {
				_format.FormatFlags = StringFormatFlags.DirectionRightToLeft;
			}
			switch(this.Alignment) {
				case HorizontalAlignment.Left:
					_format.Alignment = StringAlignment.Near;
					break;
				case HorizontalAlignment.Right:
					_format.Alignment = StringAlignment.Far;
					break;
				case HorizontalAlignment.Center:
					_format.Alignment = StringAlignment.Center;
					break;
			}
			_format.FormatFlags =StringFormatFlags.NoWrap;
			g.FillRectangle(_backBrush, _rect);
			_rect.Offset(0, _yMargin);
			_rect.Height -= _yMargin;
			g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, _foreBrush, _rectF, _format);
			_format.Dispose();
		}

		private string GetText(object val) {
			if(val==System.DBNull.Value) {
				return this.NullText;
			}
			if(val!=null) {
				return val.ToString();
			}
			else {
				return string.Empty;
			}
		}
	}
}