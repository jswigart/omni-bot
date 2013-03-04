using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Text;
using System.IO;

namespace WaypointTool
{
	public class NodeControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.Panel pnlActions;
		private System.Windows.Forms.DataGrid dgNodes;
		private System.Windows.Forms.Button btnConnect;
		private System.Windows.Forms.Button btnValidate;

		private System.ComponentModel.Container components = null;

		public NodeControl()
		{
			InitializeComponent();
			InitializeGridStyle();
			this.Resize += new EventHandler(NodeControl_Resize);
		}

		private void InitializeGridStyle() 
		{
			dgNodes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
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
			idColumn.ReadOnly = true;
			ts.GridColumnStyles.Add(idColumn);

			InitializeFlags(ts);

			DataGridTextBoxColumn radiusColumn = new DataGridTextBoxColumn();
			radiusColumn.HeaderText = "Radius";
			radiusColumn.MappingName = "Radius";
			ts.GridColumnStyles.Add(radiusColumn);

			DataGridTextBoxColumn entityColumn = new DataGridTextBoxColumn();
			entityColumn.HeaderText = "Entity";
			entityColumn.MappingName = "Entity";
			ts.GridColumnStyles.Add(entityColumn);

			InitializeTeam(ts);
			
			DataGridTextBoxColumn groupColumn = new DataGridTextBoxColumn();
			groupColumn.HeaderText = "Group";
			groupColumn.MappingName = "Group";
			ts.GridColumnStyles.Add(groupColumn);

			DataGridTextBoxColumn connect1Column = new DataGridTextBoxColumn();
			connect1Column.HeaderText = "Connection 1";
			connect1Column.MappingName = "Connection1";
			ts.GridColumnStyles.Add(connect1Column);

			DataGridTextBoxColumn connect2Column = new DataGridTextBoxColumn();
			connect2Column.HeaderText = "Connection 2";
			connect2Column.MappingName = "Connection2";
			ts.GridColumnStyles.Add(connect2Column);

			DataGridTextBoxColumn connect3Column = new DataGridTextBoxColumn();
			connect3Column.HeaderText = "Connection 3";
			connect3Column.MappingName = "Connection3";
			ts.GridColumnStyles.Add(connect3Column);

			DataGridTextBoxColumn connect4Column = new DataGridTextBoxColumn();
			connect4Column.HeaderText = "Connection 4";
			connect4Column.MappingName = "Connection4";
			ts.GridColumnStyles.Add(connect4Column);

			dgNodes.TableStyles.Add(ts);
		}

