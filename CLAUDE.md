## Purpose

This document defines the **engineering principles, workflow, and guardrails**
for building a **Neo Geo emulator** in **C**, targeting correctness first,
then performance, portability, and maintainability.

The goal is to **emulate the hardware accurately**, not to approximate it.

---

## Core Principles

### 1. Don’t Guess — Be Data-Driven

- Always rely on **official documentation**, **service manuals**, **datasheets**, and **die-verified research**.
- Prefer:
  - SNK schematics and service manuals
  - Chip datasheets (e.g. 68000, Z80, YM2610)
  - MAME source comments and test cases (as a reference, not a blueprint)
- If behavior is unclear:
  - Document the unknown
  - Add a TODO with sources
  - Write a test that captures the uncertainty

No “it probably works like this”.

---

### 2. Baby Steps — One Change at a Time

- Implement **one small, verifiable feature per step**
- After each step:
  - Build
  - Run unit tests
  - Validate against known behavior (logs, test ROMs, traces)
- Never stack multiple unverified assumptions in a single change

Slow is smooth, smooth is fast.

---

### 3. Plan Ahead, Measure, Iterate

- Before coding:
  - Define the **scope** of the step
  - Define **success criteria**
- After coding:
  - Measure correctness first
  - Measure performance only when correctness is established
- Iterate intentionally:
  - Make it work
  - Make it correct
  - Make it clean
  - Make it fast (last)

---

### 4. Coding Style & Quality

- **C (C11 or newer)**, no compiler extensions unless justified
- Prefer:
  - Explicit types
  - Small, composable functions
  - Clear ownership and lifetimes
- Avoid:
  - Hidden globals
  - Implicit state changes
  - Clever tricks over clarity

Readable > clever.

---

### 5. Tests Are Mandatory

- Every subsystem must have **unit tests**
- Tests are required for:
  - CPU instructions
  - Timers
  - Memory mapping
  - I/O behavior
- Tests should:
  - Be deterministic
  - Run headless
  - Be fast enough to run on every build

If it’s not testable, the design is wrong.

---

## Development Workflow

1. Pick **one hardware component**
2. Gather **primary sources**
3. Write **tests first** (or alongside)
4. Implement the minimal behavior
5. Verify against:
   - Test ROMs
   - Known logs / traces
6. Commit
7. Move to the next smallest step

---

## Architecture Guidelines

- Hardware-oriented structure:
  - CPU(s)
  - Memory map
  - Video
  - Audio
  - I/O
- Emulation code must be:
  - Platform-agnostic
  - Independent from rendering, audio backends, and UI
- Platform code lives at the edges only

---

## Portability

- Target:
  - Linux
  - macOS
  - Windows
- Use:
  - Standard C library
  - Thin platform abstraction layers where needed
- No OS-specific logic in core emulation

---

## What We Explicitly Avoid (For Now)

- Premature optimization
- JIT / dynarec
- Heavily threaded designs
- GUI polish
- Feature creep

Correctness first.

---

## Optional (Nice to Have, Later)

- Golden test logs (known-good traces)
- Per-frame determinism checks
- Replay / input recording
- Cycle-accuracy validation tools

Only add these when they solve a real problem.

---

## Guiding Question

> “Can we prove this is correct?”

If the answer is “not yet”, stop and verify before moving on.
