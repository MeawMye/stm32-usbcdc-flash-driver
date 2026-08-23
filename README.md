# STM32 USB CDC Flash Driver

STM32F429ZI 보드에서 USB CDC 통신을 통해 내부 Flash의 읽기, 삭제, 쓰기 동작을 시험하는 펌웨어 프로젝트입니다.

USB CDC와 물리 UART를 공통 인터페이스로 추상화했으며, PC의 시리얼 터미널에서 간단한 명령을 전송해 Flash를 제어할 수 있습니다.

## 개발 환경

- Board: NUCLEO-F429ZI
- MCU: STM32F429ZI
- Framework: STM32Cube HAL
- USB Class: CDC Virtual COM Port
- IDE: STM32CubeIDE
- Language: C

## 주요 기능

- USB CDC 데이터 송수신
- 512바이트 CDC RX 링버퍼
- USB CDC와 USART1의 공통 UART 인터페이스
- USART1 DMA 수신
- Flash 섹터 검색 및 Bank 판별
- Flash erase, half-word write, memory-mapped read
- USB 시리얼 명령을 통한 Flash 동작 시험

## 동작 구조

```mermaid
flowchart LR
    PC["PC 시리얼 터미널"]
    CDC["USB CDC"]
    BUFFER["RX 링버퍼"]
    UART["UART 추상화"]
    APP["명령 처리"]
    FLASH["Internal Flash"]

    PC --> CDC
    CDC --> BUFFER
    BUFFER --> UART
    UART --> APP
    APP --> FLASH
    APP -. "결과 출력" .-> PC
```

애플리케이션은 USB CDC를 직접 호출하지 않고 공통 UART API를 사용합니다.

- `_DEF_UART1`: USB CDC
- `_DEF_UART2`: USART1 + RX DMA

## 사용 방법

1. STM32CubeIDE에서 `APP` 프로젝트를 엽니다.
2. 프로젝트를 빌드하고 NUCLEO-F429ZI에 기록합니다.
3. 생성된 USB Virtual COM Port를 시리얼 터미널로 엽니다.
4. 다음 명령을 입력합니다.

| 입력 | 동작 |
|---|---|
| `1` | `0x08010000`부터 32바이트 읽기 |
| `2` | 대상 Flash 섹터 삭제 |
| `3` | `0x00`부터 `0x1F`까지 32바이트 기록 |

권장 시험 순서:

```text
2 → Erase
3 → Write
1 → Read
```

> Flash 삭제는 바이트가 아닌 섹터 단위로 수행됩니다. 현재 시험 주소 `0x08010000`은 64KB 크기의 Sector 4에 포함됩니다.

## 실행 흐름

```text
main()
 ├─ hwInit()   : HAL, USB, UART, LED 등 초기화
 ├─ apInit()   : USB CDC 및 USART1 채널 개방
 └─ apMain()   : LED 토글 및 Flash 명령 처리
```

## 프로젝트 구조

```text
APP/src/
├── ap/          애플리케이션 및 명령 처리
├── bsp/         보드 초기화와 시스템 설정
├── common/      공통 정의와 링버퍼
├── hw/driver/   Flash, USB, UART 등 하드웨어 드라이버
├── lib/         STM32Cube HAL 및 USB 생성 코드
└── main.c       프로그램 진입점
```

## 추가로 개선이 필요한 점

- 쓰기 명령에서 `erase → write → verify` 절차 통합
- Flash 주소와 크기 유효성 검사
- 실행 중인 펌웨어 영역 삭제 방지
- 홀수 크기 Flash write 처리
- Bank 경계를 넘는 erase 요청 처리
- CDC overflow 및 Flash 오류 응답 추가
- 주소와 payload를 지정할 수 있는 명령 프로토콜 구현

`flashWrite()`가 내부에서 항상 erase를 수행하도록 하면 같은 섹터의 다른 데이터까지 삭제될 수 있습니다. 따라서 드라이버의 erase와 write는 분리하고, 상위 명령 처리 계층에서 다음 순서를 관리하는 방식이 안전합니다.

```text
주소 검사 → 섹터 삭제 → 데이터 기록 → 읽기 검증
```

## 참고

이 프로젝트는 STM32 내부 Flash와 USB CDC 통신 구조를 학습하고 검증하기 위한 프로젝트입니다. 실제 제품에 적용하려면 Flash 영역 보호, 데이터 무결성 검증 및 전원 차단 복구 기능을 추가해야 합니다.
