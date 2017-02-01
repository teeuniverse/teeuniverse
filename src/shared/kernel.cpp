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

//TAG_NEW_SHARED_COMPONENT
#include <shared/components/storage.h>
#include <shared/components/localization.h>
#include <shared/components/cli.h>
#include <shared/components/assetsmanager.h>

CSharedKernel::CSharedKernel()
{
	SetType(KERNEL_SHARED);
	
	m_pComponents.emplace_back(m_pStorage = new CStorage(this));
	m_pComponents.emplace_back(m_pLocalization = new CLocalization(this));
	m_pComponents.emplace_back(m_pCLI = new CCommandLineInterpreter(this));
	m_pComponents.emplace_back(m_pAssetsManager = new CAssetsManager(this));
}
	
bool CSharedKernel::Init(int argc, const char** argv)
{	
	for(unsigned int i=0; i<m_pComponents.size(); i++)
		if(!m_pComponents[i]->InitConfig(argc, argv))
		{
			debug::ErrorStream("ErrorManager") << "Config initialization of " << m_pComponents[i]->GetName() << " failed" << std::endl;
			return false;
		}
		
	for(unsigned int i=0; i<m_pComponents.size(); i++)
		if(!m_pComponents[i]->Init())
		{
			debug::ErrorStream("ErrorManager") << "Initialization of " << m_pComponents[i]->GetName() << " failed" << std::endl;
			return false;
		}
	
	return true;
}
	
bool CSharedKernel::PreUpdate()
{
	for(unsigned int i=0; i<m_pComponents.size(); i++)
		if(!m_pComponents[i]->PreUpdate())
			return false;
	
	return true;
}
	
bool CSharedKernel::PostUpdate()
{
	for(int i=m_pComponents.size()-1; i>=0; i--)
		if(!m_pComponents[i]->PostUpdate())
			return false;
	
	return true;
}
	
void CSharedKernel::Shutdown()
{
	for(int i=m_pComponents.size()-1; i>=0; i--)
		m_pComponents[i]->Shutdown();
}

void CSharedKernel::Save(CCLI_Output* pOutput)
{
	for(unsigned int i=0; i<m_pComponents.size(); i++)
		m_pComponents[i]->SaveConfig(pOutput);
}
