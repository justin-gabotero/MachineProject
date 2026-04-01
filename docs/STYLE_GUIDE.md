# Style Guide

This guide defines default standards that apply to all languages unless a language pack says otherwise.

## Core Principles

- Prefer readability over cleverness.
- Keep functions small and focused.
- Handle errors explicitly.
- Avoid hidden side effects.
- Keep behavior deterministic where possible.

## Naming

- Use descriptive names.
- Use consistent casing per language norms.
- Avoid abbreviations unless widely accepted.

## Functions

- Validate inputs at function boundaries.
- Keep one clear responsibility per function.
- Document non-obvious assumptions.

### C Project Conventions (Reference Profile)

Use this profile when you want behavior consistent with the C conventions from
this project.

- Use status-code driven flow in non-trivial functions.
- Prefer explicit status variables (for example `int status = -1;`).
- Use these default status meanings unless the module requires a special code:
	- `0`: success
	- `-1`: generic failure or invalid input
	- Additional negative codes: reserved for explicit cases (for example
		interrupted input).
- Define local variables at the top of the function body.
- Avoid pre-terminating output parameters on failure paths.
	- Do not mutate output structs/buffers until validation is complete.
	- Write to output parameters only on success.
- Prefer single-exit style for non-trivial functions.
	- Initialize a status/result variable, update it through guarded logic, and
		return once at the end when practical.
- If a module already uses a documented status-code map, preserve it.

Reference pattern:

```c
int updateThing(const Thing *in, Thing *out) {
	int status = -1;
	Thing temp;

	if (in != NULL && out != NULL) {
		if (isValidThing(in)) {
			temp = *in;
			status = 0;
		}

		if (status == 0) {
			*out = temp;
		}
	}

	return status;
}
```

## Comments And Docs

- Document intent, not obvious syntax.
- Keep comments accurate after refactors.
- Use a standard API doc format per language.

C/C++ Doxygen baseline:

```c
/**
 * @brief One-line summary.
 * @param input Description.
 * @param[out] output Description.
 * @return 0 on success, non-zero on failure.
 */
```

## Errors

- Return clear errors or throw explicit exceptions, based on language norms.
- Do not silently ignore failures.
- Include enough context in error messages.
- For C functions, document exact status codes in the Doxygen `@return`
	section.

## Testing

- Add tests for behavior changes when a test framework exists.
- Cover happy path and at least one failure path.
- Keep tests deterministic and isolated.

## Formatting

- Use `.editorconfig` defaults.
- Use language formatter and linter when available.
- Do not mix unrelated reformatting with logic changes.
