using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Printing;
using System.Data;
using System.IO;
using System.Windows.Forms;

namespace WaypointTool
{
    public class WaypointControl : System.Windows.Forms.UserControl, IPrintableUserControl, IExportableUserControl
	{
		private System.Windows.Forms.Panel pnlMain;
		private System.Windows.Forms.TabControl tcMain;
		private System.Drawing.Printing.PrintDocument printDocument;

		private System.ComponentModel.Container components = null;

		private NodeControl _nodeControl;
		private ActionControl _actionControl;
		private RouteControl _routeControl;
		private NodePrinter _nodePrinter;
		private ActionPrinter _actionPrinter;
		private RoutePrinter _routePrinter;

		public WaypointControl()
		{
			InitializeComponent();
			InitializePrinting();
			tcMain.SelectedIndexChanged += new EventHandler(tcMain_SelectedIndexChanged);
		}

		public PrintDocument PrintDocument 
		{
			get { return printDocument; }
		}

		private void InitializePrinting() 
		{
			printDocument.PrintPage += new PrintPageEventHandler(printDocument_PrintPage);
			printDocument.DefaultPageSettings.Landscape = true;
			printDocument.DefaultPageSettings.Margins.Top = 50; //was 100 -crapshoot
			printDocument.DefaultPageSettings.Margins.Left = 100;
			printDocument.DefaultPageSettings.Margins.Right = 100;
			printDocument.DefaultPageSettings.Margins.Bottom = 25; //was 100 -crapshoot
		}
		
		public void InitializeWaypoints(NavigationReader reader) 
		{
			this.SuspendLayout();
			_nodeControl = new NodeControl();
			_nodeControl.Dock = DockStyle.Fill;

			_actionControl = new ActionControl();
			_actionControl.Dock = DockStyle.Fill;

			_routeControl = new RouteControl();
			_routeControl.Dock = DockStyle.Fill;

			TabPage nodes = new TabPage("Nodes");
			nodes.Controls.Add(_nodeControl);

			TabPage actions = new TabPage("Actions");
			actions.Controls.Add(_actionControl);

			TabPage routes = new TabPage("Routes");
			routes.Controls.Add(_routeControl);

			tcMain.TabPages.Clear();
			tcMain.TabPages.Add(nodes);
			tcMain.TabPages.Add(actions);
			tcMain.TabPages.Add(routes);

			_nodeControl.InitializeNodes(reader.Nodes, reader.HasBadConnects);
			_actionControl.InitializeActions(reader.Actions);
			_routeControl.InitializeRoutes(reader.Routes, _actionControl.Actions);

			this.ResumeLayout(false);

            try
            {
                if (System.Drawing.Printing.PrinterSettings.InstalledPrinters.Count > 0)
                {
                    _nodePrinter = new NodePrinter(printDocument, _nodeControl.Nodes, Path.GetFileName(reader.FileName));
                    _actionPrinter = new ActionPrinter(printDocument, _actionControl.Actions, Path.GetFileName(reader.FileName));
                    _routePrinter = new RoutePrinter(printDocument, _routeControl.Routes, Path.GetFileName(reader.FileName));
                }
            }
            catch
            {
            }
		}

		public Nodes Nodes 
		{
			get { return _nodeControl.Nodes; }
		}

		public Actions Actions 
		{
			get { return _actionControl.Actions; }
		}

