using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Printing;
using System.Data;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;

using ICSharpCode.TextEditor;
using ICSharpCode.TextEditor.Actions;
using ICSharpCode.TextEditor.Document;
using ICSharpCode.TextEditor.Gui.CompletionWindow;
using ICSharpCode.TextEditor.Gui.InsightWindow;

namespace WaypointTool
{
	public class AIScriptEditor : System.Windows.Forms.UserControl, IPrintableUserControl
	{
		private System.Windows.Forms.OpenFileDialog openFile;
		private System.Windows.Forms.SaveFileDialog saveFile;
		private System.Windows.Forms.ContextMenu editorContext;
		private System.Windows.Forms.MenuItem mnuCollapseAll;
		private System.Windows.Forms.MenuItem mnuExpandAll;
		private System.Windows.Forms.Panel pnlActionsNodes;
		private System.Windows.Forms.Splitter spltActionsNodes;
		private System.Windows.Forms.DataGrid dgActions;
		private System.Windows.Forms.Splitter spltActions;
		private System.Windows.Forms.Panel pnlNodes;
		private System.Windows.Forms.DataGrid dgNodes;
		private System.Windows.Forms.ImageList imgToolbar;
		private System.Windows.Forms.Panel pnlEditor;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem mnuScriptHelp;
		private System.Windows.Forms.Panel pnlOutput;
		private System.Windows.Forms.ListView lstOutput;
		private System.Windows.Forms.Splitter spltDetails;
		private System.Windows.Forms.Panel pnlScriptEditor;
		private System.Windows.Forms.ColumnHeader chLine;
		private System.Windows.Forms.ColumnHeader chMessage;
		private System.Windows.Forms.ImageList imgCompletion;
		private System.Windows.Forms.ToolBar tbEditor;
		private System.Windows.Forms.ToolBarButton tbOpen;
		private System.Windows.Forms.ToolBarButton tbNew;
		private System.Windows.Forms.ToolBarButton tbSave;
		private System.Windows.Forms.ToolBarButton tbSaveSplitSep;
		private System.Windows.Forms.ToolBarButton tbScriptHelp;
		private System.Windows.Forms.ToolBarButton tbSplit;
		private System.Windows.Forms.ToolBarButton tbValidateExpandSep;
		private System.Windows.Forms.ToolBarButton tbExpand;
		private System.Windows.Forms.Panel pnlPadding;
		private ICSharpCode.TextEditor.TextEditorControl scriptEditor;
        private ToolBarButton tbValidate;
		private System.ComponentModel.IContainer components;

		public AIScriptEditor()
		{
			InitializeComponent();

			this.SuspendLayout();
			pnlActionsNodes.Visible = false;
			this.spltActionsNodes.Visible = false;
			
			//InitializeActionGridStyle();
			//InitializeNodeGridStyle();
			InitializeEditor();
			
			pnlOutput.Visible = false;
			pnlActionsNodes.BorderStyle = BorderStyle.FixedSingle;
			pnlEditor.BorderStyle = BorderStyle.FixedSingle;
			tbEditor.ButtonClick += new ToolBarButtonClickEventHandler(tbEditor_ButtonClick);
			this.ResumeLayout(false);

            //try to open the last aiscript used -crapshoot
            OpenLastFile();
		}

		private NavigationReader _reader = null;
		public void InitializeFromReader(NavigationReader reader, bool isNew) 
		{
			_reader = reader;

            if (isNew)
            {
                OpenLastFile();
            }
			/* -- don't load this anymore, it seems to be causing lockups.
			Thread thread = new Thread(new ThreadStart(InitializeFromReaderAsync));
			thread.Start();
			*/
		}

		private delegate void AsyncHandler();

		private void InitializeFromReaderAsync() 
		{
			this.BeginInvoke(new AsyncHandler(InitializeAsyncHandler));
		}

		private void InitializeAsyncHandler() 
		{
			// show the left panel, load in applicable actions and node list
			this.pnlActionsNodes.Visible = true;
			InitializeActions(_reader.Actions);
			InitializeNodes(_reader.Nodes);
		}

