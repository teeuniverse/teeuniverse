/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverse.
 * 
 * TeeUniverse is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <shared/system/debug.h>
#include <shared/system/string.h>
#include <shared/kernel.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <client/loading_tools.h>

#include <cstdlib>

#define CREATE_ZONEINDEX(groupid, name, desc) {\
	SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());\
	pAsset->SetIndexTitle(SubPath, name);\
	pAsset->SetIndexDescription(SubPath, desc);\
	pAsset->SetIndexGroup(SubPath, groupid);\
}

#define CREATE_ZONEINDEX_NOUSE() {\
	SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());\
	pAsset->SetIndexUsed(SubPath, false);\
}

int main(int argc, char* argv[])
{
	{
		std::unique_ptr<CSharedKernel> pKernel(new CSharedKernel());
		
		if(!pKernel->Init(argc, (const char**) argv))
		{
			debug::ErrorStream("Kernel") << "unable to initialize shared kernel" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		int PackageId = pKernel->AssetsManager()->NewPackage("ddnet");
		pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
		pKernel->AssetsManager()->SetPackageCredits(PackageId, "Lady Saavik, Soreu, Fisico, necropotame");
		pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
		
		CAssetPath ImageZonesPath = CreateNewImage(pKernel.get(), PackageId, "zones_game", "images/univ_ddnet/zones_game.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		CAssetPath ImageSwitchPath = CreateNewImage(pKernel.get(), PackageId, "zones_switch", "images/univ_ddnet/zones_switch.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		CAssetPath ImageTelePath = CreateNewImage(pKernel.get(), PackageId, "zones_teleport", "images/univ_ddnet/zones_teleport.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		CAssetPath ImageTunePath = CreateNewImage(pKernel.get(), PackageId, "zones_tune", "images/univ_ddnet/zones_tune.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		
		//Game/Front Layer
		//Informations took from https://ddnet.tw/explain/explanations.json
		for(int Pass=0; Pass<3; Pass++)
		{
			CAssetPath AssetPath;
			CSubPath SubPath;
			
			CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
			
			int GroupId_Physics;
			int GroupId_Freeze;
			int GroupId_Race;
			int GroupId_Switch;
			int GroupId_GlobalOption;
			int GroupId_Signs;
			int GroupId_Structures;
			
			if(Pass == 2)
			{
				GroupId_Switch = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Switch), "Switches");
				
				GroupId_Race = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Race), "Race");
				
				GroupId_Structures = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Structures), "Structures");
			}
			else
			{
				GroupId_Physics = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Physics), "Physics");
				
				GroupId_Freeze = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Freeze), "Freeze");
				
				GroupId_Race = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Race), "Race");
				
				GroupId_Switch = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Switch), "Local Options");
				
				GroupId_GlobalOption = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_GlobalOption), "Global Options");
				
				GroupId_Structures = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Structures), "Structures");
				
				GroupId_Signs = pAsset->AddGroup();
				pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Signs), "Signs");
			}
			
			if(Pass == 0)
			{
				pAsset->SetName("ddGame");
				pAsset->SetImagePath(ImageZonesPath);
			}
			else if(Pass == 1)
			{
				pAsset->SetName("ddFront");
				pAsset->SetImagePath(ImageZonesPath);
			}
			else
			{
				pAsset->SetName("ddSwitch");
				pAsset->SetImagePath(ImageSwitchPath);
				{
					SubPath = CAsset_ZoneType::SubPath_DataInt(pAsset->AddDataInt());
					pAsset->SetDataIntTitle(SubPath, "Delay");
					pAsset->SetDataIntDescription(SubPath, "Delay before activation");
					pAsset->SetDataIntDefaultValue(SubPath, 0);
					pAsset->SetDataIntMinValue(SubPath, 0);
					pAsset->SetDataIntMaxValue(SubPath, 255);
					pAsset->SetDataIntNullValue(SubPath, -1);
				}
				{
					SubPath = CAsset_ZoneType::SubPath_DataInt(pAsset->AddDataInt());
					pAsset->SetDataIntTitle(SubPath, "Switch ID");
					pAsset->SetDataIntDescription(SubPath, "The Switch ID defines groups of tiles (door, time switch, ...) that will work together");
					pAsset->SetDataIntDefaultValue(SubPath, 0);
					pAsset->SetDataIntMinValue(SubPath, 0);
					pAsset->SetDataIntMaxValue(SubPath, 255);
					pAsset->SetDataIntNullValue(SubPath, 0);
				}
			}
			
			if(Pass < 2)
			{
				for(int i=0; i<4; i++)
				{
					CREATE_ZONEINDEX_NOUSE()
				}
				
				CREATE_ZONEINDEX(GroupId_Structures, "Laser blocker", "Doesn't let \"Dragging and Spinning Laser\" and \"Plasma Turret\" reach tees through it.")
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX(GroupId_Physics, "Hookthrough", "Combined with \"Hookable/Unhookable Ground\" tiles, allows to hook through the walls.")
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				{
					SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
					pAsset->SetIndexTitle(SubPath, "Freeze");
					pAsset->SetIndexUsed(SubPath, true);
					pAsset->SetIndexBorderIndex(SubPath, 1);
					pAsset->SetIndexBorderColor(SubPath, vec4(0.0f, 52.0f/255.0f, 104.0f/255.0f, 1.0f));
					pAsset->SetIndexGroup(SubPath, GroupId_Freeze);
				}
				
				CREATE_ZONEINDEX_NOUSE()
				
				{
					SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
					pAsset->SetIndexTitle(SubPath, "Unfreeze");
					pAsset->SetIndexUsed(SubPath, true);
					pAsset->SetIndexBorderIndex(SubPath, 2);
					pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 153.0f/255.0f, 0.0f, 1.0f));
					pAsset->SetIndexGroup(SubPath, GroupId_Freeze);
				}
				
				{
					SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
					pAsset->SetIndexTitle(SubPath, "Deep Freeze");
					pAsset->SetIndexDescription(SubPath, "Permanent freeze. Only Undeep tile can cancel this effect.");
					pAsset->SetIndexUsed(SubPath, true);
					pAsset->SetIndexBorderIndex(SubPath, 3);
					pAsset->SetIndexBorderColor(SubPath, vec4(0.0f, 0.0f, 0.0f, 1.0f));
					pAsset->SetIndexGroup(SubPath, GroupId_Freeze);
				}
				
				{
					SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
					pAsset->SetIndexTitle(SubPath, "Undeep Freeze");
					pAsset->SetIndexDescription(SubPath, "Removes Deep Freeze effect.");
					pAsset->SetIndexUsed(SubPath, true);
					pAsset->SetIndexBorderIndex(SubPath, 4);
					pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 0.0f, 0.0f, 1.0f));
					pAsset->SetIndexGroup(SubPath, GroupId_Freeze);
				}
				
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//16
				CREATE_ZONEINDEX(GroupId_Physics, "Wall Jump", "Placed next to a wall. Enables to climb up the wall.")
				CREATE_ZONEINDEX(GroupId_Switch, "Endless Hook On", "Endless hook has been activated")
				CREATE_ZONEINDEX(GroupId_Switch, "Endless Hook Off", "Endless hook has been deactivated")
				CREATE_ZONEINDEX(GroupId_Switch, "Hit Others On", "You can hit others.")
				CREATE_ZONEINDEX(GroupId_Switch, "Hit Others Off", "You can't hit others.")
				CREATE_ZONEINDEX(GroupId_Switch, "Solo On", "You are now in a solo part.")
				CREATE_ZONEINDEX(GroupId_Switch, "Solo Off", "You are now out of the solo part.")
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//32
				CREATE_ZONEINDEX(GroupId_Physics, "Refill Jumps", "Restores all jumps.")
				CREATE_ZONEINDEX(GroupId_Race, "Start", "Starts counting your race time.")
				CREATE_ZONEINDEX(GroupId_Race, "Finish", "End of race.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #1", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #2", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #3", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #4", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #5", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #6", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #7", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #8", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #9", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #10", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #11", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #12", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #13", "Compares your current race time with your record to show you whether you are running faster or slower.")

				
				//48
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #14", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #15", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #16", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #17", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #18", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #19", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #20", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #21", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #22", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #23", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #24", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Race, "Time Checkpoint #25", "Compares your current race time with your record to show you whether you are running faster or slower.")
				CREATE_ZONEINDEX(GroupId_Physics, "Stopper (one direction)", "You can hook and shoot through it. You can't go through it against the arrow.")
				CREATE_ZONEINDEX(GroupId_Physics, "Stopper (two directions)", "You can hook and shoot through it. You can't go through it against the arrow.")
				CREATE_ZONEINDEX(GroupId_Physics, "Stopper (all directions)", "You can hook and shoot through it. You can't go through it.")
				CREATE_ZONEINDEX_NOUSE()
				
				//64
				CREATE_ZONEINDEX(GroupId_Physics, "Speeder (slow)", "Causes weapons, \"Shield\", \"Heart\" and \"Spinning Laser\" to move.")
				CREATE_ZONEINDEX(GroupId_Physics, "Speeder (fast)", "Causes weapons, \"Shield\", \"Heart\" and \"Spinning Laser\" to move.")
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX(GroupId_GlobalOption, "Old Laser", "Shotgun drags others always towards the shooter, even after having bounced. Shooter can't hit himself. Place only one tile somewhere on the map.")
				CREATE_ZONEINDEX(GroupId_GlobalOption, "Collision Off", "Nobody can collide with others. Place only one tile somewhere on the map.")
				CREATE_ZONEINDEX(GroupId_GlobalOption, "Endless Hook On", "Everyone has endless hook. Place only one tile somewhere on the map.")
				CREATE_ZONEINDEX(GroupId_GlobalOption, "Hit Others Off", "Nobody can hit others. Place only one tile somewhere on the map.")
				CREATE_ZONEINDEX(GroupId_GlobalOption, "Hook Others Off", "Nobody can hook others. Place only one tile somewhere on the map.")
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//80
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX(GroupId_Switch, "Collision Off", "You can't collide with others.")
				CREATE_ZONEINDEX(GroupId_Switch, "Super Jump Off", "You don't have unlimited air jumps.")
				CREATE_ZONEINDEX(GroupId_Switch, "Jetpack Off", "You lost your jetpack gun.")
				CREATE_ZONEINDEX(GroupId_Switch, "Hook Others Off", "You can't hook others.")
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//96
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX(GroupId_Switch, "Collision On", "You can collide with others.")
				CREATE_ZONEINDEX(GroupId_Switch, "Super Jump On", "You have unlimited air jumps.")
				CREATE_ZONEINDEX(GroupId_Switch, "Jetpack On", "You have a jetpack gun.")
				CREATE_ZONEINDEX(GroupId_Switch, "Hook Others On", "You can hook others.")
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//112
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//128
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//144
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//160
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				
				//176
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				if(Pass==0)
				{
					CREATE_ZONEINDEX(GroupId_Signs, "Entities Off Sign (1)", "Informs people playing with entities about important marks, tips, information, or text on the map. Players should turn entities off to see the note before continuing the race.")
					CREATE_ZONEINDEX(GroupId_Signs, "Entities Off Sign (2)", "Informs people playing with entities about important marks, tips, information, or text on the map. Players should turn entities off to see the note before continuing the race.")
				}
				else
				{
					CREATE_ZONEINDEX_NOUSE()
					CREATE_ZONEINDEX_NOUSE()
				}
				
				//192
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
				CREATE_ZONEINDEX_NOUSE()
			}
			else
			{
				for(int i=0; i<22; i++)
				{
					CREATE_ZONEINDEX_NOUSE()
				}
				
				CREATE_ZONEINDEX(GroupId_Switch, "Time Switch (Activate)", "Activates switch (e.g. closes door) with the same Switch ID for set amount of seconds.")
				CREATE_ZONEINDEX(GroupId_Switch, "Time Switch (Desactivate)", "Desactivate switch (e.g. opens door) with the same Switch ID for set amount of seconds.")
				CREATE_ZONEINDEX(GroupId_Switch, "Switch (Activate)", "Activates switch (e.g. closes door) with the same Switch ID.")
				CREATE_ZONEINDEX(GroupId_Switch, "Switch (Desactivate)", "Desactivate switch (e.g. closes door) with the same Switch ID.")
				
				for(int i=26; i<79; i++)
				{
					CREATE_ZONEINDEX_NOUSE()
				}
				
				CREATE_ZONEINDEX(GroupId_Race, "Penalty", "Adds time to your current race time.")
				
				for(int i=80; i<95; i++)
				{
					CREATE_ZONEINDEX_NOUSE()
				}
				
				CREATE_ZONEINDEX(GroupId_Race, "Bonus", "Substracts time to your current race time.")
				
				for(int i=96; i<203; i++)
				{
					CREATE_ZONEINDEX_NOUSE()
				}
			}
			
			CREATE_ZONEINDEX(GroupId_Structures, "Spinning Laser (ccw, fast)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
			CREATE_ZONEINDEX(GroupId_Structures, "Spinning Laser (ccw, medium)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
			CREATE_ZONEINDEX(GroupId_Structures, "Spinning Laser (ccw, slow)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
			CREATE_ZONEINDEX(GroupId_Structures, "Non-Spinning Laser", "Tile, where freezing laser (made with \"Laser Length\") begins.")
			CREATE_ZONEINDEX(GroupId_Structures, "Spinning Laser (cw, slow)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
			
			//208
			CREATE_ZONEINDEX(GroupId_Structures, "Spinning Laser (cw, medium)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
			CREATE_ZONEINDEX(GroupId_Structures, "Spinning Laser (cw, fast)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length 3", "Combined with \"Door\" or \"Spinning Laser\", makes it 3 tiles long.")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length 6", "Combined with \"Door\" or \"Spinning Laser\", makes it 6 tiles long.")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length 9", "Combined with \"Door\" or \"Spinning Laser\", makes it 9 tiles long.")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length Change (lenghten, slow)", "Combined with \"Laser Length\", causes it to lengthen and shorten constantly. It's not working with \"Door\"")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length Change (lenghten, medium)", "Combined with \"Laser Length\", causes it to lengthen and shorten constantly. It's not working with \"Door\"")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length Change (lenghten, fast)", "Combined with \"Laser Length\", causes it to lengthen and shorten constantly. It's not working with \"Door\"")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length Change (shorten, slow)", "Combined with \"Laser Length\", causes it to shorten and lengthen constantly. It's not working with \"Door\"")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length Change (shorten, medium)", "Combined with \"Laser Length\", causes it to shorten and lengthen constantly. It's not working with \"Door\"")
			CREATE_ZONEINDEX(GroupId_Structures, "Laser Length Change (shorten, fast)", "Combined with \"Laser Length\", causes it to shorten and lengthen constantly. It's not working with \"Door\"")
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX(GroupId_Structures, "Plasma Turret (explosive)", "Shoots plasma bullets at the closest tee. They explode on an obstacle they hit (wall or tee).")
			CREATE_ZONEINDEX(GroupId_Structures, "Plasma Turret (freeze)", "Shoots plasma bullets that work like Freeze at the closest tee.")
			CREATE_ZONEINDEX(GroupId_Structures, "Plasma Turret (explosive and freeze)", "Shoots plasma bullets that work like Freeze at the closest tee. They also explode on an obstacle they hit (wall or tee).")
			CREATE_ZONEINDEX(GroupId_Structures, "Plasma Turret (unfreeze)", "Shoots plasma bullets that work like Unfreeze at the closest tee.")
			
			//224
			CREATE_ZONEINDEX(GroupId_Structures, "Explosing bullet", "Bounces off the walls with explosion. Touching the bullet works like Freeze tile (freezes for 3 seconds by default")
			CREATE_ZONEINDEX(GroupId_Structures, "Bullet", "Bounces off the walls without explosion. Touching the bullet works like Freeze tile (freezes for 3 seconds by default")
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX(GroupId_Structures, "Dragging Laser (weak)", "Grabs and attracts the closest tee to it. Can't reach tees through the walls and Laser Blocker.")
			CREATE_ZONEINDEX(GroupId_Structures, "Dragging Laser (medium)", "Grabs and attracts the closest tee to it. Can't reach tees through the walls and Laser Blocker.")
			CREATE_ZONEINDEX(GroupId_Structures, "Dragging Laser (strong)", "Grabs and attracts the closest tee to it. Can't reach tees through the walls and Laser Blocker.")
			CREATE_ZONEINDEX(GroupId_Structures, "Dragging Laser (through walls, weak)", "Grabs and attracts the closest tee to it. Can reach tees through the walls but not Laser Blocker.")
			CREATE_ZONEINDEX(GroupId_Structures, "Dragging Laser (through walls, medium)", "Grabs and attracts the closest tee to it. Can reach tees through the walls but not Laser Blocker.")
			CREATE_ZONEINDEX(GroupId_Structures, "Dragging Laser (through walls, strong)", "Grabs and attracts the closest tee to it. Can reach tees through the walls but not Laser Blocker.")
			CREATE_ZONEINDEX_NOUSE()
			
			//240
			CREATE_ZONEINDEX(GroupId_Structures, "Door", "Combined with \"Laser Length\" will create doors. Doesn't allow to go through it (only with NINJA).")
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
		}
		
		//Tele layer
		{
			CAssetPath AssetPath;
			CSubPath SubPath;
			
			CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
			pAsset->SetName("ddTele");
			pAsset->SetImagePath(ImageTelePath);
			
			int GroupId_Checkpoint = pAsset->AddGroup();
			
			pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Checkpoint), "Checkpoints");
			
			{
				SubPath = CAsset_ZoneType::SubPath_DataInt(pAsset->AddDataInt());
				pAsset->SetDataIntTitle(SubPath, "Teleport ID");
				pAsset->SetDataIntDescription(SubPath, "The Teleport ID defines groups of tiles (destination, teleport, ...) that will work together");
				pAsset->SetDataIntDefaultValue(SubPath, -1);
				pAsset->SetDataIntMinValue(SubPath, -1);
				pAsset->SetDataIntMaxValue(SubPath, 1024);
			}
			
			for(int i=0; i<10; i++)
			{
				CREATE_ZONEINDEX_NOUSE()
			}
			
			//10
			CREATE_ZONEINDEX(-1, "Evil Teleport", "After falling into this tile, tees appear on \"Destination\" tile with the same ID. Speed and hook are deleted.")
			
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			CREATE_ZONEINDEX_NOUSE()
			
			//14
			CREATE_ZONEINDEX(-1, "Weapon Teleport", "Teleports bullets shot into it to \"Destination\" tile, where it comes out. Direction, angle and length are kept.")
			//15
			CREATE_ZONEINDEX(-1, "Hook Teleport", "Teleports hooks entered into it to \"Destination\" tile, where it comes out. Direction, angle and length are kept.")
			
			for(int i=16; i<26; i++)
			{
				CREATE_ZONEINDEX_NOUSE()
			}
			
			//26
			CREATE_ZONEINDEX(-1, "Teleport", "After falling into this tile, tees appear on \"Destination\" tile with the same ID. Speed and hook are kept.")
			//27
			CREATE_ZONEINDEX(-1, "Destination", "Teleport destination tile for \"Teleport\", \"Evil Teleport\", \"Hook Teleport\" and \"Weapon Teleport\" tiles with the same ID.")
			
			CREATE_ZONEINDEX_NOUSE()
			
			//29
			CREATE_ZONEINDEX(GroupId_Checkpoint, "Checkpoint Trigger", "After having touched this tile, any \"Checkpoint Teleport\" tile will teleport you to \"Checkpoint Destination\" with the same ID.")
			//30
			CREATE_ZONEINDEX(GroupId_Checkpoint, "Checkpoint Destination", "Here tees will appear after touching \"Checkpoint Trigger\" with the same ID and falling into Checkpoint Teleport.")
			//31
			CREATE_ZONEINDEX(GroupId_Checkpoint, "Checkpoint Teleport", "Sends tees to \"Checkpoint Destination\" with the same ID as the last touched \"Checkpoint Trigger\". Speed and hook are kept")
			
			for(int i=32; i<63; i++)
			{
				CREATE_ZONEINDEX_NOUSE()
			}
			
			CREATE_ZONEINDEX(GroupId_Checkpoint, "Evil Checkpoint Teleport", "Sends tees to \"Checkpoint Destination\" with the same ID as the last touched \"Checkpoint Trigger\". Speed and hook are deleted")
		}
		
		//Tele layer
		{
			CAssetPath AssetPath;
			CSubPath SubPath;
			
			CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
			pAsset->SetName("ddTune");
			pAsset->SetImagePath(ImageTunePath);
			
			{
				SubPath = CAsset_ZoneType::SubPath_DataInt(pAsset->AddDataInt());
				pAsset->SetDataIntTitle(SubPath, "Tune ID");
				pAsset->SetDataIntDescription(SubPath, "The Tune ID represent a set of tuning parameters");
				pAsset->SetDataIntDefaultValue(SubPath, 1);
				pAsset->SetDataIntMinValue(SubPath, 1);
				pAsset->SetDataIntMaxValue(SubPath, 255);
			}
			
			for(int i=0; i<68; i++)
			{
				CREATE_ZONEINDEX_NOUSE()
			}
			
			CREATE_ZONEINDEX(-1, "Tune zone", "Area where tuning parameters are different")
		}
		
		pKernel->AssetsManager()->Save_AssetsFile_SaveDir(PackageId);
		
		pKernel->Shutdown();
	}

	exit(EXIT_SUCCESS);
}
