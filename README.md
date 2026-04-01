# FoodConnect

FoodConnect is a web application used to connect food donors with food banks and people in need.

## Planned Features
- Web interface for donors to list their available food items and for recipients to browse and request food.
- Automatic matching of donors and recipients based on location and food preferences.
- Optional notifications for donors and recipients about new matches and updates on their requests.

## Migration from C to GoLang

The original codebase of FoodConnect was written in C for the purpose of a university project. I decided to migrate the backend to GoLang to improve scalability and ease of maintenance.

## Requirements
- Go `1.24` or higher
- A web server (`nginx` or `apache`) for deployment (optional)
- C compiler (for `go-sqlite3`)

## Setup

```sh
# Clone the repository
git clone https://github.com/justin-gabotero/FoodConnect
cd FoodConnect
# Install dependencies
go mod tidy
# Run the application
go run main.go
```

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your changes.