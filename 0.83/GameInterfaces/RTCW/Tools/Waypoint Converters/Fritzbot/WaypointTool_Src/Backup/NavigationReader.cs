using System;
using System.Collections;
using System.IO;
using System.Text;

namespace WaypointTool
{
	public class NavigationReader
	{
		private int _numberOfNodes = Int32.MinValue;
		private ArrayList _nodes = new ArrayList();
		private ArrayList _actions = new ArrayList();
		private ArrayList _routes = new ArrayList();
		private string _fileName;
		private bool _hasBadConnects = false;

		public string FileName 
		{
			get { return _fileName; }
		}

		public int NumberOfNodes 
		{
			get { return _numberOfNodes; }
		}

		public ArrayList Nodes 
		{
			get { return _nodes; }
		}

		public ArrayList Actions 
		{
			get { return _actions; }
		}

		public ArrayList Routes 
		{
			get { return _routes; }
		}

		public bool HasBadConnects 
		{
			get { return _hasBadConnects; }
		}

		private byte[] FromInt(int val) 
		{
			Bits bits = new Bits(val, 16);
			byte[] bytes = bits.ToBytes();
			byte[] newBytes = new byte[2];
			newBytes[0] = bytes[1];
			newBytes[1] = bytes[0];
			return newBytes;
		}
		
		public void Write(string fileName, Nodes nodes, Actions actions, Routes routes) 
		{
			if (File.Exists(fileName)) 
			{
				// copy the .nav file
				string backupNavFile = fileName + "_" + DateTime.Now.Month + "_" + DateTime.Now.Day + "_" + DateTime.Now.Year + "_" + DateTime.Now.Hour + "_" + DateTime.Now.Minute + "_" + DateTime.Now.Second + ".nav";
				File.Move(fileName, backupNavFile);
				File.SetLastWriteTime(backupNavFile, DateTime.Now);
			}

			FileStream fs = File.Create(fileName);
			BinaryWriter writer = new BinaryWriter(fs);

			byte[] header = new byte[] { 70, 114, 105, 116, 122, 66, 111, 116, 0, 7, 0, 0, 0 };
			
			writer.Write(header);
			writer.Write(FromInt(nodes.Node.Count));

			if (nodes.Node.Count > 0) 
			{
				int nodeCounter = 0;
				foreach (Nodes.NodeRow nodeRow in nodes.Node) 
				{
					Node node = (Node)_nodes[nodeCounter];
					node.FromNodeRow(nodeRow);
					writer.Write(node.ToByteArray());
					nodeCounter++;
				}

				if (actions.Action.Count > 0) 
				{
					int actionCounter = 0;
					writer.Write(FromInt(actions.Action.Count));
					foreach (Actions.ActionRow actionRow in actions.Action) 
					{
						Action action = (Action)_actions[actionCounter];
						action.FromActionRow(actionRow);
						writer.Write(action.ToByteArray());
						actionCounter++;
					}

					if (routes.Route.Count > 0) 
					{
						int routeCounter = 0;
						writer.Write(FromInt(routes.Route.Count));
						foreach (Routes.RouteRow routeRow in routes.Route) 
						{
							Route route = (Route)_routes[routeCounter];
							route.FromRouteRow(routeRow);
							writer.Write(route.ToByteArray());
							routeCounter++;
						}
					} 
					else 
					{
						writer.Write(FromInt(0));
					}
				} 
				else 
				{
					writer.Write(FromInt(0));
					writer.Write(FromInt(0));
				}
			} 
			else 
			{
				writer.Write(FromInt(0));
				writer.Write(FromInt(0));
			}

			writer.Close();
			fs.Close();
		}

