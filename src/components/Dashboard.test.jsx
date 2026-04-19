import { render, screen } from '@testing-library/react'
import { vi } from 'vitest'
import Dashboard from './Dashboard'

vi.mock('../firebase', () => ({ auth: {}, db: {} }))
vi.mock('firebase/auth', () => ({ signOut: vi.fn() }))
vi.mock('firebase/firestore', () => ({
  collection: vi.fn(),
  onSnapshot: vi.fn(() => () => {}),
  orderBy: vi.fn(),
  query: vi.fn(),
  limit: vi.fn(),
}))

const mockUser = { email: 'user@example.com' }

test('renders user email in header', () => {
  render(<Dashboard user={mockUser} />)
  expect(screen.getByText('user@example.com')).toBeInTheDocument()
})

test('renders sign out button', () => {
  render(<Dashboard user={mockUser} />)
  expect(screen.getByText('Sign out')).toBeInTheDocument()
})

test('renders Sound Events title', () => {
  render(<Dashboard user={mockUser} />)
  expect(screen.getByText('Sound Events')).toBeInTheDocument()
})
