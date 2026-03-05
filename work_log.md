
## Objective
- Refactor the `run` command so that execution logic is separated from CLI parsing and can be reused by other entry points (tests, batch runs).

## Decisions
### Decision
- Decision: Separate the `run` command into a thin CLI layer and a reusable execution function.
- Alternatives considered:
  - Keep all logic inside the CLI command
  - Duplicate logic for future commands
- Why:
  - Keeping logic inside the CLI would tightly couple user interaction with simulation execution, making testing and automation harder.

## What I did
- Identified which parts of `run` are CLI-specific vs execution-specific
- Extracted execution logic into a separate function/module.

# 31-12-2025 

## Objective 
- Write `pass_to_core()` function. It takes the input json path and output dir path and run the sbe engine with those arguments. 

## Decisions 
### Decision 1 
- Decision: Fixed the path where the binary file of simulation engine to be at core/build/bin/sbe_engine.
- Alternatives considered:
  - Keep the sbe_engine in build directory itself.
- Why:
  - Cleaner separation of concerns. All the binaries will be in the binary folder inside build more industry standard.

## What I did 
- I m

