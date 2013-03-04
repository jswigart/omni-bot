using System;
using System.Collections;
using System.IO;
using System.Text;

namespace WaypointTool
{
	public class BspReader
	{
		public struct BspEntry
		{
			//size = 8
			int offset;
			int length;

			public int Offset
			{
				get { return offset; }
			}
			public int Length
			{
				get { return length; }
			}
			public static int Size
			{
				get { return 8; }
			}

			public void Read(BinaryReader br)
			{
				offset = br.ReadInt32();
				length = br.ReadInt32();
			}
		}

		public struct BspHeader
		{
			//size = 12
			char[] idNumber;
			int version;
			BspEntry[] entries;

			public BspEntry[] Entries
			{
				get { return entries; }
			}
			
			public static int Size
			{
				get { return 12; }
			}

			public void Read(BinaryReader br)
			{
				idNumber = new char[4];
				version = 0;
				entries = new BspEntry[17];
				//read the data into the BSP_HEADER
				idNumber = br.ReadChars(4);
				version = br.ReadInt32();
				for(int i = 0; i < entries.Length; i++)
				{
					entries[i].Read(br); //read each of the structures into the bsp header structure
				}
			}
		}

		private Hashtable GetIgnoredEntityClasses() 
		{
			Hashtable hash = new Hashtable();

			hash.Add("UNDEFINED", "");
			hash.Add("light", "");
			hash.Add("lightJunior", "");
			hash.Add("path_corner_2", "");
			hash.Add("info_train_spline_control", "");
			hash.Add("info_train_spline_main", "");
			hash.Add("misc_model", "");

			/*
			hash.Add("info_limbo_camera", "");
			
			hash.Add("misc_light_surface", "");
			*/

			return hash;
		}

		private Hashtable GetConditionalIgnoredEntityClasses() 
		{
			Hashtable hash = new Hashtable();
			
			hash.Add("misc_vis_dummy", "target");
			hash.Add("misc_vis_dummy_multiple", "targetname");
			hash.Add("info_null", "targetname");
			hash.Add("path_corner", "targetname");

			return hash;
		}

