package handlers

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"
)

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------

// postJSON builds a POST request with a JSON body.
func postJSON(t *testing.T, path string, body any) *http.Request {
	t.Helper()
	data, err := json.Marshal(body)
	if err != nil {
		t.Fatalf("json.Marshal: %v", err)
	}
	req := httptest.NewRequest(http.MethodPost, path, bytes.NewReader(data))
	req.Header.Set("Content-Type", "application/json")
	return req
}

// ----------------------------------------------------------------------------
// POST /register
// ----------------------------------------------------------------------------

func TestRegister_Success(t *testing.T) {
	t.Skip("TODO: implement once Register handler is defined")

	// w := httptest.NewRecorder()
	// req := postJSON(t, "/register", map[string]any{
	// 	"username": "alice",
	// 	"password": "Secret123!",
	// 	"role":     0,
	// })
	// Register(w, req)
	// if w.Code != http.StatusCreated {
	// 	t.Errorf("expected 201, got %d", w.Code)
	// }
}

func TestRegister_MissingFields(t *testing.T) {
	t.Skip("TODO: implement once Register handler is defined")
	// Send empty body; expect 400 Bad Request.
}

func TestRegister_WeakPassword(t *testing.T) {
	t.Skip("TODO: implement if password strength validation is added")
}

func TestRegister_DuplicateUsername(t *testing.T) {
	t.Skip("TODO: implement once Register handler is defined")
	// Register the same username twice; expect 409 Conflict on second call.
}

func TestRegister_InvalidRole(t *testing.T) {
	t.Skip("TODO: implement if role validation is added")
}

// ----------------------------------------------------------------------------
// POST /login
// ----------------------------------------------------------------------------

func TestLogin_Success(t *testing.T) {
	t.Skip("TODO: implement once Login handler is defined")

	// w := httptest.NewRecorder()
	// req := postJSON(t, "/login", map[string]any{
	// 	"username": "alice",
	// 	"password": "Secret123!",
	// })
	// Login(w, req)
	// if w.Code != http.StatusOK {
	// 	t.Errorf("expected 200, got %d", w.Code)
	// }
	// Verify response contains a token / session cookie.
}

func TestLogin_WrongPassword(t *testing.T) {
	t.Skip("TODO: implement once Login handler is defined")
	// Expect 401 Unauthorized.
}

func TestLogin_UnknownUser(t *testing.T) {
	t.Skip("TODO: implement once Login handler is defined")
	// Expect 401 Unauthorized (same error as wrong password — no username enumeration).
}

func TestLogin_MissingFields(t *testing.T) {
	t.Skip("TODO: implement once Login handler is defined")
	// Expect 400 Bad Request.
}
