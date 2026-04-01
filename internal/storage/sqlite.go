package storage

import (
	"database/sql"
	"os"
	"path/filepath"

	_ "github.com/mattn/go-sqlite3"
)

type SQLiteStore struct {
	db *sql.DB
}

// Initiliaze new `SQLiteStore` instance, create new db file if it doesnt exist
// then run migrations to set up the necessary tables.
func NewSQLiteStore(dataDir string) (*SQLiteStore, error) {
	// check if dataDir exists, if not create it
	if _, err := os.Stat(dataDir); os.IsNotExist(err) {
		err = os.MkdirAll(dataDir, 0755)
		if err != nil {
			return nil, err
		}
	}

	dbPath := filepath.Join(dataDir, "foodconnect")
	db, err := sql.Open("sqlite3", dbPath)
	if err != nil {
		return nil, err
	}

	store := &SQLiteStore{db: db}
	if err := store.migrate(); err != nil {
		return nil, err
	}

	return store, nil
}

// create necessary tables if they dont exist
func (s *SQLiteStore) migrate() error {
	queries := []string{
		`CREATE TABLE IF NOT EXISTS users (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			username TEXT NOT NULL UNIQUE,
			password TEXT NOT NULL,
			role INTEGER NOT NULL
		);`,
		`CREATE TABLE IF NOT EXISTS food_items (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name TEXT NOT NULL,
			description TEXT,
			location TEXT,
			expiration_date TEXT,
			user_id INTEGER,
			FOREIGN KEY(user_id) REFERENCES users(id)
		);`,
	}

	for _, query := range queries {
		if _, err := s.db.Exec(query); err != nil {
			return err
		}
	}

	return nil
}
