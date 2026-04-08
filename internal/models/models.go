package models

import "time"

type User struct {
	ID                 int        `json:"id"`
	Username           string     `json:"username"`
	Email              string     `json:"email"`
	PasswordHash       string     `json:"-"`    // store only a bcrypt hash, never the raw password
	Role               int        `json:"role"` // 0 for recipient, 1 for donor
	EmailVerified      bool       `json:"email_verified"`
	PasswordResetToken string     `json:"-"`
	PasswordResetExp   *time.Time `json:"-"`
	VerifiedAt         *time.Time `json:"verified_at,omitempty"`
}

type Session struct {
	SessionID string    `json:"session_id"`
	UserID    int       `json:"user_id"`
	CreatedAt time.Time `json:"created_at"`
	ExpiresAt time.Time `json:"expires_at"`
	IPAddress string    `json:"ip_address,omitempty"`
}

// UserRegisterRequest is used for incoming registration payloads.
type UserRegisterRequest struct {
	Username string `json:"username"`
	Email    string `json:"email"`
	Password string `json:"password"`
	Role     int    `json:"role"`
}

// UserLoginRequest is used for incoming login payloads.
type UserLoginRequest struct {
	Username string `json:"username"`
	Password string `json:"password"`
}

// UserResponse is safe to return in API responses.
type UserResponse struct {
	ID            int        `json:"id"`
	Username      string     `json:"username"`
	Email         string     `json:"email"`
	Role          int        `json:"role"`
	EmailVerified bool       `json:"email_verified"`
	VerifiedAt    *time.Time `json:"verified_at,omitempty"`
}

// FoodItem represents a food item posted by a donor, which can be claimed by recipients.
type FoodItem struct {
	ID             int    `json:"id"`
	Name           string `json:"name"`
	Description    string `json:"description,omitempty"`
	Location       string `json:"location,omitempty"`
	ExpirationDate string `json:"expiration_date,omitempty"`
	UserID         int    `json:"user_id"` // ID of the user who posted the food item
}
