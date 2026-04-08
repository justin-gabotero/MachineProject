package storage

import (
	"context"
	"database/sql"
	"os"
	"path/filepath"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

type SQLiteStore struct {
	db *sql.DB
}

// ----------------------------------------------------------------------------
// NewSQLiteStore
// ----------------------------------------------------------------------------

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
			email TEXT NOT NULL UNIQUE,
			password TEXT NOT NULL,
			role INTEGER NOT NULL,
			email_verified INTEGER NOT NULL DEFAULT 0,
			password_reset_token TEXT,
			password_reset_exp DATETIME,
			verified_at DATETIME
		);`,
		`CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);`, // index for faster lookups by email (for login and password reset)
		`CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);`,
		`CREATE TABLE IF NOT EXISTS sessions (
			session_id TEXT PRIMARY KEY,
			user_id INTEGER NOT NULL,
			created_at DATETIME NOT NULL,
			expires_at DATETIME NOT NULL,
			ip_address TEXT,
			FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
		);`,
		`CREATE INDEX IF NOT EXISTS idx_sessions_user_id ON sessions(user_id);`,
		`CREATE INDEX IF NOT EXISTS idx_sessions_expires_at ON sessions(expires_at);`, // faster lookup for cleaning expired sessions
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

// ----------------------------------------------------------------------------
// User operations
// ----------------------------------------------------------------------------

// CreateUser inserts a new user into the database.
func (s *SQLiteStore) CreateUser(ctx context.Context, username, email, passwordHash string, role int) (*sql.Row, error) {
	query := `INSERT INTO users (username, email, password, role, email_verified) VALUES (?, ?, ?, ?, 0) RETURNING id, username, email, password, role, email_verified, password_reset_token, password_reset_exp, verified_at`
	return s.db.QueryRowContext(ctx, query, username, email, passwordHash, role), nil
}

// GetUserByUsername retrieves a user by username.
func (s *SQLiteStore) GetUserByUsername(ctx context.Context, username string) (*sql.Row, error) {
	query := `SELECT id, username, email, password, role, email_verified, password_reset_token, password_reset_exp, verified_at FROM users WHERE username = ?`
	return s.db.QueryRowContext(ctx, query, username), nil
}

// GetUserByEmail retrieves a user by email.
func (s *SQLiteStore) GetUserByEmail(ctx context.Context, email string) (*sql.Row, error) {
	query := `SELECT id, username, email, password, role, email_verified, password_reset_token, password_reset_exp, verified_at FROM users WHERE email = ?`
	return s.db.QueryRowContext(ctx, query, email), nil
}

// GetUserByID retrieves a user by id.
func (s *SQLiteStore) GetUserByID(ctx context.Context, userID int) (*sql.Row, error) {
	query := `SELECT id, username, email, password, role, email_verified, password_reset_token, password_reset_exp, verified_at FROM users WHERE id = ?`
	return s.db.QueryRowContext(ctx, query, userID), nil
}

// UpdatePasswordResetToken stores a password reset token with expiration time.
func (s *SQLiteStore) UpdatePasswordResetToken(ctx context.Context, email, token string, expiresAt time.Time) error {
	query := `UPDATE users SET password_reset_token = ?, password_reset_exp = ? WHERE email = ?`
	_, err := s.db.ExecContext(ctx, query, token, expiresAt, email)
	return err
}

// VerifyUserEmail marks a user's email as verified and clears any verification token.
func (s *SQLiteStore) VerifyUserEmail(ctx context.Context, userID int) error {
	query := `UPDATE users SET email_verified = 1, verified_at = ?, password_reset_token = NULL, password_reset_exp = NULL WHERE id = ?`
	_, err := s.db.ExecContext(ctx, query, time.Now(), userID)
	return err
}

// ResetPassword applies a new password hash and clears the reset token.
func (s *SQLiteStore) ResetPassword(ctx context.Context, userID int, newPasswordHash string) error {
	query := `UPDATE users SET password = ?, password_reset_token = NULL, password_reset_exp = NULL WHERE id = ?`
	_, err := s.db.ExecContext(ctx, query, newPasswordHash, userID)
	return err
}

// UpdatePassword updates a user's password (for authenticated password change).
func (s *SQLiteStore) UpdatePassword(ctx context.Context, userID int, newPasswordHash string) error {
	query := `UPDATE users SET password = ? WHERE id = ?`
	_, err := s.db.ExecContext(ctx, query, newPasswordHash, userID)
	return err
}

// ----------------------------------------------------------------------------
// Session operations
// ----------------------------------------------------------------------------

// CreateSession inserts a new session into the database.
func (s *SQLiteStore) CreateSession(ctx context.Context, sessionID string, userID int, expiresAt time.Time, ipAddress string) error {
	query := `INSERT INTO sessions (session_id, user_id, created_at, expires_at, ip_address) VALUES (?, ?, ?, ?, ?)`
	_, err := s.db.ExecContext(ctx, query, sessionID, userID, time.Now(), expiresAt, ipAddress)
	return err
}

// GetSession retrieves a user by session ID, checking that the session hasn't expired.
func (s *SQLiteStore) GetSession(ctx context.Context, sessionID string) (*sql.Row, error) {
	query := `
		SELECT u.id, u.username, u.email, u.password, u.role, u.email_verified, 
		       u.password_reset_token, u.password_reset_exp, u.verified_at
		FROM sessions s
		JOIN users u ON s.user_id = u.id
		WHERE s.session_id = ? AND s.expires_at > ?
	`
	return s.db.QueryRowContext(ctx, query, sessionID, time.Now()), nil
}

// DeleteSession removes a session from the database.
func (s *SQLiteStore) DeleteSession(ctx context.Context, sessionID string) error {
	query := `DELETE FROM sessions WHERE session_id = ?`
	_, err := s.db.ExecContext(ctx, query, sessionID)
	return err
}

// DeleteUserSessions removes all sessions for a given user.
func (s *SQLiteStore) DeleteUserSessions(ctx context.Context, userID int) error {
	query := `DELETE FROM sessions WHERE user_id = ?`
	_, err := s.db.ExecContext(ctx, query, userID)
	return err
}

// CleanExpiredSessions removes all expired sessions.
func (s *SQLiteStore) CleanExpiredSessions(ctx context.Context) error {
	query := `DELETE FROM sessions WHERE expires_at <= ?`
	_, err := s.db.ExecContext(ctx, query, time.Now())
	return err
}
