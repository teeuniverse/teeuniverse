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

#ifndef __CLIENT_KERNEL__
#define __CLIENT_KERNEL__

#include <shared/kernel.h>

//TAG_NEW_CLIENT_COMPONENT
class CGraphics;
class CTextRenderer;
class CAssetsRenderer;
class CInput;
class CBindsManager;

class CClientKernel: public CSharedKernel
{
public:
	class CGuest : public CSharedKernel::CGuest
	{
	private:
		CClientKernel* m_pClientKernel;
		
	public:
		CGuest(CClientKernel* pKernel) : CSharedKernel::CGuest(pKernel), m_pClientKernel(pKernel) { }
		inline CClientKernel* ClientKernel() { return m_pClientKernel; }
		inline const CClientKernel* ClientKernel() const { return m_pClientKernel; }
		
		//TAG_NEW_CLIENT_COMPONENT
		inline CGraphics* Graphics() { return m_pClientKernel->Graphics(); }
		inline CTextRenderer* TextRenderer() { return m_pClientKernel->TextRenderer(); }
		inline CAssetsRenderer* AssetsRenderer() { return m_pClientKernel->AssetsRenderer(); }
		inline CInput* Input() { return m_pClientKernel->Input(); }
		inline CBindsManager* BindsManager() { return m_pClientKernel->BindsManager(); }
	};
	
	class CComponent : public CGuest, public CSharedKernel::IComponent
	{
	public:
		CComponent(CClientKernel* pKernel) : CGuest(pKernel) { }
	};

private:
	//TAG_NEW_CLIENT_COMPONENT
	CGraphics* m_pGraphics;
	CTextRenderer* m_pTextRenderer;
	CAssetsRenderer* m_pAssetsRenderer;
	CInput* m_pInput;
	CBindsManager* m_pBindsManager;
	
public:	
	CClientKernel();
	
	//TAG_NEW_CLIENT_COMPONENT
	inline CGraphics* Graphics() { return m_pGraphics; }
	inline CTextRenderer* TextRenderer() { return m_pTextRenderer; }
	inline CAssetsRenderer* AssetsRenderer() { return m_pAssetsRenderer; }
	inline CInput* Input() { return m_pInput; }
	inline CBindsManager* BindsManager() { return m_pBindsManager; }
	
	//TAG_NEW_CLIENT_COMPONENT
	inline const CGraphics* Graphics() const { return m_pGraphics; }
	inline const CTextRenderer* TextRenderer() const { return m_pTextRenderer; }
	inline const CAssetsRenderer* AssetsRenderer() const { return m_pAssetsRenderer; }
	inline const CInput* Input() const { return m_pInput; }
	inline const CBindsManager* BindsManager() const { return m_pBindsManager; }
};

#endif
