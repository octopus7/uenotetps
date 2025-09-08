# [001] TPS character enhanced input

요청 요약:
- 보편적인 TPS 이동/카메라를 가진 캐릭터 C++ 클래스 추가.
- Enhanced Input 사용. 코드에서 매핑 컨텍스트/인풋 액션은 생성하지 않음(애셋은 사용자가 생성/연결).
- 입력 설정 가이드는 별도 MD로 작성하고, AGENTS 지침에도 반영.

응답:
- 새로운 C++ 캐릭터 클래스 `ATpsCharacter`를 추가했습니다.
  - 파일: `Source/TpsAnimSystem/TpsCharacter.h`, `Source/TpsAnimSystem/TpsCharacter.cpp`
  - 구성: `USpringArmComponent` + `UCameraComponent`로 TPS 카메라, 이동(앞/뒤/좌/우), 마우스/패드 룩, 점프, 스프린트(홀드/토글), 앉기 토글.
  - 캐릭터 이동 기본값: 이동 시 방향으로 회전(`bOrientRotationToMovement=true`), 카메라 붐 길이 300cm, 점프 Z는 600.
  - 스프린트 속도 전환: `WalkSpeed`/`SprintSpeed`(기본 450/650). `bSprintToggle`로 토글/홀드 전환.
  - Enhanced Input 바인딩: `Move`(Axis2D), `Look`(Axis2D), `Jump`(Digital), `Sprint`(Digital), `Crouch`(Digital).
  - 코드에서는 애셋을 생성하지 않습니다. 블루프린트에서 아래 UPROPERTY에 할당해 사용합니다.
    - `DefaultMappingContext`(추가 우선순위: `InputMappingPriority`)
    - `MoveAction`, `LookAction`, `JumpAction`, `SprintAction`, `CrouchAction`
  - `BeginPlay`에서 `DefaultMappingContext`가 지정된 경우 `UEnhancedInputLocalPlayerSubsystem`에 `AddMappingContext`만 수행합니다(애셋 자체는 사용자가 생성/할당).

- 입력/애셋 설정 가이드 문서를 추가했습니다.
  - 경로: `Docs/enhanced-input-guide.md`
  - 내용: 인풋 액션/매핑 컨텍스트 애셋 생성, 추천 바인딩, 블루프린트에서 `ATpsCharacter` 속성 연결 절차.

- AGENTS 지침을 갱신했습니다.
  - 입력 가이드 문서의 위치와 유지보수 규칙을 명시했습니다.

다음 단계(사용자 수행):
1) 언리얼 에디터에서 인풋 애셋(IA/IMC) 생성 → `Docs/enhanced-input-guide.md` 절차 참조.
2) `ATpsCharacter`를 기반으로 블루프린트 생성(BP_TpsCharacter 예시) 후 위 속성에 애셋 연결.
3) 레벨/게임모드에 BP를 기본 폰으로 지정하고 플레이.

