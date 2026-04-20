import { render, screen } from '@testing-library/react'
import { vi } from 'vitest'
import Login from './Login'

vi.mock('../firebase', () => ({ auth: {} }))
vi.mock('firebase/auth', () => ({
  GoogleAuthProvider: vi.fn().mockImplementation(() => ({})),
  signInWithPopup: vi.fn(),
}))

test('renders app title', () => {
  render(<Login />)
  expect(screen.getByText('IOT Dashboard')).toBeInTheDocument()
})

test('renders sign in button', () => {
  render(<Login />)
  expect(screen.getByText(/sign in with google/i)).toBeInTheDocument()
})
