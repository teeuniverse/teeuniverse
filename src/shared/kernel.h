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

#ifndef __SHARED_KERNEL__
#define __SHARED_KERNEL__

#include <shared/system/string.h>

#include <memory>
#include <vector>

//TAG_NEW_SHARED_COMPONENT
class CStorage;
class CLocalization;
class CCommandLineInterpreter;
class CAssetsManager;

enum
{
	KERNEL_SHARED=0,
	KERNEL_CLIENT,
	KERNEL_GAME,
	KERNEL_EDITOR,
	KERNEL_SERVER,
};

class CSharedKernel
{
public:
	class CGuest
	{
	private:
		CSharedKernel* m_pSharedKernel;
		
	public:
		CGuest(CSharedKernel* pSharedKernel) : m_pSharedKernel(pSharedKernel) { }
		virtual ~CGuest() { }
		
		inline CSharedKernel* SharedKernel() { return m_pSharedKernel; }
		inline const CSharedKernel* SharedKernel() const { return m_pSharedKernel; }
	
		//TAG_NEW_SHARED_COMPONENT
		inline CStorage* Storage() { return m_pSharedKernel->Storage(); }
		inline CLocalization* Localization() { return m_pSharedKernel->Localization(); }
		inline CCommandLineInterpreter* CLI() { return m_pSharedKernel->CLI(); }
		inline CAssetsManager* AssetsManager() { return m_pSharedKernel->AssetsManager(); }
	};
	
	class IComponent
	{
	private:
		dynamic_string m_Name;
		
	public:
		virtual ~IComponent() {}
		
		virtual bool InitConfig(int argc, const char** argv) { return true; }
		virtual void SaveConfig(class CCLI_Output* pOutput) {}
		
		virtual bool Init() { return true; }
		virtual bool PreUpdate() { return true; }
		virtual bool PostUpdate() { return true; }
		virtual void Shutdown() {}
		
		void SetName(const char* pName) { m_Name = pName; }
		const char* GetName() { return m_Name.buffer(); }
	};
	
	class CComponent : public CGuest, public IComponent
	{
	public:
		CComponent(CSharedKernel* pSharedKernel) : CGuest(pSharedKernel) { }
		virtual ~CComponent() = default;
	};

protected:
	std::vector< std::unique_ptr<IComponent> > m_pSharedComponents;
	std::vector< std::unique_ptr<IComponent> > m_pComponents;
	dynamic_string m_ConfigFilename;
	CStorage* m_pStorage;
	CLocalization* m_pLocalization;
	CCommandLineInterpreter* m_pCLI;
	CAssetsManager* m_pAssetsManager;
	
	int m_Type;

public:	
	CSharedKernel();
	virtual ~CSharedKernel() {};
	
	bool Init(int argc, const char** argv);
	bool PreUpdate();
	bool PostUpdate();
	void Shutdown();
	void Save(class CCLI_Output* pOutput);
	
	//TAG_NEW_SHARED_COMPONENT
	inline CStorage* Storage() { return m_pStorage; }
	inline CLocalization* Localization() { return m_pLocalization; }
	inline CCommandLineInterpreter* CLI() { return m_pCLI; }
	inline CAssetsManager* AssetsManager() { return m_pAssetsManager; }
	
	inline int Type() const { return m_Type; }
	inline void SetType(int Type) { m_Type = Type; }
	
	inline void SetConfigFilename(const char* pFilename) { m_ConfigFilename = pFilename; }
};

#endif
