using System;

namespace WaypointTool
{
	public class KeywordFactory
	{
		public static KeywordCollection GetKeywords() 
		{
			KeywordCollection collection = new KeywordCollection();

			collection.Add(GetIfObjHomeFalse());
			collection.Add(GetIfObjHomeTrue());
			collection.Add(GetIfConstructBuiltFalse());
			collection.Add(GetIfConstructBuiltTrue());
			collection.Add(GetIfFlagOwnerAxis());
			collection.Add(GetIfFlagOwnerAllies());
			collection.Add(GetNodeConnect());
			collection.Add(GetNodeDisconnect());
			collection.Add(GetIfObjCaptured());
			collection.Add(GetActivateAction());
			collection.Add(GetDeactivateAction());
			collection.Add(GetActivateActionGroup());
			collection.Add(GetDeactivateActionGroup());
			collection.Add(GetResetNodeFlag());
			collection.Add(GetActionTrue());
			collection.Add(GetActionFalse());
			collection.Add(GetBotSwitchToWeap());
			collection.Add(GetWaitCommand());
			collection.Add(GetIfAlliedCpBuiltTrue());
			collection.Add(GetIfAlliedCpBuiltFalse()); 
			collection.Add(GetIfAxisCpBuiltTrue()); 
			collection.Add(GetIfAxisCpBuiltFalse());
            //crapshoot: add some missing ones
            collection.Add(GetSetMapHasVehicle());
            collection.Add(GetSetVehicleNumber());
            collection.Add(GetSetVehicleOwner());
            collection.Add(GetKillAction());
            collection.Add(GetOpenNodeGrpToTeam());
            //end crapshoot

			collection.Sort(KeywordComparer.SortBy.FriendlyName);

			return collection;
		}

        //crapshoot: openNodeGrpToTeam
        private static Keyword GetOpenNodeGrpToTeam()
        {
            Keyword keyword = new Keyword();

            keyword.Command = "openNodeGrpToTeam";
            keyword.FriendlyName = "Open Node Group";
            keyword.HelpText = "Used to open a group of nodes to a team";

            keyword.Inputs.Add(new KeywordInput(KeywordInputType.Group, "Node Group", "The Group ID of nodes to open."));

            KeywordInput input = new KeywordInput(KeywordInputType.PredefinedList, "Team", "The Team to open the nodes for.");
            input.AllowableValues.Add("axis");
            input.AllowableValues.Add("allies");
            keyword.Inputs.Add(input);

            return keyword;
        }

        //crapshoot: Kill_Action
        private static Keyword GetKillAction()
        {
            Keyword keyword = new Keyword();

            keyword.Command = "kill_action";
            keyword.FriendlyName = "Kill Action";
            keyword.HelpText = "Set action types to -1";

            keyword.Inputs.Add(new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated."));

            return keyword;
        }

        //crapshoot: set_Vehicle_Owner
        private static Keyword GetSetVehicleOwner()
        {
            Keyword keyword = new Keyword();

            keyword.Command = "set_Vehicle_Owner";
            keyword.FriendlyName = "Vehicle Owner";
            keyword.HelpText = "Who owns the vehicle?";

            KeywordInput input = new KeywordInput(KeywordInputType.PredefinedList, "Vehicle Owner", "Who Owns the Vehicle");
            input.AllowableValues.Add("allies");
            input.AllowableValues.Add("axis");
            keyword.Inputs.Add(input);

            return keyword;
        }

        //crapshoot: set_Vehicle_Number
        private static Keyword GetSetVehicleNumber()
        {
            Keyword keyword = new Keyword();

            keyword.Command = "set_Vehicle_Number";
            keyword.FriendlyName = "Vehicle Number";
            keyword.HelpText = "The entity number of the vehicle (script_mover)";

            KeywordInput entity = new KeywordInput(KeywordInputType.Integer, "Entity Number", "Entity Number for Vehicle.");
            keyword.Inputs.Add(entity);

            return keyword;
        }

        //crapshoot: set_Map_Has_Vehicle
        private static Keyword GetSetMapHasVehicle()
        {
            Keyword keyword = new Keyword();

            keyword.Command = "set_Map_Has_Vehicle";
            keyword.FriendlyName = "Map Has Vehicle";
            keyword.HelpText = "Used for turning movers on and off";

            KeywordInput input = new KeywordInput(KeywordInputType.PredefinedList, "Has Vehicle", "Whether or not a Vehicle is active.");
            input.AllowableValues.Add("true");
            input.AllowableValues.Add("false");
            keyword.Inputs.Add(input);

            return keyword;
        }

		private static Keyword GetBotSwitchToWeap() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "bot_switchToWeap";
			keyword.FriendlyName = "Switch Weapons";
			keyword.HelpText = "Will cause all soldiers on the specified team to switch to the defined weapon.";

			KeywordInput weapon = new KeywordInput(KeywordInputType.PredefinedList, "Weapon", "The weapon to switch to.");
			weapon.AllowableValues.Add("FG42");
			weapon.AllowableValues.Add("flame");
			weapon.AllowableValues.Add("garand");
			weapon.AllowableValues.Add("k43");
			weapon.AllowableValues.Add("MG42");
			weapon.AllowableValues.Add("mortar");
			weapon.AllowableValues.Add("panzer");
			weapon.AllowableValues.Add("random");
			weapon.AllowableValues.Add("random_f");
			weapon.AllowableValues.Add("random_fp");
			weapon.AllowableValues.Add("random_p");
			weapon.AllowableValues.Add("random_pf");
			weapon.AllowableValues.Add("smg");
			weapon.AllowableValues.Add("sniper");
			weapon.AllowableValues.Add("venom");

			keyword.Inputs.Add(weapon);

			KeywordInput team = new KeywordInput(KeywordInputType.PredefinedList, "Team", "The team to switch weapons for");
			team.AllowableValues.Add("allies");
			team.AllowableValues.Add("axis");

			keyword.Inputs.Add(team);

			return keyword;
		}

