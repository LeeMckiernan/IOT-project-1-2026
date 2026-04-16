import { render, screen } from '@testing-library/react'
import EventTable from './EventTable'

const mockEvents = [
  {
    id: '1',
    time: { toDate: () => new Date('2024-01-01T14:32:01') },
    db: 102,
    duration: 5,
    deviceName: 'garage-sensor',
  },
  {
    id: '2',
    time: { toDate: () => new Date('2024-01-01T14:28:47') },
    db: 87,
    duration: 2,
    deviceName: 'garage-sensor',
  },
  {
    id: '3',
    time: { toDate: () => new Date('2024-01-01T14:19:22') },
    db: 74,
    duration: 1,
    deviceName: 'garage-sensor',
  },
]

test('renders all event rows', () => {
  render(<EventTable events={mockEvents} />)
  expect(screen.getAllByText('garage-sensor')).toHaveLength(3)
})

test('renders dB values', () => {
  render(<EventTable events={mockEvents} />)
  expect(screen.getByText('102')).toBeInTheDocument()
  expect(screen.getByText('87')).toBeInTheDocument()
  expect(screen.getByText('74')).toBeInTheDocument()
})

test('renders duration with s suffix', () => {
  render(<EventTable events={mockEvents} />)
  expect(screen.getByText('5s')).toBeInTheDocument()
})

test('applies db-red class for dB >= 100', () => {
  render(<EventTable events={mockEvents} />)
  expect(screen.getByText('102')).toHaveClass('db-red')
})

test('applies db-amber class for dB 80-99', () => {
  render(<EventTable events={mockEvents} />)
  expect(screen.getByText('87')).toHaveClass('db-amber')
})

test('applies db-green class for dB < 80', () => {
  render(<EventTable events={mockEvents} />)
  expect(screen.getByText('74')).toHaveClass('db-green')
})
