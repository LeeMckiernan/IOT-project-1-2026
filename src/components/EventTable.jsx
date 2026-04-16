function dbClass(db) {
  if (db >= 100) return 'db-red'
  if (db >= 80) return 'db-amber'
  return 'db-green'
}

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
