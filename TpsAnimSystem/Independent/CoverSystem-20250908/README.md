Cover System (Independent Drop-in)

Purpose
- Provide cover points, a cover object actor, and a detector component to let any Pawn/Character find and take cover without depending on a specific Player class.

Contents
- Source/CoverSystem/Public/CoverTypes.h
- Source/CoverSystem/Public/CoverPointComponent.h
- Source/CoverSystem/Private/CoverPointComponent.cpp
- Source/CoverSystem/Public/CoverObjectActor.h
- Source/CoverSystem/Private/CoverObjectActor.cpp
- Source/CoverSystem/Public/CoverDetectorComponent.h
- Source/CoverSystem/Private/CoverDetectorComponent.cpp
- Source/CoverSystem/Public/CoverUser.h

Notes
- This folder is not part of your build by default. Move into a UE module/plugin to compile.
- No Player-specific dependencies; uses APawn/ACharacter common properties only.
- Debug drawing indicates eligibility: Red (not candidate), Yellow (distance OK), Green (distance+angle OK), Cyan (best).

