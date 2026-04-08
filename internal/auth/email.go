package auth

import (
	"context"
	"fmt"
	"log"
)

// EmailSender defines the interface for sending emails.
type EmailSender interface {
	SendVerificationEmail(ctx context.Context, email, verificationToken string) error
	SendPasswordResetEmail(ctx context.Context, email, resetToken string) error
}

// Placeholder implementation of EmailSender for mvp development.
type LogEmailSender struct{}

// SendVerificationEmail logs a verification email.
func (s *LogEmailSender) SendVerificationEmail(ctx context.Context, email, verificationToken string) error {
	link := fmt.Sprintf("https://app.example.com/verify-email?token=%s", verificationToken)
	msg := fmt.Sprintf("Verification email for %s:\n%s\n", email, link)
	log.Println("[EMAIL - VERIFICATION]", msg)
	return nil
}

// SendPasswordResetEmail logs a password reset email.
func (s *LogEmailSender) SendPasswordResetEmail(ctx context.Context, email, resetToken string) error {
	link := fmt.Sprintf("https://app.example.com/reset-password?token=%s", resetToken)
	msg := fmt.Sprintf("Password reset email for %s:\n%s\n", email, link)
	log.Println("[EMAIL - PASSWORD RESET]", msg)
	return nil
}

// NewLogEmailSender creates a new LogEmailSender.
func NewLogEmailSender() *LogEmailSender {
	return &LogEmailSender{}
}
