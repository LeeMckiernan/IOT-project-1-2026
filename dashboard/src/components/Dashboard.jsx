// Main dashboard view — streams the latest 100 sound events from Firestore in real time
import { signOut } from 'firebase/auth'
import { collection, limit, onSnapshot, orderBy, query } from 'firebase/firestore'
import { useEffect, useState } from 'react'
import { auth, db } from '../firebase'
import EventTable from './EventTable'

export default function Dashboard({ user }) {
  const [events, setEvents] = useState([])

  useEffect(() => {
    // Query the 'events' collection, newest first, capped at 100 rows
    const q = query(
      collection(db, 'events'),
      orderBy('time', 'desc'),
      limit(100)
    )
    // onSnapshot keeps the list live; returns unsubscribe for cleanup
    return onSnapshot(q, snap => {
      setEvents(snap.docs.map(doc => ({ id: doc.id, ...doc.data() })))
    }, err => console.error('Firestore error:', err))
  }, [])

  return (
    <div className="dashboard">
      <header className="dashboard-header">
        <span className="app-title">Sound Events</span>
        <div className="header-right">
          <span className="user-email">{user.email}</span>
          <button className="signout-btn" onClick={() => signOut(auth).catch(err => console.error('Sign-out failed:', err))}>
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
