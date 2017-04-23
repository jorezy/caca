/* ====================================================================
 * File: SmsEnginePanics.pan
 * Created: 08/03/06
 * Author: 
 * Copyright (c): , All rights reserved
 * ==================================================================== */

#ifndef __SMSENGINE_PAN__
#define __SMSENGINE_PAN__

_LIT(applicationName,"SMSEngine");

/** SMSEngine application panic codes */
enum TSmsEnginePanics 
    {
    ESmsNoServiceSenterNumberSet = 1,
    ESmsEngineInternal,
    ESmsEngineNotInitialized
    // add further panics here
    };

inline void Panic(TSmsEnginePanics aReason)
    {
    User::Panic(applicationName, aReason);
    }

#endif // __SMSENGINE_PAN__
