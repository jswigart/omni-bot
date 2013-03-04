using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.IO;
using System.Text;

namespace WaypointTool
{
	public class BotsControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.GroupBox gbBotsDirectory;
		private System.Windows.Forms.Button btnOpenDirectory;
		private System.Windows.Forms.Button btnImport;
		private System.Windows.Forms.Label lblFileLocation;
		private System.Windows.Forms.GroupBox gbBuiltInBots;
		private System.Windows.Forms.Panel pnlBuiltInBots;
		private System.Windows.Forms.FolderBrowserDialog dirOpen;
		private System.Windows.Forms.ImageList imgIcons;
		private TreeViewDragDrop tvBots;
		private System.Windows.Forms.TextBox txtBotDirectory;
		private System.Windows.Forms.GroupBox gbSave;
		private System.Windows.Forms.GroupBox gbBotsForFile;
		private System.Windows.Forms.Panel pnlNewBots;
		private TreeViewDragDrop tvNewBots;
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.Label lblNote;
		private System.Windows.Forms.SaveFileDialog saveFile;
		private System.Windows.Forms.ContextMenu mnuBots;
		private System.Windows.Forms.MenuItem mnuClearAll;
		private System.ComponentModel.IContainer components;

		public BotsControl()
		{
			InitializeComponent();
			InitializeBots();
			InitializeNewBots();

			tvBots.AllowDrop = false;
			tvNewBots.DragStart += new DragItemEventHandler(tvNewBots_DragStart);
			tvNewBots.DragComplete += new DragCompleteEventHandler(tvNewBots_DragComplete);
			tvNewBots.DoubleClick +=new EventHandler(tvNewBots_DoubleClick);
			tvBots.DragStart += new DragItemEventHandler(tvBots_DragStart);
		}

		private TreeNode _axisSoldier = new TreeNode("Soldier", 12, 12);
		private TreeNode _alliesSoldier = new TreeNode("Soldier", 12, 12);

		private TreeNode _axisMedic = new TreeNode("Medic", 13, 13);
		private TreeNode _alliesMedic = new TreeNode("Medic", 13, 13);

		private TreeNode _axisEngineer = new TreeNode("Engineer", 14, 14);
		private TreeNode _alliesEngineer = new TreeNode("Engineer", 14, 14);

		private TreeNode _axisFieldOps = new TreeNode("Field Ops", 15, 15);
		private TreeNode _alliesFieldOps = new TreeNode("Field Ops", 15, 15);

		private TreeNode _axisCovertOps = new TreeNode("Covert Ops", 16, 16);
		private TreeNode _alliesCovertOps = new TreeNode("Covert Ops", 16, 16);

		private void InitializeNewBots() 
		{
			tvNewBots.Nodes.Clear();

			TreeNode axis = new TreeNode("Axis", 17, 17);
			TreeNode allies = new TreeNode("Allies", 18, 18);


			tvNewBots.Nodes.Add(axis);
			tvNewBots.Nodes.Add(allies);

			axis.Nodes.Add(_axisCovertOps);
			axis.Nodes.Add(_axisEngineer);
			axis.Nodes.Add(_axisFieldOps);
			axis.Nodes.Add(_axisMedic);
			axis.Nodes.Add(_axisSoldier);

			allies.Nodes.Add(_alliesCovertOps);
			allies.Nodes.Add(_alliesEngineer);
			allies.Nodes.Add(_alliesFieldOps);
			allies.Nodes.Add(_alliesMedic);
			allies.Nodes.Add(_alliesSoldier);
		}

