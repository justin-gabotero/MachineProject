# FoodConnect

FoodConnect is a food donation and request-matching platform in the Philippines. It is designed to connect food donors with recipients through food listings, and matching-related data.

## Project Purpose

The repository currently serves two goals:

- Preserve the original C implementation used in the initial university project.
- Build and evolve a Go backend that is easier to maintain and extend.

FoodConnect is planned as a full web application. Development is intentionally backend-first so the
data model, business logic, and API foundations are stable before frontend work begins.

## Migration from C to Go

The original codebase of FoodConnect was written in C for the purpose of a university project. I decided to migrate the backend to GoLang to improve scalability and ease of maintenance.

## Repository Structure

- `c/`: Legacy C implementation and tests.
- `internal/`: Go backend packages (config, storage, API, app logic).
- `cmd/app/`: Go application entrypoint.
- `docs/`: Style and project documentation.

## Requirements
- Go `1.24` or higher
- C compiler (for `go-sqlite3` or for the legacy C code if needed)

## Setup

```sh
# Clone the repository
git clone https://github.com/justin-gabotero/FoodConnect
cd FoodConnect
# Install dependencies
go mod tidy
# Run the Go app entrypoint
go run ./cmd/app
```

## Current Status

- Core backend modules are being implemented in Go.
- SQLite is used for local persistence.
- Test coverage is being expanded as modules are completed.
- Frontend implementation is planned after backend milestones are complete.

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your changes.