		private void InitializeFlags(DataGridTableStyle ts) 
		{
			DataTable flags = new DataTable();

			flags.Columns.Add(new DataColumn("value", typeof(string)));
			flags.Columns.Add(new DataColumn("description", typeof(string)));
			
			flags.Rows.Add(new object[] { "4", "Button" });
			flags.Rows.Add(new object[] { "7", "Constructable" });
			flags.Rows.Add(new object[] { "3", "Flag" });
			flags.Rows.Add(new object[] { "-1", "Invalid" });
			flags.Rows.Add(new object[] { "5", "Jump" });
			flags.Rows.Add(new object[] { "2", "Ladder Bottom" });
			flags.Rows.Add(new object[] { "1", "Ladder Top" });
			flags.Rows.Add(new object[] { "6", "Leap" });
			flags.Rows.Add(new object[] { "0", "Normal" });
			flags.Rows.Add(new object[] { "9", "Walk" });

			DataGridComboBoxColumn c1 = 
				new DataGridComboBoxColumn("Flag", flags, "description", "value", dgNodes);

			ts.GridColumnStyles.Add(c1);
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

			ts.GridColumnStyles.Add(c1);
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
			this.pnlActions = new System.Windows.Forms.Panel();
			this.btnValidate = new System.Windows.Forms.Button();
			this.btnConnect = new System.Windows.Forms.Button();
			this.dgNodes = new System.Windows.Forms.DataGrid();
			this.pnlActions.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dgNodes)).BeginInit();
			this.SuspendLayout();
			// 
			// pnlActions
			// 
			this.pnlActions.Controls.Add(this.btnValidate);
			this.pnlActions.Controls.Add(this.btnConnect);
			this.pnlActions.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pnlActions.Location = new System.Drawing.Point(0, 416);
			this.pnlActions.Name = "pnlActions";
			this.pnlActions.Size = new System.Drawing.Size(656, 32);
			this.pnlActions.TabIndex = 1;
			// 
			// btnValidate
			// 
			this.btnValidate.Location = new System.Drawing.Point(112, 8);
			this.btnValidate.Name = "btnValidate";
			this.btnValidate.Size = new System.Drawing.Size(96, 20);
			this.btnValidate.TabIndex = 1;
			this.btnValidate.Text = "Validate";
			this.btnValidate.Click += new System.EventHandler(this.btnValidate_Click);
			// 
			// btnConnect
			// 
			this.btnConnect.Location = new System.Drawing.Point(8, 8);
			this.btnConnect.Name = "btnConnect";
			this.btnConnect.Size = new System.Drawing.Size(96, 20);
			this.btnConnect.TabIndex = 0;
			this.btnConnect.Text = "Auto-Connect";
			this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
			// 
			// dgNodes
			// 
			this.dgNodes.AllowSorting = false;
			this.dgNodes.AlternatingBackColor = System.Drawing.Color.LightGreen;
			this.dgNodes.BackColor = System.Drawing.Color.AliceBlue;
			this.dgNodes.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dgNodes.CaptionVisible = false;
			this.dgNodes.DataMember = "";
			this.dgNodes.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgNodes.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.dgNodes.GridLineColor = System.Drawing.Color.Black;
			this.dgNodes.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.dgNodes.ImeMode = System.Windows.Forms.ImeMode.Off;
			this.dgNodes.Location = new System.Drawing.Point(0, 0);
			this.dgNodes.Name = "dgNodes";
			this.dgNodes.RowHeadersVisible = false;
			this.dgNodes.Size = new System.Drawing.Size(656, 416);
			this.dgNodes.TabIndex = 2;
			// 
			// NodeControl
			// 
			this.Controls.Add(this.dgNodes);
			this.Controls.Add(this.pnlActions);
			this.Name = "NodeControl";
			this.Size = new System.Drawing.Size(656, 448);
			this.pnlActions.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dgNodes)).EndInit();
			this.ResumeLayout(false);

		}

		private Nodes _nodes;

		public Nodes Nodes 
		{
			get { return _nodes; }
		}

		private bool _hasConnectErrors = false;
		public void InitializeNodes(ArrayList nodes, bool hasConnectErrors) 
		{
			_hasConnectErrors = hasConnectErrors;
			_nodes = new Nodes();

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
					connect1, connect2, connect3, connect4);

				count++;
			}

			this.SuspendLayout();
			dgNodes.DataSource = _nodes.Tables[0];
			this.ResumeLayout(false);

			//no adding of new rows thru dataview... 
			CurrencyManager cm = (CurrencyManager)this.BindingContext[dgNodes.DataSource, dgNodes.DataMember];      
			((DataView)cm.List).AllowNew = false; 
		}

		private class NodeToConnect 
		{
			public Nodes.NodeRow LeftNode;
			public Nodes.NodeRow RightNode;
			public int Connection;

			public NodeToConnect(Nodes.NodeRow leftNode, Nodes.NodeRow rightNode, int connectionNumber) 
			{
				LeftNode = leftNode;
				RightNode = rightNode;
				Connection = connectionNumber;
			}
		}

		private void btnConnect_Click(object sender, System.EventArgs e)
		{
			ArrayList nodesToConnect = new ArrayList();

			for (int x = 0; x < _nodes.Node.Count; x++) 
			{
				Nodes.NodeRow nodeRow = _nodes.Node[x];

				if (x < _nodes.Node.Count - 1) 
				{
					Nodes.NodeRow nextNode = _nodes.Node[x + 1];

					if (!NodeHasConnections(nodeRow)) 
					{
						nodesToConnect.Add(new NodeToConnect(nodeRow, nextNode, 1));

						if (!NodeHasConnections(nextNode)) 
						{
							if (x == _nodes.Node.Count - 2) 
							{
								nodesToConnect.Add(new NodeToConnect(nextNode, nodeRow, 1));
							} 
							else 
							{
								nodesToConnect.Add(new NodeToConnect(nextNode, nodeRow, 2));
							}
						}
					}
				}
			}

			Hashtable nodeCount = new Hashtable();
			foreach (NodeToConnect nodeToConnect in nodesToConnect) 
			{
				if (!nodeCount.ContainsKey(nodeToConnect.LeftNode.ID)) 
				{
					nodeCount.Add(nodeToConnect.LeftNode.ID, nodeToConnect.LeftNode.ID);
				}

				switch (nodeToConnect.Connection) 
				{
					case 1:
						nodeToConnect.LeftNode.Connection1 = nodeToConnect.RightNode.ID;
						break;
					case 2:
						nodeToConnect.LeftNode.Connection2 = nodeToConnect.RightNode.ID;
						break;
					case 3:
						nodeToConnect.LeftNode.Connection3 = nodeToConnect.RightNode.ID;
						break;
					case 4:
						nodeToConnect.LeftNode.Connection4 = nodeToConnect.RightNode.ID;
						break;
				}
			}

			MessageBox.Show(this.ParentForm, "Connected " + nodeCount.Count + " nodes with " + nodesToConnect.Count + " connections.", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
		}

		private bool NodeHasConnections(Nodes.NodeRow node) 
		{
			if (!node.IsConnection1Null() && node.Connection1 != "-1") 
			{
				return true;
			}

			if (!node.IsConnection2Null() && node.Connection2 != "-1") 
			{
				return true;
			}

			if (!node.IsConnection3Null() && node.Connection3 != "-1") 
			{
				return true;
			}

			if (!node.IsConnection4Null() && node.Connection4 != "-1") 
			{
				return true;
			}
			
			return false;
		}

		private void btnValidate_Click(object sender, System.EventArgs e)
		{
			if (IsValid()) 
			{
				MessageBox.Show(this.ParentForm, "Validation successful", "Valid", MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
		}

		// loop through the defined nodes and check against rules
		private bool IsValid() 
		{
			if (_hasConnectErrors) 
			{
				MessageBox.Show(this.ParentForm, "Some nodes have the wrong number of connections defined. Please 'Save Waypoints' to correct this error.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return false;
			}

			for (int x = 0; x < _nodes.Node.Count; x++) 
			{
				if (!IsNodeValid(_nodes.Node[x])) 
				{
					return false;
				}
			}
			return true;
		}

		// just simple validation here
		private bool IsNodeValid(Nodes.NodeRow node) 
		{
			if (!node.IsFlagNull() && node.Flag == "3") 
			{
				if (node.IsEntityNull() || node.Entity == "1023") 
				{
					MessageBox.Show(this.ParentForm, "Node " + node.ID + " is a Flag node and must have an entity number.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return false;
				}
			}

			// TODO: this will have to check the aiscript for node_connect X Y [true|false]
			//       if / when aiscripts are handled, we can add this back.
			/*
			if (
				(node.IsConnection1Null() || node.Connection1 == "-1")
				&&
				(node.IsConnection2Null() || node.Connection2 == "-1")
				&&
				(node.IsConnection3Null() || node.Connection3 == "-1")
				&&
				(node.IsConnection4Null() || node.Connection4 == "-1")
				) 
			{
				MessageBox.Show(this.ParentForm, "Node " + node.ID + " has no connections.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return false;
			}
			*/

			return true;
		}

		private void NodeControl_Resize(object sender, EventArgs e)
		{
			Refresh();
		}
	}
}
