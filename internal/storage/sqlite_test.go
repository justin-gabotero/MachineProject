package storage

import (
	"testing"
)

// newTestStore creates a SQLiteStore backed by a temporary directory.
// It is automatically cleaned up when the test ends.
func newTestStore(t *testing.T) *SQLiteStore {
	t.Helper()
	store, err := NewSQLiteStore(t.TempDir())
	if err != nil {
		t.Fatalf("NewSQLiteStore() unexpected error: %v", err)
	}
	t.Cleanup(func() { store.db.Close() })
	return store
}

// ----------------------------------------------------------------------------
// NewSQLiteStore
// ----------------------------------------------------------------------------

func TestNewSQLiteStore_CreatesDatabase(t *testing.T) {
	store := newTestStore(t)
	if store == nil {
		t.Fatal("expected non-nil store")
	}
}

func TestNewSQLiteStore_InvalidDir(t *testing.T) {
	// A file path used as a directory should fail.
	t.Skip("TODO: implement once storage methods are in place")
}

// ----------------------------------------------------------------------------
// User operations — fill in once methods are implemented on SQLiteStore
// ----------------------------------------------------------------------------

func TestCreateUser(t *testing.T) {
	t.Skip("TODO: implement once CreateUser is defined")

	// store := newTestStore(t)
	// user := &models.User{Username: "alice", PasswordHash: "hash", Role: 0}
	// id, err := store.CreateUser(user)
	// ...
}

func TestGetUserByUsername(t *testing.T) {
	t.Skip("TODO: implement once GetUserByUsername is defined")
}

func TestGetUserByUsername_NotFound(t *testing.T) {
	t.Skip("TODO: implement once GetUserByUsername is defined")
}

func TestCreateUser_DuplicateUsername(t *testing.T) {
	t.Skip("TODO: implement once CreateUser is defined")
}

// ----------------------------------------------------------------------------
// FoodItem operations — fill in once methods are implemented on SQLiteStore
// ----------------------------------------------------------------------------

func TestCreateFoodItem(t *testing.T) {
	t.Skip("TODO: implement once CreateFoodItem is defined")
}

func TestGetFoodItems(t *testing.T) {
	t.Skip("TODO: implement once GetFoodItems is defined")
}

func TestGetFoodItemsByUser(t *testing.T) {
	t.Skip("TODO: implement once GetFoodItemsByUser is defined")
}

func TestDeleteFoodItem(t *testing.T) {
	t.Skip("TODO: implement once DeleteFoodItem is defined")
}

func TestDeleteFoodItem_WrongOwner(t *testing.T) {
	t.Skip("TODO: verify that a non-owner cannot delete a food item")
}
