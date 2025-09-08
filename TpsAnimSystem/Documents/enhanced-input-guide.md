# Enhanced Input Guide (TPS Character)

이 문서는 `ATpsCharacter`에 Enhanced Input 애셋을 연결하는 절차를 안내합니다. 코드에서는 인풋 애셋을 생성하지 않으며, 아래 단계에 따라 콘텐츠 브라우저에서 직접 생성/연결합니다.

## 사전 준비
- 플러그인: Enhanced Input 활성화
- 프로젝트가 UE5 Enhanced Input 경로를 사용 중인지 확인 (기존 Input System과 혼용하지 않음)

## 인풋 애셋 생성
1) 인풋 액션(Input Action) 5개 생성
   - `IA_Move`  
     - Value Type: Axis2D
   - `IA_Look`  
     - Value Type: Axis2D
   - `IA_Jump`  
     - Value Type: Digital (bool)
   - `IA_Sprint`  
     - Value Type: Digital (bool)
   - `IA_Crouch`  
     - Value Type: Digital (bool)

2) 매핑 컨텍스트(Input Mapping Context) 생성
   - 예: `IMC_Player`
   - 아래 권장 바인딩을 추가합니다(키보드/마우스/패드 예시).

## 권장 바인딩 예시
- Move (`IA_Move`, Axis2D)
  - Keyboard: W → (Y=+1), S → (Y=-1), A → (X=-1), D → (X=+1)
  - Gamepad: Left Stick → 2D Axis
- Look (`IA_Look`, Axis2D)
  - Mouse: Turn (X), LookUp (Y)
  - Gamepad: Right Stick → 2D Axis
- Jump (`IA_Jump`, Digital)
  - Keyboard: Spacebar
  - Gamepad: Face Button Bottom (A/Cross)
- Sprint (`IA_Sprint`, Digital)
  - Keyboard: Left Shift (hold 또는 toggle 용)
  - Gamepad: Left Stick Press (L3) 또는 Left Shoulder
- Crouch (`IA_Crouch`, Digital)
  - Keyboard: Left Ctrl (toggle)
  - Gamepad: B/Circle 또는 Right Stick Press (R3)

참고: 축 스케일 또는 감도는 프로젝트 선호에 맞게 조정하세요. 마우스 룩 감도는 인풋 스케일(예: 1.0)과 프로젝트 마우스 감도 설정을 함께 고려합니다.

## 캐릭터 블루프린트 연결
1) C++ 클래스 `ATpsCharacter` 기반 블루프린트 생성 (예: `BP_TpsCharacter`).
2) 클래스 디폴트에서 다음 속성을 할당합니다.
   - Input
     - `DefaultMappingContext` → `IMC_Player`
     - `InputMappingPriority` → 0 (또는 다른 시스템과 병행 시 우선순위 조정)
   - Input | Actions
     - `MoveAction` → `IA_Move`
     - `LookAction` → `IA_Look`
     - `JumpAction` → `IA_Jump`
     - `SprintAction` → `IA_Sprint`
     - `CrouchAction` → `IA_Crouch`
   - Movement
     - `WalkSpeed` / `SprintSpeed` (예: 450 / 650)
     - `bSprintToggle` (선호하는 방식 선택: true=토글, false=홀드)

3) 레벨 배치/게임모드 설정
   - 레벨에 `BP_TpsCharacter`를 배치하거나, GameMode의 Default Pawn Class로 지정합니다.
   - 플레이어 컨트롤러는 기본값으로 충분합니다. `ATpsCharacter`가 `BeginPlay`에서 `DefaultMappingContext`를 `UEnhancedInputLocalPlayerSubsystem`에 추가합니다.

## 동작 개요 (코드 바인딩)
- Move: Axis2D → 컨트롤러 Yaw 기준 전/측방 입력 적용
- Look: Axis2D → Yaw/Pitch 입력
- Jump: Started=Jump(), Completed/Canceled=StopJumping()
- Sprint: Toggle 모드일 때 Started에서 토글, Hold 모드일 때 Started=활성/Completed=해제
- Crouch: Started에서 토글(Crouch/UnCrouch)

## 트러블슈팅
- 입력이 동작하지 않을 때
  - Enhanced Input 플러그인이 활성인지 확인
  - `DefaultMappingContext`/각 인풋 액션이 블루프린트에 정상 할당되었는지 확인
  - 레벨에서 실제로 `BP_TpsCharacter`가 플레이어 폰으로 사용되는지 확인
  - 다른 시스템이 Mapping Context 우선순위로 덮어쓰지 않는지 확인 (`InputMappingPriority` 조정)

## 유지보수 규칙
- 인풋 액션/매핑 컨텍스트 변경 시 본 문서를 함께 갱신합니다.
- 키 바인딩 표는 실제 애셋 상태를 반영하도록 최신화합니다.
- 코드 변경 없이도 에디터 설정만으로 동작이 바뀌는 부분을 명확히 표기합니다.

