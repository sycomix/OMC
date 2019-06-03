//**************************************************************************************
// Filename:	NibDialogControl.cp
//				Part of Contextual Menu Workshop by Abracode Inc.
//				http://free.abracode.com/cmworkshop/
// Copyright � 2002-2004 Abracode, Inc.  All rights reserved.
//
//
//**************************************************************************************
// Revision History:
// July 21, 2004 - Original
//**************************************************************************************

#include "NibDialogControl.h"
#include "OMCDialog.h"
#include "CFObj.h"

//get the controlID from __NIB_DIALOG_CONTROL_XXX_VALUE__ or environment variable style OMC_NIB_DIALOG_CONTROL_XXX_VALUE
//TODO: if we implement more control id modifiers they should be added here and masked before being used for searching real IDs
CFStringRef
CreateControlIDFromString(CFStringRef inControlIDString, bool isEnvStyle)
{
	if(inControlIDString == NULL)
		return NULL;
	
//the first part of the string is constant:
	CFStringRef prefixString = isEnvStyle ? CFSTR("OMC_NIB_DIALOG_CONTROL_") : CFSTR("__NIB_DIALOG_CONTROL_");
	CFIndex prefixLen = ::CFStringGetLength(prefixString);
	CFStringRef suffixString = isEnvStyle ? CFSTR("_VALUE") : CFSTR("_VALUE__");
	CFIndex suffixLen  =  ::CFStringGetLength(suffixString);
	
	CFIndex actualLen = ::CFStringGetLength(inControlIDString);

//there needs to be at least one character between prefix and suffix
	if(actualLen < (prefixLen + 1 + suffixLen) )
		return NULL;

	if( ::CFStringHasPrefix(inControlIDString, prefixString) == false)
		return NULL;

	if( ::CFStringHasSuffix(inControlIDString, suffixString) == false)
		return NULL;

	CFRange idStrRange;
	idStrRange.location = prefixLen;
	idStrRange.length = actualLen - prefixLen - suffixLen;
	
	//in OMC 3 it does not have to be a number
	return ::CFStringCreateWithSubstring(kCFAllocatorDefault, inControlIDString, idStrRange);

//	CFObj<CFStringRef> numberStr( ::CFStringCreateWithSubstring(kCFAllocatorDefault, inControlIDString, numberStrRange) );
//	if(numberStr != NULL)
//	{
//		return ::CFStringGetIntValue (numberStr);
//	}
//
//	return NULL;
}

//get the params from __NIB_TABLE_XXX_COLUMN_XXX_VALUE__ or environment variable style OMC_NIB_TABLE_XXX_COLUMN_XXX_VALUE
//TODO: if we implement more control id modifiers they should be added here and masked before being used for searching real IDs
CFStringRef
CreateTableIDAndColumnFromString(CFStringRef inControlIDString, CFIndex &outColumnIndex, bool useAllRows, bool isEnvStyle)
{
	outColumnIndex = 0;

	if(inControlIDString == NULL)
		return NULL;
	
//the first part of the string is constant:
	CFStringRef prefixString = isEnvStyle ? CFSTR("OMC_NIB_TABLE_") : CFSTR("__NIB_TABLE_");
	CFIndex prefixLen = ::CFStringGetLength(prefixString);

	CFStringRef columnString = CFSTR("_COLUMN_");
	CFIndex columnLen  =  ::CFStringGetLength(columnString);
	
	CFStringRef suffixString = NULL;
	if( useAllRows )
		suffixString = isEnvStyle ? CFSTR("_ALL_ROWS") : CFSTR("_ALL_ROWS__");
	else
		suffixString = isEnvStyle ? CFSTR("_VALUE") : CFSTR("_VALUE__");

	CFIndex suffixLen  =  ::CFStringGetLength(suffixString);
	
	CFIndex actualLen = ::CFStringGetLength(inControlIDString);

//there needs to be at least one character between prefix and column and one between column and suffix
	if(actualLen < (prefixLen + 1 + columnLen + 1 + suffixLen) )
		return NULL;

	if( ::CFStringHasPrefix(inControlIDString, prefixString) == false)
		return NULL;

	if( ::CFStringHasSuffix(inControlIDString, suffixString) == false)
		return NULL;

	CFRange columnStrRange = ::CFStringFind( inControlIDString, columnString, 0 );
	if(columnStrRange.length != columnLen)
		return NULL;

	CFStringRef controlID = NULL;
	CFRange idStrRange;
	idStrRange.location = prefixLen;
	idStrRange.length = columnStrRange.location - prefixLen;
	if( idStrRange.length > 0)
	{
		controlID = ::CFStringCreateWithSubstring(kCFAllocatorDefault, inControlIDString, idStrRange);
		//if(numberStr != NULL)
		//	controlID = ::CFStringGetIntValue (numberStr);
	}

	CFRange numberStrRange;
	numberStrRange.location = columnStrRange.location + columnStrRange.length;
	numberStrRange.length = actualLen - (columnStrRange.location + columnStrRange.length) - suffixLen;
	if( numberStrRange.length > 0 )
	{
		CFObj<CFStringRef> numberStr( ::CFStringCreateWithSubstring(kCFAllocatorDefault, inControlIDString, numberStrRange) );
		if(numberStr != NULL)
			outColumnIndex = (CFIndex)::CFStringGetIntValue (numberStr);
	}
	return controlID;
}

CFStringRef kControlModifierString_AllRows = CFSTR("|ALL_ROWS");

CFStringRef
CreateControlIDByStrippingModifiers(CFStringRef inControlIDWithModifiers, UInt32 &outModifiers)
{
	if(inControlIDWithModifiers == NULL)
		return NULL;

	outModifiers = 0;
	Boolean hasAllRows = CFStringHasSuffix(inControlIDWithModifiers, kControlModifierString_AllRows);
	if(hasAllRows)
	{
		outModifiers |= kControlModifier_AllRows;
	
		CFIndex totalLen = CFStringGetLength(inControlIDWithModifiers);
		CFIndex modifierLen = CFStringGetLength(kControlModifierString_AllRows);
		CFRange mainControlIDRange = CFRangeMake(0, totalLen - modifierLen);
		return CFStringCreateWithSubstring(kCFAllocatorDefault, inControlIDWithModifiers, mainControlIDRange);
	}
		
	CFRetain(inControlIDWithModifiers);
	return inControlIDWithModifiers;
}

CFStringRef
CreateControlIDByAddingModifiers(CFStringRef inControlID, UInt32 inModifiers)
{
	if(inControlID == NULL)
		return NULL;

	if(inModifiers == 0)
	{
		CFRetain(inControlID);
		return inControlID;
	}

	CFObj<CFStringRef> outControlID;
	if( (inModifiers & kControlModifier_AllRows) != 0 )
	{
		outControlID.Adopt( CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@%@"), inControlID, kControlModifierString_AllRows) );
	}
	return outControlID.Detach();
}

