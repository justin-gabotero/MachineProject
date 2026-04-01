# Contributing

Thanks for contributing.

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
- [ ] Build passes locally
- [ ] Tests pass locally (if tests exist)
- [ ] No new warnings introduced
- [ ] Changelog updated if needed
- [ ] Status-code behavior is documented and consistent (`0` success, `-1` failure, additional negative values only for clearly documented special cases)
- [ ] Output parameters are only written on success paths
- [ ] Local variables are declared at the top of C functions

Note: `scripts/bootstrap.sh` can generate baseline smoke tests. Treat those as a starting point and add project-specific tests for behavior changes.

For `fix/*` branches, structure the pull request description in this order:

1. Summary
2. Problem
3. Cause
4. Solution
5. Changes

Use `.github/PULL_REQUEST_TEMPLATE.md` as the default format source.

## Merge Strategy

- Default to **Squash and merge** so each pull request lands as one clean commit.
- GitHub preserves the individual commits in the pull request for reference, but the main branch history remains clean.
- Use a non-squash merge only when the commit series is intentionally structured and each commit is independently meaningful.
