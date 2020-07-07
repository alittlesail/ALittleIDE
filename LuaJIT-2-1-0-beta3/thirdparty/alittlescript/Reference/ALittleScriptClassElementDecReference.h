﻿
#ifndef _ALITTLE_ALITTLESCRIPTCLASSELEMENTDECREFERENCE_H_
#define _ALITTLE_ALITTLESCRIPTCLASSELEMENTDECREFERENCE_H_

#include "ALittleScriptReferenceTemplate.h"

#include "../Generate/ALittleScriptClassElementDecElement.h"

class ALittleScriptClassElementDecReference : public ALittleScriptReferenceTemplate<ALittleScriptClassElementDecElement>
{
    ABnfGuessError CheckError() override;

    int QueryClassificationTag(bool& blur) override;
};

#endif // _ALITTLE_ALITTLESCRIPTCLASSELEMENTDECREFERENCE_H_