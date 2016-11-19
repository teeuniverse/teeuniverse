/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverses.
 * 
 * TeeUniverses is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverses is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EDITOR_KERNEL__
#define __EDITOR_KERNEL__

#include <client/kernel.h>

//TAG_NEW_EDITOR_COMPONENT
class CGuiEditor;

class CEditorKernel: public CClientKernel
{
public:
	class CGuest : public CClientKernel::CGuest
	{
	private:
		CEditorKernel* m_pEditorKernel;
		
	public:
		CGuest(CEditorKernel* pKernel) : CClientKernel::CGuest(pKernel), m_pEditorKernel(pKernel) { }
		inline CEditorKernel* EditorKernel() { return m_pEditorKernel; }
		inline const CEditorKernel* EditorKernel() const { return m_pEditorKernel; }
		
		//TAG_NEW_EDITOR_COMPONENT
		inline CGuiEditor* Gui() { return m_pEditorKernel->Gui(); }
	};
	
	class CComponent : public CGuest
	{
	public:
		CComponent(CEditorKernel* pKernel) : CGuest(pKernel) { }
		virtual ~CComponent() {}
		
		virtual bool Init(int argc, const char** argv) = 0;
		virtual bool Update() = 0;
		virtual void Shutdown() = 0;
	};

private:
	//TAG_NEW_EDITOR_COMPONENT
	CGuiEditor* m_pGui;
	
public:	
	CEditorKernel();
	
	//TAG_NEW_EDITOR_COMPONENT
	inline CGuiEditor* Gui() { return m_pGui; }
	
	//TAG_NEW_EDITOR_COMPONENT
	inline const CGuiEditor* Gui() const { return m_pGui; }
};

#endif
