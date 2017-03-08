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

#include <shared/kernel.h>
#include <shared/system/debug.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
	{
		std::unique_ptr<CSharedKernel> pKernel(new CSharedKernel());
	
		if(!pKernel->Init(argc, (const char**) argv))
		{
			debug::ErrorStream("Kernel") << "unable to initialize shared kernel" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		for(int i=1; i<argc; i++)
		{
			int PackageId = pKernel->AssetsManager()->Load_AssetsFile(argv[i]);
			if(PackageId >= 0)
			{
				std::cout << "Author: " << pKernel->AssetsManager()->GetPackageAuthor(PackageId) << std::endl;
				std::cout << "Credits: " << pKernel->AssetsManager()->GetPackageCredits(PackageId) << std::endl;
				std::cout << "License: " << pKernel->AssetsManager()->GetPackageLicense(PackageId) << std::endl;
				std::cout << "Version: " << pKernel->AssetsManager()->GetPackageVersion(PackageId) << std::endl;
			}
			else
			{
				debug::ErrorStream() << "The package \"" << argv[i] << "\" can't be loaded" << std::endl;
			}
		}
		
		pKernel->Shutdown();
	}

	std::exit(EXIT_SUCCESS);
}