		public Routes Routes 
		{
			get { return _routeControl.Routes; }
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
			this.pnlMain = new System.Windows.Forms.Panel();
			this.tcMain = new System.Windows.Forms.TabControl();
			this.printDocument = new System.Drawing.Printing.PrintDocument();
			this.pnlMain.SuspendLayout();
			this.SuspendLayout();
			// 
			// pnlMain
			// 
			this.pnlMain.Controls.Add(this.tcMain);
			this.pnlMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pnlMain.Location = new System.Drawing.Point(0, 0);
			this.pnlMain.Name = "pnlMain";
			this.pnlMain.Size = new System.Drawing.Size(512, 416);
			this.pnlMain.TabIndex = 0;
			// 
			// tcMain
			// 
			this.tcMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tcMain.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.tcMain.Location = new System.Drawing.Point(0, 0);
			this.tcMain.Name = "tcMain";
			this.tcMain.SelectedIndex = 0;
			this.tcMain.Size = new System.Drawing.Size(512, 416);
			this.tcMain.TabIndex = 0;
			// 
			// WaypointControl
			// 
			this.Controls.Add(this.pnlMain);
			this.Name = "WaypointControl";
			this.Size = new System.Drawing.Size(512, 416);
			this.pnlMain.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private void printDocument_PrintPage(object sender, PrintPageEventArgs e)
		{
			switch (tcMain.SelectedIndex) 
			{
				case 0:
					PrintNodes(sender, e);
					break;
				case 1:
					PrintActions(sender, e);
					break;
				case 2:
					PrintRoutes(sender, e);
					break;
			}
		}

		private void PrintNodes(object sender, PrintPageEventArgs e) 
		{
			e.HasMorePages = _nodePrinter.Print(e.Graphics);
		}

		private void PrintActions(object sender, PrintPageEventArgs e) 
		{
			e.HasMorePages = _actionPrinter.Print(e.Graphics);
		}

		private void PrintRoutes(object sender, PrintPageEventArgs e) 
		{
			e.HasMorePages = _routePrinter.Print(e.Graphics);
		}

		private void tcMain_SelectedIndexChanged(object sender, EventArgs e)
		{
			this.Refresh();
			this._actionControl.Refresh();
			this._nodeControl.Refresh();
			this._routeControl.Refresh();
		}

        #region IExportableUserControl Members

        public CsvWriter GetCsvWriter()
        {
            switch (tcMain.SelectedIndex)
            {
                case 0:
                    return GetNodeCsvWriter();
                case 1:
                    return GetActionCsvWriter();
                case 2:
                    return GetRouteCsvWriter();
            }

            throw new Exception("Unexpected error exporting");        
        }

        private CsvWriter GetNodeCsvWriter()
        {
            CsvWriter writer = new CsvWriter();

            writer.Columns.Add("ID");
            writer.Columns.Add("Radius");
            writer.Columns.Add("Entity");
            writer.Columns.Add("Group");
            writer.Columns.Add("Connection 1");
            writer.Columns.Add("Connection 2");
            writer.Columns.Add("Connection 3");
            writer.Columns.Add("Connection 4");

            for (int x = 0; x < _nodeControl.Nodes.Node.Rows.Count; x++)
            {
                Node node = new Node();
                node.FromNodeRow((Nodes.NodeRow)_nodeControl.Nodes.Node.Rows[x]);

                ArrayList nodeRow = new ArrayList();

                nodeRow.Add(Convert.ToString(node.ID));
                nodeRow.Add(Convert.ToString(node.Radius));
                nodeRow.Add(Convert.ToString(node.Entity));
                nodeRow.Add(Convert.ToString(node.Group));
                nodeRow.Add(Convert.ToString(node.Connect1));
                nodeRow.Add(Convert.ToString(node.Connect2));
                nodeRow.Add(Convert.ToString(node.Connect3));
                nodeRow.Add(Convert.ToString(node.Connect4));

                writer.AddRow(nodeRow);
            }

            return writer;
        }

        private CsvWriter GetActionCsvWriter()
        {
            CsvWriter writer = new CsvWriter();

            writer.Columns.Add("ID");
            writer.Columns.Add("Active");
            writer.Columns.Add("CloseNode");
            writer.Columns.Add("Allies Action");
            writer.Columns.Add("Axis Action");
            writer.Columns.Add("Radius");
            writer.Columns.Add("Goal");
            writer.Columns.Add("Group");
            writer.Columns.Add("Class");
            writer.Columns.Add("Links");
            writer.Columns.Add("Prone");
            writer.Columns.Add("Entity");

            for (int x = 0; x < _actionControl.Actions.Action.Rows.Count; x++)
            {
                Action action = new Action();
                action.FromActionRow((Actions.ActionRow)_actionControl.Actions.Action.Rows[x]);

                ArrayList actionRow = new ArrayList();

                actionRow.Add(Convert.ToString(action.ID));
                actionRow.Add(Convert.ToString(action.Active));
                actionRow.Add(Convert.ToString(action.CloseNode));
                actionRow.Add(Convert.ToString(action.AllyAction));
                actionRow.Add(Convert.ToString(action.AxisAction));
                actionRow.Add(Convert.ToString(action.Radius));
                actionRow.Add(Convert.ToString(action.Goal));
                actionRow.Add(Convert.ToString(action.Group));
                actionRow.Add(Convert.ToString(action.Class));
                actionRow.Add(Convert.ToString(action.Links));
                actionRow.Add(Convert.ToString(action.Prone));
                actionRow.Add(Convert.ToString(action.Entity));

                writer.AddRow(actionRow);
            }

            return writer;
        }

        private CsvWriter GetRouteCsvWriter()
        {
            CsvWriter writer = new CsvWriter();

            writer.Columns.Add("ID");
            writer.Columns.Add("Team");
            writer.Columns.Add("Radius");
            writer.Columns.Add("Actions");
            writer.Columns.Add("Path Actions");

            for (int x = 0; x < _routeControl.Routes.Route.Rows.Count; x++)
            {
                Route route = new Route();
                route.FromRouteRow((Routes.RouteRow)_routeControl.Routes.Route.Rows[x]);

                ArrayList routeRow = new ArrayList();

                routeRow.Add(Convert.ToString(route.ID));
                routeRow.Add(Convert.ToString(route.Team));
                routeRow.Add(Convert.ToString(route.Radius));
                routeRow.Add(Convert.ToString(route.Actions));
                routeRow.Add(Convert.ToString(route.PathActions));

                writer.AddRow(routeRow);
            }

            return writer;
        }

        #endregion
    }
}
