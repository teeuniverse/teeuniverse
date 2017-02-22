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

#include "autolayer.h"

#include <random>

void ApplyTilingMaterials_FullLayer(CAssetsManager* pAssetsManager, CAssetPath LayerPath, int Token)
{
	CAsset_MapLayerTiles* pLayer = pAssetsManager->GetAsset_Hard<CAsset_MapLayerTiles>(LayerPath);
	if(!pLayer)
		return;
	
	ApplyTilingMaterials_Layer(pAssetsManager, LayerPath, 0, 0, pLayer->GetTileWidth(), pLayer->GetTileHeight(), Token);
}

void ApplyTilingMaterials_Layer(CAssetsManager* pAssetsManager, CAssetPath LayerPath, int X, int Y, int W, int H, int Token)
{
	CAsset_MapLayerTiles* pLayer = pAssetsManager->GetAsset_Hard<CAsset_MapLayerTiles>(LayerPath);
	if(!pLayer)
		return;
	
	pAssetsManager->SaveAssetInHistory(LayerPath, Token);
	
	array2d<CAsset_MapLayerTiles::CTile>& Tiles = pLayer->GetTileArray();
	
	//Step 1: copy zone layer in the brush
	const CAsset_MapZoneTiles* pSource = pAssetsManager->GetAsset<CAsset_MapZoneTiles>(pLayer->GetSourcePath());
	if(pSource)
	{
		if(pSource->GetTileWidth() != Tiles.get_width() || pSource->GetTileHeight() != Tiles.get_height())
		{
			Tiles.resize(pSource->GetTileWidth(), pSource->GetTileHeight());
			
			X = 0;
			Y = 0;
			W = pSource->GetTileWidth();
			H = pSource->GetTileHeight();
		}
		else
		{
			W = clamp(W, 0, pSource->GetTileWidth() - X);
			H = clamp(H, 0, pSource->GetTileHeight() - Y);
		}
		
		pLayer->SetPositionX(pSource->GetPositionX());
		pLayer->SetPositionY(pSource->GetPositionY());
		
		const array2d<CAsset_MapZoneTiles::CTile>& SrcTiles = pSource->GetTileArray();
		
		if(pLayer->GetStylePath().GetType() == CAsset_TilingMaterial::TypeId)
		{
			const CAsset_TilingMaterial* pMaterial = pAssetsManager->GetAsset<CAsset_TilingMaterial>(pLayer->GetStylePath());
			if(pMaterial)
			{
				const std::vector<CAsset_TilingMaterial::CZoneConverter>& ZoneConverter = pMaterial->GetZoneConverterArray();
				
				for(int j=Y; j<Y+H; j++)
				{
					for(int i=X; i<X+W; i++)
					{
						int IndexFound = -1;
						
						for(unsigned int z=0; z<ZoneConverter.size(); z++)
						{
							if(ZoneConverter[z].GetZoneTypePath() != pSource->GetZoneTypePath() || ZoneConverter[z].GetOldIndex() != SrcTiles.get_clamp(i, j).GetIndex())
								continue;
							
							IndexFound = ZoneConverter[z].GetNewIndex();
						}
						
						if(IndexFound)
							Tiles.get_clamp(i, j).SetBrush(IndexFound);
						else
							Tiles.get_clamp(i, j).SetBrush(0);
					}
				}
			}
		}
	}
	
	//Step 2: Create tiles from brush
	ApplyTilingMaterials_Tiles(pAssetsManager, Tiles, pLayer->GetStylePath(), X, Y, W, H, pLayer->GetRandomSeed());
}

