package config

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"
)

type SMTPConfig struct {
	Host     string
	Port     int
	Username string
	Password string
	FromAddr string
}

type Config struct {
	DataDir                   string
	SessionLifetime           time.Duration
	EmailVerificationRequired bool
	PasswordResetTokenExpiry  time.Duration
	SMTP                      *SMTPConfig
}

func LoadConfig() (*Config, error) {
	cfg := &Config{
		DataDir:                   os.Getenv("FOODCONNECT_DATA_DIR"),
		SessionLifetime:           7 * 24 * time.Hour, // 7 days
		EmailVerificationRequired: true,
		PasswordResetTokenExpiry:  1 * time.Hour,
		SMTP:                      nil,
	}

	allowedBase := filepath.Join(string(os.PathSeparator), "var", "lib", "foodconnect")
	if base, err := os.UserConfigDir(); err == nil && base != "" {
		allowedBase = filepath.Join(base, "foodconnect")
	}

	if cfg.DataDir == "" {
		cfg.DataDir = filepath.Join(allowedBase, "data") // default directory for storing the SQLite database
	}

	cfg.DataDir = filepath.Clean(cfg.DataDir)
	abs, err := filepath.Abs(cfg.DataDir)
	if err != nil {
		return nil, err
	}

	allowedBaseAbs, err := filepath.Abs(allowedBase)
	if err != nil {
		return nil, err
	}
	if !isWithinBase(abs, allowedBaseAbs) {
		return nil, fmt.Errorf("FOODCONNECT_DATA_DIR must be within %q", allowedBaseAbs)
	}

	cfg.DataDir = abs

	return cfg, nil
}

func isWithinBase(path, base string) bool {
	rel, err := filepath.Rel(base, path)
	if err != nil {
		return false
	}

	if rel == "." {
		return true
	}

	return !strings.HasPrefix(rel, "..") && rel != ".."
}