		public void Read(string fileName) 
		{
			_fileName = fileName;
			FileStream stream = null;
			try 
			{
				int pos = 0;

				stream = File.OpenRead(fileName);
				
				// just read it all into memory
				int length = (int)stream.Length;
				byte[] readBytes = new byte[length];
				stream.Read(readBytes, 0, length);
				stream.Close();
				stream = null;

				_numberOfNodes = GetInt(readBytes[14], readBytes[13]);

				pos = 15;

				if (_numberOfNodes > 0) 
				{
					for (int x = 0; x < _numberOfNodes; x++) 
					{
						Node node = new Node();

						byte[] nodeBytes = new byte[24];
						ReadSubBytes(readBytes, nodeBytes, pos);

						node.Bytes = nodeBytes;
						node.ID = x;
						
						node.Flags = (int)nodeBytes[12];
						node.Entity = GetInt(nodeBytes[15], nodeBytes[14]);
						node.NumberOfConnects = GetInt(nodeBytes[17], nodeBytes[16]);
						node.Radius = GetInt(nodeBytes[19], nodeBytes[18]);
						node.Team = GetInt(nodeBytes[21], nodeBytes[20]);
						node.Group = GetInt(nodeBytes[23], nodeBytes[22]);

						pos += 24;

						// read connects if there are any
						if (node.NumberOfConnects != 0) 
						{
							for (int y = 0; y < node.NumberOfConnects; y++) 
							{
								byte[] connectBytes = new byte[2];
								ReadSubBytes(readBytes, connectBytes, pos);
								pos += 2;

								int connectNum = GetInt(connectBytes[1], connectBytes[0]);

								if (connectNum < 0) 
								{
									_hasBadConnects = true;
								}

								// we can only have 4
								switch (y) 
								{
									case 0:
										node.Connect1 = connectNum;
										break;
									case 1:
										node.Connect2 = connectNum;
										break;
									case 2:
										node.Connect3 = connectNum;
										break;
									case 3:
										node.Connect4 = connectNum;
										break;
								}
							}
						}
						
						_nodes.Add(node);
					}

					// first coupla bytes here should be the count of actions :)
					int numberOfActions = GetInt(readBytes[pos + 1], readBytes[pos]);

					if (numberOfActions > 0) 
					{
						// skip past the bytes for the action count
						pos += 2;
						for (int x = 0; x < numberOfActions; x++) 
						{
							Action action = new Action();

							byte[] actionBytes = new byte[52];
							ReadSubBytes(readBytes, actionBytes, pos);
							pos += 52;

							action.Bytes = actionBytes;
							action.ID = x;
							action.AllyAction = GetInt(actionBytes[9], actionBytes[8]);
							action.AxisAction = GetInt(actionBytes[11], actionBytes[10]);
							action.Group = GetInt(actionBytes[7], actionBytes[6]);
							action.Active = GetInt(actionBytes[1], actionBytes[0]);
							action.CloseNode = GetInt(actionBytes[5], actionBytes[4]);
							action.Entity = GetInt(actionBytes[27], actionBytes[26]);
							action.Radius = GetInt(actionBytes[25], actionBytes[24]);
							action.Class = GetInt(actionBytes[29], actionBytes[28]);
							action.Goal = GetInt(actionBytes[31], actionBytes[30]);
							action.Prone = GetInt(actionBytes[33], actionBytes[32]);

							if (!((int)actionBytes[44] == 255 && (int)actionBytes[45] == 255)) 
							{
								action.Links = Convert.ToString(GetInt(actionBytes[45], actionBytes[44]));

								if (!((int)actionBytes[46] == 255 && (int)actionBytes[47] == 255)) 
								{
									action.Links +=  " " + Convert.ToString(GetInt(actionBytes[47], actionBytes[46]));

									if (!((int)actionBytes[48] == 255 && (int)actionBytes[49] == 255)) 
									{
										action.Links +=  " " + Convert.ToString(GetInt(actionBytes[49], actionBytes[48]));

										if (!((int)actionBytes[50] == 255 && (int)actionBytes[51] == 255)) 
										{
											action.Links +=  " " + Convert.ToString(GetInt(actionBytes[51], actionBytes[50]));
										}
									}
								}
							}

							_actions.Add(action);
						}
					}
				}

				// first coupla bytes here should be the count of routes
				int numberOfRoutes = GetInt(readBytes[pos + 1], readBytes[pos]);

				if (numberOfRoutes > 0) 
				{
					// skip past the bytes for the action count
					pos += 2;
					for (int x = 0; x < numberOfRoutes; x++) 
					{
						Route route = new Route();

						byte[] routeBytes = new byte[40];
						ReadSubBytes(readBytes, routeBytes, pos);
						pos += 40;

						route.Bytes = routeBytes;
						route.ID = x;
						route.Team = GetInt(routeBytes[15], routeBytes[14]);
						route.Radius = GetInt(routeBytes[13], routeBytes[12]);

						if (!((int)routeBytes[17] == 255 && (int)routeBytes[16] == 255)) 
						{
							route.Actions = Convert.ToString(GetInt(routeBytes[17], routeBytes[16]));

							if (!((int)routeBytes[19] == 255 && (int)routeBytes[18] == 255)) 
							{
								route.Actions +=  " " + Convert.ToString(GetInt(routeBytes[19], routeBytes[18]));

								if (!((int)routeBytes[21] == 255 && (int)routeBytes[20] == 255)) 
								{
									route.Actions +=  " " + Convert.ToString(GetInt(routeBytes[21], routeBytes[20]));

									if (!((int)routeBytes[23] == 255 && (int)routeBytes[22] == 255)) 
									{
										route.Actions +=  " " + Convert.ToString(GetInt(routeBytes[23], routeBytes[22]));

										if (!((int)routeBytes[25] == 255 && (int)routeBytes[24] == 255)) 
										{
											route.Actions +=  " " + Convert.ToString(GetInt(routeBytes[25], routeBytes[24]));

											if (!((int)routeBytes[27] == 255 && (int)routeBytes[26] == 255)) 
											{
												route.Actions +=  " " + Convert.ToString(GetInt(routeBytes[27], routeBytes[26]));
											}
										}
									}
								}
							}
						}

						if (!((int)routeBytes[29] == 255 && (int)routeBytes[28] == 255)) 
						{
							route.PathActions = Convert.ToString(GetInt(routeBytes[29], routeBytes[28]));

							if (!((int)routeBytes[31] == 255 && (int)routeBytes[30] == 255)) 
							{
								route.PathActions +=  " " + Convert.ToString(GetInt(routeBytes[31], routeBytes[30]));

								if (!((int)routeBytes[33] == 255 && (int)routeBytes[32] == 255)) 
								{
									route.PathActions +=  " " + Convert.ToString(GetInt(routeBytes[33], routeBytes[32]));

									if (!((int)routeBytes[35] == 255 && (int)routeBytes[34] == 255)) 
									{
										route.PathActions +=  " " + Convert.ToString(GetInt(routeBytes[35], routeBytes[34]));

										if (!((int)routeBytes[37] == 255 && (int)routeBytes[36] == 255)) 
										{
											route.PathActions +=  " " + Convert.ToString(GetInt(routeBytes[37], routeBytes[36]));

											if (!((int)routeBytes[39] == 255 && (int)routeBytes[38] == 255)) 
											{
												route.PathActions +=  " " + Convert.ToString(GetInt(routeBytes[39], routeBytes[38]));
											}
										}
									}
								}
							}
						}

						_routes.Add(route);
					}
				}
			} 
			finally 
			{
				if (stream != null) stream.Close();
			}
		}

		private int GetInt(byte left, byte right) 
		{
			byte[] bytes = new byte[2];
			bytes[0] = left;
			bytes[1] = right;
			Bits bits = new Bits(bytes);
			return bits.ToInt();
		}

		private void ReadSubBytes(byte[] stream, byte[] buff, int pos) 
		{
			for (int x = 0; x < buff.Length; x++) 
			{
				buff[x] = stream[pos + x];
			}
		}
	}
}
