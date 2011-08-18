using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace WaypointTool
{
	public class NavigationCompareControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.GroupBox gbNavigationFiles;
		private System.Windows.Forms.GroupBox gbSummary;
		private System.Windows.Forms.Label lblNavigationFile1;
		private System.Windows.Forms.Label lblNavigationFile2;
		private System.Windows.Forms.Button btnCompare;
		private System.Windows.Forms.Button btnOpenFile1;
		private System.Windows.Forms.Button btnOpenFile2;
		private System.Windows.Forms.TextBox txtNavigationFile1;
		private System.Windows.Forms.TextBox txtNavigationFile2;
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		private System.Windows.Forms.RichTextBox txtSummary;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public NavigationCompareControl()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call

		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
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

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.gbNavigationFiles = new System.Windows.Forms.GroupBox();
			this.btnOpenFile2 = new System.Windows.Forms.Button();
			this.btnOpenFile1 = new System.Windows.Forms.Button();
			this.btnCompare = new System.Windows.Forms.Button();
			this.txtNavigationFile2 = new System.Windows.Forms.TextBox();
			this.lblNavigationFile2 = new System.Windows.Forms.Label();
			this.txtNavigationFile1 = new System.Windows.Forms.TextBox();
			this.lblNavigationFile1 = new System.Windows.Forms.Label();
			this.gbSummary = new System.Windows.Forms.GroupBox();
			this.txtSummary = new System.Windows.Forms.RichTextBox();
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.gbNavigationFiles.SuspendLayout();
			this.gbSummary.SuspendLayout();
			this.SuspendLayout();
			// 
			// gbNavigationFiles
			// 
			this.gbNavigationFiles.BackColor = System.Drawing.SystemColors.ControlLight;
			this.gbNavigationFiles.Controls.Add(this.btnOpenFile2);
			this.gbNavigationFiles.Controls.Add(this.btnOpenFile1);
			this.gbNavigationFiles.Controls.Add(this.btnCompare);
			this.gbNavigationFiles.Controls.Add(this.txtNavigationFile2);
			this.gbNavigationFiles.Controls.Add(this.lblNavigationFile2);
			this.gbNavigationFiles.Controls.Add(this.txtNavigationFile1);
			this.gbNavigationFiles.Controls.Add(this.lblNavigationFile1);
			this.gbNavigationFiles.Dock = System.Windows.Forms.DockStyle.Top;
			this.gbNavigationFiles.Location = new System.Drawing.Point(0, 0);
			this.gbNavigationFiles.Name = "gbNavigationFiles";
			this.gbNavigationFiles.Size = new System.Drawing.Size(528, 104);
			this.gbNavigationFiles.TabIndex = 0;
			this.gbNavigationFiles.TabStop = false;
			this.gbNavigationFiles.Text = "Navigation Files";
			// 
			// btnOpenFile2
			// 
			this.btnOpenFile2.Location = new System.Drawing.Point(424, 48);
			this.btnOpenFile2.Name = "btnOpenFile2";
			this.btnOpenFile2.Size = new System.Drawing.Size(32, 20);
			this.btnOpenFile2.TabIndex = 7;
			this.btnOpenFile2.Text = "...";
			this.btnOpenFile2.Click += new System.EventHandler(this.btnOpenFile2_Click);
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
			// btnCompare
			// 
			this.btnCompare.Location = new System.Drawing.Point(96, 72);
			this.btnCompare.Name = "btnCompare";
			this.btnCompare.Size = new System.Drawing.Size(88, 20);
			this.btnCompare.TabIndex = 5;
			this.btnCompare.Text = "Compare";
			this.btnCompare.Click += new System.EventHandler(this.btnCompare_Click);
			// 
			// txtNavigationFile2
			// 
			this.txtNavigationFile2.Location = new System.Drawing.Point(96, 48);
			this.txtNavigationFile2.Name = "txtNavigationFile2";
			this.txtNavigationFile2.Size = new System.Drawing.Size(320, 20);
			this.txtNavigationFile2.TabIndex = 4;
			this.txtNavigationFile2.Text = "";
			// 
			// lblNavigationFile2
			// 
			this.lblNavigationFile2.Location = new System.Drawing.Point(8, 48);
			this.lblNavigationFile2.Name = "lblNavigationFile2";
			this.lblNavigationFile2.Size = new System.Drawing.Size(96, 16);
			this.lblNavigationFile2.TabIndex = 3;
			this.lblNavigationFile2.Text = "Right Navigation";
			this.lblNavigationFile2.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
			// 
			// txtNavigationFile1
			// 
			this.txtNavigationFile1.Location = new System.Drawing.Point(96, 24);
			this.txtNavigationFile1.Name = "txtNavigationFile1";
			this.txtNavigationFile1.Size = new System.Drawing.Size(320, 20);
			this.txtNavigationFile1.TabIndex = 2;
			this.txtNavigationFile1.Text = "";
			// 
			// lblNavigationFile1
			// 
			this.lblNavigationFile1.Location = new System.Drawing.Point(8, 24);
			this.lblNavigationFile1.Name = "lblNavigationFile1";
			this.lblNavigationFile1.Size = new System.Drawing.Size(96, 16);
			this.lblNavigationFile1.TabIndex = 0;
			this.lblNavigationFile1.Text = "Left Navigation";
			this.lblNavigationFile1.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
			// 
			// gbSummary
			// 
			this.gbSummary.BackColor = System.Drawing.SystemColors.ControlLight;
			this.gbSummary.Controls.Add(this.txtSummary);
			this.gbSummary.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbSummary.Location = new System.Drawing.Point(0, 104);
			this.gbSummary.Name = "gbSummary";
			this.gbSummary.Size = new System.Drawing.Size(528, 272);
			this.gbSummary.TabIndex = 1;
			this.gbSummary.TabStop = false;
			this.gbSummary.Text = "Difference Summary";
			// 
			// txtSummary
			// 
			this.txtSummary.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtSummary.Location = new System.Drawing.Point(3, 16);
			this.txtSummary.Name = "txtSummary";
			this.txtSummary.Size = new System.Drawing.Size(522, 253);
			this.txtSummary.TabIndex = 0;
			this.txtSummary.Text = "";
			// 
			// NavigationCompareControl
			// 
			this.Controls.Add(this.gbSummary);
			this.Controls.Add(this.gbNavigationFiles);
			this.Name = "NavigationCompareControl";
			this.Size = new System.Drawing.Size(528, 376);
			this.gbNavigationFiles.ResumeLayout(false);
			this.gbSummary.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void OpenFile(TextBox textBox) 
		{
			openFileDialog.Filter = "Fritz Navigation Files(*.nav)|*.nav";
			openFileDialog.ShowDialog(this.ParentForm);

			if (openFileDialog.FileName != null && openFileDialog.FileName.Trim().Length > 0) 
			{
				textBox.Text = openFileDialog.FileName;
			}		
		}

		private void btnOpenFile1_Click(object sender, System.EventArgs e)
		{
			OpenFile(txtNavigationFile1);
		}

		private void btnOpenFile2_Click(object sender, System.EventArgs e)
		{
			OpenFile(txtNavigationFile2);
		}

		private void btnCompare_Click(object sender, System.EventArgs e)
		{
			try 
			{
				txtSummary.Clear();

				if (txtNavigationFile1.Text.Trim().Length == 0 ||
					txtNavigationFile2.Text.Trim().Length == 0) 
				{
					MessageBox.Show(this.ParentForm, "Please choose two files to compare.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				NavigationReader left = new NavigationReader();
				NavigationReader right = new NavigationReader();

				left.Read(txtNavigationFile1.Text);
				right.Read(txtNavigationFile2.Text);

				NavigationDiff diff = new NavigationDiff();
				diff.DoDiff(left, right);

				if (diff.DifferentActions.Count == 0 && diff.DifferentNodes.Count == 0 && diff.DifferentRoutes.Count == 0) 
				{
					txtSummary.AppendText("The navigation files are the same.");
					return;
				}

				foreach (NodeDifference nodeDiff in diff.DifferentNodes) 
				{
					if (nodeDiff.LeftNode == null) 
					{
						txtSummary.AppendText("Left Navigation is missing Node " + nodeDiff.RightNode.ID + "\r\n\r\n");
					} 
					else if (nodeDiff.RightNode == null) 
					{
						txtSummary.AppendText("Right Navigation is missing Node " + nodeDiff.LeftNode.ID + "\r\n\r\n");
					} 
					else 
					{
						txtSummary.AppendText("Node " + nodeDiff.LeftNode.ID + " is different.\r\n");

						if (nodeDiff.LeftNode.Connect1 != nodeDiff.RightNode.Connect1) 
						{
							txtSummary.AppendText("\tLeft Connect 1 = " + nodeDiff.LeftNode.Connect1 + ", Right Connect 1 = " + nodeDiff.RightNode.Connect1 + "\r\n");
						}

						if (nodeDiff.LeftNode.Connect2 != nodeDiff.RightNode.Connect2) 
						{
							txtSummary.AppendText("\tLeft Connect 2 = " + nodeDiff.LeftNode.Connect2 + ", Right Connect 2 = " + nodeDiff.RightNode.Connect2 + "\r\n");
						}

						if (nodeDiff.LeftNode.Connect3 != nodeDiff.RightNode.Connect3) 
						{
							txtSummary.AppendText("\tLeft Connect 3 = " + nodeDiff.LeftNode.Connect3 + ", Right Connect 3 = " + nodeDiff.RightNode.Connect3 + "\r\n");
						}

						if (nodeDiff.LeftNode.Connect4 != nodeDiff.RightNode.Connect4) 
						{
							txtSummary.AppendText("\tLeft Connect 4 = " + nodeDiff.LeftNode.Connect4 + ", Right Connect 4 = " + nodeDiff.RightNode.Connect4 + "\r\n");
						}

						if (nodeDiff.LeftNode.Entity != nodeDiff.RightNode.Entity) 
						{
							txtSummary.AppendText("\tLeft Entity = " + nodeDiff.LeftNode.Entity + ", Right Entity = " + nodeDiff.RightNode.Entity + "\r\n");
						}

						if (nodeDiff.LeftNode.Flags != nodeDiff.RightNode.Flags) 
						{
							txtSummary.AppendText("\tLeft Flags = " + nodeDiff.LeftNode.Flags + ", Right Flags = " + nodeDiff.RightNode.Flags + "\r\n");
						}

						if (nodeDiff.LeftNode.Group != nodeDiff.RightNode.Group) 
						{
							txtSummary.AppendText("\tLeft Group = " + nodeDiff.LeftNode.Group + ", Right Group = " + nodeDiff.RightNode.Group + "\r\n");
						}

						if (nodeDiff.LeftNode.NumberOfConnects != nodeDiff.RightNode.NumberOfConnects) 
						{
							txtSummary.AppendText("\tLeft Number of Connections = " + nodeDiff.LeftNode.NumberOfConnects + ", Right Number of Connections = " + nodeDiff.RightNode.NumberOfConnects + "\r\n");
						}

						if (nodeDiff.LeftNode.Radius != nodeDiff.RightNode.Radius) 
						{
							txtSummary.AppendText("\tLeft Radius = " + nodeDiff.LeftNode.Radius + ", Right Radius = " + nodeDiff.RightNode.Radius + "\r\n");
						}

						if (nodeDiff.LeftNode.Team != nodeDiff.RightNode.Team) 
						{
							txtSummary.AppendText("\tLeft Team = " + nodeDiff.LeftNode.Team + ", Right Team = " + nodeDiff.RightNode.Team + "\r\n");
						}
					}
				}

				foreach (ActionDifference actionDiff in diff.DifferentActions) 
				{
					if (actionDiff.LeftAction == null) 
					{
						txtSummary.AppendText("Left Navigation is missing Action " + actionDiff.RightAction.ID + "\r\n\r\n");
					} 
					else if (actionDiff.RightAction == null) 
					{
						txtSummary.AppendText("Right Navigation is missing Action " + actionDiff.LeftAction.ID + "\r\n\r\n");
					} 
					else 
					{
						txtSummary.AppendText("Action " + actionDiff.LeftAction.ID + " is different.\r\n");

						if (actionDiff.LeftAction.Active != actionDiff.RightAction.Active) 
						{
							txtSummary.AppendText("\tLeft Active = " + actionDiff.LeftAction.Active + ", Right Active = " + actionDiff.RightAction.Active + "\r\n");
						}

						if (actionDiff.LeftAction.AllyAction != actionDiff.RightAction.AllyAction) 
						{
							txtSummary.AppendText("\tLeft Ally Action = " + actionDiff.LeftAction.AllyAction + ", Right Ally Action = " + actionDiff.RightAction.AllyAction + "\r\n");
						}

						if (actionDiff.LeftAction.AxisAction != actionDiff.RightAction.AxisAction) 
						{
							txtSummary.AppendText("\tLeft Axis Action = " + actionDiff.LeftAction.AxisAction + ", Right Axis Action = " + actionDiff.RightAction.AxisAction + "\r\n");
						}

						if (actionDiff.LeftAction.Class != actionDiff.RightAction.Class) 
						{
							txtSummary.AppendText("\tLeft Class = " + actionDiff.LeftAction.Class + ", Right Class = " + actionDiff.RightAction.Class + "\r\n");
						}

						if (actionDiff.LeftAction.CloseNode != actionDiff.RightAction.CloseNode) 
						{
							txtSummary.AppendText("\tLeft Close Node = " + actionDiff.LeftAction.CloseNode + ", Right Close Node = " + actionDiff.RightAction.CloseNode + "\r\n");
						}

						if (actionDiff.LeftAction.Entity != actionDiff.RightAction.Entity) 
						{
							txtSummary.AppendText("\tLeft Entity = " + actionDiff.LeftAction.Entity + ", Right Entity = " + actionDiff.RightAction.Entity + "\r\n");
						}

						if (actionDiff.LeftAction.Goal != actionDiff.RightAction.Goal) 
						{
							txtSummary.AppendText("\tLeft Goal = " + actionDiff.LeftAction.Goal + ", Right Goal = " + actionDiff.RightAction.Goal + "\r\n");
						}

						if (actionDiff.LeftAction.Group != actionDiff.RightAction.Group) 
						{
							txtSummary.AppendText("\tLeft Group = " + actionDiff.LeftAction.Group + ", Right Group = " + actionDiff.RightAction.Group + "\r\n");
						}

						if (actionDiff.LeftAction.Links != actionDiff.RightAction.Links) 
						{
							txtSummary.AppendText("\tLeft Links = " + actionDiff.LeftAction.Links + ", Right Links = " + actionDiff.RightAction.Links + "\r\n");
						}

						if (actionDiff.LeftAction.Prone != actionDiff.RightAction.Prone) 
						{
							txtSummary.AppendText("\tLeft Prone = " + actionDiff.LeftAction.Prone + ", Right Prone = " + actionDiff.RightAction.Prone + "\r\n");
						}

						if (actionDiff.LeftAction.Radius != actionDiff.RightAction.Radius) 
						{
							txtSummary.AppendText("\tLeft Radius = " + actionDiff.LeftAction.Radius + ", Right Radius = " + actionDiff.RightAction.Radius + "\r\n");
						}
					}
				}

				foreach (RouteDifference routeDiff in diff.DifferentRoutes) 
				{
					if (routeDiff.LeftRoute == null) 
					{
						txtSummary.AppendText("Left Navigation is missing Route " + routeDiff.RightRoute.ID + "\r\n\r\n");
					} 
					else if (routeDiff.RightRoute == null) 
					{
						txtSummary.AppendText("Right Navigation is missing Route " + routeDiff.LeftRoute.ID + "\r\n\r\n");
					} 
					else 
					{
						txtSummary.AppendText("Route " + routeDiff.LeftRoute.ID + " is different.\r\n");

						if (routeDiff.LeftRoute.Team != routeDiff.RightRoute.Team) 
						{
							txtSummary.AppendText("\tLeft Team = " + routeDiff.LeftRoute.Team + ", Right Team = " + routeDiff.RightRoute.Team + "\r\n");
						}

						if (routeDiff.LeftRoute.Radius != routeDiff.RightRoute.Radius) 
						{
							txtSummary.AppendText("\tLeft Radius = " + routeDiff.LeftRoute.Radius + ", Right Radius = " + routeDiff.RightRoute.Radius + "\r\n");
						}

						if (routeDiff.LeftRoute.Actions != routeDiff.RightRoute.Actions) 
						{
							txtSummary.AppendText("\tLeft Actions = " + routeDiff.LeftRoute.Actions + ", Right Actions = " + routeDiff.RightRoute.Actions + "\r\n");
						}
							
						if (routeDiff.LeftRoute.PathActions != routeDiff.RightRoute.PathActions) 
						{
							txtSummary.AppendText("\tLeft PathActions = " + routeDiff.LeftRoute.PathActions + ", Right PathActions = " + routeDiff.RightRoute.PathActions + "\r\n");
						}
					}
				}
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error comparing navigation files. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}
