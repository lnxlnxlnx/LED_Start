# Repository Guidelines

## Project Structure & Module Organization
`USER/` contains the application entry point, interrupt handlers, and Keil project files such as `F103RC.uvprojx`. `CORE/` and `STM32F10x_FWLib/` provide CMSIS startup code and the STM32F10x Standard Peripheral Library. Shared platform code lives in `SYSTEM/`, board drivers live in `HARDWARE/`, and feature-level demos live in `IO/`. Third-party or reusable libraries are grouped under `easylogger/`, `XinYueC/`, `ulos/`, and `MALLOC/`. Generated outputs are written to `OBJ/` and `build/`; treat both as build artifacts.

## Build, Test, and Development Commands
Use Keil uVision or EIDE; this repo is not set up for CMake or Make.

- `UV4.exe USER\\F103RC.uvprojx -b Template` builds the Keil target and emits `.axf`/`.hex` into `OBJ/`.
- Open `.eide/eide.yml` in Embedded IDE and build target `Template` when working outside Keil.
- `USER\\F103RC.BAT` shows the generated ARMCC 5 build flow; use it for reference, not as a hand-maintained script.

Flash and runtime verification are hardware-dependent. Confirm output over UART after each firmware change.

## Coding Style & Naming Conventions
Follow the existing embedded C style: 4-space indentation in new code, braces on their own lines for functions, and short module-oriented filenames such as `led.c`, `rtc.h`, and `key_led_one.c`. Keep driver APIs in matching `.h/.c` pairs. Use `CamelCase` for vendor/library types already present, `snake_case` for local functions, and uppercase macros like `STM32F10X_HD`. Avoid editing generated Keil metadata unless the project configuration changes.

## Testing Guidelines
There is no automated host-side test suite. Most checks are board-level smoke tests wired into `USER/main.c`, with helper headers under `IO/XinYue/` and `ulos/test/`. Add new tests as small callable functions, invoke them temporarily from `main()`, and verify expected UART logs or peripheral behavior on the STM32F103RC board.

## Commit & Pull Request Guidelines
Recent commits use short, task-focused messages in Chinese that describe the immediate hardware or porting step. Keep commits scoped to one hardware feature or library change. Pull requests should state the target board/peripheral, summarize files touched, include build status, and attach test evidence such as serial output, photos, or a brief hardware reproduction note.

## Configuration & Artifacts
Key preprocessor symbols are defined in the Keil target: `STM32F10X_HD` and `USE_STDPERIPH_DRIVER`. Update include paths or target settings in `USER/F103RC.uvprojx` and `.eide/eide.yml` together to keep both IDE flows aligned.

## Tutorial Documentation Requirements
This repository now includes a beginner-oriented learning track under `Tutorial/` for combining XinYueC with design patterns in embedded C.

- Keep the tutorial index in `Tutorial/README.md` and update reading order whenever new tutorial files are added.
- Keep course-level planning in `Tutorial/01_*.md` to `Tutorial/06_*.md` (overview, 12-week outline, pattern mapping, milestones, 23-pattern handbook, and implementation template).
- Keep lesson details in `Tutorial/details/` as split files (for example: `L01_*.md`, `L02_*.md`, `L03_*.md`).
- Each detailed lesson file must include: learning goals, beginner-friendly concept explanation, repository-specific hands-on steps, common pitfalls, exercises/thought questions, and acceptance criteria.
- For design pattern guidance, always provide three concrete elements: when to use it, where it maps in this project, and the first minimal implementation step.
- Prefer practical, code-near explanations over abstract theory. Examples should reference real repository paths such as `HARDWARE/`, `IO/`, `USER/`, and `XinYueC/`.
- When updating `Tutorial/02_12周课程大纲.md`, sync related files in `Tutorial/details/` so outline and detailed lessons stay consistent.
