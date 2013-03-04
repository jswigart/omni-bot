using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Text;

namespace WaypointTool
{
	public class CodeHelperDialog : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox gbMain;
		private System.Windows.Forms.Panel pnlCommandHelp;
		private System.Windows.Forms.RichTextBox txtCommandHelp;
		private System.Windows.Forms.Label lblKeyword;
		private System.Windows.Forms.ComboBox cboKeyword;
		private System.Windows.Forms.Button btnInsert;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.ToolTip toolTipInput;
		private System.ComponentModel.IContainer components;

		public CodeHelperDialog()
		{
			InitializeComponent();
			InitializeCommands();
		}

		private void InitializeCommands() 
		{
			KeywordCollection keywords = KeywordFactory.GetKeywords();

			foreach (Keyword keyword in keywords) 
			{
				cboKeyword.Items.Add(keyword);
			}

			cboKeyword.SelectedIndex = 0;
			cboKeyword.SelectedIndexChanged += new EventHandler(cboKeyword_SelectedIndexChanged);

			UpdateUI();
		}

		private ArrayList _controls = new ArrayList();

		private void UpdateUI() 
		{
			// loop through and dispose of already existing controls
			foreach (Control control in _controls) 
			{
				gbMain.Controls.Remove(control);
				control.Dispose();
			}
			_controls.Clear();

			// draw the inputs, fill in the help text
			Keyword keyword = (Keyword)cboKeyword.SelectedItem;
			txtCommandHelp.Text = "[" + keyword.Command + "] " + keyword.HelpText;

			int rowOffset = lblKeyword.Height + 10;
			int rowTop = lblKeyword.Top + rowOffset;
			
			foreach (KeywordInput input in keyword.Inputs) 
			{
				switch (input.InputType) 
				{
					case KeywordInputType.Action:
					case KeywordInputType.Goal:
					case KeywordInputType.Group:
					case KeywordInputType.Node:
					case KeywordInputType.Flag:
					case KeywordInputType.Integer:
						Label label = new Label();
						label.TextAlign = ContentAlignment.MiddleRight;
						label.Width = lblKeyword.Width;
						label.Height = lblKeyword.Height;
						label.Text = input.Label;
						label.Top = rowTop;
						label.Left = lblKeyword.Left;

						gbMain.Controls.Add(label);
						_controls.Add(label);
			
						TextBox textBox = new TextBox();
						textBox.Width = 40;
						textBox.Height = 20;
						textBox.Left = cboKeyword.Left;
						textBox.Top = rowTop;

						this.toolTipInput.SetToolTip(textBox, input.HelpText);
						gbMain.Controls.Add(textBox);
						_controls.Add(textBox);

						break;
					case KeywordInputType.Boolean:
						CheckBox checkBox = new CheckBox();
						checkBox.Text = input.Label;
						checkBox.Left = cboKeyword.Left;
						checkBox.Width = 200;
						checkBox.Top = rowTop;

						this.toolTipInput.SetToolTip(checkBox, input.HelpText);
						gbMain.Controls.Add(checkBox);
						_controls.Add(checkBox);

						break;
					case KeywordInputType.PredefinedList:
						Label listLabel = new Label();
						listLabel.TextAlign = ContentAlignment.MiddleRight;
						listLabel.Width = lblKeyword.Width;
						listLabel.Height = lblKeyword.Height;
						listLabel.Text = input.Label;
						listLabel.Top = rowTop;
						listLabel.Left = lblKeyword.Left;

						gbMain.Controls.Add(listLabel);
						_controls.Add(listLabel);

						ComboBox combo = new ComboBox();
						combo.Left = cboKeyword.Left;
						combo.Top = rowTop;
						combo.DropDownStyle = ComboBoxStyle.DropDownList;

						foreach (string val in input.AllowableValues) 
						{
							combo.Items.Add(val);
						}

						combo.SelectedIndex = 0;
						this.toolTipInput.SetToolTip(combo, input.HelpText);
						gbMain.Controls.Add(combo);
						_controls.Add(combo);
						break;
				}

				rowTop += rowOffset;
			}
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.gbMain = new System.Windows.Forms.GroupBox();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnInsert = new System.Windows.Forms.Button();
			this.cboKeyword = new System.Windows.Forms.ComboBox();
			this.lblKeyword = new System.Windows.Forms.Label();
			this.pnlCommandHelp = new System.Windows.Forms.Panel();
			this.txtCommandHelp = new System.Windows.Forms.RichTextBox();
			this.toolTipInput = new System.Windows.Forms.ToolTip(this.components);
			this.gbMain.SuspendLayout();
			this.pnlCommandHelp.SuspendLayout();
			this.SuspendLayout();
			// 
			// gbMain
			// 
			this.gbMain.Controls.Add(this.btnCancel);
			this.gbMain.Controls.Add(this.btnInsert);
			this.gbMain.Controls.Add(this.cboKeyword);
			this.gbMain.Controls.Add(this.lblKeyword);
			this.gbMain.Controls.Add(this.pnlCommandHelp);
			this.gbMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbMain.Location = new System.Drawing.Point(0, 0);
			this.gbMain.Name = "gbMain";
			this.gbMain.Size = new System.Drawing.Size(330, 229);
			this.gbMain.TabIndex = 0;
			this.gbMain.TabStop = false;
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(216, 136);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(56, 20);
			this.btnCancel.TabIndex = 4;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// btnInsert
			// 
			this.btnInsert.Location = new System.Drawing.Point(152, 136);
			this.btnInsert.Name = "btnInsert";
			this.btnInsert.Size = new System.Drawing.Size(56, 20);
			this.btnInsert.TabIndex = 3;
			this.btnInsert.Text = "Insert";
			this.btnInsert.Click += new System.EventHandler(this.btnInsert_Click);
			// 
			// cboKeyword
			// 
			this.cboKeyword.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboKeyword.Location = new System.Drawing.Point(152, 8);
			this.cboKeyword.Name = "cboKeyword";
			this.cboKeyword.Size = new System.Drawing.Size(168, 21);
			this.cboKeyword.TabIndex = 1;
			// 
			// lblKeyword
			// 
			this.lblKeyword.Location = new System.Drawing.Point(8, 16);
			this.lblKeyword.Name = "lblKeyword";
			this.lblKeyword.Size = new System.Drawing.Size(128, 16);
			this.lblKeyword.TabIndex = 1;
			this.lblKeyword.Text = "Keyword";
			this.lblKeyword.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// pnlCommandHelp
			// 
			this.pnlCommandHelp.Controls.Add(this.txtCommandHelp);
			this.pnlCommandHelp.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pnlCommandHelp.Location = new System.Drawing.Point(3, 164);
			this.pnlCommandHelp.Name = "pnlCommandHelp";
			this.pnlCommandHelp.Size = new System.Drawing.Size(324, 62);
			this.pnlCommandHelp.TabIndex = 0;
			// 
			// txtCommandHelp
			// 
			this.txtCommandHelp.BackColor = System.Drawing.Color.Beige;
			this.txtCommandHelp.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.txtCommandHelp.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtCommandHelp.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.txtCommandHelp.Location = new System.Drawing.Point(0, 0);
			this.txtCommandHelp.Name = "txtCommandHelp";
			this.txtCommandHelp.ReadOnly = true;
			this.txtCommandHelp.Size = new System.Drawing.Size(324, 62);
			this.txtCommandHelp.TabIndex = 0;
			this.txtCommandHelp.TabStop = false;
			this.txtCommandHelp.Text = "";
			// 
			// CodeHelperDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(330, 229);
			this.Controls.Add(this.gbMain);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "CodeHelperDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Script Help";
			this.gbMain.ResumeLayout(false);
			this.pnlCommandHelp.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private void cboKeyword_SelectedIndexChanged(object sender, EventArgs e)
		{
			UpdateUI();
		}

		private bool _apply = false;
		private string _codeResult = "";

		public bool Apply
		{
			get { return _apply; }
			set { _apply = value; } 
		}

		public string CodeResult
		{
			get { return _codeResult; }
			set { _codeResult = value; } 
		}

		private void btnInsert_Click(object sender, System.EventArgs e)
		{
			StringBuilder buff = new StringBuilder();

			Keyword keyword = (Keyword)cboKeyword.SelectedItem;

			buff.Append(keyword.Command);

			foreach (Control control in _controls) 
			{
				if (control is TextBox) 
				{
					buff.Append(" ");
					buff.Append(((TextBox)control).Text);
				} 

				if (control is CheckBox) 
				{
					buff.Append(" ");

					CheckBox checkBox = (CheckBox)control;

					if (checkBox.Checked) 
					{
						buff.Append("true");
					} 
					else 
					{
						buff.Append("false");
					}
				}

				if (control is ComboBox) 
				{
					buff.Append(" ");
					ComboBox combo = (ComboBox)control;
					buff.Append(Convert.ToString(combo.SelectedItem));
				}
			}

			_apply = true;
			_codeResult = buff.ToString();
			this.Close();
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			_apply = false;
			this.Close();
		}
	}
}
