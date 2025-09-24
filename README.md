# ClairObscur
시네마틱 실루엣을 강조한 턴제 보스전 프로토타입.

## 목차
1. [프로젝트 개요](#프로젝트-개요)
2. [개발 타임라인](#개발-타임라인)
3. [시스템 구성](#시스템-구성)
4. [플로우 & 설계 구조](#플로우--설계-구조)
5. [추가 정보](#추가-정보)

## 프로젝트 개요
- Unreal Engine 5.6 기반의 3rd 팀 프로젝트로, 둘 이상의 플레이어 캐릭터와 다단계 보스를 중심으로 한 시네마틱 턴제 전투를 지향했다.
- 코어 게임 플레이는 QTE 타이밍과 패링 윈도우를 활용한 액션 리듬에 초점을 두었고, 레벨 시퀀스와 카메라 연출로 몰입감을 강화했다.
- 개발은 세 차례 마일스톤(프로토타입 → 알파 → 베타)으로 관리해 시스템 확장과 연출 자산 통합을 단계적으로 진행했다.

## 개발 타임라인
### 프로토타입 (2025.09.03 ~ 2025.09.15)
- ce83146과 2bc1dd0에서 전투 액터와 플레이어·무기 베이스 클래스를 마련하고, 5bef0e와 831eaf7에서 턴 관리와 FSM 컴포넌트의 뼈대를 세웠다.
- 9545259로 BattleTiming 컴포넌트를 도입하고, 1417bf와 96963f1에서 체력·AP 동기화를 구현해 첫 전투 루프를 완성했다.
- 8162a98와 759b44를 통해 에너미 스켈레톤 및 HP 연계를 마무리한 뒤, 911f2e 커밋에서 “프로토타입 ver1”을 확정했다.

### 알파 (2025.09.16 ~ 2025.09.19)
- 7563d21과 49b134e로 알파 맵과 카메라 포지션을 정비해 전투 무대를 구성했다.
- 91183d9, 95c7ccb, 4f7d630에서 전투 HUD와 턴 UI를 연결하고 알파 빌드 플로우를 다듬었다.
- c0dad23, 740ab96, b64b1a에서 무기·스킬 연출을 통합했고, 53ff3a와 1fe446b에서 Alpha Fin 단계로 전환했다.

### 베타 (2025.09.20 ~ 2025.09.24)
- 3c2a84와 e7ead8에서 2페이즈 보스 FSM과 패턴을 추가하고, 21fa3e8과 3ee9e20으로 전투 시퀀스를 스크립트에 통합했다.
- 6d8263, d465b0d, 988e78f에서 로코모션, 히트스탑, 패링 피드백을 고도화했다.
- 5be99ab의 그래플링 맵과 eb3ee35 엔딩 시퀀스로 레벨 플로우를 마감한 뒤, 5af51c에서 “Beta FinFin” 커밋으로 최종 통합을 완료했다.

## 시스템 구성
### 플레이어
- 주요 클래스는 Source/ClairObscur/PlayerDirectory/PlayerBase.h:21, Source/ClairObscur/PlayerDirectory/PlayerFSM.h:16, Source/ClairObscur/CharacterComponent/SkillComponent.h:17이며 입력 바인딩·턴 FSM·스킬 실행을 담당한다.
- PlayerBase는 스프링암·카메라, 그랩플·스킬 컴포넌트, 무기 스폰 로직을 묶어 캐릭터 제어를 총괄하고 체력·AP 변경을 브로드캐스트한다.
- PlayerFSM은 ECommandedPlayerState 기반으로 행동 선택, 스킬 이동, 히트스톱, 복귀 루틴을 ExecuteSkill에서 일괄 처리한다.
- PlayerAnimInstance와 PlayerNotifyClass는 노티파이를 통해 QTE 성공, 패링, 카운터 연출을 호출하고 베타 단계에서 그래플링 컴포넌트와 연동된다.

### 에너미
- 주요 클래스는 Source/ClairObscur/Enemy/Enemy.h:20, Source/ClairObscur/Enemy/EnemyFSM.h:13, Source/ClairObscur/Enemy/EnemyAnimInstance.h:12이다.
- AEnemy는 스킬 데이터 테이블(SkillRow) 기반 패턴과 HP 관리, 패링 윈도우 브로드캐스트를 제공하며 2페이즈 전환 플래그를 포함한다.
- UEnemyFSM은 Idle→Move→Attack→Damage→Die 상태 전이를 처리하고, 패링 대응 및 카운터 공격 플래그(CounterAttackIng)를 제어한다.
- EnemyAnimInstance와 AnimNotifyStateParry는 공격 노티파이 구간에서 패링 가능 상태를 트리거해 전투 타이밍 시스템과 정보를 교환한다.

### 전투 시스템
- 중심 액터는 Source/ClairObscur/GameSystem/BattleManager.h:13의 ABattleManager로, FSM·턴·카메라·UI·결과 컴포넌트를 소유한 전투 컨트롤러다.
- BattleFSMComponent(Source/ClairObscur/GameSystem/Component/BattleFSMComponent.h:10)는 StartBattle→SelectAction→SelectSkill→SelectTarget→PlayerPlayAction→EnemyPlayAction→Waiting→PhaseChange→End 순환을 관리한다.
- BattleTurnComponent(Source/ClairObscur/GameSystem/Component/BattleTurnComponent.h:24)는 현재 턴 캐릭터를 추적하고, BattleTimingComponent(Source/ClairObscur/GameSystem/Component/BattleTimingComponent.cpp:61)는 공격·패링 QTE 타이밍과 결과 브로드캐스트를 담당한다.
- BattleUIComponent(Source/ClairObscur/GameSystem/Component/BattleUIComponent.h:14)는 선택 UI, 타이밍 위젯, 승·패 위젯을 상태에 따라 토글하며, BattleCameraComponent는 컷신형 카메라 전환을 제공한다.
- BattleDamageCalculateComponent와 BattleResultDataComponent는 피해량 산정, 승리 결과 기록, 엔딩 연출 트리거를 담당한다.

### 공통 레벨 디자인
- BattleFieldComponent(Source/ClairObscur/GameSystem/Component/BattleFieldComponent.h:24)는 플레이어·에너미 포지션 노드를 관리해 전투 시작 위치를 제어한다.
- 커밋 1bd8d92, 364cecc, 770876에서 레벨 시퀀스와 환경 요소(비, 로프, 그래플링) 배치가 진행되었고 ALevelSequenceActor 태그(예: WinCinematic)로 전투 결과와 연동된다.
- Content 폴더의 시네마틱 리소스와 Saved/의 레벨 시퀀스 설정은 전투 단계별 연출, 페이즈 전환, 엔딩을 통합한다.

## 플로우 & 설계 구조
1. 월드에서 ABattleManager::StartBattle가 호출되면 플레이어 입력을 잠그고 전투 컴포넌트 초기화를 실행한다.
2. BattleFSMComponent가 SelectAction 상태로 전환하면서 BattleUIComponent와 BattleCameraComponent가 선택 UI와 시점을 맞춘다.
3. 플레이어가 스킬을 확정하면 PlayerFSM::ExecuteSkill이 이동 및 몽타주를 처리하고, BattleTimingComponent가 QTE 성공 여부를 판정한다.
4. 공격 이벤트는 데미지 Delegate를 통해 BattleDamageCalculateComponent로 전달되어 HP를 갱신하고, 에너미 FSM이 맞대응한다.
5. 턴 종료 시 BattleTurnComponent::AdvanceTurn이 다음 행동 주체를 선택하며, 필요 시 PhaseChange 상태를 통해 보스 2페이즈 또는 승·패 시퀀스를 호출한다.
6. BattleResultDataComponent가 승패 데이터를 정리하고, BattleUIComponent가 엔딩 위젯과 레벨 시퀀스를 띄운 뒤 ABattleManager::EndBattle로 제어를 반환한다.

## 추가 정보
### 빌드 & 실행
- ClairObscur.uproject는 Unreal Engine 5.6에 연동되어 있으므로 동일 버전 이상의 에디터에서 열어야 한다.
- Visual Studio 2022와 Rider 모두 지원하며, ClairObscur.sln으로 C++ 모듈을 빌드한 후 에디터에서 콘텐츠를 리빌드한다.
- 첫 실행 전 DerivedDataCache나 Saved 폴더를 정리하면 시퀀스 파일 업데이트 시 충돌을 줄일 수 있다.

### 조작 체계
- 필드 이동: WASD 이동, Shift 런(ia_run), Space 점프(ia_jump).
- 전투 입력: Q/W/E/R 스킬 선택, F 상호작용 혹은 카운터, ESC는 이전 단계로 되돌아간다.
- QTE/패링: 타이밍 위젯이 표시될 때 입력하면 판정되며, 성공 시 “PERFECT” 또는 “PARRIED” 팝업(BattleTimingComponent)으로 피드백된다.

### QA 체크리스트
- 전투 시작 전 BattleUIComponent 위젯 로드 여부와 HUD 표시 상태를 확인한다.
- PlayerFSM 스킬 실행 시 AP가 정상 차감되고 복귀 이동이 완료되는지 반복 테스트한다.
- 에너미 패링 윈도우(AnimNotifyStateParry)가 정상적으로 열리고 닫히는지 시퀀스 타이밍과 비교한다.
- 2페이즈 진입 플래그(AEnemy::bPhaseTwo) 및 BattleFSMComponent::PhaseChange가 블루프린트에서도 일관되게 호출되는지 검증한다.

### 향후 개선 제안
- 스킬 데이터 테이블과 애니메이션 자산을 데이터 파이프라인으로 정규화해 디자이너 편집성을 높인다.
- BattleManager의 상태별 분기 로직을 서브 상태 머신 또는 Behavior Tree로 분리해 유지보수를 개선한다.
- 패링 실패 시 카메라 쉐이크, 사운드 큐 강화 등 리스크·리워드 피드백을 추가한다.
- 그래플링 맵과 전투 맵 사이의 시네마틱 전환을 더 정제해 플레이어 위치 동기화를 자동화한다.

