using System;
using System.Text;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;

namespace WaypointTool
{
	public class AboutDialog : System.Windows.Forms.Form
	{
		private System.Windows.Forms.PictureBox pbLogo;
		private System.Windows.Forms.GroupBox gbVersion;
		private System.Windows.Forms.Label lblVersion;
		private System.Windows.Forms.Label lblAuthor;
		private System.Windows.Forms.Label lblCredits;
		private System.Windows.Forms.TabControl tcInfo;
		private System.Windows.Forms.TabPage tpInfo;
		private System.Windows.Forms.TabPage tpTodo;
		private System.Windows.Forms.RichTextBox txtInfo;
		private System.Windows.Forms.RichTextBox txtTodo;
		private System.Windows.Forms.TabPage tcChanges;
		private System.Windows.Forms.RichTextBox txtChanges;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public AboutDialog()
		{
			InitializeComponent();
			InitializeVersion();
			InitializeText();
		}

		private void InitializeVersion() 
		{
			int major = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.Major;
			int minor = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.Minor;
			int build = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.Build;
            //bah, not working for me...just gonna hard code it for now :( -crapshoot
			//lblVersion.Text = "Version: " + major + "." + minor + "." + build + " Beta";
            lblVersion.Text = "Version: 1.23 Beta";
		}

		private void InitializeText() 
		{
			txtInfo.Text = ReadFile("Info.txt");
			txtChanges.Text = ReadFile("Changes.txt");
			txtTodo.Text = ReadFile("Todo.txt");
		}

		private string ReadFile(string fileName) 
		{
			string path = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().CodeBase);
			fileName = path + "\\" + fileName;

			if (fileName.ToLower().StartsWith("file:\\")) 
			{
				fileName = fileName.Substring(6);
			}

