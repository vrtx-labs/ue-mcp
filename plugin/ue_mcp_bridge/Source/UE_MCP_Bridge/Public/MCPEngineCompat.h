#pragma once

// Engine headers that moved between UE 5.4 and 5.5, in one place.
//
// The plugin builds on UE 5.3 through 5.8. Several headers this module includes
// were relocated in 5.5, when StructUtils graduated from an experimental plugin
// into CoreUObject and PerPlatformProperties moved out of Engine. A translation
// unit that hard-codes either spelling compiles on one half of the supported
// range and fails with C1083 on the other, which is what a 5.4 user sees: 30-odd
// files failing on 'StructUtils/InstancedStruct.h' before anything else is even
// attempted.
//
// Include this header instead of the moved ones. It is include paths and one
// type alias, no API shims: the types themselves are the same on both sides
// of each move. 5.3 lays these headers out the way 5.4 does.
//
// On 5.4 the StructUtils module comes in transitively: Chooser and
// StateTreeModule both list it as a public dependency and this module depends
// on both, so the include path and the symbols are already there. Nothing has
// to be added to Build.cs or the .uplugin for it.

#include "Runtime/Launch/Resources/Version.h"

#if (ENGINE_MAJOR_VERSION > 5) || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5)
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/PropertyBag.h"
#include "StructUtils/StructView.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UObject/PerPlatformProperties.h"
#else
// UE 5.4: StructUtils is Engine/Plugins/Experimental/StructUtils, whose public
// headers sit at the include root; UserDefinedStruct and PerPlatformProperties
// are still in Engine.
#include "InstancedStruct.h"
#include "PropertyBag.h"
#include "StructView.h"
#include "Engine/UserDefinedStruct.h"
#include "PerPlatformProperties.h"
#endif

// EMovieSceneTimeUnit arrived in 5.4, in MovieScene. 5.3 spells the same two
// values ESequenceTimeUnit, in the SequencerScripting plugin, and its sequencer
// scripting libraries take that type. One alias, so call sites name the modern
// enum on every engine.
#if (ENGINE_MAJOR_VERSION > 5) || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4)
#include "MovieSceneTimeUnit.h"
#else
#include "SequenceTimeUnit.h"
using EMovieSceneTimeUnit = ESequenceTimeUnit;
#endif
