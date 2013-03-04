using System;
using System.Data;
using System.IO;
using System.Text;
using System.Runtime;

namespace WaypointTool
{
	public class ConfigWriter
	{
		private static string GetNodeFile(string projectFile) 
		{
			return Path.GetFileNameWithoutExtension(projectFile) + ".nodes.cfg";
		}

		private static string GetActionFile(string projectFile) 
		{
			return Path.GetFileNameWithoutExtension(projectFile) + ".actions.cfg";
		}

		private static string GetRouteFile(string projectFile) 
		{
			return Path.GetFileNameWithoutExtension(projectFile) + ".routes.cfg";
		}

		public static void WriteTopConfig(string projectFile) 
		{
			StringBuilder buff = new StringBuilder();

			string nodeFile = GetNodeFile(projectFile);
			string actionFile = GetActionFile(projectFile);
			string routeFile = GetRouteFile(projectFile);

			buff.Append("exec " + Path.GetFileName(nodeFile) + "\r\n");
			buff.Append("exec " + Path.GetFileName(actionFile) + "\r\n");
			buff.Append("exec " + Path.GetFileName(routeFile) + "\r\n");

			string fileName = Path.GetFileNameWithoutExtension(projectFile) + ".cfg";

			WriteConfig(fileName, buff);
		}

		public static void WriteNodes(string fileName, Nodes nodes) 
		{
			fileName = GetNodeFile(fileName);
			StringBuilder buff = new StringBuilder();
			StringBuilder execBuff = new StringBuilder();

			int startNode = 0;
			int endNode = 99;
			int count = 0;

			foreach (DataRow row in nodes.Node.Rows) 
			{
				Nodes.NodeRow nodeRow = (Nodes.NodeRow)row;

				if (nodeRow.ID == null) 
				{
					continue;
				}

				buff.Append("node_resetlinks " + nodeRow.ID + ";");

				// set flags
				if (!nodeRow.IsFlagNull()) 
				{
					buff.Append("node_flag " + nodeRow.ID + " " + nodeRow.Flag + ";");
				}

				// connection
				if (!nodeRow.IsConnection1Null() && nodeRow.Connection1 != "-1") 
				{
					buff.Append("node_connect " + nodeRow.ID + " " + nodeRow.Connection1 + ";");
				}

				// connection
				if (!nodeRow.IsConnection2Null() && nodeRow.Connection2 != "-1") 
				{
					buff.Append("node_connect " + nodeRow.ID + " " + nodeRow.Connection2 + ";");
				}

				// connection
				if (!nodeRow.IsConnection3Null() && nodeRow.Connection3 != "-1") 
				{
					buff.Append("node_connect " + nodeRow.ID + " " + nodeRow.Connection3 + ";");
				}

				// connection
				if (!nodeRow.IsConnection4Null() && nodeRow.Connection4 != "-1") 
				{
					buff.Append("node_connect " + nodeRow.ID + " " + nodeRow.Connection4 + ";");
				}

				// entity
				if (!nodeRow.IsEntityNull()) 
				{
					buff.Append("node_ent " + nodeRow.ID + " " + nodeRow.Entity + ";");
				}

				// team 
				if (!nodeRow.IsTeamNull()) 
				{
					buff.Append("node_team " + nodeRow.ID + " " + nodeRow.Team + ";");
				}

				// group
				if (!nodeRow.IsGroupNull()) 
				{
					buff.Append("node_group " + nodeRow.ID + " " + nodeRow.Group + ";");
				}

				// radius
				if (!nodeRow.IsRadiusNull()) 
				{
					buff.Append("node_radius " + nodeRow.ID + " " + nodeRow.Radius + ";");
				}
				
				buff.Append("\r\n");

				count++;

				if (count > endNode) 
				{
					string subConfigFileName = fileName.Substring(0, fileName.LastIndexOf(".")) + "_" + startNode + "_" + endNode + ".cfg";
					WriteConfig(subConfigFileName, buff);
					buff = new StringBuilder();
					startNode = endNode + 1;
					endNode = startNode + 99;
					execBuff.Append("exec " + subConfigFileName + "\r\n");
				}
			}

			if (buff.Length > 0) 
			{
				string subConfigFileName = fileName.Substring(0, fileName.LastIndexOf(".")) + "_" + startNode + "_" + (count - 1) + ".cfg";
				WriteConfig(subConfigFileName, buff);
				execBuff.Append("exec " + subConfigFileName + "\r\n");
			}

			WriteConfig(fileName, execBuff);
		}

