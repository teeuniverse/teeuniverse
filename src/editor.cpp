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
#include <shared/components/assetsmanager.h>
#include <shared/components/cli.h>
#include <editor/kernel.h>

#include <cstdlib>
#include <SDL.h>

int main(int argc, char* argv[])
{
	//Init SDL
	{
		if(SDL_Init(0) < 0)
		{
			debug::ErrorStream("Main") << "unable to init SDL base: " << SDL_GetError() << std::endl;
			return 0;
		}

		atexit(SDL_Quit);
	}
	
	CEditorKernel* pKernel = new CEditorKernel();
	pKernel->SetConfigFilename("config/settings_editor.cfg");
	if(!pKernel->Init(argc, (const char**) argv))
	{
		debug::ErrorStream("Main") << "unable to initialize editor kernel" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	pKernel->AssetsManager()->EnableAssetsHistory();
	
	bool Running = true;
	do
	{
		Running = Running && pKernel->PreUpdate();
		Running = Running && pKernel->PostUpdate();
	}
	while(Running);
	
	pKernel->CLI()->Execute("save_config config/settings_editor.cfg");
	
	pKernel->Shutdown();
	delete pKernel;

	exit(EXIT_SUCCESS);
}
