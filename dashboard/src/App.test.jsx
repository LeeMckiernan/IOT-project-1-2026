import { render, screen } from '@testing-library/react'
import { vi } from 'vitest'

vi.mock('./firebase', () => ({ auth: {} }))
vi.mock('./components/Login', () => ({ default: () => <div>Login Screen</div> }))
vi.mock('./components/Dashboard', () => ({
  default: ({ user }) => <div>Dashboard for {user.email}</div>,
}))

import { onAuthStateChanged } from 'firebase/auth'
vi.mock('firebase/auth', () => ({ onAuthStateChanged: vi.fn() }))

import App from './App'

test('renders login when not authenticated', () => {
  onAuthStateChanged.mockImplementation((auth, cb) => { cb(null); return () => {} })
  render(<App />)
  expect(screen.getByText('Login Screen')).toBeInTheDocument()
})

test('renders dashboard when authenticated', () => {
  onAuthStateChanged.mockImplementation((auth, cb) => {
    cb({ email: 'user@test.com' })
    return () => {}
  })
  render(<App />)
  expect(screen.getByText('Dashboard for user@test.com')).toBeInTheDocument()
})
