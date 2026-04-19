import { onAuthStateChanged } from 'firebase/auth'
import { useEffect, useState } from 'react'
import { auth } from './firebase'
import Dashboard from './components/Dashboard'
import Login from './components/Login'

export default function App() {
  const [user, setUser] = useState(undefined)

  useEffect(() => {
    return onAuthStateChanged(auth, setUser)
  }, [])

  if (user === undefined) return null
  return user ? <Dashboard user={user} /> : <Login />
}
