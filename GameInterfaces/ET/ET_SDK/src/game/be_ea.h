
/*****************************************************************************
 * name:		be_ea.h
 *
 * desc:		elementary actions
 *
 *
 *****************************************************************************/

//ClientCommand elementary actions
void EA_Say(int client, char *str);
void EA_SayTeam(int client, char *str);
void EA_UseItem(int client, char *it);
void EA_DropItem(int client, char *it);
void EA_UseInv(int client, char *inv);
void EA_DropInv(int client, char *inv);
void EA_Command(int client, char *command );
//regular elementary actions
void EA_SelectWeapon(int client, int weapon);
void EA_Attack(int client);
void EA_Reload(int client);
void EA_Respawn(int client);
void EA_Talk(int client);
void EA_Gesture(int client);
void EA_Use(int client);
void EA_Jump(int client);
void EA_DelayedJump(int client);
void EA_Crouch(int client);
void EA_Walk(int client);
void EA_MoveUp(int client);
void EA_MoveDown(int client);
void EA_MoveForward(int client);
void EA_MoveBack(int client);
void EA_MoveLeft(int client);
void EA_MoveRight(int client);
void EA_Move(int client, vec3_t dir, float speed);
void EA_View(int client, vec3_t viewangles);
void EA_Prone(int client);
//send regular input to the server
void EA_EndRegular(int client, float thinktime);
void EA_GetInput(int client, float thinktime, bot_input_t *input);
void EA_ResetInput(int client, bot_input_t *init);
//setup and shutdown routines
int EA_Setup(void);
void EA_Shutdown(void);
