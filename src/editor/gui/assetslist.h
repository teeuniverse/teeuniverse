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

#ifndef __CLIENT_ASSETSORGANIZER__
#define __CLIENT_ASSETSORGANIZER__

#include <client/gui/panellayout.h>
#include <client/gui/listlayout.h>

class CAssetsTree : public gui::CVScrollLayout
{
private:
	class CGuiEditor* m_pAssetsEditor;
	class CAssetsOrganizer* m_pAssetsOrganizer;
	int m_PackageId;
	
public:
	CAssetsTree(CAssetsOrganizer* pAssetsOrganizer);
	virtual void Update(bool ParentEnabled);
	
	void Refresh();
	class CGuiEditor* AssetsEditor() { return m_pAssetsEditor; }
};

class CPackagesTree : public gui::CVScrollLayout
{
private:
	class CGuiEditor* m_pAssetsEditor;
	class CAssetsOrganizer* m_pAssetsOrganizer;
	int m_NumPackages;
	
public:
	CPackagesTree(CAssetsOrganizer* pAssetsOrganizer);
	virtual void Update(bool ParentEnabled);
	
	void Refresh();
	class CGuiEditor* AssetsEditor() { return m_pAssetsEditor; }
};

class CAssetsOrganizer : public gui::CVPanelLayout
{
private:
	class CGuiEditor* m_pAssetsEditor;
	CAssetsTree* m_pAssetsTree;
	CPackagesTree* m_pPackagesTree;
	
public:
	CAssetsOrganizer(CGuiEditor* pAssetsEditor);
	class CGuiEditor* AssetsEditor() { return m_pAssetsEditor; }
};

#endif
