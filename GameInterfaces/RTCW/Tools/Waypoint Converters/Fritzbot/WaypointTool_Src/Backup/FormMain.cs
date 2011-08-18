using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;

namespace WaypointTool
{
	public class FormMain : System.Windows.Forms.Form
	{
		private System.Windows.Forms.StatusBar sbMain;
		private System.Windows.Forms.MainMenu mnuMain;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem mnuExit;
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		private System.Windows.Forms.MenuItem mnuWriteAll;
		private System.Windows.Forms.Panel pnlMain;
		private System.Windows.Forms.MenuItem mnuReadNav;
		private System.ComponentModel.Container components = null;

		private NavigationReader _reader;
		private System.Windows.Forms.MenuItem mnuHelp;
		private System.Windows.Forms.MenuItem mnuAbout;
		private System.Windows.Forms.MenuItem mnuAIScript;
		private System.Windows.Forms.MenuItem mnuCompare;
		private System.Windows.Forms.MenuItem mnuUtility;
		private System.Windows.Forms.MenuItem mnuBspReader;
		private System.Windows.Forms.MenuItem mnuBots;
		private System.Windows.Forms.MenuItem mnuWaypoints;
		private System.Windows.Forms.Panel pnlBack;
		private System.Windows.Forms.PictureBox pbLogo;
		private System.Windows.Forms.Label lblCredits;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.MenuItem mnuWriteNavigation;
		private System.Windows.Forms.MenuItem mnuPrint;
		private System.Windows.Forms.MenuItem mnuPrintPreview;
		private System.Windows.Forms.PrintPreviewDialog printPreviewDialog;
		private System.Windows.Forms.PrintDialog printDialog;
		private System.Windows.Forms.MenuItem menuItem2;
		private string _navFileName;

		public FormMain()
		{
			InitializeComponent();
			InitializeEasterEgg();
		}

