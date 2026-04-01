package models

type User struct {
	ID           int    `json:"id"`
	Username     string `json:"username"`
	PasswordHash string `json:"-"`    // store only a bcrypt/argon2 hash, never the raw password
	Role         int    `json:"role"` // 0 for recipient, 1 for donor
}

// UserRegisterRequest is used for incoming registration payloads.
type UserRegisterRequest struct {
	Username string `json:"username"`
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
	ID       int    `json:"id"`
	Username string `json:"username"`
	Role     int    `json:"role"`
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
