/**
* =============================================================================
* Source Python
* Copyright (C) 2012-2016 Source Python Development Team.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, the Source Python Team gives you permission
* to link the code of this program (as well as its derivative works) to
* "Half-Life 2," the "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, the Source.Python
* Development Team grants this exception to all derivative works.
*/

// ============================================================================
// >> INCLUDES
// ============================================================================
// Boost
#include "boost/unordered_map.hpp"

// Source.Python
#include "utilities/conversions.h"

#include "entities_datamaps.h"
#include ENGINE_INCLUDE_PATH(entities_datamaps_wrap.h)


// ============================================================================
// >> TYPEDEFS
// ============================================================================
typedef boost::unordered_map<std::string, int> OffsetsMap;
typedef boost::unordered_map<std::string, OffsetsMap > DataMapsMap;


// ============================================================================
// >> GLOBAL VARIABLES
// ============================================================================
DataMapsMap g_DataMapsCache;


// ============================================================================
// >> FORWARD DECLARATIONS
// ============================================================================
void AddDataMap(datamap_t* pDataMap, OffsetsMap& offsets, int offset=0, const char* baseName=NULL);


// ============================================================================
// >> FUNCTIONS
// ============================================================================
void AddDataDesc(typedescription_t& dataDesc, OffsetsMap& offsets, int offset, const char* baseName)
{
	if (dataDesc.fieldName == NULL)
		return;

	int currentOffset = offset + TypeDescriptionExt::get_offset(dataDesc);

	char* currentName = NULL;
	if (baseName == NULL) {
		currentName = (char*) dataDesc.fieldName;
	}
	else {
		char tempName[256];
		sprintf(tempName, "%s.%s", baseName, dataDesc.fieldName);
		currentName = strdup(tempName);
	}

	if (dataDesc.fieldType == FIELD_EMBEDDED)
	{
		AddDataMap(dataDesc.td, offsets, currentOffset, currentName);
	}
	else
	{
		offsets.insert(std::make_pair(currentName, currentOffset));
	}
}

void AddDataMap(datamap_t* pDataMap, OffsetsMap& offsets, int offset, const char* baseName)
{
	for (int i=0; i < pDataMap->dataNumFields; i++)
	{
		AddDataDesc(pDataMap->dataDesc[i], offsets, offset, baseName);
	}
}


// ============================================================================
// >> DataMapSharedExt
// ============================================================================
typedescription_t& DataMapSharedExt::__getitem__(const datamap_t& pDataMap, int iIndex)
{
	if (iIndex < 0 || iIndex > (pDataMap.dataNumFields - 1))
	{
		BOOST_RAISE_EXCEPTION(PyExc_IndexError, "Invalid index.");
	}
	return pDataMap.dataDesc[iIndex];
}

typedescription_t* DataMapSharedExt::find(datamap_t* pDataMap, const char *szName)
	{
		while (pDataMap)
		{
			for (int iCurrentIndex=0; iCurrentIndex < pDataMap->dataNumFields; iCurrentIndex++)
			{
				typedescription_t& pCurrentDataDesc = pDataMap->dataDesc[iCurrentIndex];
				if ((pCurrentDataDesc.fieldName && strcmp(szName, pCurrentDataDesc.fieldName) == 0) ||
					(pCurrentDataDesc.externalName && strcmp(szName, pCurrentDataDesc.externalName) == 0))
				{
					return &(pDataMap->dataDesc[iCurrentIndex]);
				}
				else if (pCurrentDataDesc.fieldType == FIELD_EMBEDDED)
				{
					typedescription_t *pReturnValue = find(pCurrentDataDesc.td, szName);
					if (pReturnValue)
					{
						return pReturnValue;
					}
				}
			}
			pDataMap = pDataMap->baseMap;
		}
		return NULL;
	}

int DataMapSharedExt::find_offset(datamap_t* pDataMap, const char* name)
{
	while (pDataMap)
	{
		DataMapsMap::iterator offsets = g_DataMapsCache.find(pDataMap->dataClassName);
		if (offsets == g_DataMapsCache.end())
		{
			offsets = g_DataMapsCache.insert(std::make_pair(pDataMap->dataClassName, OffsetsMap())).first;
			AddDataMap(pDataMap, offsets->second);
		}
		
		OffsetsMap::iterator result = offsets->second.find(name);
		if (result == offsets->second.end())
		{
			pDataMap = pDataMap->baseMap;
		}
		else {
			return result->second;
		}
	}
	return -1;
}


// ============================================================================
// >> TypeDescriptionSharedExt
// ============================================================================
BoostInputFn TypeDescriptionSharedExt::get_input_function(const typedescription_t& pTypeDesc)
{
	if (!(pTypeDesc.flags & FTYPEDESC_INPUT))
		BOOST_RAISE_EXCEPTION(PyExc_TypeError, "\"%s\" is not an input.", pTypeDesc.fieldName);

	return boost::bind(pTypeDesc.inputFunc, _1, _2);
}

void* TypeDescriptionSharedExt::get_function(const typedescription_t& pTypeDesc)
{
	return (void*&) pTypeDesc.inputFunc;
}


// ============================================================================
// >> VariantSharedExt
// ============================================================================
const char* VariantSharedExt::get_string(variant_t *pVariant)
{
	return STRING(pVariant->StringID());
}

void VariantSharedExt::set_string(variant_t *pVariant, const char *szValue)
{
	return pVariant->SetString(MAKE_STRING(szValue));
}

Color* VariantSharedExt::get_color(variant_t *pVariant)
{
	color32 pColor32 = pVariant->Color32();
	return new Color((int)pColor32.r, (int)pColor32.g, (int)pColor32.b, (int)pColor32.a);
}

void VariantSharedExt::set_color(variant_t *pVariant, Color *pColor)
{
	pVariant->SetColor32(pColor->r(), pColor->g(), pColor->b(), pColor->a());
}

Vector VariantSharedExt::get_vector(variant_t *pVariant)
{
	Vector pVector;
	pVariant->Vector3D(pVector);
	return pVector;
}

unsigned int VariantSharedExt::get_entity(variant_t *pVariant)
{
	return ExcIndexFromBaseHandle(pVariant->Entity());
}

void VariantSharedExt::set_entity(variant_t *pVariant, unsigned int uiEntity)
{
	pVariant->SetEntity(ExcBaseEntityFromIndex(uiEntity));
}


// ============================================================================
// >> InputDataSharedExt
// ============================================================================
inputdata_t* InputDataSharedExt::__init__()
{
	inputdata_t *pInputData = new inputdata_t;
	pInputData->pActivator = NULL;
	pInputData->pCaller = NULL;
	pInputData->nOutputID = 0;
	return pInputData;
}

unsigned int InputDataSharedExt::get_activator(const inputdata_t& pInputData)
{
	return ExcIndexFromBaseEntity(pInputData.pActivator);
}

void InputDataSharedExt::set_activator(inputdata_t *pInputData, unsigned int uiActivator)
{
	pInputData->pActivator = ExcBaseEntityFromIndex(uiActivator);
}

unsigned int InputDataSharedExt::get_caller(const inputdata_t& pInputData)
{
	return ExcIndexFromBaseEntity(pInputData.pCaller);
}
	
void InputDataSharedExt::set_caller(inputdata_t *pInputData, unsigned int uiCaller)
{
	pInputData->pCaller = ExcBaseEntityFromIndex(uiCaller);
}