
/*****************************************************************************
 * name:		ai_dmnet_mp.h
 *
 * desc:		Wolf bot AI
 *
 *
 *****************************************************************************/

void AIEnter_MP_PlantMine(bot_state_t *bs);
void AIEnter_MP_Intermission(bot_state_t *bs);
void AIEnter_MP_Observer(bot_state_t *bs);
void AIEnter_MP_Respawn(bot_state_t *bs);
void AIEnter_MP_Stand(bot_state_t *bs);
void AIEnter_MP_Seek_ActivateEntity(bot_state_t *bs);
void AIEnter_MP_Seek_NBG(bot_state_t *bs);
void AIEnter_MP_Seek_LTG(bot_state_t *bs);
void AIEnter_MP_Seek_Camp(bot_state_t *bs);
void AIEnter_MP_Battle_Fight(bot_state_t *bs);
void AIEnter_MP_Battle_Chase(bot_state_t *bs);
void AIEnter_MP_Battle_Retreat(bot_state_t *bs);
void AIEnter_MP_DynamiteTarget(bot_state_t *bs);
void AIEnter_MP_SatchelChargeTarget(bot_state_t *bs);
void AIEnter_MP_ConstructibleTarget(bot_state_t *bs);
void AIEnter_MP_TouchTarget(bot_state_t *bs);
void AIEnter_MP_DefendTarget(bot_state_t *bs);
void AIEnter_MP_MedicRevive(bot_state_t *bs);
void AIEnter_MP_MedicGiveHealth(bot_state_t *bs);
void AIEnter_MP_DisarmDynamite(bot_state_t *bs);
void AIEnter_MP_AvoidDanger( bot_state_t *bs );
void AIEnter_MP_GiveAmmo(bot_state_t *bs);
void AIEnter_MP_PanzerTarget(bot_state_t *bs);
void AIEnter_MP_SniperSpot(bot_state_t *bs);
void AIEnter_MP_ScanForLandmines(bot_state_t *bs);
void AIEnter_MP_FixMG42(bot_state_t *bs);
void AIEnter_MP_MG42Scan(bot_state_t *bs);
void AIEnter_MP_MG42Mount(bot_state_t *bs);
void AIEnter_MP_Script_MoveToMarker(bot_state_t *bs);
void AIEnter_MP_MoveToAutonomyRange(bot_state_t *bs);
void AIEnter_MP_PanzerTarget(bot_state_t *bs);
void AIEnter_MP_AttackTarget( bot_state_t *bs );
void AIEnter_MP_NavigateFromVoid(bot_state_t *bs);
void AIEnter_MP_Battle_MobileMG42( bot_state_t *bs );
int AINode_MP_PlantMine(bot_state_t *bs);
int AINode_MP_Intermission(bot_state_t *bs);
int AINode_MP_Observer(bot_state_t *bs);
int AINode_MP_Respawn(bot_state_t *bs);
int AINode_MP_Stand(bot_state_t *bs);
int AINode_MP_Seek_ActivateEntity(bot_state_t *bs);
int AINode_MP_Seek_NBG(bot_state_t *bs);
int AINode_MP_Battle_Fight(bot_state_t *bs);
int AINode_MP_Battle_Chase(bot_state_t *bs);
int AINode_MP_Battle_Retreat(bot_state_t *bs);
int AINode_MP_DynamiteTarget(bot_state_t *bs);
int AINode_MP_SatchelChargeTarget(bot_state_t *bs);
int AINode_MP_ConstructibleTarget(bot_state_t *bs);
int AINode_MP_TouchTarget(bot_state_t *bs);
int AINode_MP_DefendTarget(bot_state_t *bs);
int AINode_MP_MedicRevive(bot_state_t *bs);
int AINode_MP_MedicGiveHealth(bot_state_t *bs);
int AINode_MP_DisarmDynamite(bot_state_t *bs);
int AINode_MP_AvoidDanger(bot_state_t *bs);
int AINode_MP_GiveAmmo(bot_state_t *bs);
int AINode_MP_PanzerTarget( bot_state_t *bs );
int AINode_MP_SniperSpot(bot_state_t *bs);
int AINode_MP_FixMG42(bot_state_t *bs);
int AINode_MP_MG42Scan(bot_state_t *bs);
int AINode_MP_MG42Mount(bot_state_t *bs);
int AINode_MP_ScanForLandmines(bot_state_t *bs);
int AINode_MP_Script_MoveToMarker(bot_state_t *bs);
int AINode_MP_MoveToAutonomyRange(bot_state_t *bs);
int AINode_MP_AttackTarget( bot_state_t *bs );
int AINode_MP_NavigateFromVoid(bot_state_t *bs);
int AINode_MP_Battle_MobileMG42(bot_state_t *bs);