		private Actions _actions = null;

		
		private void InitializeActions(ArrayList actions) 
		{
			try 
			{
				if (_actions != null) 
				{
					_actions.Clear();
				} 
				else 
				{
					_actions = new Actions();
				}

				int count = 0;

				foreach (Action action in actions) 
				{
					if (action.AllyAction == 4 || action.AxisAction == 4
						|| action.AllyAction == 8 || action.AxisAction == 8
						|| action.AllyAction == 19 || action.AxisAction == 19
						|| action.AllyAction == 2 || action.AxisAction == 2
						|| action.AllyAction == 22 || action.AxisAction == 22
						|| action.AllyAction == 6 || action.AxisAction == 6) 
					{
						_actions.Action.AddActionRow(
							Convert.ToString(count),
							Convert.ToString(action.CloseNode),
							Convert.ToString(action.AllyAction),
							Convert.ToString(action.AxisAction),
							Convert.ToString(action.Radius),
							Convert.ToString(action.Goal),
							Convert.ToString(action.Group),
							Convert.ToString(action.Class),
							Convert.ToString(action.Links),
							Convert.ToString(action.Prone),
							Convert.ToString(action.Active),
							Convert.ToString(action.Entity));
					}
					count++;
				}

				dgActions.DataSource = _actions.Tables[0];
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error Loading Actions. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private Nodes _nodes;

		public void InitializeNodes(ArrayList nodes) 
		{
			try 
			{
				if (_nodes != null) 
				{
					_nodes.Clear();
				} 
				else 
				{
					_nodes = new Nodes();
				}

				int count = 0;
				foreach (Node node in nodes) 
				{
					string connect1 = node.Connect1 == Int32.MinValue ? "-1" : Convert.ToString(node.Connect1);
					string connect2 = node.Connect2 == Int32.MinValue ? "-1" : Convert.ToString(node.Connect2);
					string connect3 = node.Connect3 == Int32.MinValue ? "-1" : Convert.ToString(node.Connect3);
					string connect4 = node.Connect4 == Int32.MinValue ? "-1" : Convert.ToString(node.Connect4);

					_nodes.Node.AddNodeRow(
						Convert.ToString(count),
						Convert.ToString(node.Flags),
						Convert.ToString(node.Radius),
						Convert.ToString(node.Entity),
						Convert.ToString(node.Team),
						Convert.ToString(node.Group),
						connect1, connect2, connect3, connect4,
                        Convert.ToString(node.PositionX),
                        Convert.ToString(node.PositionY),
                        Convert.ToString(node.PositionZ));

					count++;
				}

				dgNodes.DataSource = _nodes.Tables[0];
				dgNodes.Refresh();
			}
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error Loading Nodes. " + ex.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private CodeCompletionWindow _completionWindow = null;
		private InsightWindow _insightWindow = null;

		private void ShowCompletionWindow(char first) 
		{
			if (_completionWindow == null || _completionWindow.IsDisposed) 
			{
				KeywordCompletionDataProvider provider = new KeywordCompletionDataProvider();
				provider.ImageList = imgCompletion;

				_completionWindow = CodeCompletionWindow.ShowCompletionWindow(this.ParentForm, scriptEditor, null, provider, first);
				
				if (_completionWindow != null) 
				{
					_completionWindow.Closed += new EventHandler(_completionWindow_Closed);
				}
			}
		}

		private void ShowInsightWindow() 
		{
			if (_insightWindow == null || _insightWindow.IsDisposed) 
			{
				_insightWindow = new InsightWindow(this.ParentForm, scriptEditor, null);
				_insightWindow.Closed += new EventHandler(_insightWindow_Closed);
			}

			KeywordInsightDataProvider provider = new KeywordInsightDataProvider();			
			_insightWindow.AddInsightDataProvider(provider);
			_insightWindow.ShowInsightWindow();

			if (!provider.HasData) 
			{
				_insightWindow.Dispose();
				_insightWindow = null;
			}
		}

		// TODO: Action and Node grids should really be their own control and the
		//       DataTables should be in their own class. The Action and Nodes should have
		//       a View Style or something (maybe even class that handles the style setting for
		//       the grid). This will clean up this a lot...this was cut / paste / delete from
		//       the NodeControl and ActionControl to support viewing certain fields only as well
		//       as making it ReadOnly.
		private void InitializeActionGridStyle() 
		{
			DataGridTableStyle ts = new DataGridTableStyle();
			ts.MappingName = "Action";

			ts.AlternatingBackColor = Color.Beige;
			ts.BackColor = Color.LightYellow;
			ts.HeaderBackColor = Color.Beige;
			ts.AllowSorting = false;
			ts.RowHeadersVisible = false;

			DataGridTextBoxColumn idColumn = new DataGridTextBoxColumn();
			idColumn.HeaderText = "ID";
			idColumn.MappingName = "ID";
			idColumn.NullText = "0";
			idColumn.ReadOnly = true;
			ts.GridColumnStyles.Add(idColumn);

			InitializeActions(ts);

			DataGridTextBoxColumn goalColumn = new DataGridTextBoxColumn();
			goalColumn.HeaderText = "Goal";
			goalColumn.MappingName = "Goal";
			goalColumn.NullText = "0";
			ts.GridColumnStyles.Add(goalColumn);

			DataGridTextBoxColumn groupColumn = new DataGridTextBoxColumn();
			groupColumn.HeaderText = "Group";
			groupColumn.MappingName = "Group";
			groupColumn.NullText = "0";
			ts.GridColumnStyles.Add(groupColumn);

			dgActions.TableStyles.Add(ts);
		}

		private void InitializeActions(DataGridTableStyle ts) 
		{
			DataTable actions = new DataTable();

			actions.Columns.Add(new DataColumn("value", typeof(string)));
			actions.Columns.Add(new DataColumn("description", typeof(string)));

			actions.Rows.Add(new object[] { "3", "Aim at" });
			actions.Rows.Add(new object[] { "10", "Airstrike Cannister" });
			actions.Rows.Add(new object[] { "11", "Artillery Hint" });
			actions.Rows.Add(new object[] { "29", "Button" });
			actions.Rows.Add(new object[] { "0", "Camp" });
			actions.Rows.Add(new object[] { "16", "Camp Engineer" });
			actions.Rows.Add(new object[] { "18", "Camp MG42" });
			actions.Rows.Add(new object[] { "28", "Camp Mobile MG42" });
			actions.Rows.Add(new object[] { "22", "Command Post" });
			actions.Rows.Add(new object[] { "19", "Construct Major" });
			actions.Rows.Add(new object[] { "2", "Construct Minor" });
			actions.Rows.Add(new object[] { "9", "Defend" });
			actions.Rows.Add(new object[] { "5", "Defuse" });
			actions.Rows.Add(new object[] { "7", "Deliver" });
			actions.Rows.Add(new object[] { "4", "Dynamite" });
			actions.Rows.Add(new object[] { "15", "Grenade Hint" });
			actions.Rows.Add(new object[] { "1", "MG42" });
			actions.Rows.Add(new object[] { "25", "Mine Hunt" });
			actions.Rows.Add(new object[] { "24", "Mine Plant" });
			actions.Rows.Add(new object[] { "-1", "None" });
			actions.Rows.Add(new object[] { "14", "Panzer" });
			actions.Rows.Add(new object[] { "12", "Roam" });
			actions.Rows.Add(new object[] { "17", "Roam Alternate" });
			actions.Rows.Add(new object[] { "27", "Satchel" });
			actions.Rows.Add(new object[] { "23", "Smoke Cannister Hint" });
			actions.Rows.Add(new object[] { "13", "Snipe" });
			actions.Rows.Add(new object[] { "6", "Spawn Flag" });
			actions.Rows.Add(new object[] { "8", "Steal" });
			actions.Rows.Add(new object[] { "26", "Subversion" });
			actions.Rows.Add(new object[] { "20", "Tank" });
			actions.Rows.Add(new object[] { "21", "Train" });

			DataGridComboBoxColumn c1 = 
				new DataGridComboBoxColumn("AllyAction", actions, "description", "value", dgActions);

			c1.NullText= "-1";
			c1.ReadOnly = true;

			DataGridComboBoxColumn c2 = 
				new DataGridComboBoxColumn("AxisAction", actions, "description", "value", dgActions);

			c2.NullText= "-1";
			c2.ReadOnly = true;

			ts.GridColumnStyles.Add(c1);
			ts.GridColumnStyles.Add(c2);

		}

		private void InitializeClasses(DataGridTableStyle ts) 
		{
			DataTable classes = new DataTable();

			classes.Columns.Add(new DataColumn("bitvalue", typeof(string)));
			classes.Columns.Add(new DataColumn("description", typeof(string)));
			
			classes.Rows.Add(new object[] { "0", "Everyone" });

			classes.Rows.Add(new object[] { "16", "Coverts" });
			classes.Rows.Add(new object[] { "17", "Coverts and Soldiers" });
			classes.Rows.Add(new object[] { "18", "Coverts and Medics" });
			classes.Rows.Add(new object[] { "19", "Coverts, Medics, and Soldiers" });
			classes.Rows.Add(new object[] { "20", "Coverts and Engineers" });
			classes.Rows.Add(new object[] { "21", "Coverts, Engineers, and Soldiers" });
			classes.Rows.Add(new object[] { "22", "Coverts, Engineers and Medics" });
			classes.Rows.Add(new object[] { "23", "Coverts, Engineers, Medics and Soldiers" });
			classes.Rows.Add(new object[] { "24", "Coverts and Field Ops" });
			classes.Rows.Add(new object[] { "25", "Coverts, Field Ops, and Soldiers" });
			classes.Rows.Add(new object[] { "26", "Coverts, Field Ops, and Medics" });
			classes.Rows.Add(new object[] { "27", "Coverts, Field Ops, Medics, and Soldiers" });
			classes.Rows.Add(new object[] { "28", "Coverts, Field Ops, and Engineers" });
			classes.Rows.Add(new object[] { "29", "Coverts, Field Ops, Engineers, and Soldiers" });
			classes.Rows.Add(new object[] { "30", "Coverts, Field Ops, Engineers, and Medics" });

			classes.Rows.Add(new object[] { "4", "Engineers" });

			classes.Rows.Add(new object[] { "8", "Field Ops" });
			classes.Rows.Add(new object[] { "12", "Field Ops and Engineers" });
			classes.Rows.Add(new object[] { "10", "Field Ops and Medics" });
			classes.Rows.Add(new object[] { "9", "Field Ops and Soldiers" });
			classes.Rows.Add(new object[] { "13", "Field Ops, Engineers, and Soldiers" });
			classes.Rows.Add(new object[] { "11", "Field Ops, Medics, and Soldiers" });
			classes.Rows.Add(new object[] { "14", "Field Ops, Medics, and Engineers" });
			classes.Rows.Add(new object[] { "15", "Field Ops, Medics, Soldiers, and Engineers" });
			
			classes.Rows.Add(new object[] { "2", "Medics" });
			classes.Rows.Add(new object[] { "6", "Medics and Engineers" });
			classes.Rows.Add(new object[] { "7", "Medics, Engineers, and Soldiers" });
			
			classes.Rows.Add(new object[] { "1", "Soldiers" });
			classes.Rows.Add(new object[] { "3", "Soldiers and Medics" });
			classes.Rows.Add(new object[] { "5", "Soldiers and Engineers" });
			
			DataGridComboBoxColumn c1 = 
				new DataGridComboBoxColumn("Class", classes, "description", "bitvalue", dgActions);

			c1.NullText= "0";

			ts.GridColumnStyles.Add(c1);
		}

		private void InitializeNodeGridStyle() 
		{
			DataGridTableStyle ts = new DataGridTableStyle();
			ts.MappingName = "Node";

			ts.AlternatingBackColor = Color.Beige;
			ts.BackColor = Color.LightYellow;
			ts.HeaderBackColor = Color.Beige;
			ts.AllowSorting = false;
			ts.RowHeadersVisible = false;

			DataGridTextBoxColumn idColumn = new DataGridTextBoxColumn();
			idColumn.HeaderText = "ID";
			idColumn.MappingName = "ID";
			idColumn.NullText = "0";
			idColumn.ReadOnly = true;
			ts.GridColumnStyles.Add(idColumn);

			InitializeTeam(ts);
			
			DataGridTextBoxColumn groupColumn = new DataGridTextBoxColumn();
			groupColumn.HeaderText = "Group";
			groupColumn.MappingName = "Group";
			groupColumn.NullText = "0";
			groupColumn.ReadOnly = true;
			ts.GridColumnStyles.Add(groupColumn);

			DataGridTextBoxColumn connect1Column = new DataGridTextBoxColumn();
			connect1Column.HeaderText = "Connection 1";
			connect1Column.MappingName = "Connection1";
			connect1Column.NullText = "0";
			connect1Column.ReadOnly = true;
			ts.GridColumnStyles.Add(connect1Column);

			DataGridTextBoxColumn connect2Column = new DataGridTextBoxColumn();
			connect2Column.HeaderText = "Connection 2";
			connect2Column.MappingName = "Connection2";
			connect2Column.NullText = "0";
			connect2Column.ReadOnly = true;
			ts.GridColumnStyles.Add(connect2Column);

			DataGridTextBoxColumn connect3Column = new DataGridTextBoxColumn();
			connect3Column.HeaderText = "Connection 3";
			connect3Column.MappingName = "Connection3";
			connect3Column.NullText = "0";
			connect3Column.ReadOnly = true;
			ts.GridColumnStyles.Add(connect3Column);

			DataGridTextBoxColumn connect4Column = new DataGridTextBoxColumn();
			connect4Column.HeaderText = "Connection 4";
			connect4Column.MappingName = "Connection4";
			connect4Column.NullText = "0";
			connect4Column.ReadOnly = true;
			ts.GridColumnStyles.Add(connect4Column);

			dgNodes.TableStyles.Add(ts);
		}

		private void InitializeTeam(DataGridTableStyle ts) 
		{
			DataTable teams = new DataTable();

			teams.Columns.Add(new DataColumn("value", typeof(string)));
			teams.Columns.Add(new DataColumn("description", typeof(string)));
			
			teams.Rows.Add(new object[] { "2", "Allies" });
			teams.Rows.Add(new object[] { "1", "Axis" });
			teams.Rows.Add(new object[] { "0", "Both" });

			DataGridComboBoxColumn c1 = 
				new DataGridComboBoxColumn("Team", teams, "description", "value", dgNodes);

			c1.NullText= "0";
			c1.ReadOnly = true;

			ts.GridColumnStyles.Add(c1);
		}		

		private void InitializeEditor() 
		{
			string path = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().CodeBase);

			if (path.ToLower().StartsWith("file:\\")) 
			{
				path = path.Substring(6);
			}

			// define a provider that will look in the applications executing directory
			FileSyntaxModeProvider fsmProvider = new FileSyntaxModeProvider(path);

			// add that provider to the highlight manager
			HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider);

			scriptEditor.SuspendLayout();

			// set the highlighting, the name must match what is in the file
			scriptEditor.SetHighlighting("aiscript");
			scriptEditor.ShowEOLMarkers = false;
			scriptEditor.ShowSpaces = false;
			scriptEditor.ShowTabs = false;
			scriptEditor.IndentStyle = IndentStyle.Smart;
			scriptEditor.BracketMatchingStyle = BracketMatchingStyle.Before;
			scriptEditor.Document.FoldingManager.FoldingStrategy = new AIScriptFoldingStrategy();
			scriptEditor.TextEditorProperties.EnableFolding=true;
			scriptEditor.ActiveTextAreaControl.TextArea.KeyDown += new System.Windows.Forms.KeyEventHandler(TextArea_KeyDown);
			scriptEditor.ActiveTextAreaControl.BackColor = scriptEditor.BackColor;
			scriptEditor.ActiveTextAreaControl.TextArea.BackColor = scriptEditor.BackColor;
			scriptEditor.Document.DocumentChanged += new DocumentEventHandler(Document_DocumentChanged);
			scriptEditor.ResumeLayout(false);
		}

		private void Document_DocumentChanged(object sender, DocumentEventArgs e)
		{
			scriptEditor.Document.FoldingManager.UpdateFoldings("", null);
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AIScriptEditor));
            //ICSharpCode.TextEditor.Document.DefaultDocument defaultDocument1 = new ICSharpCode.TextEditor.Document.DefaultDocument();
            ICSharpCode.TextEditor.Document.DefaultFormattingStrategy defaultFormattingStrategy1 = new ICSharpCode.TextEditor.Document.DefaultFormattingStrategy();
            ICSharpCode.TextEditor.Document.DefaultHighlightingStrategy defaultHighlightingStrategy1 = new ICSharpCode.TextEditor.Document.DefaultHighlightingStrategy();
            ICSharpCode.TextEditor.Document.GapTextBufferStrategy gapTextBufferStrategy1 = new ICSharpCode.TextEditor.Document.GapTextBufferStrategy();
            ICSharpCode.TextEditor.Document.DefaultTextEditorProperties defaultTextEditorProperties1 = new ICSharpCode.TextEditor.Document.DefaultTextEditorProperties();
            this.editorContext = new System.Windows.Forms.ContextMenu();
            this.mnuCollapseAll = new System.Windows.Forms.MenuItem();
            this.mnuExpandAll = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.mnuScriptHelp = new System.Windows.Forms.MenuItem();
            this.openFile = new System.Windows.Forms.OpenFileDialog();
            this.saveFile = new System.Windows.Forms.SaveFileDialog();
            this.pnlActionsNodes = new System.Windows.Forms.Panel();
            this.pnlNodes = new System.Windows.Forms.Panel();
            this.dgNodes = new System.Windows.Forms.DataGrid();
            this.spltActions = new System.Windows.Forms.Splitter();
            this.dgActions = new System.Windows.Forms.DataGrid();
            this.spltActionsNodes = new System.Windows.Forms.Splitter();
            this.imgToolbar = new System.Windows.Forms.ImageList(this.components);
            this.pnlEditor = new System.Windows.Forms.Panel();
            this.pnlScriptEditor = new System.Windows.Forms.Panel();
            this.pnlPadding = new System.Windows.Forms.Panel();
            this.scriptEditor = new ICSharpCode.TextEditor.TextEditorControl();
            this.tbEditor = new System.Windows.Forms.ToolBar();
            this.tbOpen = new System.Windows.Forms.ToolBarButton();
            this.tbNew = new System.Windows.Forms.ToolBarButton();
            this.tbSave = new System.Windows.Forms.ToolBarButton();
            this.tbSaveSplitSep = new System.Windows.Forms.ToolBarButton();
            this.tbScriptHelp = new System.Windows.Forms.ToolBarButton();
            this.tbSplit = new System.Windows.Forms.ToolBarButton();
            this.tbValidateExpandSep = new System.Windows.Forms.ToolBarButton();
            this.tbExpand = new System.Windows.Forms.ToolBarButton();
            this.spltDetails = new System.Windows.Forms.Splitter();
            this.pnlOutput = new System.Windows.Forms.Panel();
            this.lstOutput = new System.Windows.Forms.ListView();
            this.chLine = new System.Windows.Forms.ColumnHeader();
            this.chMessage = new System.Windows.Forms.ColumnHeader();
            this.imgCompletion = new System.Windows.Forms.ImageList(this.components);
            this.tbValidate = new System.Windows.Forms.ToolBarButton();
            this.pnlActionsNodes.SuspendLayout();
            this.pnlNodes.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgNodes)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgActions)).BeginInit();
            this.pnlEditor.SuspendLayout();
            this.pnlScriptEditor.SuspendLayout();
            this.pnlPadding.SuspendLayout();
            this.pnlOutput.SuspendLayout();
            this.SuspendLayout();
            // 
            // editorContext
            // 
            this.editorContext.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.mnuCollapseAll,
            this.mnuExpandAll,
            this.menuItem1,
            this.mnuScriptHelp});
            // 
            // mnuCollapseAll
            // 
            this.mnuCollapseAll.Index = 0;
            this.mnuCollapseAll.Text = "Collapse All";
            this.mnuCollapseAll.Click += new System.EventHandler(this.mnuCollapseAll_Click);
            // 
            // mnuExpandAll
            // 
            this.mnuExpandAll.Index = 1;
            this.mnuExpandAll.Text = "Expand All";
            this.mnuExpandAll.Click += new System.EventHandler(this.mnuExpandAll_Click);
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 2;
            this.menuItem1.Text = "-";
            // 
            // mnuScriptHelp
            // 
            this.mnuScriptHelp.Index = 3;
            this.mnuScriptHelp.Text = "Script Help";
            this.mnuScriptHelp.Click += new System.EventHandler(this.mnuScriptHelp_Click);
            // 
            // pnlActionsNodes
            // 
            this.pnlActionsNodes.Controls.Add(this.pnlNodes);
            this.pnlActionsNodes.Controls.Add(this.spltActions);
            this.pnlActionsNodes.Controls.Add(this.dgActions);
            this.pnlActionsNodes.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlActionsNodes.Location = new System.Drawing.Point(0, 0);
            this.pnlActionsNodes.Name = "pnlActionsNodes";
            this.pnlActionsNodes.Size = new System.Drawing.Size(336, 552);
            this.pnlActionsNodes.TabIndex = 0;
            // 
            // pnlNodes
            // 
            this.pnlNodes.Controls.Add(this.dgNodes);
            this.pnlNodes.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlNodes.Location = new System.Drawing.Point(0, 252);
            this.pnlNodes.Name = "pnlNodes";
            this.pnlNodes.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
            this.pnlNodes.Size = new System.Drawing.Size(336, 300);
            this.pnlNodes.TabIndex = 2;
            // 
            // dgNodes
            // 
            this.dgNodes.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgNodes.CaptionBackColor = System.Drawing.Color.Beige;
            this.dgNodes.CaptionForeColor = System.Drawing.Color.Black;
            this.dgNodes.CaptionText = "Nodes";
            this.dgNodes.DataMember = "";
            this.dgNodes.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dgNodes.HeaderBackColor = System.Drawing.Color.Beige;
            this.dgNodes.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.dgNodes.Location = new System.Drawing.Point(0, 2);
            this.dgNodes.Name = "dgNodes";
            this.dgNodes.ReadOnly = true;
            this.dgNodes.Size = new System.Drawing.Size(336, 298);
            this.dgNodes.TabIndex = 0;
            this.dgNodes.TabStop = false;
            // 
            // spltActions
            // 
            this.spltActions.BackColor = System.Drawing.Color.Beige;
            this.spltActions.Dock = System.Windows.Forms.DockStyle.Top;
            this.spltActions.Location = new System.Drawing.Point(0, 248);
            this.spltActions.Name = "spltActions";
            this.spltActions.Size = new System.Drawing.Size(336, 4);
            this.spltActions.TabIndex = 1;
            this.spltActions.TabStop = false;
            // 
            // dgActions
            // 
            this.dgActions.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgActions.CaptionBackColor = System.Drawing.Color.Beige;
            this.dgActions.CaptionForeColor = System.Drawing.Color.Black;
            this.dgActions.CaptionText = "Actions";
            this.dgActions.DataMember = "";
            this.dgActions.Dock = System.Windows.Forms.DockStyle.Top;
            this.dgActions.HeaderBackColor = System.Drawing.Color.Beige;
            this.dgActions.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.dgActions.Location = new System.Drawing.Point(0, 0);
            this.dgActions.Name = "dgActions";
            this.dgActions.ReadOnly = true;
            this.dgActions.Size = new System.Drawing.Size(336, 248);
            this.dgActions.TabIndex = 0;
            this.dgActions.TabStop = false;
            // 
            // spltActionsNodes
            // 
            this.spltActionsNodes.BackColor = System.Drawing.Color.Beige;
            this.spltActionsNodes.Location = new System.Drawing.Point(336, 0);
            this.spltActionsNodes.Name = "spltActionsNodes";
            this.spltActionsNodes.Size = new System.Drawing.Size(4, 552);
            this.spltActionsNodes.TabIndex = 1;
            this.spltActionsNodes.TabStop = false;
            // 
            // imgToolbar
            // 
            this.imgToolbar.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imgToolbar.ImageStream")));
            this.imgToolbar.TransparentColor = System.Drawing.Color.Transparent;
            this.imgToolbar.Images.SetKeyName(0, "");
            this.imgToolbar.Images.SetKeyName(1, "");
            this.imgToolbar.Images.SetKeyName(2, "");
            this.imgToolbar.Images.SetKeyName(3, "");
            this.imgToolbar.Images.SetKeyName(4, "");
            this.imgToolbar.Images.SetKeyName(5, "");
            this.imgToolbar.Images.SetKeyName(6, "");
            this.imgToolbar.Images.SetKeyName(7, "");
            this.imgToolbar.Images.SetKeyName(8, "");
            this.imgToolbar.Images.SetKeyName(9, "");
            this.imgToolbar.Images.SetKeyName(10, "");
            this.imgToolbar.Images.SetKeyName(11, "");
            // 
            // pnlEditor
            // 
            this.pnlEditor.Controls.Add(this.pnlScriptEditor);
            this.pnlEditor.Controls.Add(this.spltDetails);
            this.pnlEditor.Controls.Add(this.pnlOutput);
            this.pnlEditor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlEditor.Location = new System.Drawing.Point(340, 0);
            this.pnlEditor.Name = "pnlEditor";
            this.pnlEditor.Padding = new System.Windows.Forms.Padding(0, 4, 0, 0);
            this.pnlEditor.Size = new System.Drawing.Size(412, 552);
            this.pnlEditor.TabIndex = 5;
            // 
            // pnlScriptEditor
            // 
            this.pnlScriptEditor.Controls.Add(this.pnlPadding);
            this.pnlScriptEditor.Controls.Add(this.tbEditor);
            this.pnlScriptEditor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlScriptEditor.Location = new System.Drawing.Point(0, 4);
            this.pnlScriptEditor.Name = "pnlScriptEditor";
            this.pnlScriptEditor.Padding = new System.Windows.Forms.Padding(3);
            this.pnlScriptEditor.Size = new System.Drawing.Size(412, 444);
            this.pnlScriptEditor.TabIndex = 7;
            // 
            // pnlPadding
            // 
            this.pnlPadding.Controls.Add(this.scriptEditor);
            this.pnlPadding.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlPadding.Location = new System.Drawing.Point(3, 43);
            this.pnlPadding.Name = "pnlPadding";
            this.pnlPadding.Padding = new System.Windows.Forms.Padding(2);
            this.pnlPadding.Size = new System.Drawing.Size(406, 398);
            this.pnlPadding.TabIndex = 6;
            // 
            // scriptEditor
            // 
            this.scriptEditor.AllowDrop = true;
            this.scriptEditor.BackColor = System.Drawing.Color.Beige;
            this.scriptEditor.ContextMenu = this.editorContext;
            this.scriptEditor.Dock = System.Windows.Forms.DockStyle.Fill;
            //defaultDocument1.FormattingStrategy = defaultFormattingStrategy1;
            defaultHighlightingStrategy1.Extensions = new string[0];
            //defaultDocument1.HighlightingStrategy = defaultHighlightingStrategy1;
            //defaultDocument1.ReadOnly = false;
            //defaultDocument1.TextBufferStrategy = gapTextBufferStrategy1;
            //defaultDocument1.TextContent = "";
            defaultTextEditorProperties1.AllowCaretBeyondEOL = false;
            defaultTextEditorProperties1.AutoInsertCurlyBracket = true;
            defaultTextEditorProperties1.BracketMatchingStyle = ICSharpCode.TextEditor.Document.BracketMatchingStyle.After;
            defaultTextEditorProperties1.ConvertTabsToSpaces = false;
            defaultTextEditorProperties1.CreateBackupCopy = false;
            defaultTextEditorProperties1.DocumentSelectionMode = ICSharpCode.TextEditor.Document.DocumentSelectionMode.Normal;
            defaultTextEditorProperties1.EnableFolding = true;
            defaultTextEditorProperties1.Encoding = ((System.Text.Encoding)(resources.GetObject("defaultTextEditorProperties1.Encoding")));
            defaultTextEditorProperties1.Font = new System.Drawing.Font("Courier New", 10F);
            defaultTextEditorProperties1.HideMouseCursor = false;
            defaultTextEditorProperties1.IndentStyle = ICSharpCode.TextEditor.Document.IndentStyle.Smart;
            defaultTextEditorProperties1.IsIconBarVisible = false;
            defaultTextEditorProperties1.LineTerminator = "\r\n";
            defaultTextEditorProperties1.LineViewerStyle = ICSharpCode.TextEditor.Document.LineViewerStyle.None;
            defaultTextEditorProperties1.MouseWheelScrollDown = true;
            defaultTextEditorProperties1.MouseWheelTextZoom = true;
            defaultTextEditorProperties1.ShowEOLMarker = true;
            defaultTextEditorProperties1.ShowHorizontalRuler = false;
            defaultTextEditorProperties1.ShowInvalidLines = true;
            defaultTextEditorProperties1.ShowLineNumbers = true;
            defaultTextEditorProperties1.ShowMatchingBracket = true;
            defaultTextEditorProperties1.ShowSpaces = true;
            defaultTextEditorProperties1.ShowTabs = true;
            defaultTextEditorProperties1.ShowVerticalRuler = true;
            defaultTextEditorProperties1.TabIndent = 4;
            defaultTextEditorProperties1.UseAntiAliasedFont = false;
            defaultTextEditorProperties1.UseCustomLine = false;
            defaultTextEditorProperties1.VerticalRulerRow = 80;
            //defaultDocument1.TextEditorProperties = defaultTextEditorProperties1;
            //this.scriptEditor.Document = defaultDocument1;
            this.scriptEditor.Encoding = ((System.Text.Encoding)(resources.GetObject("scriptEditor.Encoding")));
            this.scriptEditor.IsIconBarVisible = false;
            this.scriptEditor.Location = new System.Drawing.Point(2, 2);
            this.scriptEditor.Name = "scriptEditor";
            this.scriptEditor.ShowEOLMarkers = true;
            this.scriptEditor.ShowSpaces = true;
            this.scriptEditor.ShowTabs = true;
            this.scriptEditor.ShowVRuler = true;
            this.scriptEditor.Size = new System.Drawing.Size(402, 394);
            this.scriptEditor.TabIndex = 2;
            this.scriptEditor.TextEditorProperties = defaultTextEditorProperties1;
            // 
            // tbEditor
            // 
            this.tbEditor.Appearance = System.Windows.Forms.ToolBarAppearance.Flat;
            this.tbEditor.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.tbOpen,
            this.tbNew,
            this.tbSave,
            this.tbSaveSplitSep,
            this.tbScriptHelp,
            this.tbSplit,
            this.tbValidateExpandSep,
            this.tbExpand,
            this.tbValidate});
            this.tbEditor.Divider = false;
            this.tbEditor.DropDownArrows = true;
            this.tbEditor.ImageList = this.imgToolbar;
            this.tbEditor.Location = new System.Drawing.Point(3, 3);
            this.tbEditor.Name = "tbEditor";
            this.tbEditor.ShowToolTips = true;
            this.tbEditor.Size = new System.Drawing.Size(406, 40);
            this.tbEditor.TabIndex = 5;
            // 
            // tbOpen
            // 
            this.tbOpen.ImageIndex = 0;
            this.tbOpen.Name = "tbOpen";
            this.tbOpen.Text = "Open";
            this.tbOpen.ToolTipText = "Open File";
            // 
            // tbNew
            // 
            this.tbNew.ImageIndex = 3;
            this.tbNew.Name = "tbNew";
            this.tbNew.Text = "New";
            this.tbNew.ToolTipText = "New Script";
            // 
            // tbSave
            // 
            this.tbSave.ImageIndex = 1;
            this.tbSave.Name = "tbSave";
            this.tbSave.Text = "Save";
            this.tbSave.ToolTipText = "Save File";
            // 
            // tbSaveSplitSep
            // 
            this.tbSaveSplitSep.Name = "tbSaveSplitSep";
            this.tbSaveSplitSep.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // tbScriptHelp
            // 
            this.tbScriptHelp.ImageIndex = 11;
            this.tbScriptHelp.Name = "tbScriptHelp";
            this.tbScriptHelp.Text = "Help";
            this.tbScriptHelp.ToolTipText = "Script Help";
            // 
            // tbSplit
            // 
            this.tbSplit.ImageIndex = 2;
            this.tbSplit.Name = "tbSplit";
            this.tbSplit.Text = "Split";
            this.tbSplit.ToolTipText = "Split View";
            // 
            // tbValidateExpandSep
            // 
            this.tbValidateExpandSep.Name = "tbValidateExpandSep";
            this.tbValidateExpandSep.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // tbExpand
            // 
            this.tbExpand.ImageIndex = 10;
            this.tbExpand.Name = "tbExpand";
            this.tbExpand.Text = "Collapse";
            this.tbExpand.ToolTipText = "Expand / Collapse";
            // 
            // spltDetails
            // 
            this.spltDetails.BackColor = System.Drawing.Color.Beige;
            this.spltDetails.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.spltDetails.Location = new System.Drawing.Point(0, 448);
            this.spltDetails.Name = "spltDetails";
            this.spltDetails.Size = new System.Drawing.Size(412, 4);
            this.spltDetails.TabIndex = 6;
            this.spltDetails.TabStop = false;
            // 
            // pnlOutput
            // 
            this.pnlOutput.Controls.Add(this.lstOutput);
            this.pnlOutput.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlOutput.Location = new System.Drawing.Point(0, 452);
            this.pnlOutput.Name = "pnlOutput";
            this.pnlOutput.Size = new System.Drawing.Size(412, 100);
            this.pnlOutput.TabIndex = 5;
            // 
            // lstOutput
            // 
            this.lstOutput.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chLine,
            this.chMessage});
            this.lstOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstOutput.GridLines = true;
            this.lstOutput.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.lstOutput.Location = new System.Drawing.Point(0, 0);
            this.lstOutput.Name = "lstOutput";
            this.lstOutput.Size = new System.Drawing.Size(412, 100);
            this.lstOutput.TabIndex = 0;
            this.lstOutput.TabStop = false;
            this.lstOutput.UseCompatibleStateImageBehavior = false;
            this.lstOutput.View = System.Windows.Forms.View.Details;
            // 
            // chLine
            // 
            this.chLine.Text = "Line";
            // 
            // chMessage
            // 
            this.chMessage.Text = "Message";
            this.chMessage.Width = 400;
            // 
            // imgCompletion
            // 
            this.imgCompletion.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imgCompletion.ImageStream")));
            this.imgCompletion.TransparentColor = System.Drawing.Color.Transparent;
            this.imgCompletion.Images.SetKeyName(0, "");
            // 
            // tbValidate
            // 
            this.tbValidate.ImageIndex = 4;
            this.tbValidate.Name = "tbValidate";
            this.tbValidate.Text = "Validate";
            this.tbValidate.ToolTipText = "Validate Script";
            // 
            // AIScriptEditor
            // 
            this.Controls.Add(this.pnlEditor);
            this.Controls.Add(this.spltActionsNodes);
            this.Controls.Add(this.pnlActionsNodes);
            this.Name = "AIScriptEditor";
            this.Size = new System.Drawing.Size(752, 552);
            this.pnlActionsNodes.ResumeLayout(false);
            this.pnlNodes.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgNodes)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgActions)).EndInit();
            this.pnlEditor.ResumeLayout(false);
            this.pnlScriptEditor.ResumeLayout(false);
            this.pnlScriptEditor.PerformLayout();
            this.pnlPadding.ResumeLayout(false);
            this.pnlOutput.ResumeLayout(false);
            this.ResumeLayout(false);

		}

		private string _fileName = "";
        private string _lastFile = "";

        //new method to open last file automatically (from user.data) -crapshoot
        private void OpenLastFile()
        {
            
            UserData userData = AppData.UserDataLoad();

            if (userData != null)
            {
                //i'm saving the full path now 
                _lastFile = userData.LastAIScriptDirectory;

                try
                {
                    scriptEditor.LoadFile(_lastFile, true);
                }
                catch
                {
                    //couldn't load it, so bail quietly
                    return;
                }
                //loaded the file, validate it
                _fileName = _lastFile;

                ValidateScript();

            }
            else //no data i guess...so bail
            {
                return;
            }
            //if we made it through all that, it's clearly time to bail
            return;
            
        }
        //end crapshoot

		private void OpenFile() 
		{
			try 
			{
				UserData userData = AppData.UserDataLoad();

				if (userData != null) 
				{
					openFile.InitialDirectory = userData.LastAIScriptDirectory;
				}

				openFile.Filter = "Fritz AIScript Files(*.aiscript)|*.aiscript";
				DialogResult result = openFile.ShowDialog(this.ParentForm);

				if (result == DialogResult.OK) 
				{
					if (openFile.FileName != null && openFile.FileName.Trim().Length > 0)
					{
						if (userData == null) 
						{
							userData = new UserData();
						}

						//userData.LastAIScriptDirectory = Path.GetDirectoryName(openFile.FileName);
                        //gonna try to automatically open last used aiscript, so save full path -crapshoot
                        userData.LastAIScriptDirectory = Path.GetFullPath(openFile.FileName);
						AppData.UserDataSave(userData);

						scriptEditor.LoadFile(openFile.FileName, true);
						_fileName = openFile.FileName;
                        ValidateScript();
						//ValidateScript();
					}
				}
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error opening file. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void SaveFile() 
		{
			try 
			{
				saveFile.Filter = "Fritz AIScript Files(*.aiscript)|*.aiscript";
				saveFile.FileName = _fileName;
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

				if (!fileName.ToLower().EndsWith(".aiscript")) 
				{
					fileName += ".aiscript";
				}
				scriptEditor.SaveFile(fileName);
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error saving file. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}		
		}

		private bool _expanded = true;

		private void mnuCollapseAll_Click(object sender, System.EventArgs e)
		{
			ExpandCollapseAll(true);
		}

		private void mnuExpandAll_Click(object sender, System.EventArgs e)
		{
			ExpandCollapseAll(false);
		}

		private void ExpandCollapseAll(bool collapse) 
		{
			ArrayList folding = scriptEditor.Document.FoldingManager.FoldMarker;

			foreach (FoldMarker marker in folding)
			{
				marker.IsFolded = collapse;
			}

			scriptEditor.Refresh();

			tbExpand.Text = collapse ? "Expand" : "Collapse";
		}

		private void tbEditor_ButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
		{
			try 
			{
				if (e.Button == this.tbOpen) 
				{
					OpenFile();
				}

				if (e.Button == this.tbNew) 
				{
					NewFile();
				}

				if (e.Button == this.tbSave) 
				{
					SaveFile();
				}

				if (e.Button == this.tbSplit) 
				{
					scriptEditor.Split();
				}

				if (e.Button == this.tbScriptHelp) 
				{
					ShowScriptHelp();
				}

				if (e.Button == this.tbExpand) 
				{
					ExpandCollapseAll(_expanded);
					_expanded = !_expanded;
				}

                if (e.Button == this.tbValidate)
                {
                    ValidateScript();
                }
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error with script. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void InsertAtCursor(string val) 
		{
			scriptEditor.ActiveTextAreaControl.TextArea.InsertString(val);
		}

		private void mnuScriptHelp_Click(object sender, System.EventArgs e)
		{
			ShowScriptHelp();
		}

		private void ShowScriptHelp() 
		{
			CodeHelperDialog dlg = new CodeHelperDialog();
			dlg.ShowDialog(this.ParentForm);

			if (dlg.Apply) 
			{
				InsertAtCursor(dlg.CodeResult);
			}
		}

		private void NewFile() 
		{
			NewAIScriptDialog dlg = new NewAIScriptDialog();

			if (_actions != null) 
			{
				dlg.Actions = _actions;
			}

			dlg.ShowDialog();

			if (dlg.Apply) 
			{
				scriptEditor.Document.TextContent = dlg.CodeResult;
				ClearOutput();
			}
		}

		private void ClearOutput() 
		{
			pnlOutput.Visible = false;
			lstOutput.Items.Clear();
		}

		private void ValidateScript() 
		{
			pnlOutput.Visible = true;
			AIScriptValidator validator = new AIScriptValidator();
			ArrayList messages = validator.Validate(scriptEditor, _reader);

			lstOutput.Items.Clear();
			ClearErrors();

			foreach (AIScriptValidationMessage msg in messages) 
			{
				string line = msg.LineNumber == Int32.MinValue ? "0" : Convert.ToString(msg.LineNumber);

				ListViewItem item = new ListViewItem(line);
				
				item.SubItems.Add(msg.Message);
				lstOutput.Items.Add(item);
				SetError(msg);
			}
			scriptEditor.Refresh();
		}

		private void ClearErrors()
		{
			ArrayList markers = scriptEditor.Document.MarkerStrategy.TextMarker;
			for (int i = 0; i < markers.Count;) 
			{
				if (markers[i] is VisualError) 
				{
					markers.RemoveAt(i);
				} 
				else 
				{
					i++; // Check next one
				}
			}
		}

		private class VisualError : TextMarker
		{
			public VisualError(int offset, int length, string description, bool isError) : 
				base(offset, length, TextMarkerType.WaveLine, isError ? Color.Red : Color.Orange)
			{
				base.ToolTip = description.Replace("&", "&&&");
			}
		}

		private void SetError(AIScriptValidationMessage msg)
		{
			if (msg.ErrorWord != null) 
			{
				LineSegment lineSegment = scriptEditor.Document.GetLineSegment(msg.LineNumber - 1);
				
				int offset = lineSegment.Offset;

				for (int x = 0; x < msg.ErrorWord.Offset; x++) 
				{
					TextWord tempWord = (TextWord)lineSegment.Words[x];
					if (tempWord.Type == TextWordType.Tab) 
					{
						offset += 4;
					}
					else 
					{
						offset += tempWord.Length;
					}
				}

				scriptEditor.Document.MarkerStrategy.TextMarker.Add(new VisualError(offset, msg.ErrorWord.Length, msg.Message, true));
			}
		}

		private void _completionWindow_Closed(object sender, EventArgs e)
		{
			Form senderForm = (Form)sender;
			senderForm.Closed -= new EventHandler(_completionWindow_Closed);
			senderForm.Dispose();
			if (senderForm == _completionWindow) 
			{
				_completionWindow = null;
			}
		}

		private void TextArea_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyData == (Keys.Control | Keys.Space)) 
			{
				ShowCompletionWindow(' ');
				e.Handled = true;
			} 
			else if (e.KeyData == (Keys.Control | Keys.I)) 
			{
				ShowInsightWindow();
				e.Handled = true;
			}
			e.Handled = false;
		}

		private void _insightWindow_Closed(object sender, EventArgs e)
		{
			Form senderForm = (Form)sender;
			senderForm.Closed -= new EventHandler(_insightWindow_Closed);
			senderForm.Dispose();
			if (senderForm == _insightWindow) 
			{
				_insightWindow = null;
			}
		}

		private PrintDocument printDocument = null;
		
		public PrintDocument PrintDocument 
		{
			get 
			{
				if (printDocument == null) 
				{
					printDocument = new PrintDocument();
					printDocument.BeginPrint += new PrintEventHandler(this.BeginPrint);
					printDocument.PrintPage  += new PrintPageEventHandler(this.PrintPage);
					printDocument.DefaultPageSettings.Margins.Top = 50; //was 100 -crapshoot
					printDocument.DefaultPageSettings.Margins.Left = 100;
					printDocument.DefaultPageSettings.Margins.Right = 75; //was 150 -crapshoot
					printDocument.DefaultPageSettings.Margins.Bottom = 50; //was 100 -crapshoot
				}
				return printDocument;
			}
		}

		private int curLineNr = 0;
		private float curTabIndent = 0;
		private StringFormat printingStringFormat;
		
		private void BeginPrint(object sender, PrintEventArgs ev)
		{
			curLineNr = 0;
			printingStringFormat = (StringFormat)System.Drawing.StringFormat.GenericTypographic.Clone();
			
			// 100 should be enough for everyone ...err ?
			float[] tabStops = new float[100];
			for (int i = 0; i < tabStops.Length; ++i) 
			{
				tabStops[i] = scriptEditor.TabIndent * scriptEditor.ActiveTextAreaControl.TextArea.TextView.GetWidth(" "); 
			}
			
			printingStringFormat.SetTabStops(0, tabStops);
		}
		
		private void Advance(ref float x, ref float y, float maxWidth, float size, float fontHeight)
		{
			if (x + size < maxWidth) 
			{
				x += size;
			} 
			else 
			{
				x  = curTabIndent;
				y += fontHeight;
			}
		}
		
		private float MeasurePrintingHeight(Graphics g, LineSegment line, float maxWidth)
		{
			float xPos = 0;
			float yPos = 0;
			//float fontHeight = Font.GetHeight(g);
			float fontHeight = _printFont.GetHeight(g);
			
			//			bool  gotNonWhitespace = false;
			curTabIndent = 0;
			foreach (TextWord word in line.Words) 
			{
				switch (word.Type) 
				{
					case TextWordType.Space:
						Advance(ref xPos, ref yPos, maxWidth, scriptEditor.ActiveTextAreaControl.TextArea.TextView.GetWidth(" "), fontHeight);
						//						if (!gotNonWhitespace) {
						//							curTabIndent = xPos;
						//						}
						break;
					case TextWordType.Tab:
						Advance(ref xPos, ref yPos, maxWidth, scriptEditor.TabIndent * scriptEditor.ActiveTextAreaControl.TextArea.TextView.GetWidth(" "), fontHeight);
						//						if (!gotNonWhitespace) {
						//							curTabIndent = xPos;
						//						}
						break;
					case TextWordType.Word:
						//						if (!gotNonWhitespace) {
						//							gotNonWhitespace = true;
						//							curTabIndent    += TabIndent * primaryTextArea.TextArea.TextView.GetWidth(' ');
						//						}
						//SizeF drawingSize = g.MeasureString(word.Word, word.Font, new SizeF(maxWidth, fontHeight * 100), printingStringFormat);
						SizeF drawingSize = g.MeasureString(word.Word, _printFont, new SizeF(maxWidth, fontHeight * 100), printingStringFormat);
						Advance(ref xPos, ref yPos, maxWidth, drawingSize.Width, fontHeight);
						break;
				}
			}
			return yPos + fontHeight;
		}
		
		private void DrawLine(Graphics g, LineSegment line, float yPos, RectangleF margin)
		{
			float xPos = 0;
			float fontHeight = Font.GetHeight(g);
			//			bool  gotNonWhitespace = false;
			curTabIndent = 0 ;
			
			foreach (TextWord word in line.Words) 
			{
				switch (word.Type) 
				{
					case TextWordType.Space:
						//Advance(ref xPos, ref yPos, margin.Width, scriptEditor.ActiveTextAreaControl.TextArea.TextView.GetWidth(" "), fontHeight);
                        Advance(ref xPos, ref yPos, margin.Width, 2, fontHeight); //yay for hardcoding -crapshoot
						//						if (!gotNonWhitespace) {
						//							curTabIndent = xPos;
						//						}
						break;
					case TextWordType.Tab:
						Advance(ref xPos, ref yPos, margin.Width, scriptEditor.TabIndent * scriptEditor.ActiveTextAreaControl.TextArea.TextView.GetWidth(" "), fontHeight);
						//						if (!gotNonWhitespace) {
						//							curTabIndent = xPos;
						//						}
						break;
					case TextWordType.Word:
						//						if (!gotNonWhitespace) {
						//							gotNonWhitespace = true;
						//							curTabIndent    += TabIndent * primaryTextArea.TextArea.TextView.GetWidth(' ');
						//						}
						//g.DrawString(word.Word, word.Font, BrushRegistry.GetBrush(Color.Black), xPos + margin.X, yPos);
						//SizeF drawingSize = g.MeasureString(word.Word, word.Font, new SizeF(margin.Width, fontHeight * 100), printingStringFormat);

						g.DrawString(word.Word, _printFont, BrushRegistry.GetBrush(Color.Black), xPos + margin.X, yPos);
						SizeF drawingSize = g.MeasureString(word.Word, _printFont, new SizeF(margin.Width, fontHeight * 100), printingStringFormat);

						Advance(ref xPos, ref yPos, margin.Width, drawingSize.Width, fontHeight);
						break;
				}
			}
		}
		
        //change the font for bindle...-crapshoot
		private Font _printFont = new System.Drawing.Font("Ariel", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));

		private void PrintPage(object sender, PrintPageEventArgs ev)
		{
			Graphics g = ev.Graphics;
			float yPos = ev.MarginBounds.Top;
			
			while (curLineNr < scriptEditor.Document.TotalNumberOfLines) 
			{
				LineSegment curLine  = scriptEditor.Document.GetLineSegment(curLineNr);
				if (curLine.Words != null) 
				{
					float drawingHeight = MeasurePrintingHeight(g, curLine, ev.MarginBounds.Width);
					if (drawingHeight + yPos > ev.MarginBounds.Bottom) 
					{
						break;
					}
					
					DrawLine(g, curLine, yPos, ev.MarginBounds);
					yPos += drawingHeight;
				}
				++curLineNr;
			}
			
			// If more lines exist, print another page.
			ev.HasMorePages = curLineNr < scriptEditor.Document.TotalNumberOfLines;
		}
	}
}
