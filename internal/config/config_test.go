package config

import (
	"os"
	"path/filepath"
	"testing"
)

func TestLoadConfig_Default(t *testing.T) {
	// Ensure env var is unset so we get the default path
	t.Setenv("FOODCONNECT_DATA_DIR", "")

	cfg, err := LoadConfig()
	if err != nil {
		t.Fatalf("LoadConfig() unexpected error: %v", err)
	}
	if cfg.DataDir == "" {
		t.Error("expected non-empty DataDir")
	}
}

func TestLoadConfig_EnvOverride(t *testing.T) {
	// Use a valid subdirectory within the allowed base
	base, err := os.UserConfigDir()
	if err != nil {
		t.Skip("cannot determine user config dir:", err)
	}
	validDir := filepath.Join(base, "foodconnect", "data", "custom")
	t.Setenv("FOODCONNECT_DATA_DIR", validDir)

	cfg, err := LoadConfig()
	if err != nil {
		t.Fatalf("LoadConfig() unexpected error: %v", err)
	}
	if cfg.DataDir != validDir {
		t.Errorf("expected DataDir %q, got %q", validDir, cfg.DataDir)
	}
}

func TestLoadConfig_RejectsPathOutsideBase(t *testing.T) {
	t.Setenv("FOODCONNECT_DATA_DIR", "/tmp/sneaky-path")

	_, err := LoadConfig()
	if err == nil {
		t.Error("expected error for out-of-base path, got nil")
	}
}

func TestLoadConfig_RejectsTraversal(t *testing.T) {
	base, err := os.UserConfigDir()
	if err != nil {
		t.Skip("cannot determine user config dir:", err)
	}
	traversal := filepath.Join(base, "foodconnect", "..", "..", "etc", "passwd")
	t.Setenv("FOODCONNECT_DATA_DIR", traversal)

	_, err = LoadConfig()
	if err == nil {
		t.Error("expected error for path traversal, got nil")
	}
}

func TestIsWithinBase(t *testing.T) {
	tests := []struct {
		name string
		path string
		base string
		want bool
	}{
		{
			name: "exact match",
			path: "/var/lib/foodconnect",
			base: "/var/lib/foodconnect",
			want: true,
		},
		{
			name: "valid subdirectory",
			path: "/var/lib/foodconnect/data",
			base: "/var/lib/foodconnect",
			want: true,
		},
		{
			name: "nested subdirectory",
			path: "/var/lib/foodconnect/data/nested",
			base: "/var/lib/foodconnect",
			want: true,
		},
		{
			name: "sibling directory",
			path: "/var/lib/other",
			base: "/var/lib/foodconnect",
			want: false,
		},
		{
			name: "parent directory",
			path: "/var/lib",
			base: "/var/lib/foodconnect",
			want: false,
		},
		{
			name: "path traversal",
			path: "/var/lib/foodconnect/../../etc",
			base: "/var/lib/foodconnect",
			want: false,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got := isWithinBase(tt.path, tt.base)
			if got != tt.want {
				t.Errorf("isWithinBase(%q, %q) = %v, want %v", tt.path, tt.base, got, tt.want)
			}
		})
	}
}
