// Root component — listens for Firebase auth state and routes to Dashboard or Login
import { onAuthStateChanged } from 'firebase/auth'
import { useEffect, useState } from 'react'
import { auth } from './firebase'
import Dashboard from './components/Dashboard'
import Login from './components/Login'

export default function App() {
  // undefined = still resolving auth state; null = signed out; object = signed in
  const [user, setUser] = useState(undefined)

  useEffect(() => {
    // Subscribe to auth changes; returns an unsubscribe function for cleanup
    return onAuthStateChanged(auth, setUser)
  }, [])

  // Render nothing until Firebase resolves the initial auth state
  if (user === undefined) return null
  return user ? <Dashboard user={user} /> : <Login />
}
