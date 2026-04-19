import { signOut } from 'firebase/auth'
import { collection, limit, onSnapshot, orderBy, query } from 'firebase/firestore'
import { useEffect, useState } from 'react'
import { auth, db } from '../firebase'
import EventTable from './EventTable'

export default function Dashboard({ user }) {
  const [events, setEvents] = useState([])

  useEffect(() => {
    const q = query(
      collection(db, 'events'),
      orderBy('time', 'desc'),
      limit(100)
    )
    return onSnapshot(q, snap => {
      setEvents(snap.docs.map(doc => ({ id: doc.id, ...doc.data() })))
    })
  }, [])

  return (
    <div className="dashboard">
      <header className="dashboard-header">
        <span className="app-title">Sound Events</span>
        <div className="header-right">
          <span className="user-email">{user.email}</span>
          <button className="signout-btn" onClick={() => signOut(auth)}>
            Sign out
          </button>
        </div>
      </header>
      <main className="dashboard-main">
        <EventTable events={events} />
      </main>
    </div>
  )
}