		private void InitializeBots() 
		{
			try 
			{
				tvBots.Nodes.Clear();

				TreeNode axis = new TreeNode("Axis", 17, 17);
				TreeNode allies = new TreeNode("Allies", 18, 18);

				TreeNode axisSoldier = new TreeNode("Soldier", 12, 12);
				TreeNode alliesSoldier = new TreeNode("Soldier", 12, 12);

				TreeNode axisMedic = new TreeNode("Medic", 13, 13);
				TreeNode alliesMedic = new TreeNode("Medic", 13, 13);

				TreeNode axisEngineer = new TreeNode("Engineer", 14, 14);
				TreeNode alliesEngineer = new TreeNode("Engineer", 14, 14);

				TreeNode axisFieldOps = new TreeNode("Field Ops", 15, 15);
				TreeNode alliesFieldOps = new TreeNode("Field Ops", 15, 15);

				TreeNode axisCovertOps = new TreeNode("Covert Ops", 16, 16);
				TreeNode alliesCovertOps = new TreeNode("Covert Ops", 16, 16);


				tvBots.Nodes.Add(axis);
				tvBots.Nodes.Add(allies);

				axis.Nodes.Add(axisCovertOps);
				axis.Nodes.Add(axisEngineer);
				axis.Nodes.Add(axisFieldOps);
				axis.Nodes.Add(axisMedic);
				axis.Nodes.Add(axisSoldier);

				allies.Nodes.Add(alliesCovertOps);
				allies.Nodes.Add(alliesEngineer);
				allies.Nodes.Add(alliesFieldOps);
				allies.Nodes.Add(alliesMedic);
				allies.Nodes.Add(alliesSoldier);

				string dataFile = GetBotsDataFile();

				if (!File.Exists(dataFile)) 
				{
					MessageBox.Show(this.ParentForm, "Please import a bots data file.", "Import Needed", MessageBoxButtons.OK, MessageBoxIcon.Information);
					return;
				}

				Bots bots = new Bots();
				bots.ReadXml(dataFile);

				foreach (Bots.BotRow botRow in bots.Bot) 
				{
					try 
					{
						string name = botRow.Name;
						string funname = botRow.FunName;
						int classIndex = Convert.ToInt32(botRow.Class);
						int teamIndex = Convert.ToInt32(botRow.Team);
						int weaponIndex = Convert.ToInt32(botRow.Weapon);

						TreeNode bot = new TreeNode(name, weaponIndex, weaponIndex);
						bot.Tag = botRow; // used for drag / drop
						TreeNode botFunName = new TreeNode(funname); // empty icon
						TreeNode botWeapon = GetWeaponNode(weaponIndex);
						bot.Nodes.Add(botFunName);
						bot.Nodes.Add(botWeapon);

						switch (classIndex) 
						{
							case 0: // soldier
								botFunName.ImageIndex = 12;
								botFunName.SelectedImageIndex = 12;
								if (teamIndex == 0) 
								{
									axisSoldier.Nodes.Add(bot);
								} 
								else 
								{
									alliesSoldier.Nodes.Add(bot);
								}
								break;
							case 1: // medic
								botFunName.ImageIndex = 13;
								botFunName.SelectedImageIndex = 13;
								if (teamIndex == 0) 
								{
									axisMedic.Nodes.Add(bot);
								} 
								else 
								{
									alliesMedic.Nodes.Add(bot);
								}
								break;
							case 2: // engineer
								botFunName.ImageIndex = 14;
								botFunName.SelectedImageIndex = 14;
								if (teamIndex == 0) 
								{
									axisEngineer.Nodes.Add(bot);
								} 
								else 
								{
									alliesEngineer.Nodes.Add(bot);
								}
								break;
							case 3: // field ops
								botFunName.ImageIndex = 15;
								botFunName.SelectedImageIndex = 15;
								if (teamIndex == 0) 
								{
									axisFieldOps.Nodes.Add(bot);
								} 
								else 
								{
									alliesFieldOps.Nodes.Add(bot);
								}
								break;
							case 4: // covert ops
								botFunName.ImageIndex = 16;
								botFunName.SelectedImageIndex = 16;
								if (teamIndex == 0) 
								{
									axisCovertOps.Nodes.Add(bot);
								} 
								else 
								{
									alliesCovertOps.Nodes.Add(bot);
								}
								break;
						}
					} 
					catch 
					{
						// ignore bogus bots
					}
				}
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Unable to initialize bots. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private TreeNode GetWeaponNode(int weaponIndex) 
		{
			string weaponName = "";

			switch (weaponIndex) 
			{
				case 0:
					weaponName = "mp40";
					break;
				case 1:
					weaponName = "thompson";
					break;
				case 2:
					weaponName = "sten";
					break;
				case 3:
					weaponName = "k43";
					break;
				case 4:
					weaponName = "silenced k43";
					break;
				case 5:
					weaponName = "silenced garand";
					break;
				case 6:
					weaponName = "fg42";
					break;
				case 7:
					weaponName = "garand";
					break;
				case 8:
					weaponName = "mortar";
					break;
				case 9:
					weaponName = "panzer";
					break;
				case 10:
					weaponName = "mobile mg42";
					break;
				case 11:
					weaponName = "flame thrower";
					break;
			}

			return new TreeNode(weaponName, weaponIndex, weaponIndex);
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(BotsControl));
			this.imgIcons = new System.Windows.Forms.ImageList(this.components);
			this.gbBotsDirectory = new System.Windows.Forms.GroupBox();
			this.btnOpenDirectory = new System.Windows.Forms.Button();
			this.btnImport = new System.Windows.Forms.Button();
			this.txtBotDirectory = new System.Windows.Forms.TextBox();
			this.lblFileLocation = new System.Windows.Forms.Label();
			this.gbBuiltInBots = new System.Windows.Forms.GroupBox();
			this.pnlBuiltInBots = new System.Windows.Forms.Panel();
			this.tvBots = new WaypointTool.TreeViewDragDrop();
			this.dirOpen = new System.Windows.Forms.FolderBrowserDialog();
			this.gbSave = new System.Windows.Forms.GroupBox();
			this.lblNote = new System.Windows.Forms.Label();
			this.btnSave = new System.Windows.Forms.Button();
			this.gbBotsForFile = new System.Windows.Forms.GroupBox();
			this.pnlNewBots = new System.Windows.Forms.Panel();
			this.tvNewBots = new WaypointTool.TreeViewDragDrop();
			this.mnuBots = new System.Windows.Forms.ContextMenu();
			this.mnuClearAll = new System.Windows.Forms.MenuItem();
			this.saveFile = new System.Windows.Forms.SaveFileDialog();
			this.gbBotsDirectory.SuspendLayout();
			this.gbBuiltInBots.SuspendLayout();
			this.pnlBuiltInBots.SuspendLayout();
			this.gbSave.SuspendLayout();
			this.gbBotsForFile.SuspendLayout();
			this.pnlNewBots.SuspendLayout();
			this.SuspendLayout();
			// 
			// imgIcons
			// 
			this.imgIcons.ImageSize = new System.Drawing.Size(16, 16);
			this.imgIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imgIcons.ImageStream")));
			this.imgIcons.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// gbBotsDirectory
			// 
			this.gbBotsDirectory.BackColor = System.Drawing.SystemColors.ControlLight;
			this.gbBotsDirectory.Controls.Add(this.btnOpenDirectory);
			this.gbBotsDirectory.Controls.Add(this.btnImport);
			this.gbBotsDirectory.Controls.Add(this.txtBotDirectory);
			this.gbBotsDirectory.Controls.Add(this.lblFileLocation);
			this.gbBotsDirectory.Dock = System.Windows.Forms.DockStyle.Top;
			this.gbBotsDirectory.Location = new System.Drawing.Point(0, 0);
			this.gbBotsDirectory.Name = "gbBotsDirectory";
			this.gbBotsDirectory.Size = new System.Drawing.Size(624, 80);
			this.gbBotsDirectory.TabIndex = 5;
			this.gbBotsDirectory.TabStop = false;
			this.gbBotsDirectory.Text = "Bots Directory";
			// 
			// btnOpenDirectory
			// 
			this.btnOpenDirectory.Location = new System.Drawing.Point(424, 24);
			this.btnOpenDirectory.Name = "btnOpenDirectory";
			this.btnOpenDirectory.Size = new System.Drawing.Size(32, 20);
			this.btnOpenDirectory.TabIndex = 6;
			this.btnOpenDirectory.Text = "...";
			this.btnOpenDirectory.Click += new System.EventHandler(this.btnOpenDirectory_Click);
			// 
			// btnImport
			// 
			this.btnImport.Location = new System.Drawing.Point(96, 48);
			this.btnImport.Name = "btnImport";
			this.btnImport.Size = new System.Drawing.Size(88, 20);
			this.btnImport.TabIndex = 5;
			this.btnImport.Text = "Import";
			this.btnImport.Click += new System.EventHandler(this.btnImport_Click);
			// 
			// txtBotDirectory
			// 
			this.txtBotDirectory.BackColor = System.Drawing.SystemColors.Window;
			this.txtBotDirectory.Location = new System.Drawing.Point(96, 24);
			this.txtBotDirectory.Name = "txtBotDirectory";
			this.txtBotDirectory.Size = new System.Drawing.Size(320, 20);
			this.txtBotDirectory.TabIndex = 2;
			this.txtBotDirectory.Text = "";
			// 
			// lblFileLocation
			// 
			this.lblFileLocation.Location = new System.Drawing.Point(8, 24);
			this.lblFileLocation.Name = "lblFileLocation";
			this.lblFileLocation.Size = new System.Drawing.Size(96, 16);
			this.lblFileLocation.TabIndex = 0;
			this.lblFileLocation.Text = "~Fritzbot/bots";
			this.lblFileLocation.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
			// 
			// gbBuiltInBots
			// 
			this.gbBuiltInBots.BackColor = System.Drawing.SystemColors.ControlLight;
			this.gbBuiltInBots.Controls.Add(this.pnlBuiltInBots);
			this.gbBuiltInBots.Dock = System.Windows.Forms.DockStyle.Left;
			this.gbBuiltInBots.Location = new System.Drawing.Point(0, 80);
			this.gbBuiltInBots.Name = "gbBuiltInBots";
			this.gbBuiltInBots.Size = new System.Drawing.Size(272, 384);
			this.gbBuiltInBots.TabIndex = 6;
			this.gbBuiltInBots.TabStop = false;
			this.gbBuiltInBots.Text = "Built In Bots";
			// 
			// pnlBuiltInBots
			// 
			this.pnlBuiltInBots.Controls.Add(this.tvBots);
			this.pnlBuiltInBots.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pnlBuiltInBots.DockPadding.All = 4;
			this.pnlBuiltInBots.Location = new System.Drawing.Point(3, 16);
			this.pnlBuiltInBots.Name = "pnlBuiltInBots";
			this.pnlBuiltInBots.Size = new System.Drawing.Size(266, 365);
			this.pnlBuiltInBots.TabIndex = 0;
			// 
			// tvBots
			// 
			this.tvBots.AllowDrop = true;
			this.tvBots.BackColor = System.Drawing.SystemColors.Window;
			this.tvBots.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.tvBots.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tvBots.DragCursor = null;
			this.tvBots.DragCursorType = WaypointTool.DragCursorType.None;
			this.tvBots.DragImageIndex = 0;
			this.tvBots.DragMode = System.Windows.Forms.DragDropEffects.Move;
			this.tvBots.DragNodeFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.tvBots.DragNodeOpacity = 0.3;
			this.tvBots.DragOverNodeBackColor = System.Drawing.SystemColors.Highlight;
			this.tvBots.DragOverNodeForeColor = System.Drawing.SystemColors.HighlightText;
			this.tvBots.ImageList = this.imgIcons;
			this.tvBots.Location = new System.Drawing.Point(4, 4);
			this.tvBots.Name = "tvBots";
			this.tvBots.Size = new System.Drawing.Size(258, 357);
			this.tvBots.TabIndex = 0;
			// 
			// gbSave
			// 
			this.gbSave.BackColor = System.Drawing.SystemColors.ControlLight;
			this.gbSave.Controls.Add(this.lblNote);
			this.gbSave.Controls.Add(this.btnSave);
			this.gbSave.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.gbSave.Location = new System.Drawing.Point(272, 416);
			this.gbSave.Name = "gbSave";
			this.gbSave.Size = new System.Drawing.Size(352, 48);
			this.gbSave.TabIndex = 7;
			this.gbSave.TabStop = false;
			// 
			// lblNote
			// 
			this.lblNote.Location = new System.Drawing.Point(104, 16);
			this.lblNote.Name = "lblNote";
			this.lblNote.Size = new System.Drawing.Size(192, 16);
			this.lblNote.TabIndex = 1;
			this.lblNote.Text = "(Double-Click added bots to remove.)";
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(8, 16);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(88, 20);
			this.btnSave.TabIndex = 0;
			this.btnSave.Text = "Save";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// gbBotsForFile
			// 
			this.gbBotsForFile.BackColor = System.Drawing.SystemColors.ControlLight;
			this.gbBotsForFile.Controls.Add(this.pnlNewBots);
			this.gbBotsForFile.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbBotsForFile.Location = new System.Drawing.Point(272, 80);
			this.gbBotsForFile.Name = "gbBotsForFile";
			this.gbBotsForFile.Size = new System.Drawing.Size(352, 336);
			this.gbBotsForFile.TabIndex = 8;
			this.gbBotsForFile.TabStop = false;
			this.gbBotsForFile.Text = "Bots to add in .bots file (drag from built in)";
			// 
			// pnlNewBots
			// 
			this.pnlNewBots.Controls.Add(this.tvNewBots);
			this.pnlNewBots.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pnlNewBots.DockPadding.All = 4;
			this.pnlNewBots.Location = new System.Drawing.Point(3, 16);
			this.pnlNewBots.Name = "pnlNewBots";
			this.pnlNewBots.Size = new System.Drawing.Size(346, 317);
			this.pnlNewBots.TabIndex = 0;
			// 
			// tvNewBots
			// 
			this.tvNewBots.AllowDrop = true;
			this.tvNewBots.BackColor = System.Drawing.SystemColors.Window;
			this.tvNewBots.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.tvNewBots.ContextMenu = this.mnuBots;
			this.tvNewBots.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tvNewBots.DragCursor = null;
			this.tvNewBots.DragCursorType = WaypointTool.DragCursorType.None;
			this.tvNewBots.DragImageIndex = 0;
			this.tvNewBots.DragMode = System.Windows.Forms.DragDropEffects.Move;
			this.tvNewBots.DragNodeFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.tvNewBots.DragNodeOpacity = 0.3;
			this.tvNewBots.DragOverNodeBackColor = System.Drawing.SystemColors.Highlight;
			this.tvNewBots.DragOverNodeForeColor = System.Drawing.SystemColors.HighlightText;
			this.tvNewBots.ImageList = this.imgIcons;
			this.tvNewBots.Location = new System.Drawing.Point(4, 4);
			this.tvNewBots.Name = "tvNewBots";
			this.tvNewBots.Size = new System.Drawing.Size(338, 309);
			this.tvNewBots.TabIndex = 1;
			this.tvNewBots.Tag = "New Bots";
			// 
			// mnuBots
			// 
			this.mnuBots.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuClearAll});
			// 
			// mnuClearAll
			// 
			this.mnuClearAll.Index = 0;
			this.mnuClearAll.Text = "Clear All";
			this.mnuClearAll.Click += new System.EventHandler(this.mnuClearAll_Click);
			// 
			// saveFile
			// 
			this.saveFile.FileOk += new System.ComponentModel.CancelEventHandler(this.saveFile_FileOk);
			// 
			// BotsControl
			// 
			this.Controls.Add(this.gbBotsForFile);
			this.Controls.Add(this.gbSave);
			this.Controls.Add(this.gbBuiltInBots);
			this.Controls.Add(this.gbBotsDirectory);
			this.Name = "BotsControl";
			this.Size = new System.Drawing.Size(624, 464);
			this.gbBotsDirectory.ResumeLayout(false);
			this.gbBuiltInBots.ResumeLayout(false);
			this.pnlBuiltInBots.ResumeLayout(false);
			this.gbSave.ResumeLayout(false);
			this.gbBotsForFile.ResumeLayout(false);
			this.pnlNewBots.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private string GetWorkingDirectory() 
		{
			string path = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().CodeBase);

