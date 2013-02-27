using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace WaypointTool
{
	public class RouteControl : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.Button btnValidate;
		private System.Windows.Forms.DataGrid dgRoutes;
		private System.Windows.Forms.Panel pnlActions;
		private System.ComponentModel.Container components = null;

		public RouteControl()
		{
			InitializeComponent();
			InitializeGridStyle();		
			this.Resize += new EventHandler(RouteControl_Resize);
		}

		private void InitializeGridStyle() 
		{
			dgRoutes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			DataGridTableStyle ts = new DataGridTableStyle();
			ts.MappingName = "Route";

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

			InitializeTeam(ts);

			DataGridTextBoxColumn radiusColumn = new DataGridTextBoxColumn();
			radiusColumn.HeaderText = "Radius";
			radiusColumn.MappingName = "Radius";
			ts.GridColumnStyles.Add(radiusColumn);

			DataGridTextBoxColumn actionsColumn = new DataGridTextBoxColumn();
			actionsColumn.HeaderText = "Actions";
			actionsColumn.MappingName = "Actions";
			actionsColumn.Width = 175;
			ts.GridColumnStyles.Add(actionsColumn);
		
			DataGridTextBoxColumn pathActionsColumn = new DataGridTextBoxColumn();
			pathActionsColumn.HeaderText = "Path Actions";
			pathActionsColumn.MappingName = "PathActions";
			pathActionsColumn.Width = 175;
			ts.GridColumnStyles.Add(pathActionsColumn);	

			dgRoutes.TableStyles.Add(ts);
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
				new DataGridComboBoxColumn("Team", teams, "description", "value", dgRoutes);

			ts.GridColumnStyles.Add(c1);
		}

		private Routes _routes;
		private Actions _actions;

		public Routes Routes 
		{
			get { return _routes; }
		}

		public void InitializeRoutes(ArrayList routes, Actions actions) 
		{
			_actions = actions;
			_routes = new Routes();

			foreach (Route route in routes) 
			{
				_routes.Route.AddRouteRow(
					Convert.ToString(route.ID),
					Convert.ToString(route.Team),
					Convert.ToString(route.Radius),
					route.Actions,
					route.PathActions);
			}

			this.SuspendLayout();
			dgRoutes.DataSource = _routes.Tables[0];
			this.ResumeLayout(false);

			//no adding of new rows thru dataview... 
			CurrencyManager cm = (CurrencyManager)this.BindingContext[dgRoutes.DataSource, dgRoutes.DataMember];      
			((DataView)cm.List).AllowNew = false; 
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
			this.dgRoutes = new System.Windows.Forms.DataGrid();
			this.pnlActions = new System.Windows.Forms.Panel();
			this.btnValidate = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.dgRoutes)).BeginInit();
			this.pnlActions.SuspendLayout();
			this.SuspendLayout();
			// 
			// dgRoutes
			// 
			this.dgRoutes.AllowSorting = false;
			this.dgRoutes.AlternatingBackColor = System.Drawing.Color.LightGreen;
			this.dgRoutes.BackColor = System.Drawing.Color.AliceBlue;
			this.dgRoutes.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dgRoutes.CaptionVisible = false;
			this.dgRoutes.DataMember = "";
			this.dgRoutes.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgRoutes.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.dgRoutes.GridLineColor = System.Drawing.Color.Black;
			this.dgRoutes.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.dgRoutes.Location = new System.Drawing.Point(0, 0);
			this.dgRoutes.Name = "dgRoutes";
			this.dgRoutes.RowHeadersVisible = false;
			this.dgRoutes.Size = new System.Drawing.Size(496, 320);
			this.dgRoutes.TabIndex = 7;
			// 
			// pnlActions
			// 
			this.pnlActions.Controls.Add(this.btnValidate);
			this.pnlActions.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pnlActions.Location = new System.Drawing.Point(0, 320);
			this.pnlActions.Name = "pnlActions";
			this.pnlActions.Size = new System.Drawing.Size(496, 32);
			this.pnlActions.TabIndex = 6;
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
			// RouteControl
			// 
			this.Controls.Add(this.dgRoutes);
			this.Controls.Add(this.pnlActions);
			this.Name = "RouteControl";
			this.Size = new System.Drawing.Size(496, 352);
			((System.ComponentModel.ISupportInitialize)(this.dgRoutes)).EndInit();
			this.pnlActions.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private void btnValidate_Click(object sender, System.EventArgs e)
		{
			try 
			{
				if (IsValid()) 
				{
					MessageBox.Show(this.ParentForm, "Validation successful", "Valid", MessageBoxButtons.OK, MessageBoxIcon.Information);
				}				
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Unable to validate routes. " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private bool IsValid() 
		{
			for (int x = 0; x < _routes.Route.Count; x++) 
			{
				if (!IsRouteValid(_routes.Route[x])) 
				{
					return false;
				}
			}
			return true;
		}

		private bool IsRouteValid(Routes.RouteRow route) 
		{
			try 
			{
				bool isValid = true;
				StringBuilder buff = new StringBuilder();

				// validate actions
				if (route.IsActionsNull() || route.Actions.Trim().Length == 0) 
				{
					// do we need actions?
					isValid = false;
					buff.Append("Route " + route.ID + " has no actions defined.\r\n");
				} 
				else 
				{
					// check that the actions exist
					string[] ids = route.Actions.Split(new char[] { ' ' });

					for (int x = 0; x < ids.Length; x++) 
					{
						int actionID = Int32.MinValue;

						try 
						{
							actionID = Convert.ToInt32(ids[x]);
						} 
						catch 
						{
							isValid = false;
							buff.Append("Route " + route.ID + " has an action link that is non-numeric.\r\n");
						}

						if (actionID != Int32.MinValue) 
						{
							if (actionID < 0 || actionID > _actions.Action.Count - 1) 
							{
								isValid = false;
								buff.Append("Route " + route.ID + " is linked to action " + actionID + " which does not exist in the navigation file.\r\n");
							}
						}
					}
				}

				// validate path actions
				if (route.IsPathActionsNull() || route.PathActions.Trim().Length == 0) 
				{
					// do we need path actions?
					isValid = false;
					buff.Append("Route " + route.ID + " has no path actions defined.\r\n");
				} 
				else 
				{
					// check that the actions exist
					string[] ids = route.PathActions.Split(new char[] { ' ' });

					for (int x = 0; x < ids.Length; x++) 
					{
						int actionID = Int32.MinValue;

						try 
						{
							actionID = Convert.ToInt32(ids[x]);
						} 
						catch 
						{
							isValid = false;
							buff.Append("Route " + route.ID + " has a path action link that is non-numeric.\r\n");
						}

						if (actionID != Int32.MinValue) 
						{
							if (actionID < 0 || actionID > _actions.Action.Count - 1) 
							{
								isValid = false;
								buff.Append("Route " + route.ID + " is linked to path action " + actionID + " which does not exist in the navigation file.\r\n");
							} 
							else 
							{
								Actions.ActionRow actionRow = _actions.Action[actionID];

								// these need to be 17, alt-roams
								switch (route.Team) 
								{
									case "0":
										if (actionRow.IsAllyActionNull() || actionRow.IsAxisActionNull()) 
										{
											isValid = false;
											buff.Append("Route " + route.ID + " is linked to a path action [" + actionID + "] that is not an Alternate Roam for both teams.\r\n");
										} 
										else 
										{
											if (actionRow.AllyAction != "17" || actionRow.AxisAction != "17") 
											{
												isValid = false;
												buff.Append("Route " + route.ID + " is linked to a path action [" + actionID + "] that is not an Alternate Roam for both teams.\r\n");
											}
										}
										break;
									case "1":
										if (actionRow.IsAxisActionNull()) 
										{
											isValid = false;
											buff.Append("Route " + route.ID + " is linked to a path action [" + actionID + "] that is not an Axis Alternate Roam action.\r\n");
										} 
										else 
										{
											if (actionRow.AxisAction != "17") 
											{
												isValid = false;
												buff.Append("Route " + route.ID + " is linked to a path action [" + actionID + "] that is not an Axis Alternate Roam action.\r\n");
											}
										}
										break;
									case "2":
										if (actionRow.IsAllyActionNull()) 
										{
											isValid = false;
											buff.Append("Route " + route.ID + " is linked to a path action [" + actionID + "] that is not an Ally Alternate Roam action.\r\n");
										} 
										else 
										{
											if (actionRow.AllyAction != "17") 
											{
												isValid = false;
												buff.Append("Route " + route.ID + " is linked to a path action [" + actionID + "] that is not an Ally Alternate Roam action.\r\n");
											}
										}
										break;
								}
							}
						}
					}
				}

				if (!isValid) 
				{
					MessageBox.Show(this.ParentForm, buff.ToString(), "Invalid", MessageBoxButtons.OK, MessageBoxIcon.Error);
				}

				return isValid;
			} 
			catch (Exception ex) 
			{
				MessageBox.Show(this.ParentForm, "Error validation route " + route.ID + ". " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return false;
			}
		}

		private void RouteControl_Resize(object sender, EventArgs e)
		{
			Refresh();
		}
	}
}
