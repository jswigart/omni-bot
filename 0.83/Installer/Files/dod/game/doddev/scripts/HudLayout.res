"Resource/HudLayout.res"
{
	HudPlayerStatusPanel
	{
		"fieldName"				"HudPlayerStatusPanel"
		"xpos"					"0"
		"ypos"					"290"
		"wide"					"640"
		"tall"					"190"
		"visible"				"1"
		"enabled"				"1"
	}
	HudCapturePanel
	{
		"fieldName"				"HudCapturePanel"
		"xpos"					"c-75"
		"ypos"					"c80"
		"wide"					"150"
		"tall"					"90"
		"visible"				"1"
		"enabled"				"1"
		"icon_space"			"2"
	}
	HudObjectiveIcons
	{
		"fieldName"				"HudObjectiveIcons"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"600"
		"tall"					"200"
		"visible"				"1"
		"enabled"				"1"
		"iconsize"				"24"
		"icon_start_x"			"15"
		"icon_start_y"			"15"
		"separator_width"		"5"		// distance between the icons (including their backgrounds)
		"CornerCutSize"			"5"
		"BackgroundOverlap"		"4"		// amount the background overlaps the icons (width and height)
		
		"TimerBG"				"HudPanelObjectivesTimer"
		"TimerBG_Flash"			"HudPanelObjectivesTimer_Flash"
		
		"TimerFont"				"DefaultLarge"
		
		"time_added_height"		"12"
		
	}
	HudHintDisplay
	{
		"fieldName"				"HudHintDisplay"
		"visible"				"0"
		"enabled"				"1"
		"xpos"					"c-240"
		"ypos"					"c20"
		"wide"					"480"
		"tall"					"50"
		"text_xpos"				"8"
		"text_ypos"				"8"
		"center_x"				"0"		// center text horizontally
		"center_y"				"-1"	// align text on the bottom
	}	
	HudHintKeyDisplay
	{
		"fieldName"	"HudHintKeyDisplay"
		"visible"	"0"
		"enabled" 	"1"
		"xpos"		"r120"
		"ypos"		"r340"
		"wide"		"100"
		"tall"		"200"
		"text_xpos"	"8"
		"text_ypos"	"8"
		"text_xgap"	"8"
		"text_ygap"	"8"
		"TextColor"	"255 170 0 220"

		"PaintBackgroundType"	"2"
	}
	overview
	{
		"fieldname"				"overview"
		"visible"				"1"
		"enabled"				"1"
		"xpos"					"0"
		"ypos"					"480"
		"wide"					"0"
		"tall"					"0"
	}
	HudWeaponSelection
	{
		"fieldName"				"HudWeaponSelection"
		"xpos"					"74"
		"ypos" 					"0"
		"wide"					"100"
		"tall"					"397"
		"visible"				"1"
		"enabled"				"1"
		"SmallBoxWide"			"72"
		"SmallBoxTall"			"32"
		"SmallBoxCutSize"		"5"
		"LargeBoxWide"			"96"
		"LargeBoxTall"			"36"
		"LargeBoxCutSize"		"5"
		"BoxGap"				"8"
		"ActiveTextColor"		"WeaponSelect.ActiveText"
		"InactiveTextColor"		"WeaponSelect.InactiveText"
		"ActiveBoxColor"		"WeaponSelect.ActiveBox"
		"ActiveBoxBorder"		"WeaponSelect.ActiveBorder"
		"InactiveBoxColor"		"WeaponSelect.InactiveBox"
		"InactiveBoxBorder"		"WeaponSelect.InactiveBorder"	
		"NotUseableColor"		"WeaponSelect.NotUsable"	
		"MaxSlots"				"5"
		"PlaySelectSounds"		"0"
		"SelectionNumberXPos"	"4"		// units from the left of the panel
		"SelectionNumberYPos"	"10"	// units from the bottom of the panel
	}	
	TargetID
	{
		"fieldName"				"TargetID"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudZoom
	{
		"fieldName"				"HudZoom"
		"visible"				"1"
		"enabled"				"1"
		"Circle1Radius"			"66"
		"Circle2Radius"			"74"
		"DashGap"				"16"
		"DashHeight"			"4"
		"BorderThickness"		"88"
	}
	HudDODCrosshair
	{
		"fieldName"				"HudDODCrosshair"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudSpecCrosshair
	{
		"fieldName"				"HudSpecCrosshair"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudDeathNotice
	{
		"fieldName"				"HudDeathNotice"
		"visible"				"1"
		"enabled"				"1"
		"xpos"					"r640"
		"wide"					"640"
		"tall"					"480"

		"MaxDeathNotices"		"4"
		"LineHeight"			"19"
		"RightJustify"			"1"	// If 1, draw notices from the right
		
		"TextFont"				"ChatFont"
		"CTTextColor"			"CT_Blue"
		"TerroristTextColor"	"T_Red"	
		
		"BackgroundColor"		"HudBackground"
		"ActiveBackgroundColor"	"HudBackgroundActiveSolid"
	}
	HudVehicle
	{
		"fieldName"				"HudVehicle"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	CVProfPanel
	{
		"fieldName"				"CVProfPanel"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	ScorePanel
	{
		"fieldName"				"ScorePanel"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudTrain
	{
		"fieldName"				"HudTrain"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudMOTD
	{
		"fieldName"				"HudMOTD"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudMessage
	{
		"fieldName"				"HudMessage"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudMenu
	{
		"fieldName"				"HudMenu"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"

		"TextFont"				"Default"
		"ItemFont"				"Default"
		"ItemFontPulsing"		"Default"

	}
	HudCloseCaption
	{
		"fieldName"				"HudCloseCaption"
		"visible"				"1"
		"enabled"				"1"
		"wide"					"640"
		"tall"					"480"
	}
	HudHistoryResource
	{
		"fieldName"				"HudHistoryResource"
		"visible" 				"0"
		"enabled" 				"1"
		"xpos"	 				"0"
		"wide"	 				"0"
		"tall"	 				"0"
	}
	HUDQuickInfo
	{
		"fieldName"				"HUDQuickInfo"
		"visible" 				"1"
		"enabled" 				"1"
		"wide"	 				"640"
		"tall"	 				"480"
	}
	HudAnimationInfo
	{
		"fieldName"				"HudAnimationInfo"
		"visible" 				"1"
		"enabled" 				"1"
		"wide"	 				"640"
		"tall"	 				"480"
	}
	CBudgetPanel
	{
		"fieldName"				"CBudgetPanel"
		"visible" 				"1"
		"enabled"				"1"
		"wide"	 				"640"
		"tall"	 				"480"
	}
	CTextureBudgetPanel
	{
		"fieldName"				"CTextureBudgetPanel"
		"visible" 				"1"
		"enabled" 				"1"
		"wide"	 				"640"
		"tall"					"480"
	}
	HudPredictionDump
	{
		"fieldName"				"HudPredictionDump"
		"visible"				"1"
		"enabled" 				"1"
		"wide"	 				"640"
		"tall"	 				"480"
	}
	HudScope
	{
		"fieldName"				"HudZoom"
		"visible" 				"1"
		"enabled" 				"1"
		"wide"	 				"640"
		"tall"	 				"480"
	}
	HudVoiceSelfStatus
	{
		"fieldName"				"HudVoiceSelfStatus"
		"visible" 				"1"
		"enabled" 				"1"
		"xpos" 					"r43"
		"ypos" 					"355"
		"wide"					"24"
		"tall" 					"24"
	}
	HudVoiceStatus
	{
		"fieldName"				"HudVoiceStatus"
		"visible" 				"1"
		"enabled" 				"1"
		"xpos" 					"r130"
		"ypos" 					"0"
		"wide" 					"150"
		"tall" 					"400"

		"item_tall"				"16"
		"item_wide"				"120"

		"item_spacing"			"2"

		"icon_ypos"				"0"
		"icon_xpos"				"0"
		"icon_tall"				"16"
		"icon_wide"				"16"

		"text_xpos"				"18"
	}
	RoundState
	{
		"fieldName"				"RoundState"
		"visible"				"0"
		"enabled"				"1"
		"xpos"					"10"
		"ypos"					"c-30"
		"wide"					"180"
		"tall"					"60"
		"text_xpos"				"8"
		"text_ypos"				"8"
	}
	WarmupLabel
	{
		"fieldName"				"WarmupLabel"
		"visible"				"0"
		"enabled"				"1"
		"xpos"					"10"
		"ypos"					"c-30"
		"wide"					"200"
		"tall"					"60"
		"text_xpos"				"8"
		"text_ypos"				"8"
	}
	RestartRoundLabel
	{
		"fieldName"				"RestartRoundLabel"
		"visible"				"0"
		"enabled"				"1"
		"xpos"					"10"
		"ypos"					"c0"
		"wide"					"200"
		"tall"					"60"
		"text_xpos"				"8"
		"text_ypos"				"8"
	}
	ReadyRestartLabel
	{
		"fieldName"				"ReadyRestartLabel"
		"visible"				"0"
		"enabled"				"1"
		"xpos"					"10"
		"ypos"					"c30"
		"wide"					"400"
		"tall"					"60"
		"text_xpos"				"8"
		"text_ypos"				"8"
	}
	HudDamageIndicator
	{
		"fieldName"				"HudDamageIndicator"
		"visible"				"1"
		"enabled"				"1"
		"DmgColorLeft"			"255 0 0 0"
		"DmgColorRight"			"255 0 0 0"
		
		"dmg_xpos"				"160"
		"dmg_ypos"				"100"
		"dmg_wide"				"50"
		"dmg_tall1"				"240"
		"dmg_tall2"				"200"
	}
	
	WinPanel_Allies
	{
		"fieldName"				"WinPanel_Allies"
		"visible"				"1"
		"enabled"				"1"
		"xpos"					"c-150"
		"ypos"					"255"
		"wide"					"300"
		"tall"					"155"
		"PaintBackgroundType"	"2"
		
		"icon_ypos"				"4"
		"icon_xpos_left"		"4"
		"icon_xpos_right"		"264"
		"icon_w"				"32"
		"icon_h"				"32"	
		
		"icon_stat_size"			"12"
		"icon_left_stat_x"			"90" 
		"icon_left_stat_y1"			"96"
		"icon_left_stat_y2"			"107"
		"icon_left_stat_y3"			"118"
		"icon_right_stat_x"			"230"
		"icon_right_stat_y1"		"96"
		"icon_right_stat_y2"		"107"
		"icon_right_stat_y3"		"118"
	}
	WinPanel_Axis
	{
		"fieldName"				"WinPanel_Axis"	
		"visible"				"1"
		"enabled"				"1"
		"xpos"					"c-150"
		"ypos"					"255"
		"wide"					"300"
		"tall"					"155"
		"PaintBackgroundType"	"2"
		
		"icon_ypos"				"4"
		"icon_xpos_left"		"4"
		"icon_xpos_right"		"264"
		"icon_w"				"32"
		"icon_h"				"32"
		
		"icon_stat_size"			"12"
		"icon_left_stat_x"			"90" 
		"icon_left_stat_y1"			"96"
		"icon_left_stat_y2"			"107"
		"icon_left_stat_y3"			"118"
		"icon_right_stat_x"			"230"
		"icon_right_stat_y1"		"96"
		"icon_right_stat_y2"		"107"
		"icon_right_stat_y3"		"118"
	}

	HudCommentary
	{
		"fieldName" "HudCommentary"
		"xpos"	"c-190"
		"ypos"	"350"
		"wide"	"380"
		"tall"  "40"
		"visible" "1"
		"enabled" "1"
		
		"PaintBackgroundType"	"2"
		
		"bar_xpos"		"50"
		"bar_ypos"		"20"
		"bar_height"	"8"
		"bar_width"		"320"
		"speaker_xpos"	"50"
		"speaker_ypos"	"8"
		"count_xpos_from_right"	"10"	// Counts from the right side
		"count_ypos"	"8"
		
		"icon_texture"	"vgui/hud/icon_commentary"
		"icon_xpos"		"0"
		"icon_ypos"		"0"		
		"icon_width"	"40"
		"icon_height"	"40"
		
		"BackgroundColor"	"CommentaryBG"
	}

	HudCrosshair
	{
		"fieldName" "HudCrosshair"
		"visible" "0"
		"enabled" "0"
		"wide"	 "640"
		"tall"	 "480"
		"never_draw"	"1"
	}
	
	HudTNTPickupPanel
	{
		"fieldName"				"HudTNTPickupPanel"
		"xpos"					"0"
		"ypos"					"c80"
		"wide"					"640"
		"tall"					"70"
		"visible"				"1"
		"enabled"				"1"
	}
	"HudChat"
	{
		"ControlName"		"EditablePanel"
		"fieldName" 		"HudChat"
		"visible" 		"1"
		"enabled" 		"1"
		"xpos"			"10"	[$WIN32]
		"xpos"			"42"	[$X360]
		"ypos"			"275"
		"wide"	 		"320"
		"tall"	 		"120"
		"PaintBackgroundType"	"2"
	}
	
	AchievementNotificationPanel	
	{
		"fieldName"				"AchievementNotificationPanel"
		"visible"				"1"
		"enabled"				"1"
		"xpos"					"0"
		"ypos"					"180"
		"wide"					"f10"	[$WIN32]
		"wide"					"f60"	[$X360]
		"tall"					"100"
	}
	
	FreezePanel
	{
		"fieldName"				"FreezePanel"
		"visible"				"1"
		"enabled"				"1"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"f0"
		"tall"					"480"
	}

	FreezePanelCallout
	{
		"fieldName"				"FreezePanelCallout"
		"visible"				"1"
		"enabled"				"1"
		"xpos"					"200"
		"ypos"					"200"
		"wide"					"100"
		"tall"					"50"
	}
}