			if (path.ToLower().StartsWith("file:\\")) 
			{
				path = path.Substring(6);
			}

			return path;
		}

		private string GetBotsDataFile() 
		{
			return GetWorkingDirectory() + Path.DirectorySeparatorChar + "bots.data";
		}

		private void btnImport_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (txtBotDirectory.Text.Trim().Length == 0) 
				{
					MessageBox.Show(this.ParentForm, "Please choose the bots directory.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				Bots bots = BotReader.ReadBots(txtBotDirectory.Text);

				if (bots.Bot.Count == 0) 
				{
					MessageBox.Show(this.ParentForm, "No bots found in directory.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				string dataFile = GetBotsDataFile();

				if (File.Exists(dataFile)) 
				{
					File.Delete(dataFile);
				}

				bots.WriteXml(dataFile);

				MessageBox.Show(this.ParentForm, "Imported " + bots.Bot.Count + " bots.", "Imported", MessageBoxButtons.OK, MessageBoxIcon.Information);

				InitializeBots();
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Unable to import bots. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void btnOpenDirectory_Click(object sender, System.EventArgs e)
		{
			try 
			{
				dirOpen.SelectedPath = GetWorkingDirectory();
				dirOpen.ShowDialog(this.ParentForm);
				txtBotDirectory.Text = dirOpen.SelectedPath;
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Unable to browse for bots directory. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void tvNewBots_DragStart(object sender, DragItemEventArgs e)
		{
			tvNewBots.AllowDrop = false;
			tvBots.AllowDrop = false;
		}

		private void tvBots_DragStart(object sender, DragItemEventArgs e)
		{
			if (e.Node.Tag == null) 
			{
				tvNewBots.AllowDrop = false;
			} 
			else 
			{
				tvNewBots.AllowDrop = true;
			}
		}

		private void tvNewBots_DragComplete(object sender, DragCompleteEventArgs e)
		{
			DoDrop(e.SourceNode);
		}

		// prevents adding duplicate child nodes to the parent
		private void AddBotNode(TreeNode parent, TreeNode bot) 
		{
			Bots.BotRow childBot = bot.Tag as Bots.BotRow;
			if (childBot == null) return;

			foreach (TreeNode child in parent.Nodes) 
			{
				Bots.BotRow existingChildBot = child.Tag as Bots.BotRow;

				if (existingChildBot != null) 
				{
					// dupe found?
					if (existingChildBot.Name == childBot.Name) 
					{
						// expand the parent for visual indication that the
						// bot is there
						parent.Parent.Expand();
						parent.Expand();
						return;
					}
				}
			}

			parent.Nodes.Add(bot);
			parent.Parent.Expand();
			parent.Expand();
		}

		private void DoDrop(TreeNode node) 
		{
			Bots.BotRow botRow = node.Tag as Bots.BotRow;

			if (botRow == null) return;
			try 
			{
				string name = botRow.Name;
				string funname = botRow.FunName;
				int classIndex = Convert.ToInt32(botRow.Class);
				int teamIndex = Convert.ToInt32(botRow.Team);
				int weaponIndex = Convert.ToInt32(botRow.Weapon);

				TreeNode bot = new TreeNode(name, weaponIndex, weaponIndex);
				bot.Tag = botRow; // used for file creation
				TreeNode botFunName = new TreeNode(funname); // empty icon
				TreeNode botWeapon = GetWeaponNode(weaponIndex);
				bot.Nodes.Add(botFunName);
				bot.Nodes.Add(botWeapon);

				switch (classIndex) 
				{
					case 0: // soldier
						botFunName.ImageIndex = 12;
						botFunName.SelectedImageIndex = 12;
						if (teamIndex == 0) 
						{
							AddBotNode(_axisSoldier, bot);
						} 
						else 
						{
							AddBotNode(_alliesSoldier, bot);
						}
						break;
					case 1: // medic
						botFunName.ImageIndex = 13;
						botFunName.SelectedImageIndex = 13;
						if (teamIndex == 0) 
						{
							AddBotNode(_axisMedic, bot);
						} 
						else 
						{
							AddBotNode(_alliesMedic, bot);
						}
						break;
					case 2: // engineer
						botFunName.ImageIndex = 14;
						botFunName.SelectedImageIndex = 14;
						if (teamIndex == 0) 
						{
							AddBotNode(_axisEngineer, bot);
						} 
						else 
						{
							AddBotNode(_alliesEngineer, bot);
						}
						break;
					case 3: // field ops
						botFunName.ImageIndex = 15;
						botFunName.SelectedImageIndex = 15;
						if (teamIndex == 0) 
						{
							AddBotNode(_axisFieldOps, bot);
						} 
						else 
						{
							AddBotNode(_alliesFieldOps, bot);
						}
						break;
					case 4: // covert ops
						botFunName.ImageIndex = 16;
						botFunName.SelectedImageIndex = 16;
						if (teamIndex == 0) 
						{
							AddBotNode(_axisCovertOps, bot);
						} 
						else 
						{
							AddBotNode(_alliesCovertOps, bot);
						}
						break;
				}
			} 
			catch 
			{
				// ignore bogus bots
			}
		}

		private void tvNewBots_DoubleClick(object sender, EventArgs e)
		{
			try 
			{
				TreeNode selected = tvNewBots.SelectedNode;
				if (selected != null) 
				{
					if (selected.Tag != null) 
					{
						selected.Remove();
					}
				}
			} 
			catch {}
		}

		private void btnSave_Click(object sender, System.EventArgs e)
		{
			try 
			{
				SaveFile();
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error saving bots file. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void SaveFile() 
		{
			try 
			{
				saveFile.Filter = "Fritz Bots Files(*.bots)|*.bots";
				saveFile.FileOk += new CancelEventHandler(saveFile_FileOk);
				saveFile.ShowDialog(this.ParentForm);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error saving file. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}		
		}

		private void saveFile_FileOk(object sender, CancelEventArgs e)
		{
			try 
			{
				string fileName = ((SaveFileDialog)sender).FileName;

				if (!fileName.ToLower().EndsWith(".bots")) 
				{
					fileName += ".bots";
				}
				
				WriteBotsFile(fileName);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error saving file. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}		
		}

		private void AppendBot(TreeNode node, AlternatingBotRowCollection bots) 
		{
			Bots.BotRow bot = node.Tag as Bots.BotRow;

			if (bot != null) 
			{
				bots.Add(bot);
			}

			foreach (TreeNode child in node.Nodes) 
			{
				AppendBot(child, bots);
			}
		}

		private void WriteBotsFile(string fileName) 
		{
			StringBuilder buff = new StringBuilder();

			// random dude goes first
			buff.Append("random // random bot\r\n");

			AlternatingBotRowCollection bots = new AlternatingBotRowCollection();

			// engineer, medic, field ops, coverts, soldiers.
			AppendBot(_axisEngineer, bots);
			AppendBot(_alliesEngineer, bots);
			AppendBot(_axisMedic, bots);
			AppendBot(_alliesMedic, bots);
			AppendBot(_axisFieldOps, bots);
			AppendBot(_alliesFieldOps, bots);
			AppendBot(_axisCovertOps, bots);
			AppendBot(_alliesCovertOps, bots);
			AppendBot(_axisSoldier, bots);
			AppendBot(_alliesSoldier, bots);

			foreach (Bots.BotRow bot in bots) 
			{
				buff.Append(bot.Name);
				buff.Append(" // ");

				buff.Append(bot.Team == "0" ? "axis " : "allies ");

				switch (bot.Class) 
				{
					case "0":
						buff.Append("soldier\r\n");
						break;
					case "1":
						buff.Append("medic\r\n");
						break;
					case "2":
						buff.Append("engineer\r\n");
						break;
					case "3":
						buff.Append("field ops\r\n");
						break;
					case "4":
						buff.Append("covert ops\r\n");
						break;
				}
			}

			buff.Append("\r\n#EOF\r\n");

			StreamWriter writer = new StreamWriter(fileName, false);
			writer.Write(buff.ToString());
			writer.Close();
		}

		private void ClearBotNode(TreeNode node) 
		{
			Bots.BotRow bot = node.Tag as Bots.BotRow;

			if (bot != null) 
			{
				node.Remove();
			} 
			else 
			{
				foreach (TreeNode child in node.Nodes) 
				{
					ClearBotNode(child);
				}
			}
		}

		private void mnuClearAll_Click(object sender, System.EventArgs e)
		{
			try 
			{
				foreach (TreeNode node in tvNewBots.Nodes) 
				{
					ClearBotNode(node);
				}
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Unable to clear bots. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}