		private static Keyword GetIfObjHomeFalse() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_obj_home_false";
			keyword.FriendlyName = "If Objective Not Home";
			keyword.HelpText = "Used for determining if an objective has been stolen. The action associated with this Keyword is typically a 'Steal' action for documents.";
			keyword.AllowConditional = true;

			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(8);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetWaitCommand() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "wait";
			keyword.FriendlyName = "Wait";
			keyword.HelpText = "Used to wait a specified period (in seconds) before executing the next lines command.";

			KeywordInput seconds = new KeywordInput(KeywordInputType.Integer, "Seconds", "The amount of seconds to wait before executing the next command.");
			keyword.Inputs.Add(seconds);

			return keyword;
		}

		private static Keyword GetIfObjHomeTrue() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_obj_home_true";
			keyword.FriendlyName = "If Objective Is Home";
			keyword.HelpText = "Used for determining if an objective is in place. The action associated with this Keyword is typically a 'Steal' action for documents.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(8);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfAlliedCpBuiltTrue() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_allied_cp_built_true";
			keyword.FriendlyName = "If Allied Command Post Built";
			keyword.HelpText = "Used for determining if an Allied Command Post is built.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(22);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfAlliedCpBuiltFalse() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_allied_cp_built_false";
			keyword.FriendlyName = "If Allied Command Post Is Not Built";
			keyword.HelpText = "Used for determining if an Allied Command Post is not built.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(22);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfAxisCpBuiltTrue() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_axis_cp_built_true";
			keyword.FriendlyName = "If Axis Command Post Built";
			keyword.HelpText = "Used for determining if an Axis Command Post is built.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(22);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfAxisCpBuiltFalse() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_axis_cp_built_false";
			keyword.FriendlyName = "If Axis Command Post Is Not Built";
			keyword.HelpText = "Used for determining if an Axis Command Post is not built.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(22);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfConstructBuiltFalse() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_construct_built_false";
			keyword.FriendlyName = "If Constructible Not Built";
			keyword.HelpText = "Used for determining if an item needs constructing. Associate a construct major or minor action with this command.";
			keyword.AllowConditional = true;

			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(19);
			action.AllowableActions.Add(2);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfConstructBuiltTrue() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_construct_built_true";
			keyword.FriendlyName = "If Constructible Is Built";
			keyword.HelpText = "Used for determining if an has been constructed. Associate a construct major or minor action with this command.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(19);
			action.AllowableActions.Add(2);
			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfFlagOwnerAxis() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_fda_owner_axis";
			keyword.FriendlyName = "If Axis Have Flag";
			keyword.HelpText = "Used for determining if the Axis have the flag. Associate a 'Spawn Flag' action with this command.";
			keyword.AllowConditional = true;

			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(6);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetIfFlagOwnerAllies() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_fda_owner_allies";
			keyword.FriendlyName = "If Allies Have Flag";
			keyword.HelpText = "Used for determining if the Allies have the flag. Associate a 'Spawn Flag' action with this command.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(6);

			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetNodeConnect() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "node_connect";
			keyword.FriendlyName = "Node Connect";
			keyword.HelpText = "Used to connect nodes";
			
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Node, "Node ID 1", "The ID of the first node to connect."));
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Node, "Node ID 2", "The ID of the second node to connect."));
			
			KeywordInput input = new KeywordInput(KeywordInputType.PredefinedList, "Two Way Connect", "Whether or not this is a two way connection.");
			input.AllowableValues.Add("true");
			input.AllowableValues.Add("false");
			keyword.Inputs.Add(input);

			return keyword;
		}

		private static Keyword GetNodeDisconnect() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "node_disconnect";
			keyword.FriendlyName = "Node Disconnect";
			keyword.HelpText = "Used to disconnect nodes";
			
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Node, "Node ID 1", "The ID of the first node to disconnect."));
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Node, "Node ID 2", "The ID of the second node to discconnect."));

			KeywordInput input = new KeywordInput(KeywordInputType.PredefinedList, "Two Way Disconnect", "Whether or not this is a two way connection.");
			input.AllowableValues.Add("true");
			input.AllowableValues.Add("false");
			keyword.Inputs.Add(input);
			return keyword;
		}

		private static Keyword GetIfObjCaptured() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_obj_captured";
			keyword.FriendlyName = "If An Objective Captured";
			keyword.HelpText = "Used for determining if an objective has been captured. Associate the action that represents the objective.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(8);
			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetActivateAction() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "activateAction";
			keyword.FriendlyName = "Action Activate";
			keyword.HelpText = "Used to activate an action.";
			
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated."));

			return keyword;
		}

		private static Keyword GetDeactivateAction() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "deactivateAction";
			keyword.FriendlyName = "Action Deactivate";
			keyword.HelpText = "Used to deactivate an action.";
			
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated."));

			return keyword;
		}

		private static Keyword GetActivateActionGroup() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "activateAction_Group";
			keyword.FriendlyName = "Action Group Activate";
			keyword.HelpText = "Used to activate an action group.";
			
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated."));

			return keyword;
		}

		private static Keyword GetDeactivateActionGroup() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "deactivateAction_Group";
			keyword.FriendlyName = "Action Group Deactivate";
			keyword.HelpText = "Used to deactivate an action group.";
			
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated."));

			return keyword;
		}

		private static Keyword GetResetNodeFlag() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "resetNodeFlag";
			keyword.FriendlyName = "Reset Node Flag";
			keyword.HelpText = "Used to set a nodes flag as the result of an action.";
			
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Node, "Node ID", "The ID of the Action for which the objective is associated."));
			keyword.Inputs.Add(new KeywordInput(KeywordInputType.Flag, "Flag", "The new node flag value."));

			return keyword;
		}

		private static Keyword GetActionTrue() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_action_true";
			keyword.FriendlyName = "Action Complete";
			keyword.HelpText = "Used for determining if a given action has been completed.";
			keyword.AllowConditional = true;
			
			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(2);
			action.AllowableActions.Add(4);
			action.AllowableActions.Add(6);
			action.AllowableActions.Add(8);
			action.AllowableActions.Add(19);
			action.AllowableActions.Add(22);
            //add some that are missing -crapshoot
            action.AllowableActions.Add(33); //expire
            action.AllowableActions.Add(34); //explode
            //this technically can be used with any action because it just checks goalnum
			keyword.Inputs.Add(action);

			return keyword;
		}

		private static Keyword GetActionFalse() 
		{
			Keyword keyword = new Keyword();

			keyword.Command = "if_action_false";
			keyword.FriendlyName = "Action Not Complete";
			keyword.HelpText = "Used for determining if a given action has not been completed.";
			keyword.AllowConditional = true;

			KeywordInput action = new KeywordInput(KeywordInputType.Action, "Action ID", "The ID of the Action for which the objective is associated.");
			action.AllowableActions.Add(2);
			action.AllowableActions.Add(4);
			action.AllowableActions.Add(6);
			action.AllowableActions.Add(8);
			action.AllowableActions.Add(19);
			action.AllowableActions.Add(22);
            //add some that are missing -crapshoot
            action.AllowableActions.Add(33); //expire
            action.AllowableActions.Add(34); //explode
            //this technically can be used with any action because it just checks goalnum
			keyword.Inputs.Add(action);

			return keyword;
		}
	}
}
