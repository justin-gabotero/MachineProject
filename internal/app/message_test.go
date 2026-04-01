package app

import "testing"

func TestMessage(t *testing.T) {
  if Message() != "Hello, Go!" {
    t.Fatalf("unexpected message: %q", Message())
  }
}