void ApplyTilingMaterials_Tiles(CAssetsManager* pAssetsManager, array2d<CAsset_MapLayerTiles::CTile>& Tiles, CAssetPath StylePath, int X, int Y, int W, int H, int Seed)
{
	W = clamp(W, 0, Tiles.get_width() - X);
	H = clamp(H, 0, Tiles.get_height() - Y);
	
	if(StylePath.GetType() == CAsset_TilingMaterial::TypeId)
	{
		const CAsset_TilingMaterial* pMaterial = pAssetsManager->GetAsset<CAsset_TilingMaterial>(StylePath);
		if(pMaterial)
		{
			//Increase the size of the area to change to take take of distant tiles that may need to be changed
			{
				int MaxDistance = 0;
				CAsset_TilingMaterial::CIteratorRule RuleIter;
				for(RuleIter = pMaterial->ReverseBeginRule(); RuleIter != pMaterial->ReverseEndRule(); ++RuleIter)
				{
					const std::vector<CAsset_TilingMaterial::CRule::CCondition>& Conditions = pMaterial->GetRuleConditionArray(*RuleIter);
					for(unsigned int c=0; c<Conditions.size(); c++)
					{
						MaxDistance = std::max(MaxDistance, std::abs(Conditions[c].GetRelPosX()));
						MaxDistance = std::max(MaxDistance, std::abs(Conditions[c].GetRelPosY()));
					}
				}
				
				if(MaxDistance > 0)
				{
					X -= MaxDistance;
					Y -= MaxDistance;
					W += MaxDistance*2;
					H += MaxDistance*2;
					
					if(X < 0)
					{
						W -= X;
						X = 0;
					}
					if(Y < 0)
					{
						H -= Y;
						Y = 0;
					}
					
					W = clamp(W, 0, Tiles.get_width() - X);
					H = clamp(H, 0, Tiles.get_height() - Y);
				}
			}
			
			//Apply the material
			std::mt19937 RandomEngine(Seed);
			std::uniform_real_distribution<float> Distribution(0.0f, 1.0f);
			
			for(int j=Y; j<Y+H; j++)
			{
				for(int i=X; i<X+W; i++)
				{
					RandomEngine.seed(Seed + j*65536 + i);
					
					int Index = 0;
					int Flags = 0x0;
				
					//Apply rules
					CAsset_TilingMaterial::CIteratorRule RuleIter;
					for(RuleIter = pMaterial->ReverseBeginRule(); RuleIter != pMaterial->ReverseEndRule(); ++RuleIter)
					{
						bool IsValidRule = true;
						const std::vector<CAsset_TilingMaterial::CRule::CCondition>& Conditions = pMaterial->GetRuleConditionArray(*RuleIter);
						for(unsigned int c=0; c<Conditions.size(); c++)
						{
							if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_INDEX)
							{
								if(Tiles.get_clamp(i + Conditions[c].GetRelPosX(), j + Conditions[c].GetRelPosY()).GetBrush() != Conditions[c].GetValue())
								{
									IsValidRule = false;
									break;
								}
							}
							else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTINDEX)
							{
								if(Tiles.get_clamp(i + Conditions[c].GetRelPosX(), j + Conditions[c].GetRelPosY()).GetBrush() == Conditions[c].GetValue())
								{
									IsValidRule = false;
									break;
								}
							}
							else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_LABEL)
							{
								bool LabelIndexFound = false;
								CSubPath LabelPath = CAsset_TilingMaterial::SubPath_Label(Conditions[c].GetValue());
								if(pMaterial->IsValidLabel(LabelPath))
								{
									const std::vector<uint8>& Indices = pMaterial->GetLabelIndexArray(LabelPath);
									for(unsigned int li=0; li<Indices.size(); li++)
									{
										if(Indices[li] == Tiles.get_clamp(i + Conditions[c].GetRelPosX(), j + Conditions[c].GetRelPosY()).GetBrush())
										{
											LabelIndexFound = true;
											break;
										}
									}
								}
									
								if(!LabelIndexFound)
								{
									IsValidRule = false;
									break;
								}
							}
							else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOTLABEL)
							{
								bool LabelIndexFound = false;
								CSubPath LabelPath = CAsset_TilingMaterial::SubPath_Label(Conditions[c].GetValue());
								if(pMaterial->IsValidLabel(LabelPath))
								{
									const std::vector<uint8>& Indices = pMaterial->GetLabelIndexArray(LabelPath);
									for(unsigned int li=0; li<Indices.size(); li++)
									{
										if(Indices[li] == Tiles.get_clamp(i + Conditions[c].GetRelPosX(), j + Conditions[c].GetRelPosY()).GetBrush())
										{
											LabelIndexFound = true;
											break;
										}
									}
								}
									
								if(LabelIndexFound)
								{
									IsValidRule = false;
									break;
								}
							}
							else if(Conditions[c].GetType() == CAsset_TilingMaterial::CONDITIONTYPE_NOBORDER)
							{
								if(
									i + Conditions[c].GetRelPosX() <= 0 ||
									i + Conditions[c].GetRelPosX() >= Tiles.get_width()-1 ||
									j + Conditions[c].GetRelPosY() <= 0 ||
									j + Conditions[c].GetRelPosY() >= Tiles.get_height()-1
								)
								{
									IsValidRule = false;
									break;
								}
							}
						}
						
						float Probability = pMaterial->GetRuleProbability(*RuleIter);
						if(IsValidRule && (Probability >= 1.0f || Distribution(RandomEngine) < Probability))
						{
							Index = pMaterial->GetRuleTileIndex(*RuleIter);
							Flags = pMaterial->GetRuleTileFlags(*RuleIter);
							break;
						}
					}
					
					Tiles.get_clamp(i, j).SetIndex(Index);
					Tiles.get_clamp(i, j).SetFlags(Flags);
				}
			}
		}
	}
	else
	{
		for(int j=0; j<Tiles.get_height(); j++)
		{
			for(int i=0; i<Tiles.get_width(); i++)
			{
				if(Tiles.get_clamp(i, j).GetBrush() > 0)
					Tiles.get_clamp(i, j).SetIndex(1);
				else
					Tiles.get_clamp(i, j).SetIndex(0);
				Tiles.get_clamp(i, j).SetFlags(0x0);
			}
		}
	}
}
