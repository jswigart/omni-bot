using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace WaypointTool
{
	public class BspReaderControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		private System.Windows.Forms.GroupBox gbBSPFile;
		private System.Windows.Forms.Button btnOpenFile1;
		private System.Windows.Forms.Button btnRead;
		private System.Windows.Forms.TextBox txtFileLocation;
		private System.Windows.Forms.Label lblFileLocation;
		private System.Windows.Forms.GroupBox gbEntities;
		private System.Windows.Forms.GroupBox gbFilters;
		private System.Windows.Forms.GroupBox gbEntityDefinition;
		private System.Windows.Forms.Panel pnlEntities;
		private System.Windows.Forms.ListView lstEntities;
		private System.Windows.Forms.Panel pnlDefinition;
		private System.Windows.Forms.RichTextBox txtEntityDefinition;
		private System.Windows.Forms.ColumnHeader chID;
		private System.Windows.Forms.ColumnHeader chClassName;
		private System.Windows.Forms.Label lblClassName;
		private System.Windows.Forms.Label lblTargetName;
		private System.Windows.Forms.ComboBox cboClassName;
		private System.Windows.Forms.ComboBox cboTargetName;
		private System.Windows.Forms.Button btnApplyFilter;
		private System.Windows.Forms.Button btnReset;
		private System.Windows.Forms.ComboBox cboTarget;
		private System.Windows.Forms.Label lblTarget;

		private System.ComponentModel.Container components = null;

		public BspReaderControl()
		{
			InitializeComponent();
			lstEntities.SelectedIndexChanged += new EventHandler(lstEntities_SelectedIndexChanged);
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
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.gbBSPFile = new System.Windows.Forms.GroupBox();
			this.btnOpenFile1 = new System.Windows.Forms.Button();
			this.btnRead = new System.Windows.Forms.Button();
			this.txtFileLocation = new System.Windows.Forms.TextBox();
			this.lblFileLocation = new System.Windows.Forms.Label();
			this.gbEntities = new System.Windows.Forms.GroupBox();
			this.pnlEntities = new System.Windows.Forms.Panel();
			this.lstEntities = new System.Windows.Forms.ListView();
			this.chID = new System.Windows.Forms.ColumnHeader();
			this.chClassName = new System.Windows.Forms.ColumnHeader();
			this.gbFilters = new System.Windows.Forms.GroupBox();
			this.cboTarget = new System.Windows.Forms.ComboBox();
			this.lblTarget = new System.Windows.Forms.Label();
			this.btnReset = new System.Windows.Forms.Button();
			this.btnApplyFilter = new System.Windows.Forms.Button();
			this.cboTargetName = new System.Windows.Forms.ComboBox();
			this.cboClassName = new System.Windows.Forms.ComboBox();
			this.lblTargetName = new System.Windows.Forms.Label();
			this.lblClassName = new System.Windows.Forms.Label();
			this.gbEntityDefinition = new System.Windows.Forms.GroupBox();
			this.pnlDefinition = new System.Windows.Forms.Panel();
			this.txtEntityDefinition = new System.Windows.Forms.RichTextBox();
			this.gbBSPFile.SuspendLayout();
			this.gbEntities.SuspendLayout();
			this.pnlEntities.SuspendLayout();
			this.gbFilters.SuspendLayout();
			this.gbEntityDefinition.SuspendLayout();
			this.pnlDefinition.SuspendLayout();
			this.SuspendLayout();
			// 
			// gbBSPFile
			// 
			this.gbBSPFile.BackColor = System.Drawing.SystemColors.Control;
			this.gbBSPFile.Controls.Add(this.btnOpenFile1);
			this.gbBSPFile.Controls.Add(this.btnRead);
			this.gbBSPFile.Controls.Add(this.txtFileLocation);
			this.gbBSPFile.Controls.Add(this.lblFileLocation);
			this.gbBSPFile.Dock = System.Windows.Forms.DockStyle.Top;
			this.gbBSPFile.Location = new System.Drawing.Point(0, 0);
			this.gbBSPFile.Name = "gbBSPFile";
			this.gbBSPFile.Size = new System.Drawing.Size(584, 80);
			this.gbBSPFile.TabIndex = 4;
			this.gbBSPFile.TabStop = false;
			this.gbBSPFile.Text = "BSP File";
			// 
			// btnOpenFile1
			// 
			this.btnOpenFile1.Location = new System.Drawing.Point(424, 24);
			this.btnOpenFile1.Name = "btnOpenFile1";
			this.btnOpenFile1.Size = new System.Drawing.Size(32, 20);
			this.btnOpenFile1.TabIndex = 6;
			this.btnOpenFile1.Text = "...";
			this.btnOpenFile1.Click += new System.EventHandler(this.btnOpenFile1_Click);
			// 
			// btnRead
			// 
			this.btnRead.Location = new System.Drawing.Point(96, 48);
			this.btnRead.Name = "btnRead";
			this.btnRead.Size = new System.Drawing.Size(88, 20);
			this.btnRead.TabIndex = 5;
			this.btnRead.Text = "Read";
			this.btnRead.Click += new System.EventHandler(this.btnRead_Click);
			// 
			// txtFileLocation
			// 
			this.txtFileLocation.BackColor = System.Drawing.SystemColors.Window;
			this.txtFileLocation.Location = new System.Drawing.Point(96, 24);
			this.txtFileLocation.Name = "txtFileLocation";
			this.txtFileLocation.Size = new System.Drawing.Size(320, 20);
			this.txtFileLocation.TabIndex = 2;
			this.txtFileLocation.Text = "";
			// 
			// lblFileLocation
			// 
			this.lblFileLocation.Location = new System.Drawing.Point(8, 24);
			this.lblFileLocation.Name = "lblFileLocation";
			this.lblFileLocation.Size = new System.Drawing.Size(96, 16);
			this.lblFileLocation.TabIndex = 0;
			this.lblFileLocation.Text = "File Location";
			this.lblFileLocation.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
			// 
			// gbEntities
			// 
			this.gbEntities.BackColor = System.Drawing.SystemColors.Control;
			this.gbEntities.Controls.Add(this.pnlEntities);
			this.gbEntities.Dock = System.Windows.Forms.DockStyle.Left;
			this.gbEntities.Location = new System.Drawing.Point(0, 80);
			this.gbEntities.Name = "gbEntities";
			this.gbEntities.Size = new System.Drawing.Size(248, 408);
			this.gbEntities.TabIndex = 5;
			this.gbEntities.TabStop = false;
			this.gbEntities.Text = "Entities";
			// 
			// pnlEntities
			// 
			this.pnlEntities.Controls.Add(this.lstEntities);
			this.pnlEntities.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pnlEntities.DockPadding.All = 4;
			this.pnlEntities.Location = new System.Drawing.Point(3, 16);
			this.pnlEntities.Name = "pnlEntities";
			this.pnlEntities.Size = new System.Drawing.Size(242, 389);
			this.pnlEntities.TabIndex = 0;
			// 
			// lstEntities
			// 
			this.lstEntities.BackColor = System.Drawing.SystemColors.Window;
			this.lstEntities.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.lstEntities.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																						  this.chID,
																						  this.chClassName});
			this.lstEntities.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lstEntities.FullRowSelect = true;
			this.lstEntities.GridLines = true;
			this.lstEntities.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.lstEntities.Location = new System.Drawing.Point(4, 4);
			this.lstEntities.MultiSelect = false;
			this.lstEntities.Name = "lstEntities";
			this.lstEntities.Size = new System.Drawing.Size(234, 381);
			this.lstEntities.TabIndex = 0;
			this.lstEntities.View = System.Windows.Forms.View.Details;
			// 
			// chID
			// 
			this.chID.Text = "Number";
			// 
			// chClassName
			// 
			this.chClassName.Text = "Class Name";
			this.chClassName.Width = 155;
			// 
			// gbFilters
			// 
			this.gbFilters.BackColor = System.Drawing.SystemColors.Control;
			this.gbFilters.Controls.Add(this.cboTarget);
			this.gbFilters.Controls.Add(this.lblTarget);
			this.gbFilters.Controls.Add(this.btnReset);
			this.gbFilters.Controls.Add(this.btnApplyFilter);
			this.gbFilters.Controls.Add(this.cboTargetName);
			this.gbFilters.Controls.Add(this.cboClassName);
			this.gbFilters.Controls.Add(this.lblTargetName);
			this.gbFilters.Controls.Add(this.lblClassName);
			this.gbFilters.Dock = System.Windows.Forms.DockStyle.Top;
			this.gbFilters.Location = new System.Drawing.Point(248, 80);
			this.gbFilters.Name = "gbFilters";
			this.gbFilters.Size = new System.Drawing.Size(336, 120);
			this.gbFilters.TabIndex = 6;
			this.gbFilters.TabStop = false;
			this.gbFilters.Text = "Filters";
			// 
			// cboTarget
			// 
			this.cboTarget.BackColor = System.Drawing.SystemColors.Window;
			this.cboTarget.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboTarget.Location = new System.Drawing.Point(88, 64);
			this.cboTarget.Name = "cboTarget";
			this.cboTarget.Size = new System.Drawing.Size(240, 21);
			this.cboTarget.TabIndex = 7;
			// 
			// lblTarget
			// 
			this.lblTarget.Location = new System.Drawing.Point(8, 64);
			this.lblTarget.Name = "lblTarget";
			this.lblTarget.Size = new System.Drawing.Size(72, 16);
			this.lblTarget.TabIndex = 6;
			this.lblTarget.Text = "Target";
			this.lblTarget.TextAlign = System.Drawing.ContentAlignment.BottomRight;
			// 
			// btnReset
			// 
			this.btnReset.Location = new System.Drawing.Point(168, 96);
			this.btnReset.Name = "btnReset";
			this.btnReset.Size = new System.Drawing.Size(72, 20);
			this.btnReset.TabIndex = 5;
			this.btnReset.Text = "Reset";
			this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
			// 
			// btnApplyFilter
			// 
			this.btnApplyFilter.Location = new System.Drawing.Point(88, 96);
			this.btnApplyFilter.Name = "btnApplyFilter";
			this.btnApplyFilter.Size = new System.Drawing.Size(72, 20);
			this.btnApplyFilter.TabIndex = 4;
			this.btnApplyFilter.Text = "Apply";
			this.btnApplyFilter.Click += new System.EventHandler(this.btnApplyFilter_Click);
			// 
			// cboTargetName
			// 
			this.cboTargetName.BackColor = System.Drawing.SystemColors.Window;
			this.cboTargetName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboTargetName.Location = new System.Drawing.Point(88, 40);
			this.cboTargetName.Name = "cboTargetName";
			this.cboTargetName.Size = new System.Drawing.Size(240, 21);
			this.cboTargetName.TabIndex = 3;
			// 
			// cboClassName
			// 
			this.cboClassName.BackColor = System.Drawing.SystemColors.Window;
			this.cboClassName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cboClassName.Location = new System.Drawing.Point(88, 16);
			this.cboClassName.Name = "cboClassName";
			this.cboClassName.Size = new System.Drawing.Size(240, 21);
			this.cboClassName.TabIndex = 2;
			// 
			// lblTargetName
			// 
			this.lblTargetName.Location = new System.Drawing.Point(8, 40);
			this.lblTargetName.Name = "lblTargetName";
			this.lblTargetName.Size = new System.Drawing.Size(72, 16);
			this.lblTargetName.TabIndex = 1;
			this.lblTargetName.Text = "Target Name";
			this.lblTargetName.TextAlign = System.Drawing.ContentAlignment.BottomRight;
			// 
			// lblClassName
			// 
			this.lblClassName.Location = new System.Drawing.Point(8, 16);
			this.lblClassName.Name = "lblClassName";
			this.lblClassName.Size = new System.Drawing.Size(72, 16);
			this.lblClassName.TabIndex = 0;
			this.lblClassName.Text = "Class Name";
			this.lblClassName.TextAlign = System.Drawing.ContentAlignment.BottomRight;
			// 
			// gbEntityDefinition
			// 
			this.gbEntityDefinition.BackColor = System.Drawing.SystemColors.Control;
			this.gbEntityDefinition.Controls.Add(this.pnlDefinition);
			this.gbEntityDefinition.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbEntityDefinition.Location = new System.Drawing.Point(248, 200);
			this.gbEntityDefinition.Name = "gbEntityDefinition";
			this.gbEntityDefinition.Size = new System.Drawing.Size(336, 288);
			this.gbEntityDefinition.TabIndex = 7;
			this.gbEntityDefinition.TabStop = false;
			this.gbEntityDefinition.Text = "Entity Definition";
			// 
			// pnlDefinition
			// 
			this.pnlDefinition.Controls.Add(this.txtEntityDefinition);
			this.pnlDefinition.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pnlDefinition.DockPadding.All = 4;
			this.pnlDefinition.Location = new System.Drawing.Point(3, 16);
			this.pnlDefinition.Name = "pnlDefinition";
			this.pnlDefinition.Size = new System.Drawing.Size(330, 269);
			this.pnlDefinition.TabIndex = 0;
			// 
			// txtEntityDefinition
			// 
			this.txtEntityDefinition.BackColor = System.Drawing.SystemColors.Window;
			this.txtEntityDefinition.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.txtEntityDefinition.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtEntityDefinition.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.txtEntityDefinition.Location = new System.Drawing.Point(4, 4);
			this.txtEntityDefinition.Name = "txtEntityDefinition";
			this.txtEntityDefinition.ReadOnly = true;
			this.txtEntityDefinition.Size = new System.Drawing.Size(322, 261);
			this.txtEntityDefinition.TabIndex = 0;
			this.txtEntityDefinition.Text = "";
			// 
			// BspReaderControl
			// 
			this.Controls.Add(this.gbEntityDefinition);
			this.Controls.Add(this.gbFilters);
			this.Controls.Add(this.gbEntities);
			this.Controls.Add(this.gbBSPFile);
			this.Name = "BspReaderControl";
			this.Size = new System.Drawing.Size(584, 488);
			this.gbBSPFile.ResumeLayout(false);
			this.gbEntities.ResumeLayout(false);
			this.pnlEntities.ResumeLayout(false);
			this.gbFilters.ResumeLayout(false);
			this.gbEntityDefinition.ResumeLayout(false);
			this.pnlDefinition.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private void OpenFile(TextBox textBox) 
		{
			openFileDialog.Filter = "Quake BSP Files(*.bsp)|*.bsp";
			openFileDialog.ShowDialog(this.ParentForm);

			if (openFileDialog.FileName != null && openFileDialog.FileName.Trim().Length > 0) 
			{
				textBox.Text = openFileDialog.FileName;
			}		
		}

		private void btnOpenFile1_Click(object sender, System.EventArgs e)
		{
			OpenFile(txtFileLocation);
		}

		private ArrayList _entities = new ArrayList();
		private Hashtable _entityLookup = new Hashtable();

		private void btnRead_Click(object sender, System.EventArgs e)
		{
			try 
			{
				
				if (txtFileLocation.Text.Trim().Length == 0) 
				{
					MessageBox.Show(this.ParentForm, "Please choose a file to read.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				cboClassName.Items.Clear();
				cboTargetName.Items.Clear();
				cboTarget.Items.Clear();
				lstEntities.Items.Clear();
				txtEntityDefinition.Clear();

				BspReader reader = new BspReader();
				_entities = reader.Read(txtFileLocation.Text);
				_entityLookup = new Hashtable();

				Hashtable classes = new Hashtable();
				Hashtable targetNames = new Hashtable();
				Hashtable targets = new Hashtable();

				foreach (BspEntity entity in _entities) 
				{
					// skip these until we figure out the right ent num
					if (entity.ClassName == "misc_mg42") 
					{
						continue;
					}

					if (classes.ContainsKey(entity.ClassName)) 
					{
						int count = (int)classes[entity.ClassName];
						count++;
						classes[entity.ClassName] = count;
					} 
					else 
					{
						classes.Add(entity.ClassName, 1);
					}

					if (targetNames.ContainsKey(entity.TargetName)) 
					{
						int count = (int)targetNames[entity.TargetName];
						count++;
						targetNames[entity.TargetName] = count;
					} 
					else 
					{
						targetNames.Add(entity.TargetName, 1);
					}

					if (targets.ContainsKey(entity.Target)) 
					{
						int count = (int)targets[entity.Target];
						count++;
						targets[entity.Target] = count;
					} 
					else 
					{
						targets.Add(entity.Target, 1);
					}

					ListViewItem item = new ListViewItem(Convert.ToString(entity.Number));
					item.SubItems.Add(entity.ClassName);
					lstEntities.Items.Add(item);

					_entityLookup.Add(Convert.ToString(entity.Number), entity);
				}

				cboClassName.Items.Add("All");
				cboClassName.SelectedIndex = 0;
				cboTargetName.Items.Add("All");
				cboTargetName.SelectedIndex = 0;
				cboTarget.Items.Add("All");
				cboTarget.SelectedIndex = 0;

				ArrayList classList = new ArrayList();
				ArrayList targetNameList = new ArrayList();
				ArrayList targetList = new ArrayList();

				foreach (string key in classes.Keys) 
				{
					classList.Add(key);
				}

				foreach (string key in targetNames.Keys) 
				{
					targetNameList.Add(key);
				}
				
				foreach (string key in targets.Keys) 
				{
					targetList.Add(key);
				}

				classList.Sort();
				targetNameList.Sort();
				targetList.Sort();

				foreach (string key in classList) 
				{
					cboClassName.Items.Add(key);
				}

				foreach (string key in targetNameList) 
				{
					cboTargetName.Items.Add(key);
				}

				foreach (string key in targetList) 
				{
					cboTarget.Items.Add(key);
				}
				
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error reading bsp file. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void ApplyFilter() 
		{
			lstEntities.Items.Clear();

			string classFilter = Convert.ToString(cboClassName.SelectedItem);
			string targetNameFilter = Convert.ToString(cboTargetName.SelectedItem);
			string targetFilter = Convert.ToString(cboTarget.SelectedItem);

			foreach (BspEntity entity in _entities) 
			{
				// skip these until we figure out the right ent num
				if (entity.ClassName == "misc_mg42") 
				{
					continue;
				}

				if ((cboClassName.SelectedIndex == 0 || entity.ClassName == classFilter)
					&&
					(cboTargetName.SelectedIndex == 0 || entity.TargetName == targetNameFilter)
					&&
					(cboTarget.SelectedIndex == 0 || entity.Target == targetFilter))
				{
					ListViewItem item = new ListViewItem(Convert.ToString(entity.Number));
					item.SubItems.Add(entity.ClassName);
					lstEntities.Items.Add(item);
				}
			}
		}

		private void btnApplyFilter_Click(object sender, System.EventArgs e)
		{
			try 
			{
				ApplyFilter();
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error filtering entities. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void lstEntities_SelectedIndexChanged(object sender, EventArgs e)
		{
			try 
			{
				string id = lstEntities.SelectedItems[0].Text;

				BspEntity entity = _entityLookup[id] as BspEntity;

				if (entity != null) 
				{
					txtEntityDefinition.Text = entity.Text;
				}
			} 
			catch {}
		}

		private void btnReset_Click(object sender, System.EventArgs e)
		{
			try 
			{
				cboClassName.SelectedIndex = 0;
				cboTargetName.SelectedIndex = 0;
				cboTarget.SelectedIndex = 0;
				ApplyFilter();
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error filtering entities. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}
