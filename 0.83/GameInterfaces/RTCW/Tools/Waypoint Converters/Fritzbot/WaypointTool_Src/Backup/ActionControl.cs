using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace WaypointTool
{
	public class ActionControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.Panel pnlActions;
		private System.Windows.Forms.DataGrid dgActions;
		private System.Windows.Forms.Button btnValidate;

		private System.ComponentModel.Container components = null;

		public ActionControl()
		{
			InitializeComponent();
			InitializeGridStyle();
			this.Resize += new EventHandler(ActionControl_Resize);
		}

		private void InitializeGridStyle() 
		{
			dgActions.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
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
			idColumn.ReadOnly = true;
			ts.GridColumnStyles.Add(idColumn);

			DataGridTextBoxColumn activeColumn = new DataGridTextBoxColumn();
			activeColumn.HeaderText = "Active";
			activeColumn.MappingName = "Active";
			//activeColumn.Width = 100;
			ts.GridColumnStyles.Add(activeColumn);

			DataGridTextBoxColumn closeNodeColumn = new DataGridTextBoxColumn();
			closeNodeColumn.HeaderText = "Close Node";
			closeNodeColumn.MappingName = "CloseNode";
			ts.GridColumnStyles.Add(closeNodeColumn);

			InitializeActions(ts);

			DataGridTextBoxColumn radiusColumn = new DataGridTextBoxColumn();
			radiusColumn.HeaderText = "Radius";
			radiusColumn.MappingName = "Radius";
			radiusColumn.Width = 50;
			ts.GridColumnStyles.Add(radiusColumn);

			DataGridTextBoxColumn goalColumn = new DataGridTextBoxColumn();
			goalColumn.HeaderText = "Goal";
			goalColumn.MappingName = "Goal";
			goalColumn.Width = 50;
			ts.GridColumnStyles.Add(goalColumn);

			DataGridTextBoxColumn groupColumn = new DataGridTextBoxColumn();
			groupColumn.HeaderText = "Group";
			groupColumn.MappingName = "Group";
			groupColumn.Width = 50;
			ts.GridColumnStyles.Add(groupColumn);

			InitializeClasses(ts);

			DataGridTextBoxColumn linkColumn = new DataGridTextBoxColumn();
			linkColumn.HeaderText = "Links";
			linkColumn.MappingName = "Links";
			ts.GridColumnStyles.Add(linkColumn);

			DataGridTextBoxColumn proneColumn = new DataGridTextBoxColumn();
			proneColumn.HeaderText = "Prone";
			proneColumn.MappingName = "Prone";
			ts.GridColumnStyles.Add(proneColumn);

			DataGridTextBoxColumn entityColumn = new DataGridTextBoxColumn();
			entityColumn.HeaderText = "Entity";
			entityColumn.MappingName = "Entity";
			ts.GridColumnStyles.Add(entityColumn);

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
			actions.Rows.Add(new object[] { "32", "Construct Major Plant" });
			actions.Rows.Add(new object[] { "2", "Construct Minor" });
			actions.Rows.Add(new object[] { "9", "Defend" });
			actions.Rows.Add(new object[] { "5", "Defuse" });
			actions.Rows.Add(new object[] { "7", "Deliver" });
			actions.Rows.Add(new object[] { "4", "Dynamite" });
			actions.Rows.Add(new object[] { "33", "Expire" });
			actions.Rows.Add(new object[] { "34", "Explode" });
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

			DataGridComboBoxColumn c2 = 
				new DataGridComboBoxColumn("AxisAction", actions, "description", "value", dgActions);

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
			this.dgActions = new System.Windows.Forms.DataGrid();
			this.pnlActions = new System.Windows.Forms.Panel();
			this.btnValidate = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.dgActions)).BeginInit();
			this.pnlActions.SuspendLayout();
			this.SuspendLayout();
			// 
			// dgActions
			// 
			this.dgActions.AllowSorting = false;
			this.dgActions.AlternatingBackColor = System.Drawing.Color.LightGreen;
			this.dgActions.BackColor = System.Drawing.Color.AliceBlue;
			this.dgActions.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dgActions.CaptionVisible = false;
			this.dgActions.DataMember = "";
			this.dgActions.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgActions.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.dgActions.GridLineColor = System.Drawing.Color.Black;
			this.dgActions.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.dgActions.Location = new System.Drawing.Point(0, 0);
			this.dgActions.Name = "dgActions";
			this.dgActions.RowHeadersVisible = false;
			this.dgActions.Size = new System.Drawing.Size(632, 416);
			this.dgActions.TabIndex = 5;
			// 
			// pnlActions
			// 
			this.pnlActions.Controls.Add(this.btnValidate);
			this.pnlActions.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pnlActions.Location = new System.Drawing.Point(0, 416);
			this.pnlActions.Name = "pnlActions";
			this.pnlActions.Size = new System.Drawing.Size(632, 32);
			this.pnlActions.TabIndex = 4;
			// 
			// btnValidate
			// 
			this.btnValidate.Location = new System.Drawing.Point(8, 8);
			this.btnValidate.Name = "btnValidate";
			this.btnValidate.Size = new System.Drawing.Size(88, 20);
			this.btnValidate.TabIndex = 6;
			this.btnValidate.Text = "Validate";
			this.btnValidate.Click += new System.EventHandler(this.btnValidate_Click);
			// 
			// ActionControl
			// 
			this.Controls.Add(this.dgActions);
			this.Controls.Add(this.pnlActions);
			this.Name = "ActionControl";
			this.Size = new System.Drawing.Size(632, 448);
			((System.ComponentModel.ISupportInitialize)(this.dgActions)).EndInit();
			this.pnlActions.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private Actions _actions;

		public Actions Actions 
		{
			get { return _actions; }
		}

		public void InitializeActions(ArrayList actions) 
		{
			_actions = new Actions();

			int count = 0;

			foreach (Action action in actions) 
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

				count++;
			}

			this.SuspendLayout();
			dgActions.DataSource = _actions.Tables[0];
			this.ResumeLayout(false);

			//no adding of new rows thru dataview... 
			CurrencyManager cm = (CurrencyManager)this.BindingContext[dgActions.DataSource, dgActions.DataMember];      
			((DataView)cm.List).AllowNew = false; 
		}

		// loop through the defined actions and check against some rules
		private bool IsValid() 
		{
			ArrayList dynamiteActions = new ArrayList();

			for (int x = 0; x < _actions.Action.Count; x++) 
			{
				Actions.ActionRow actionRow = _actions.Action[x];
				if (!IsActionValid(actionRow)) 
				{
					return false;
				}

				// if either action is a dynamite action, add to a collection. we will
				// be checking for identical entity numbers and the existence of links
				if ((!actionRow.IsAllyActionNull() && actionRow.AllyAction == "4") || 
					(!actionRow.IsAxisActionNull() && actionRow.AxisAction == "4")) 
				{
					dynamiteActions.Add(actionRow);
				}
			}
			
			return DynamiteActionsAreValid(dynamiteActions);
		}

		private bool DynamiteActionsAreValid(ArrayList dynamiteActions) 
		{
			// if none, its valid
			if (dynamiteActions.Count == 0) return true;

			for (int x = 0; x < dynamiteActions.Count; x++) 
			{
				Actions.ActionRow actionRow = (Actions.ActionRow)dynamiteActions[x];

				// find another dynamite action in the list with
				// the same entity
				for (int y = x + 1; y < dynamiteActions.Count; y++) 
				{
					if (y == dynamiteActions.Count) break;

					Actions.ActionRow temp = (Actions.ActionRow)dynamiteActions[y];

					if (temp.Entity == actionRow.Entity) 
					{
						// check the links for both
						if (actionRow.IsLinksNull()) 
						{
							MessageBox.Show(this.ParentForm, "Action " + actionRow.ID + " and Action " + temp.ID + " are dynamite actions with the same entity, but are not linked.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
							return false;
						}

						if (temp.IsLinksNull()) 
						{
							MessageBox.Show(this.ParentForm, "Action " + temp.ID + " and Action " + actionRow.ID + " are dynamite actions with the same entity, but are not linked.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
							return false;
						}

						// both have links, now verify they point to each other
						string[] linksA = actionRow.Links.Split(new char[] { ' ' });

						// look for temp ID
						bool idFound = false;
						for (int z = 0; z < linksA.Length; z++) 
						{
							if (linksA[z] == temp.ID) 
							{
								idFound = true;
								break;
							}
						}

						if (!idFound) 
						{
							MessageBox.Show(this.ParentForm, "Action " + actionRow.ID + " and Action " 
								+ temp.ID + " are dynamite actions with the same entity, but are not linked.", 
								"Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
							return false;
						}

						string[] linksB = temp.Links.Split(new char[] { ' ' });

						// look for actionRow ID
						idFound = false;
						for (int z = 0; z < linksB.Length; z++) 
						{
							if (linksB[z] == actionRow.ID) 
							{
								idFound = true;
								break;
							}
						}

						if (!idFound) 
						{
							MessageBox.Show(this.ParentForm, "Action " + temp.ID + " and Action " 
								+ actionRow.ID + " are dynamite actions with the same entity, but are not linked.", 
								"Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
							return false;
						}
					}
				}
			}

			return true;
		}


		private static Hashtable _entityActions = GetEntityActions();
		private static Hashtable _campActions = GetCampActions();

		private static Hashtable GetEntityActions() 
		{
			Hashtable hash = new Hashtable();

			hash.Add("2", "construct minor");
			hash.Add("4", "dynamite");
			hash.Add("5", "defuse");
			hash.Add("6", "flag");
			hash.Add("8", "steal");
			hash.Add("18", "mg42 camp");
			hash.Add("19", "construct major");
			hash.Add("20", "tank");
			hash.Add("21", "train");
			hash.Add("22", "command post");
			hash.Add("24", "mine plant (used for number of plants)");
			hash.Add("27", "satchel");

			return hash;
		}

		private static Hashtable GetCampActions() 
		{
			Hashtable hash = new Hashtable();

			hash.Add("0", "camp");
			hash.Add("16", "camp engineer");
			hash.Add("28", "camp mobile mg42");
			hash.Add("9", "defend");
			hash.Add("14", "panzer");
			hash.Add("13", "snipe");
			hash.Add("25", "mine hunt");

			return hash;
		}

		private bool IsActionValid(Actions.ActionRow actionRow) 
		{
			bool isValid = true;
			StringBuilder msg = new StringBuilder();
			string id = "";
			string allyAction = "";
			string axisAction = "";
			string ent = "";
			string linked = "";
			string classNum = "";

			if (!actionRow.IsIDNull()) 
			{
				id = Convert.ToString(actionRow["ID"]);
			}

			if (!actionRow.IsAllyActionNull()) 
			{
				allyAction = Convert.ToString(actionRow["AllyAction"]);
			}

			if (!actionRow.IsAxisActionNull()) 
			{
				axisAction = Convert.ToString(actionRow["AxisAction"]);
			}

			if (!actionRow.IsEntityNull())
			{
				ent = Convert.ToString(actionRow["Entity"]);

				if (ent == "1023") ent = ""; // default
				if (ent == "-1") ent = ""; // shouldn't be -1
			}

			if (!actionRow.IsLinksNull()) 
			{
				linked = Convert.ToString(actionRow["Links"]);
			}

			if (!actionRow.IsClassNull()) 
			{
				classNum = Convert.ToString(actionRow["Class"]);
			}

			if ((allyAction == "-1" || allyAction == "")
				&& (axisAction == "-1" || axisAction == "")) 
			{
				msg.Append("Action " + id + " has no valid ally or axis actions.\r\n");
				isValid = false;
			}

			if (ent == "") 
			{
				if (_entityActions.ContainsKey(allyAction)) 
				{
					msg.Append("Action " + id + " is an " + _entityActions[allyAction] + " action with no entity.\r\n");
					isValid = false;
				}

				if (_entityActions.ContainsKey(axisAction)) 
				{
					msg.Append("Action " + id + " is an " + _entityActions[axisAction] + " action with no entity.\r\n");
					isValid = false;
				}
			}

			if (_campActions.ContainsKey(allyAction)) 
			{
				if (linked.Trim() == "") 
				{
					msg.Append("Action " + id + " is an ally " + _campActions[allyAction] + " with no links.\r\n");
					isValid = false;
				} 
				else 
				{
					// check the links
					string[] links = linked.Split(new char[] { ' ' });

					for (int x = 0; x < links.Length; x++) 
					{
						int ndx = Convert.ToInt32(links[x]);

						// check to see if the action exists!!!
						if (ndx > _actions.Action.Count - 1) 
						{
							msg.Append("Action " + id + " has link to an action that doesn't exist [" + ndx + "]");
							isValid = false;
						} 
						else 
						{
							Actions.ActionRow linkedAction = _actions.Action[ndx]; // we don't allow sorting, so this is ok
							if (linkedAction.IsAllyActionNull() || linkedAction.AllyAction != "3") 
							{
								msg.Append("Action " + id + " is an ally " + _campActions[allyAction] + " with an invalid link to Action " + links[x] + " (either no ally action or an ally action that is not an Aim action)\r\n");
								isValid = false;
							}
						}
					}
				}
			}

			if (_campActions.ContainsKey(axisAction)) 
			{
				if (linked.Trim() == "") 
				{
					msg.Append("Action " + id + " is an axis " + _campActions[axisAction] + " with no links.\r\n");
					isValid = false;
				} 
				else 
				{
					// check the links
					string[] links = linked.Split(new char[] { ' ' });

					for (int x = 0; x < links.Length; x++) 
					{
						int ndx = Convert.ToInt32(links[x]);

						// the axis action should be 3
						Actions.ActionRow linkedAction = _actions.Action[ndx]; // we don't allow sorting, so this is ok
						if (linkedAction.IsAxisActionNull() || linkedAction.AxisAction != "3") 
						{
							msg.Append("Action " + id + " is an axis " + _campActions[axisAction] + " with an invalid link to Action " + links[x] + " (either no axis action or an axis action that is not an Aim action)\r\n");
							isValid = false;
						}
					}
				}
			}

			if (!isValid) 
			{
				MessageBox.Show(this.ParentForm, msg.ToString(), "Invalid", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}

			return isValid;
		}

		private void btnValidate_Click(object sender, System.EventArgs e)
		{
			if (IsValid()) 
			{
				MessageBox.Show(this.ParentForm, "Validation successful", "Valid", MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
		}

		private void ActionControl_Resize(object sender, EventArgs e)
		{
			Refresh();
		}
	}
}