		public ArrayList Read(string filename)
		{
			//open the file for reading
			FileStream fs = null;

			try 
			{
				fs = new FileStream(filename, FileMode.Open, FileAccess.Read);
				BinaryReader reader = new BinaryReader(fs);
				//read the contens from the file
				//init the header
				BspHeader header = new BspHeader();
				header.Read(reader);
				//allocate space needed for the data structures

				int entityOffset = header.Entries[0].Offset;
				int entityLength = header.Entries[0].Length;

				fs.Seek(entityOffset, SeekOrigin.Begin);

				ArrayList entities = new ArrayList();

				StringBuilder entityText = new StringBuilder();

				int count = 72; // entities start at this number?

				bool inEnt = false;

				Hashtable ignored = GetIgnoredEntityClasses();
				Hashtable conditional = GetConditionalIgnoredEntityClasses();
				Hashtable targetNames = new Hashtable();

				ArrayList spawners = new ArrayList();

				for (int x = 0; x < entityLength; x++) 
				{
					char read = reader.ReadChar();

					if (inEnt || read == '{') 
					{
						entityText.Append(read);
						inEnt = true;
					}

					if (read == '}') 
					{
						BspEntity ent = new BspEntity(0, entityText.ToString());

						bool ignore = false;
						if (ignored.ContainsKey(ent.ClassName)) 
						{
							ignore = true;
						} 
						else if (conditional.ContainsKey(ent.ClassName)) 
						{
							string propNeeded = (string)conditional[ent.ClassName];

							if (ent.GetProperty(propNeeded) == "UNDEFINED") 
							{
								ignore = true;
							}
						} 

						if (ent.ClassName == "misc_gamemodel" &&
							ent.TargetName == "UNDEFINED" &&
							ent.ScriptName == "UNDEFINED" &&
							ent.SpawnFlags == "UNDEFINED")
						{
							ignore = true;
						}

						if (!ignore) 
						{
							if (ent.ClassName == "worldspawn") 
							{
								ent.Number = 1023;
								entities.Add(ent);
							} 
							else 
							{
								if (ent.ClassName == "misc_mg42") 
								{
									spawners.Add(ent); // mg42's here are just placements, the engine creates em
								} 
								else if (ent.ClassName == "misc_flak" || ent.ClassName == "misc_grabber_trap") 
								{
									BspEntity spawned = new BspEntity(0, "This is a spawned entity not defined in the bsp");
									spawners.Add(spawned);

									ent.Number = count;
									entities.Add(ent);
								} 
								else if (ent.ClassName == "func_plat") 
								{
									if (ent.TargetName == "UNDEFINED") 
									{
										BspEntity spawned = new BspEntity(0, "This is a spawned entity not defined in the bsp");
										spawners.Add(spawned);
									}
									ent.Number = count;
									entities.Add(ent);
								}
								else if (ent.ClassName == "func_door_rotating") 
								{
									/*
									else if ((ent->spawnflags & 8) && (strcmp( ent->classname, "func_door_rotating" )))
										ent->think = Think_SpawnNewDoorTrigger;
									*/
									bool handledDoor = false;
									if (ent.SpawnFlags != "UNDEFINED") 
									{
										try 
										{
											int flags = Convert.ToInt32(ent.SpawnFlags);

											if ((flags & 8) == 8) 
											{
												BspEntity doorTrigger = new BspEntity(0, "This is a spawned entity not defined in the bsp");
												
												spawners.Add(doorTrigger);

												ent.Number = count;
												entities.Add(ent);

												handledDoor = true;
											} 
										} 
										catch {}
									} 

									if (!handledDoor) 
									{
										ent.Number = count;
										entities.Add(ent);
									}
								} 
								else if (ent.ClassName == "trigger_objective_info") 
								{
									BspEntity checkIndicator = new BspEntity(0, ent.Text);
									spawners.Add(checkIndicator);

									ent.Number = count;
									entities.Add(ent);
								}
								else 
								{
									ent.Number = count;
									entities.Add(ent);
								}
								count++;
							}
						}

						string targetKey = ent.TargetName;

						if (targetKey != "UNDEFINED" && !targetNames.ContainsKey(targetKey)) 
						{
							targetNames.Add(targetKey, ent);
						}

						entityText = new StringBuilder();
						inEnt = false;
					} 
				}

				foreach (BspEntity spawner in spawners) 
				{
					bool addSpawner = false;
					if (spawner.ClassName == "misc_mg42") 
					{
						count++; // skip the base
						spawner.Number = count;
						addSpawner = true;
					} 
					else if (spawner.ClassName == "trigger_objective_info") 
					{
						// check to see if there should be exlosive or construct indicators
						// spawned for this item
						if (spawner.Target != "UNDEFINED") 
						{
							BspEntity target = targetNames[spawner.Target] as BspEntity;

							if (target != null) 
							{
								if (target.ClassName == "func_constructible") 
								{
									spawner.Number = count;
									spawner.Text = "{ \"classname\" \"constructible_indicator\" }";
									addSpawner = false; // added in the fourth frame
								} 
								else if (target.ClassName == "func_explosive" && target.SpawnFlags != "UNDEFINED") 
								{
									// if there is an axis or ally spawnflags bit, add it
									try 
									{
										int spawnflags = Convert.ToInt32(spawner.SpawnFlags);

										if (((spawnflags & 1) == 1) || ((spawnflags & 2) == 2)) 
										{
											spawner.Number = count;
											spawner.Text = "{ \"classname\" \"explosive_indicator\" }";
											addSpawner = false; // added in the fourth frame
										}
									}	
									catch {}
								}
							}
						}
					}
					else 
					{
						spawner.Number = count;
						addSpawner = true;
					}

					if (addSpawner) 
					{
						entities.Add(spawner);
						count++;
					}
				}
				return entities;
			} 
			finally 
			{
				if (fs != null) 
				{
					fs.Close();
				}
			}
		}
	}
}
