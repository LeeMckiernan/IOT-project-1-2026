// Color-codes a dB value: red ≥ 100, amber 80–99, green < 80
function dbClass(db) {
  if (db >= 100) return 'db-red'
  if (db >= 80)  return 'db-amber'
  return 'db-green'
}

// Renders sound events as a table; expects events from Firestore with Timestamp fields
export default function EventTable({ events }) {
  return (
    <table className="event-table">
      <thead>
        <tr>
          <th>Time</th>
          <th>dB</th>
          <th>Duration</th>
          <th>Device</th>
        </tr>
      </thead>
      <tbody>
        {events.map(event => (
          <tr key={event.id}>
            {/* .toDate() converts a Firestore Timestamp to a JS Date */}
            <td>{event.time.toDate().toLocaleTimeString()}</td>
            <td><span className={dbClass(event.db)}>{event.db}</span></td>
            <td>{event.duration}s</td>
            <td>{event.deviceName}</td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