		public static void WriteActions(string fileName, Actions actions) 
		{
			fileName = GetActionFile(fileName);
			StringBuilder buff = new StringBuilder();
			StringBuilder execBuff = new StringBuilder();

			int startNode = 0;
			int endNode = 99;
			int count = 0;

			foreach (DataRow row in actions.Action.Rows) 
			{
				Actions.ActionRow actionRow = (Actions.ActionRow)row;

				if (actionRow.ID == null) 
				{
					continue;
				}

				// action_axis
				if (!actionRow.IsAxisActionNull()) 
				{
					buff.Append("action_axis " + actionRow.ID + " " + actionRow.AxisAction + ";");
				} 
				else 
				{
					buff.Append("action_axis " + actionRow.ID + " -1;");
				}

				// action_allies
				if (!actionRow.IsAllyActionNull()) 
				{
					buff.Append("action_allies " + actionRow.ID + " " + actionRow.AllyAction + ";");
				}
				else 
				{
					buff.Append("action_allies " + actionRow.ID + " -1;");
				}

				if (!actionRow.IsCloseNodeNull()) 
				{
					buff.Append("action_closenode " + actionRow.ID + " " + actionRow.CloseNode + ";");
				}

				if (!actionRow.IsEntityNull()) 
				{
					buff.Append("action_ent " + actionRow.ID + " " + actionRow.Entity + ";");
				}

				if (!actionRow.IsRadiusNull()) 
				{
					buff.Append("action_radius " + actionRow.ID + " " + actionRow.Radius + ";");
				}

				if (!actionRow.IsGoalNull()) 
				{
					buff.Append("action_goal " + actionRow.ID + " " + actionRow.Goal + ";");
				}

				if (!actionRow.IsGroupNull()) 
				{
					buff.Append("action_group " + actionRow.ID + " " + actionRow.Group + ";");
				}

				if (!actionRow.IsActiveNull()) 
				{
					buff.Append("action_active " + actionRow.ID + " " + actionRow.Active + ";");
				}

				if (!actionRow.IsClassNull()) 
				{
					buff.Append("action_class " + actionRow.ID + " " + actionRow.Class + ";");
				}

				if (!actionRow.IsLinksNull()) 
				{
					buff.Append("action_links " + actionRow.ID + " " + actionRow.Links + ";");
				}

				if (!actionRow.IsProneNull()) 
				{
					buff.Append("action_prone " + actionRow.ID + " " + actionRow.Prone + ";");
				}
				buff.Append("\r\n");

				count++;

				if (count > endNode) 
				{
					string subConfigFileName = fileName.Substring(0, fileName.LastIndexOf(".")) + "_" + startNode + "_" + endNode + ".cfg";
					WriteConfig(subConfigFileName, buff);
					buff = new StringBuilder();
					startNode = endNode + 1;
					endNode = startNode + 99;
					execBuff.Append("exec " + subConfigFileName + "\r\n");
				}
			}

			if (buff.Length > 0) 
			{
				string subConfigFileName = fileName.Substring(0, fileName.LastIndexOf(".")) + "_" + startNode + "_" + (count - 1) + ".cfg";
				WriteConfig(subConfigFileName, buff);
				execBuff.Append("exec " + subConfigFileName + "\r\n");
			}

			WriteConfig(fileName, execBuff);
		}

		public static void WriteRoutes(string fileName, Routes routes) 
		{
			fileName = GetRouteFile(fileName);
			StringBuilder buff = new StringBuilder();
			StringBuilder execBuff = new StringBuilder();

			int startNode = 0;
			int endNode = 99;
			int count = 0;

			foreach (Routes.RouteRow row in routes.Route.Rows) 
			{
				// team
				if (!row.IsTeamNull()) 
				{
					buff.Append("route_team " + row.ID + " " + row.Team + "; ");
				}

				if (!row.IsRadiusNull()) 
				{
					buff.Append("route_radius " + row.ID + " " + row.Radius + "; ");
				}

				if (!row.IsActionsNull()) 
				{
					buff.Append("route_actions " + row.ID + " " + row.Actions + "; ");
				}

				if (!row.IsPathActionsNull()) 
				{
					buff.Append("route_pathactions " + row.ID + " " + row.PathActions + "; ");
				}

				buff.Append("\r\n");

				count++;

				if (count > endNode) 
				{
					string subConfigFileName = fileName.Substring(0, fileName.LastIndexOf(".")) + "_" + startNode + "_" + endNode + ".cfg";
					WriteConfig(subConfigFileName, buff);
					buff = new StringBuilder();
					startNode = endNode + 1;
					endNode = startNode + 99;
					execBuff.Append("exec " + subConfigFileName + "\r\n");
				}
			}

			if (buff.Length > 0) 
			{
				string subConfigFileName = fileName.Substring(0, fileName.LastIndexOf(".")) + "_" + startNode + "_" + (count - 1) + ".cfg";
				WriteConfig(subConfigFileName, buff);
				execBuff.Append("exec " + subConfigFileName + "\r\n");
			}

			WriteConfig(fileName, execBuff);
		}

		private static void WriteConfig(string fileName, StringBuilder buff) 
		{
			string path = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().CodeBase);
			fileName = path + "\\" + fileName;

			if (fileName.ToLower().StartsWith("file:\\")) 
			{
				fileName = fileName.Substring(6);
			}

			if (File.Exists(fileName)) 
			{
				File.Delete(fileName);
			}

			FileStream configFile = File.OpenWrite(fileName);
			configFile.Write(ASCIIEncoding.ASCII.GetBytes(buff.ToString()), 0, buff.Length);
			configFile.Close();
		}
	}
}
