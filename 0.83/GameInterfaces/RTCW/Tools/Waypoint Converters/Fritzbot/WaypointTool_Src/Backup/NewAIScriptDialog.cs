using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Text;

namespace WaypointTool
{
	public class NewAIScriptDialog : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox gbMain;
		private System.Windows.Forms.Label lblCreatedBy;
		private System.Windows.Forms.Label lblBotSightDistance;
		private System.Windows.Forms.TextBox txtCreatedBy;
		private System.Windows.Forms.TextBox txtBotSightDistance;
		private System.Windows.Forms.CheckBox chkSpawnFlag;
		private System.Windows.Forms.CheckBox chkCommandPost;
		private System.Windows.Forms.CheckBox chkConstruct;
		private System.Windows.Forms.CheckBox chkVehicle;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Label lblMapName;
		private System.Windows.Forms.TextBox txtMapName;

		private System.ComponentModel.Container components = null;

		private Actions _actions = null;

		public NewAIScriptDialog()
		{
			InitializeComponent();
		}

		public Actions Actions 
		{
			set { _actions = value; }
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
			this.gbMain = new System.Windows.Forms.GroupBox();
			this.txtMapName = new System.Windows.Forms.TextBox();
			this.lblMapName = new System.Windows.Forms.Label();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnOK = new System.Windows.Forms.Button();
			this.chkVehicle = new System.Windows.Forms.CheckBox();
			this.chkConstruct = new System.Windows.Forms.CheckBox();
			this.chkCommandPost = new System.Windows.Forms.CheckBox();
			this.chkSpawnFlag = new System.Windows.Forms.CheckBox();
			this.txtBotSightDistance = new System.Windows.Forms.TextBox();
			this.txtCreatedBy = new System.Windows.Forms.TextBox();
			this.lblBotSightDistance = new System.Windows.Forms.Label();
			this.lblCreatedBy = new System.Windows.Forms.Label();
			this.gbMain.SuspendLayout();
			this.SuspendLayout();
			// 
			// gbMain
			// 
			this.gbMain.Controls.Add(this.txtMapName);
			this.gbMain.Controls.Add(this.lblMapName);
			this.gbMain.Controls.Add(this.btnCancel);
			this.gbMain.Controls.Add(this.btnOK);
			this.gbMain.Controls.Add(this.chkVehicle);
			this.gbMain.Controls.Add(this.chkConstruct);
			this.gbMain.Controls.Add(this.chkCommandPost);
			this.gbMain.Controls.Add(this.chkSpawnFlag);
			this.gbMain.Controls.Add(this.txtBotSightDistance);
			this.gbMain.Controls.Add(this.txtCreatedBy);
			this.gbMain.Controls.Add(this.lblBotSightDistance);
			this.gbMain.Controls.Add(this.lblCreatedBy);
			this.gbMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbMain.Location = new System.Drawing.Point(0, 0);
			this.gbMain.Name = "gbMain";
			this.gbMain.Size = new System.Drawing.Size(330, 207);
			this.gbMain.TabIndex = 0;
			this.gbMain.TabStop = false;
			// 
			// txtMapName
			// 
			this.txtMapName.Location = new System.Drawing.Point(120, 40);
			this.txtMapName.Name = "txtMapName";
			this.txtMapName.Size = new System.Drawing.Size(200, 20);
			this.txtMapName.TabIndex = 2;
			this.txtMapName.Text = "";
			// 
			// lblMapName
			// 
			this.lblMapName.Location = new System.Drawing.Point(8, 40);
			this.lblMapName.Name = "lblMapName";
			this.lblMapName.Size = new System.Drawing.Size(104, 16);
			this.lblMapName.TabIndex = 10;
			this.lblMapName.Text = "Map Name";
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(184, 184);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(56, 20);
			this.btnCancel.TabIndex = 9;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(120, 184);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(56, 20);
			this.btnOK.TabIndex = 8;
			this.btnOK.Text = "OK";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// chkVehicle
			// 
			this.chkVehicle.Location = new System.Drawing.Point(120, 160);
			this.chkVehicle.Name = "chkVehicle";
			this.chkVehicle.Size = new System.Drawing.Size(200, 16);
			this.chkVehicle.TabIndex = 7;
			this.chkVehicle.Text = "Map Has Vehicle";
			// 
			// chkConstruct
			// 
			this.chkConstruct.Location = new System.Drawing.Point(120, 136);
			this.chkConstruct.Name = "chkConstruct";
			this.chkConstruct.Size = new System.Drawing.Size(200, 16);
			this.chkConstruct.TabIndex = 6;
			this.chkConstruct.Text = "Construction Is Priority";
			// 
			// chkCommandPost
			// 
			this.chkCommandPost.Location = new System.Drawing.Point(120, 112);
			this.chkCommandPost.Name = "chkCommandPost";
			this.chkCommandPost.Size = new System.Drawing.Size(200, 16);
			this.chkCommandPost.TabIndex = 5;
			this.chkCommandPost.Text = "Command Post Is Priority";
			// 
			// chkSpawnFlag
			// 
			this.chkSpawnFlag.Location = new System.Drawing.Point(120, 88);
			this.chkSpawnFlag.Name = "chkSpawnFlag";
			this.chkSpawnFlag.Size = new System.Drawing.Size(200, 16);
			this.chkSpawnFlag.TabIndex = 4;
			this.chkSpawnFlag.Text = "Spawn Flag Is Priority";
			// 
			// txtBotSightDistance
			// 
			this.txtBotSightDistance.Location = new System.Drawing.Point(120, 64);
			this.txtBotSightDistance.Name = "txtBotSightDistance";
			this.txtBotSightDistance.Size = new System.Drawing.Size(40, 20);
			this.txtBotSightDistance.TabIndex = 3;
			this.txtBotSightDistance.Text = "1500";
			// 
			// txtCreatedBy
			// 
			this.txtCreatedBy.Location = new System.Drawing.Point(120, 16);
			this.txtCreatedBy.Name = "txtCreatedBy";
			this.txtCreatedBy.Size = new System.Drawing.Size(200, 20);
			this.txtCreatedBy.TabIndex = 1;
			this.txtCreatedBy.Text = "";
			// 
			// lblBotSightDistance
			// 
			this.lblBotSightDistance.Location = new System.Drawing.Point(8, 64);
			this.lblBotSightDistance.Name = "lblBotSightDistance";
			this.lblBotSightDistance.Size = new System.Drawing.Size(104, 16);
			this.lblBotSightDistance.TabIndex = 1;
			this.lblBotSightDistance.Text = "Bot Sight Distance";
			// 
			// lblCreatedBy
			// 
			this.lblCreatedBy.Location = new System.Drawing.Point(8, 16);
			this.lblCreatedBy.Name = "lblCreatedBy";
			this.lblCreatedBy.Size = new System.Drawing.Size(104, 16);
			this.lblCreatedBy.TabIndex = 0;
			this.lblCreatedBy.Text = "Created By";
			// 
			// NewAIScriptDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(330, 207);
			this.Controls.Add(this.gbMain);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "NewAIScriptDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "New AIScript";
			this.gbMain.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private bool _apply = false;
		private string _dialogResult = "";

		public bool Apply 
		{
			get { return _apply; }
		}

		public string CodeResult
		{
			get { return _dialogResult; }
		}

		private void btnOK_Click(object sender, System.EventArgs e)
		{
			StringBuilder buff = new StringBuilder();

			buff.Append("// AI Script for " + txtMapName.Text +  "\r\n");
			buff.Append("// Created by: " + txtCreatedBy.Text + "\r\n");
			buff.Append("// Last Modified: " + DateTime.Now.ToShortDateString() + "\r\n\r\n");

			buff.Append("bot_SightDist " + txtBotSightDistance.Text + " //how far the highest skill bot can see on this map\r\n\r\n");

			buff.Append("spawnflag_is_priority  " + (chkSpawnFlag.Checked ? "1" : "0") + " //should bots focus on spawnflags?\r\n\r\n");

			buff.Append("cmdpost_is_priority  " + (chkCommandPost.Checked ? "1" : "0") + " //are cmdposts critical on this map?\r\n\r\n");

			buff.Append("construct_is_priority  " + (chkConstruct.Checked ? "1" : "0") + " //should engs focus more on constructibles\r\n\r\n");

			buff.Append("map_has_vehicle  " + (chkVehicle.Checked ? "1" : "0") + " //does this map have a tank or a train?\r\n\r\n");

			if (_actions != null) 
			{
				foreach (Actions.ActionRow actionRow in _actions.Action.Rows) 
				{
					buff.Append("action ");
					buff.Append(actionRow.ID);
					buff.Append("\r\n{\r\n\r\n\r\n}\r\n\r\n");
				}
			}

			buff.Append("#EOF\r\n");
			_dialogResult = buff.ToString();
			_apply = true;
			this.Close();
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			_apply = false;
			this.Close();
		}
	}
}