			TextReader reader = null;
			try 
			{
				if (!File.Exists(fileName)) 
				{
					return fileName + " is not included in the application directory.";
				}

				reader = new StreamReader(fileName);
				return reader.ReadToEnd();
			} 
			catch (Exception ex) 
			{
				return "Unable to read file [" + fileName + "]. " + ex.Message;
			} 
			finally 
			{
				if (reader != null) 
				{
					reader.Close();
				}
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(AboutDialog));
			this.pbLogo = new System.Windows.Forms.PictureBox();
			this.gbVersion = new System.Windows.Forms.GroupBox();
			this.lblCredits = new System.Windows.Forms.Label();
			this.lblAuthor = new System.Windows.Forms.Label();
			this.lblVersion = new System.Windows.Forms.Label();
			this.tcInfo = new System.Windows.Forms.TabControl();
			this.tpInfo = new System.Windows.Forms.TabPage();
			this.txtInfo = new System.Windows.Forms.RichTextBox();
			this.tcChanges = new System.Windows.Forms.TabPage();
			this.txtChanges = new System.Windows.Forms.RichTextBox();
			this.tpTodo = new System.Windows.Forms.TabPage();
			this.txtTodo = new System.Windows.Forms.RichTextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.gbVersion.SuspendLayout();
			this.tcInfo.SuspendLayout();
			this.tpInfo.SuspendLayout();
			this.tcChanges.SuspendLayout();
			this.tpTodo.SuspendLayout();
			this.SuspendLayout();
			// 
			// pbLogo
			// 
			this.pbLogo.Dock = System.Windows.Forms.DockStyle.Top;
			this.pbLogo.Image = ((System.Drawing.Image)(resources.GetObject("pbLogo.Image")));
			this.pbLogo.Location = new System.Drawing.Point(0, 0);
			this.pbLogo.Name = "pbLogo";
			this.pbLogo.Size = new System.Drawing.Size(698, 120);
			this.pbLogo.TabIndex = 0;
			this.pbLogo.TabStop = false;
			// 
			// gbVersion
			// 
			this.gbVersion.Controls.Add(this.label2);
			this.gbVersion.Controls.Add(this.label1);
			this.gbVersion.Controls.Add(this.lblCredits);
			this.gbVersion.Controls.Add(this.lblAuthor);
			this.gbVersion.Controls.Add(this.lblVersion);
			this.gbVersion.Dock = System.Windows.Forms.DockStyle.Left;
			this.gbVersion.Location = new System.Drawing.Point(0, 120);
			this.gbVersion.Name = "gbVersion";
			this.gbVersion.Size = new System.Drawing.Size(264, 247);
			this.gbVersion.TabIndex = 1;
			this.gbVersion.TabStop = false;
			this.gbVersion.Text = "Version";
			// 
			// lblCredits
			// 
			this.lblCredits.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblCredits.Location = new System.Drawing.Point(8, 96);
			this.lblCredits.Name = "lblCredits";
			this.lblCredits.Size = new System.Drawing.Size(240, 16);
			this.lblCredits.TabIndex = 2;
			this.lblCredits.Text = "Crapshoot - Guidance / Testing";
			// 
			// lblAuthor
			// 
			this.lblAuthor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblAuthor.Location = new System.Drawing.Point(8, 48);
			this.lblAuthor.Name = "lblAuthor";
			this.lblAuthor.Size = new System.Drawing.Size(232, 16);
			this.lblAuthor.TabIndex = 1;
			this.lblAuthor.Text = "Author: Hobbit (bill.nickel@gmail.com)";
			// 
			// lblVersion
			// 
			this.lblVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblVersion.Location = new System.Drawing.Point(8, 24);
			this.lblVersion.Name = "lblVersion";
			this.lblVersion.Size = new System.Drawing.Size(232, 16);
			this.lblVersion.TabIndex = 0;
			this.lblVersion.Text = "Version: 1.0 Beta";
			// 
			// tcInfo
			// 
			this.tcInfo.Appearance = System.Windows.Forms.TabAppearance.FlatButtons;
			this.tcInfo.Controls.Add(this.tpInfo);
			this.tcInfo.Controls.Add(this.tcChanges);
			this.tcInfo.Controls.Add(this.tpTodo);
			this.tcInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tcInfo.Location = new System.Drawing.Point(264, 120);
			this.tcInfo.Name = "tcInfo";
			this.tcInfo.SelectedIndex = 0;
			this.tcInfo.Size = new System.Drawing.Size(434, 247);
			this.tcInfo.TabIndex = 3;
			// 
			// tpInfo
			// 
			this.tpInfo.Controls.Add(this.txtInfo);
			this.tpInfo.Location = new System.Drawing.Point(4, 25);
			this.tpInfo.Name = "tpInfo";
			this.tpInfo.Size = new System.Drawing.Size(426, 218);
			this.tpInfo.TabIndex = 0;
			this.tpInfo.Text = "Info";
			// 
			// txtInfo
			// 
			this.txtInfo.BackColor = System.Drawing.Color.WhiteSmoke;
			this.txtInfo.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.txtInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtInfo.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.txtInfo.Location = new System.Drawing.Point(0, 0);
			this.txtInfo.Name = "txtInfo";
			this.txtInfo.ReadOnly = true;
			this.txtInfo.Size = new System.Drawing.Size(426, 218);
			this.txtInfo.TabIndex = 3;
			this.txtInfo.Text = "";
			// 
			// tcChanges
			// 
			this.tcChanges.Controls.Add(this.txtChanges);
			this.tcChanges.Location = new System.Drawing.Point(4, 25);
			this.tcChanges.Name = "tcChanges";
			this.tcChanges.Size = new System.Drawing.Size(442, 218);
			this.tcChanges.TabIndex = 2;
			this.tcChanges.Text = "Changes";
			// 
			// txtChanges
			// 
			this.txtChanges.BackColor = System.Drawing.Color.WhiteSmoke;
			this.txtChanges.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.txtChanges.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtChanges.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.txtChanges.Location = new System.Drawing.Point(0, 0);
			this.txtChanges.Name = "txtChanges";
			this.txtChanges.ReadOnly = true;
			this.txtChanges.Size = new System.Drawing.Size(442, 218);
			this.txtChanges.TabIndex = 5;
			this.txtChanges.Text = "";
			// 
			// tpTodo
			// 
			this.tpTodo.Controls.Add(this.txtTodo);
			this.tpTodo.Location = new System.Drawing.Point(4, 25);
			this.tpTodo.Name = "tpTodo";
			this.tpTodo.Size = new System.Drawing.Size(442, 218);
			this.tpTodo.TabIndex = 1;
			this.tpTodo.Text = "To do";
			// 
			// txtTodo
			// 
			this.txtTodo.BackColor = System.Drawing.Color.WhiteSmoke;
			this.txtTodo.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.txtTodo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtTodo.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.txtTodo.Location = new System.Drawing.Point(0, 0);
			this.txtTodo.Name = "txtTodo";
			this.txtTodo.ReadOnly = true;
			this.txtTodo.Size = new System.Drawing.Size(442, 218);
			this.txtTodo.TabIndex = 4;
			this.txtTodo.Text = "";
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label1.Location = new System.Drawing.Point(8, 120);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(240, 16);
			this.label1.TabIndex = 3;
			this.label1.Text = "Maleficus - Guidance / Creating Fritz";
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label2.Location = new System.Drawing.Point(8, 72);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(240, 16);
			this.label2.TabIndex = 4;
			this.label2.Text = "Credits:";
			// 
			// AboutDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(698, 367);
			this.Controls.Add(this.tcInfo);
			this.Controls.Add(this.gbVersion);
			this.Controls.Add(this.pbLogo);
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AboutDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "About";
			this.gbVersion.ResumeLayout(false);
			this.tcInfo.ResumeLayout(false);
			this.tpInfo.ResumeLayout(false);
			this.tcChanges.ResumeLayout(false);
			this.tpTodo.ResumeLayout(false);
			this.ResumeLayout(false);

		}
	}
}