		private void InitializeEasterEgg() 
		{
			this.KeyDown += new KeyEventHandler(FormMain_KeyDown);
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(FormMain));
			this.sbMain = new System.Windows.Forms.StatusBar();
			this.mnuMain = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.mnuReadNav = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.mnuWriteAll = new System.Windows.Forms.MenuItem();
			this.mnuWriteNavigation = new System.Windows.Forms.MenuItem();
			this.mnuPrint = new System.Windows.Forms.MenuItem();
			this.mnuPrintPreview = new System.Windows.Forms.MenuItem();
			this.mnuExit = new System.Windows.Forms.MenuItem();
			this.mnuWaypoints = new System.Windows.Forms.MenuItem();
			this.mnuAIScript = new System.Windows.Forms.MenuItem();
			this.mnuUtility = new System.Windows.Forms.MenuItem();
			this.mnuBspReader = new System.Windows.Forms.MenuItem();
			this.mnuBots = new System.Windows.Forms.MenuItem();
			this.mnuCompare = new System.Windows.Forms.MenuItem();
			this.mnuHelp = new System.Windows.Forms.MenuItem();
			this.mnuAbout = new System.Windows.Forms.MenuItem();
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.pnlMain = new System.Windows.Forms.Panel();
			this.pnlBack = new System.Windows.Forms.Panel();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.lblCredits = new System.Windows.Forms.Label();
			this.pbLogo = new System.Windows.Forms.PictureBox();
			this.printPreviewDialog = new System.Windows.Forms.PrintPreviewDialog();
			this.printDialog = new System.Windows.Forms.PrintDialog();
			this.pnlMain.SuspendLayout();
			this.pnlBack.SuspendLayout();
			this.SuspendLayout();
			// 
			// sbMain
			// 
			this.sbMain.Location = new System.Drawing.Point(0, 543);
			this.sbMain.Name = "sbMain";
			this.sbMain.Size = new System.Drawing.Size(824, 10);
			this.sbMain.TabIndex = 0;
			// 
			// mnuMain
			// 
			this.mnuMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.menuItem1,
																					this.mnuWaypoints,
																					this.mnuAIScript,
																					this.mnuUtility,
																					this.mnuHelp});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.mnuReadNav,
																					  this.menuItem2,
																					  this.mnuWriteAll,
																					  this.mnuWriteNavigation,
																					  this.mnuPrint,
																					  this.mnuPrintPreview,
																					  this.mnuExit});
			this.menuItem1.Text = "&File";
			// 
			// mnuReadNav
			// 
			this.mnuReadNav.Index = 0;
			this.mnuReadNav.Text = "&Open";
			this.mnuReadNav.Click += new System.EventHandler(this.mnuReadNav_Click);
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 1;
			this.menuItem2.Text = "-";
			// 
			// mnuWriteAll
			// 
			this.mnuWriteAll.Enabled = false;
			this.mnuWriteAll.Index = 2;
			this.mnuWriteAll.Text = "&Write Configs";
			this.mnuWriteAll.Visible = false;
			this.mnuWriteAll.Click += new System.EventHandler(this.mnuWriteAll_Click);
			// 
			// mnuWriteNavigation
			// 
			this.mnuWriteNavigation.Index = 3;
			this.mnuWriteNavigation.Text = "Save &Waypoints";
			this.mnuWriteNavigation.Click += new System.EventHandler(this.mnuWriteNavigation_Click);
			// 
			// mnuPrint
			// 
			this.mnuPrint.Index = 4;
			this.mnuPrint.Text = "&Print";
			this.mnuPrint.Visible = false;
			this.mnuPrint.Click += new System.EventHandler(this.mnuPrint_Click);
			// 
			// mnuPrintPreview
			// 
			this.mnuPrintPreview.Index = 5;
			this.mnuPrintPreview.Text = "Print Pre&view";
			this.mnuPrintPreview.Visible = false;
			this.mnuPrintPreview.Click += new System.EventHandler(this.mnuPrintPreview_Click);
			// 
			// mnuExit
			// 
			this.mnuExit.Index = 6;
			this.mnuExit.Text = "E&xit";
			this.mnuExit.Click += new System.EventHandler(this.mnuExit_Click);
			// 
			// mnuWaypoints
			// 
			this.mnuWaypoints.Index = 1;
			this.mnuWaypoints.Text = "&Waypoints";
			this.mnuWaypoints.Click += new System.EventHandler(this.mnuWaypoints_Click);
			// 
			// mnuAIScript
			// 
			this.mnuAIScript.Index = 2;
			this.mnuAIScript.Text = "A&IScript";
			this.mnuAIScript.Click += new System.EventHandler(this.mnuAIScript_Click);
			// 
			// mnuUtility
			// 
			this.mnuUtility.Index = 3;
			this.mnuUtility.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					   this.mnuBspReader,
																					   this.mnuBots,
																					   this.mnuCompare});
			this.mnuUtility.Text = "&Utility";
			// 
			// mnuBspReader
			// 
			this.mnuBspReader.Index = 0;
			this.mnuBspReader.Text = "BSP &Reader";
			this.mnuBspReader.Click += new System.EventHandler(this.mnuBspReader_Click);
			// 
			// mnuBots
			// 
			this.mnuBots.Index = 1;
			this.mnuBots.Text = "B&ots";
			this.mnuBots.Click += new System.EventHandler(this.mnuBots_Click);
			// 
			// mnuCompare
			// 
			this.mnuCompare.Index = 2;
			this.mnuCompare.Text = "&Compare Navigation Files";
			this.mnuCompare.Click += new System.EventHandler(this.mnuCompare_Click);
			// 
			// mnuHelp
			// 
			this.mnuHelp.Index = 4;
			this.mnuHelp.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuAbout});
			this.mnuHelp.Text = "&Help";
			// 
			// mnuAbout
			// 
			this.mnuAbout.Index = 0;
			this.mnuAbout.Text = "&About";
			this.mnuAbout.Click += new System.EventHandler(this.mnuAbout_Click);
			// 
			// pnlMain
			// 
			this.pnlMain.BackColor = System.Drawing.Color.Beige;
			this.pnlMain.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pnlMain.Controls.Add(this.pnlBack);
			this.pnlMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pnlMain.DockPadding.All = 1;
			this.pnlMain.Location = new System.Drawing.Point(0, 0);
			this.pnlMain.Name = "pnlMain";
			this.pnlMain.Size = new System.Drawing.Size(824, 543);
			this.pnlMain.TabIndex = 2;
			// 
			// pnlBack
			// 
			this.pnlBack.BackColor = System.Drawing.Color.Black;
			this.pnlBack.Controls.Add(this.label2);
			this.pnlBack.Controls.Add(this.label1);
			this.pnlBack.Controls.Add(this.lblCredits);
			this.pnlBack.Controls.Add(this.pbLogo);
			this.pnlBack.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pnlBack.Location = new System.Drawing.Point(1, 1);
			this.pnlBack.Name = "pnlBack";
			this.pnlBack.Size = new System.Drawing.Size(820, 539);
			this.pnlBack.TabIndex = 1;
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label2.ForeColor = System.Drawing.Color.Gainsboro;
			this.label2.Location = new System.Drawing.Point(54, 488);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(712, 32);
			this.label2.TabIndex = 4;
			this.label2.Text = "With Credits to CrapShoot and Maleficus for Guidance and Testing";
			this.label2.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label1.ForeColor = System.Drawing.Color.Gainsboro;
			this.label1.Location = new System.Drawing.Point(56, 160);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(712, 32);
			this.label1.TabIndex = 3;
			this.label1.Text = "By Hobbit";
			this.label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// lblCredits
			// 
			this.lblCredits.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblCredits.ForeColor = System.Drawing.Color.Gainsboro;
			this.lblCredits.Location = new System.Drawing.Point(56, 128);
			this.lblCredits.Name = "lblCredits";
			this.lblCredits.Size = new System.Drawing.Size(712, 32);
			this.lblCredits.TabIndex = 2;
			this.lblCredits.Text = "ET Waypoint Tools";
			this.lblCredits.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// pbLogo
			// 
			this.pbLogo.Image = ((System.Drawing.Image)(resources.GetObject("pbLogo.Image")));
			this.pbLogo.Location = new System.Drawing.Point(56, 8);
			this.pbLogo.Name = "pbLogo";
			this.pbLogo.Size = new System.Drawing.Size(712, 120);
			this.pbLogo.TabIndex = 1;
			this.pbLogo.TabStop = false;
			// 
			// printPreviewDialog
			// 
			this.printPreviewDialog.AutoScrollMargin = new System.Drawing.Size(0, 0);
			this.printPreviewDialog.AutoScrollMinSize = new System.Drawing.Size(0, 0);
			this.printPreviewDialog.ClientSize = new System.Drawing.Size(400, 300);
			this.printPreviewDialog.Enabled = true;
			this.printPreviewDialog.Icon = ((System.Drawing.Icon)(resources.GetObject("printPreviewDialog.Icon")));
			this.printPreviewDialog.Location = new System.Drawing.Point(238, 17);
			this.printPreviewDialog.MinimumSize = new System.Drawing.Size(375, 250);
			this.printPreviewDialog.Name = "printPreviewDialog";
			this.printPreviewDialog.TransparencyKey = System.Drawing.Color.Empty;
			this.printPreviewDialog.Visible = false;
			// 
			// FormMain
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(824, 553);
			this.Controls.Add(this.pnlMain);
			this.Controls.Add(this.sbMain);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Menu = this.mnuMain;
			this.Name = "FormMain";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Fritz Waypoint Tool (ET)";
			this.pnlMain.ResumeLayout(false);
			this.pnlBack.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		[STAThread]
		static void Main() 
		{
			Application.Run(new FormMain());
		}

		private void mnuExit_Click(object sender, System.EventArgs e)
		{
			Application.Exit();
		}

		private UserControl _activeControl = null;

		private void SetActiveControl(UserControl control) 
		{
			if (System.Drawing.Printing.PrinterSettings.InstalledPrinters.Count > 0) 
			{
				if (control is IPrintableUserControl) 
				{
					mnuPrint.Visible = true;
					mnuPrintPreview.Visible = true;
				} 
				else 
				{
					mnuPrint.Visible = false;
					mnuPrintPreview.Visible = false;
				}
			}

			_activeControl = control;
			pnlMain.Controls.Clear();
			pnlMain.Controls.Add(control);
			pnlMain.Refresh();
		}

		private void mnuWriteAll_Click(object sender, System.EventArgs e)
		{
			WriteAll();
		}

		private void WriteAll() 
		{
			try 
			{
				if (_reader == null) 
				{
					MessageBox.Show(this, "Please open a nav file first.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				// copy the .nav file
				string backupNavFile = _navFileName + "_" + DateTime.Now.Month + "_" + DateTime.Now.Day + "_" + DateTime.Now.Year + "_" + DateTime.Now.Hour + "_" + DateTime.Now.Minute + "_" + DateTime.Now.Second + ".nav";
				File.Copy(_navFileName, backupNavFile, true);
				File.SetLastWriteTime(backupNavFile, DateTime.Now);
				
				ConfigWriter.WriteNodes(_navFileName, _waypoints.Nodes);
				ConfigWriter.WriteActions(_navFileName, _waypoints.Actions);
				ConfigWriter.WriteRoutes(_navFileName, _waypoints.Routes);
				ConfigWriter.WriteTopConfig(_navFileName);

				MessageBox.Show(this, "Configurations written.", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to write configuration files. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void mnuReadNav_Click(object sender, System.EventArgs e)
		{
			try 
			{
				UserData userData = AppData.UserDataLoad();

				if (userData != null) 
				{
					openFileDialog.InitialDirectory = userData.LastNavDirectory;
				}

				openFileDialog.Filter = "Fritz Navigation Files(*.nav)|*.nav";
				openFileDialog.ShowDialog(this);

				if (openFileDialog.FileName != null && openFileDialog.FileName.Trim().Length > 0) 
				{
					if (userData == null) 
					{
						userData = new UserData();
					}
					userData.LastNavDirectory = Path.GetDirectoryName(openFileDialog.FileName);
					AppData.UserDataSave(userData);

					_reader = new NavigationReader();
					_reader.Read(openFileDialog.FileName);
					_navFileName = openFileDialog.FileName;

					this.Text = "Fritz Waypoint Tool (ET) - " + _navFileName;

					InitializeWaypoints(true);
				}
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to read .nav file. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void mnuAbout_Click(object sender, System.EventArgs e)
		{
			AboutDialog dlg = new AboutDialog();
			dlg.ShowDialog(this);
		}

		private AIScriptEditor _editor = null;

		private void InitializeEditor() 
		{
			if (_editor == null) 
			{
				_editor = new AIScriptEditor();
				_editor.Dock = DockStyle.Fill;
			}
			SetActiveControl(_editor);

			if (_reader != null) 
			{
				_editor.InitializeFromReader(_reader);
			}
		}
		
		private void mnuAIScript_Click(object sender, System.EventArgs e)
		{
			try 
			{
				InitializeEditor();
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to load script editor. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private NavigationCompareControl _navigationComparer;

		private void mnuCompare_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (_navigationComparer == null) 
				{
					_navigationComparer = new NavigationCompareControl();
					_navigationComparer.Dock = DockStyle.Fill;
				}

				SetActiveControl(_navigationComparer);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to load navigation comparer. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private BspReaderControl _bspReader;

		private void mnuBspReader_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (_bspReader == null) 
				{
					_bspReader = new BspReaderControl();
					_bspReader.Dock = DockStyle.Fill;
				}

				SetActiveControl(_bspReader);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to load Bsp Reader. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private BotsControl _bots;

		private void mnuBots_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (_bots == null) 
				{
					_bots = new BotsControl();
					_bots.Dock = DockStyle.Fill;
				}

				SetActiveControl(_bots);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to load bots. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}		
		}

		private WaypointControl _waypoints;

		private void InitializeWaypoints(bool initialize) 
		{
			try 
			{
				if (_reader == null) 
				{
					MessageBox.Show(this, "Please open a nav file first.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				if (_waypoints == null) 
				{
					_waypoints = new WaypointControl();
					_waypoints.Dock = DockStyle.Fill;
				}

				if (initialize) 
				{
					_waypoints.InitializeWaypoints(_reader);
				}

				SetActiveControl(_waypoints);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to load waypoints. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void mnuWaypoints_Click(object sender, System.EventArgs e)
		{
			InitializeWaypoints(false);
		}

		private void mnuWriteNavigation_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (_reader == null) 
				{
					MessageBox.Show(this, "Please open a nav file first.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				_reader.Write(_navFileName, _waypoints.Nodes, _waypoints.Actions, _waypoints.Routes);

				MessageBox.Show(this, "Navigation written. A backup of the previous version has been saved for you. If you have this navigation loaded in Fritz, you will need to enter '/map_restart' in the console.", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to write navigation. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void FormMain_KeyDown(object sender, KeyEventArgs e)
		{
			if ((e.KeyData & (Keys.Control | Keys.Shift | Keys.W)) == (Keys.Control | Keys.Shift | Keys.W)) 
			{
				WriteAll();
			}
		}

		private void mnuPrintPreview_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (_activeControl is IPrintableUserControl) 
				{
					printPreviewDialog.Document = ((IPrintableUserControl)_activeControl).PrintDocument;
					printPreviewDialog.ShowDialog(this);
				}
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to print. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void mnuPrint_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (_activeControl is IPrintableUserControl) 
				{
					printDialog.Document = ((IPrintableUserControl)_activeControl).PrintDocument;
					DialogResult result = printDialog.ShowDialog(this);

					if (result == DialogResult.OK) 
					{
						printDialog.Document.Print();
					}
				}
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this, "Unable to print. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}
