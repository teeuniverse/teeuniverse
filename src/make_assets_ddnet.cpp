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

#define CREATE_ZONEINDEX(name, desc) {\
	SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());\
	pAsset->SetIndexTitle(SubPath, name);\
	pAsset->SetIndexDescription(SubPath, desc);\
}

#define CREATE_ZONEINDEX_NOUSE() {\
	SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());\
	pAsset->SetIndexUsed(SubPath, false);\
}

int main(int argc, char* argv[])
{
	CSharedKernel* pKernel = new CSharedKernel();
	if(!pKernel->Init(argc, (const char**) argv))
	{
		dbg_msg("Kernel", "unable to initialize shared kernel");
		exit(EXIT_FAILURE);
	}
	
	int PackageId = pKernel->AssetsManager()->NewPackage("ddnet");
	pKernel->AssetsManager()->SetPackageAuthor(PackageId, "necropotame");
	pKernel->AssetsManager()->SetPackageCredits(PackageId, "Lady Saavik, Soreu, Fisico");
	pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
	pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
	
	CAssetPath ImageZonesPath = CreateNewImage(pKernel, PackageId, "zones_game", "datasrc/images/univ_ddnet/zones_game.png", CStorage::TYPE_ABSOLUTE, 16, 16, true, 0);
	CAssetPath ImageFreezePath = CreateNewImage(pKernel, PackageId, "zones_freeze", "datasrc/images/univ_ddnet/zones_freeze.png", CStorage::TYPE_ABSOLUTE, 16, 16, true, 0);
	
	//Game Layer
	//Informations took from https://ddnet.tw/explain/explanations.json
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("ddGame");
		pAsset->SetImagePath(ImageZonesPath);
		
		for(int i=0; i<4; i++)
		{
			CREATE_ZONEINDEX_NOUSE()
		}
		
		CREATE_ZONEINDEX("Laser blocker", "Doesn't let \"Dragging and Spinning Laser\" and \"Plasma Turret\" reach tees through it.")
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX("Hookthrough", "Combined with \"Hookable/Unhookable Ground\" tiles, allows to hook through the walls.")
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		
		//16
		CREATE_ZONEINDEX("Wall Jump", "Placed next to a wall. Enables to climb up the wall.")
		CREATE_ZONEINDEX("Endless Hook On", "Endless hook has been activated")
		CREATE_ZONEINDEX("Endless Hook Off", "Endless hook has been deactivated")
		CREATE_ZONEINDEX("Hit Others On", "You can hit others.")
		CREATE_ZONEINDEX("Hit Others Off", "You can't hit others.")
		CREATE_ZONEINDEX("Solo On", "You are now in a solo part.")
		CREATE_ZONEINDEX("Solo Off", "You are now out of the solo part.")
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
		CREATE_ZONEINDEX("Refill Jumps", "Restores all jumps.")
		CREATE_ZONEINDEX("Start", "Starts counting your race time.")
		CREATE_ZONEINDEX("Finish", "End of race.")
		CREATE_ZONEINDEX("Time Checkpoint #1", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #2", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #3", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #4", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #5", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #6", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #7", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #8", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #9", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #10", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #11", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #12", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #13", "Compares your current race time with your record to show you whether you are running faster or slower.")

		
		//48
		CREATE_ZONEINDEX("Time Checkpoint #14", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #15", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #16", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #17", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #18", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #19", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #20", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #21", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #22", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #23", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #24", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Time Checkpoint #25", "Compares your current race time with your record to show you whether you are running faster or slower.")
		CREATE_ZONEINDEX("Stopper (one direction)", "You can hook and shoot through it. You can't go through it against the arrow.")
		CREATE_ZONEINDEX("Stopper (two directions)", "You can hook and shoot through it. You can't go through it against the arrow.")
		CREATE_ZONEINDEX("Stopper (all directions)", "You can hook and shoot through it. You can't go through it.")
		CREATE_ZONEINDEX_NOUSE()
		
		//64
		CREATE_ZONEINDEX("Speeder (slow)", "Causes weapons, \"Shield\", \"Heart\" and \"Spinning Laser\" to move.")
		CREATE_ZONEINDEX("Speeder (fast)", "Causes weapons, \"Shield\", \"Heart\" and \"Spinning Laser\" to move.")
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX("Old Laser", "Shotgun drags others always towards the shooter, even after having bounced. Shooter can't hit himself. Place only one tile somewhere on the map.")
		CREATE_ZONEINDEX("Collision Off", "Nobody can collide with others. Place only one tile somewhere on the map.")
		CREATE_ZONEINDEX("Endless Hook On", "Everyone has endless hook. Place only one tile somewhere on the map.")
		CREATE_ZONEINDEX("Hit Others Off", "Nobody can hit others. Place only one tile somewhere on the map.")
		CREATE_ZONEINDEX("Hook Others Off", "Nobody can hook others. Place only one tile somewhere on the map.")
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
		CREATE_ZONEINDEX("Collision Off", "You can't collide with others.")
		CREATE_ZONEINDEX("Super Jump Off", "You don't have unlimited air jumps.")
		CREATE_ZONEINDEX("Jetpack Off", "You lost your jetpack gun.")
		CREATE_ZONEINDEX("Hook Others Off", "You can't hook others.")
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
		CREATE_ZONEINDEX("Collision On", "You can collide with others.")
		CREATE_ZONEINDEX("Super Jump On", "You have unlimited air jumps.")
		CREATE_ZONEINDEX("Jetpack On", "You have a jetpack gun.")
		CREATE_ZONEINDEX("Hook Others On", "You can hook others.")
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
		CREATE_ZONEINDEX("Entities Off Sign", "Informs people playing with entities about important marks, tips, information, or text on the map. Players should turn entities off to see the note before continuing the race.")
		CREATE_ZONEINDEX_NOUSE()
		
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
		CREATE_ZONEINDEX("Spinning Laser (ccw, fast)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
		CREATE_ZONEINDEX("Spinning Laser (ccw, medium)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
		CREATE_ZONEINDEX("Spinning Laser (ccw, slow)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
		CREATE_ZONEINDEX("Non-Spinning Laser", "Tile, where freezing laser (made with \"Laser Length\") begins.")
		CREATE_ZONEINDEX("Spinning Laser (cw, slow)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
		
		//208
		CREATE_ZONEINDEX("Spinning Laser (cw, medium)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
		CREATE_ZONEINDEX("Spinning Laser (cw, fast)", "Tile, where freezing laser (made with \"Laser Length\") begins.")
		CREATE_ZONEINDEX("Laser Length 3", "Combined with \"Door\" or \"Spinning Laser\", makes it 3 tiles long.")
		CREATE_ZONEINDEX("Laser Length 6", "Combined with \"Door\" or \"Spinning Laser\", makes it 6 tiles long.")
		CREATE_ZONEINDEX("Laser Length 9", "Combined with \"Door\" or \"Spinning Laser\", makes it 9 tiles long.")
		CREATE_ZONEINDEX("Laser Length Change (lenghten, slow)", "Combined with \"Laser Length\", causes it to lengthen and shorten constantly. It's not working with \"Door\"")
		CREATE_ZONEINDEX("Laser Length Change (lenghten, medium)", "Combined with \"Laser Length\", causes it to lengthen and shorten constantly. It's not working with \"Door\"")
		CREATE_ZONEINDEX("Laser Length Change (lenghten, fast)", "Combined with \"Laser Length\", causes it to lengthen and shorten constantly. It's not working with \"Door\"")
		CREATE_ZONEINDEX("Laser Length Change (shorten, slow)", "Combined with \"Laser Length\", causes it to shorten and lengthen constantly. It's not working with \"Door\"")
		CREATE_ZONEINDEX("Laser Length Change (shorten, medium)", "Combined with \"Laser Length\", causes it to shorten and lengthen constantly. It's not working with \"Door\"")
		CREATE_ZONEINDEX("Laser Length Change (shorten, fast)", "Combined with \"Laser Length\", causes it to shorten and lengthen constantly. It's not working with \"Door\"")
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX("Plasma Turret (explosive)", "Shoots plasma bullets at the closest tee. They explode on an obstacle they hit (wall or tee).")
		CREATE_ZONEINDEX("Plasma Turret (freeze)", "Shoots plasma bullets that work like Freeze at the closest tee.")
		CREATE_ZONEINDEX("Plasma Turret (explosive and freeze)", "Shoots plasma bullets that work like Freeze at the closest tee. They also explode on an obstacle they hit (wall or tee).")
		CREATE_ZONEINDEX("Plasma Turret (unfreeze)", "Shoots plasma bullets that work like Unfreeze at the closest tee.")
		
		//224
		CREATE_ZONEINDEX("Explosing bullet", "Bounces off the walls with explosion. Touching the bullet works like Freeze tile (freezes for 3 seconds by default")
		CREATE_ZONEINDEX("Bullet", "Bounces off the walls without explosion. Touching the bullet works like Freeze tile (freezes for 3 seconds by default")
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX_NOUSE()
		CREATE_ZONEINDEX("Dragging Laser (weak)", "Grabs and attracts the closest tee to it. Can't reach tees through the walls and Laser Blocker.")
		CREATE_ZONEINDEX("Dragging Laser (medium)", "Grabs and attracts the closest tee to it. Can't reach tees through the walls and Laser Blocker.")
		CREATE_ZONEINDEX("Dragging Laser (strong)", "Grabs and attracts the closest tee to it. Can't reach tees through the walls and Laser Blocker.")
		CREATE_ZONEINDEX("Dragging Laser (through walls, weak)", "Grabs and attracts the closest tee to it. Can reach tees through the walls but not Laser Blocker.")
		CREATE_ZONEINDEX("Dragging Laser (through walls, medium)", "Grabs and attracts the closest tee to it. Can reach tees through the walls but not Laser Blocker.")
		CREATE_ZONEINDEX("Dragging Laser (through walls, strong)", "Grabs and attracts the closest tee to it. Can reach tees through the walls but not Laser Blocker.")
		CREATE_ZONEINDEX_NOUSE()
		
		//240
		CREATE_ZONEINDEX("Door", "Combined with \"Laser Length\" will create doors. Doesn't allow to go through it (only with NINJA).")
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
	
	{
		CAssetPath AssetPath;
		CSubPath SubPath;
		
		CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&AssetPath, PackageId);
		pAsset->SetName("ddFreeze");
		pAsset->SetImagePath(ImageFreezePath);
		
		CREATE_ZONEINDEX_NOUSE()
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Freeze");
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(0.0f, 52.0f/255.0f, 104.0f/255.0f, 1.0f));
		}
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Unfreeze");
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 2);
			pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 153.0f/255.0f, 0.0f, 1.0f));
		}
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Deep Freeze");
			pAsset->SetIndexDescription(SubPath, "Permanent freeze. Only Undeep tile can cancel this effect.");
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 3);
			pAsset->SetIndexBorderColor(SubPath, vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}
		
		{
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Undeep Freeze");
			pAsset->SetIndexDescription(SubPath, "Removes Deep Freeze effect.");
			pAsset->SetIndexUsed(SubPath, true);
			pAsset->SetIndexBorderIndex(SubPath, 4);
			pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}	
	
	pKernel->AssetsManager()->Save_AssetsFile(PackageId);
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
