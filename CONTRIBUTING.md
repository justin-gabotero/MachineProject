# Contributing

Thanks for contributing.

## Project Context

FoodConnect is a mixed-language repository:

- `c/` contains the original university-project implementation. This code is preserved for reference and learning purposes but is not actively developed.
- `internal/` and `cmd/` contain the actively developed Go backend.

When contributing, preserve behavior where appropriate and keep changes scoped to the subsystem you are updating.

## Workflow

1. Create a branch from `main`.
2. Use branch naming format: `type/short-description`. example: `feat/add-new-feature`.
3. Keep pull requests focused and small when possible.
4. Link related issues in your pull request.

## Commit Format

Use short conventional prefixes:

- `feat:` new functionality
- `fix:` bug fixes
- `docs:` documentation updates
- `refac:` code changes without behavior change
- `test:` test changes
- `chore:` tooling or maintenance
- `prefix(category):` optional category for larger projects

> [!NOTE]
> Commits without a recognized prefix will be categorized as `chore` by default.

## Pull Request Requirements

Before opening a pull request:

- [ ] Code follows `docs/STYLE_GUIDE.md`
- [ ] New or changed public functions are documented
- [ ] Build passes locally and on CI
- [ ] Tests pass locally
- [ ] No new warnings introduced
- [ ] Changelog updated if needed

Recommended local checks:

- Go changes: `go test ./...`
- C changes: run tests under `c/test/` with the provided Makefile/tooling

For `fix/*` branches, structure the pull request description in this order:

1. Summary
2. Problem
3. Cause
4. Solution
5. Changes

Use `.github/PULL_REQUEST_TEMPLATE.md` as the default format source.