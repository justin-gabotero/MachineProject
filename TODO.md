# TODO Template

Use this file as a working task board for the project. Move to GitHub Projects or an issue tracker when the project grows beyond a few tasks.

## Metadata

- Project: <project-name>
- Maintainer: <name>
- Last updated: <YYYY-MM-DD>

## Legend

- Priority: P0 (critical), P1 (high), P2 (normal), P3 (low)
- Status: `todo`, `in-progress`, `blocked`, `done`

## Backlog

| ID | Task | Priority | Owner | Due | Status | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| T-001 | <task description> | P1 | <name> | <YYYY-MM-DD> | todo | <details> |
| T-002 | <task description> | P2 | <name> | <YYYY-MM-DD> | todo | <details> |
| T-006 | Harden data directory handling for production: resolve symlinks, reduce TOCTOU risk, tighten directory permissions, and define env override policy | P1 | unassigned | 2026-04-01 | todo | Follow-up from config and storage safety review |
| T-007 | Design data access with clean interfaces so database engines can be swapped without changing business logic | P1 | unassigned | 2026-04-01 | todo | Prepare repository/service abstractions and adapter boundaries |
| T-008 | Define and document PostgreSQL as the primary production migration target from SQLite | P1 | unassigned | 2026-04-01 | todo | Include migration milestones, schema strategy, and cutover criteria |

## In Progress

| ID | Task | Priority | Owner | Started | Status | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| T-003 | <task description> | P1 | <name> | <YYYY-MM-DD> | in-progress | <details> |

## Blocked

| ID | Task | Blocker | Owner | Since | Status | Next Step |
| --- | --- | --- | --- | --- | --- | --- |
| T-004 | <task description> | <dependency/issue> | <name> | <YYYY-MM-DD> | blocked | <action> |

## Done

| ID | Task | Owner | Completed | Notes |
| --- | --- | --- | --- | --- |
| T-005 | <task description> | <name> | <YYYY-MM-DD> | <result> |

## This Week Focus

- [ ] <highest-impact deliverable>
- [ ] <second deliverable>
- [ ] <risk-reduction task>

## Notes

- <meeting decisions>
- <follow-ups>
