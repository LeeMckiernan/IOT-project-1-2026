# IOT Dashboard Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a React + Vite web app with Firebase Auth (Google sign-in) and a Firestore real-time event log for a sound sensor IoT dashboard — deployable to Firebase Hosting at zero cost.

**Architecture:** Single-page app with auth-state-based view switching — `App.jsx` listens to Firebase auth state and renders either `Login` or `Dashboard`. Dashboard subscribes to Firestore via `onSnapshot` for real-time event streaming. No router library needed.

**Tech Stack:** React 18, Vite, Firebase 10 (Auth + Firestore), Vitest + React Testing Library, Firebase Hosting

---

## File Map

| File | Responsibility |
|---|---|
| `src/main.jsx` | React entry point |
| `src/App.jsx` | Auth state listener — renders Login or Dashboard |
| `src/App.test.jsx` | Auth routing tests |
| `src/firebase.js` | Firebase app init, exports `auth` and `db` |
| `src/index.css` | Global dark theme styles |
| `src/test/setup.js` | Vitest + jest-dom global setup |
| `src/components/Login.jsx` | Google sign-in screen |
| `src/components/Login.test.jsx` | Login unit tests |
| `src/components/EventTable.jsx` | Pure component — renders event rows with dB color coding |
| `src/components/EventTable.test.jsx` | EventTable unit tests |
| `src/components/Dashboard.jsx` | Header + Firestore listener + renders EventTable |
| `src/components/Dashboard.test.jsx` | Dashboard unit tests |
| `.env.local` | Firebase config values (gitignored) |
| `firebase.json` | Firebase Hosting config |
| `.firebaserc` | Firebase project alias |

---

### Task 1: Scaffold Vite + React project

**Files:**
- Modify: `vite.config.js`
- Modify: `package.json`
- Create: `src/test/setup.js`

- [ ] **Step 1: Create Vite project**

Run in `c:/Balint dolgai/Programming/IOTDashboard`:
```bash
npm create vite@latest . -- --template react
```
When prompted about the non-empty directory, choose **Ignore files and continue**.

Expected: Vite scaffolds `src/`, `index.html`, `package.json`, `vite.config.js`, `.gitignore`

- [ ] **Step 2: Install dependencies**

```bash
npm install firebase
npm install -D vitest @testing-library/react @testing-library/jest-dom jsdom
```

Expected: `node_modules/` populated, no errors.

- [ ] **Step 3: Update package.json scripts**

Add `"test": "vitest run"` to the `scripts` section in `package.json`:

```json
"scripts": {
  "dev": "vite",
  "build": "vite build",
  "preview": "vite preview",
  "test": "vitest run"
}
```

- [ ] **Step 4: Configure Vitest in vite.config.js**

Replace `vite.config.js` with:

```js
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  test: {
    environment: 'jsdom',
    globals: true,
    setupFiles: './src/test/setup.js',
  },
})
```

- [ ] **Step 5: Create test setup file**

Create `src/test/setup.js`:

```js
import '@testing-library/jest-dom'
```

- [ ] **Step 6: Delete Vite boilerplate we won't use**

```bash
rm src/App.css src/assets/react.svg public/vite.svg
```

Then clear `src/App.jsx` (leave it empty or delete it — Task 6 creates it fresh).

- [ ] **Step 7: Commit**

```bash
git init
git add -A
git commit -m "chore: scaffold Vite + React project with Vitest"
```

---

### Task 2: Dark theme CSS

**Files:**
- Replace: `src/index.css`

- [ ] **Step 1: Write global dark theme styles**

Replace `src/index.css` entirely with:

```css
*, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
  background: #0f1117;
  color: #e2e8f0;
  min-height: 100vh;
}

/* ── Login ────────────────────────────────── */
.login-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
}

.login-card {
  background: #1a1d27;
  border: 1px solid #2a2d3a;
  border-radius: 12px;
  padding: 40px;
  text-align: center;
  width: 320px;
}

.login-card h1 {
  font-size: 22px;
  font-weight: 600;
  margin-bottom: 4px;
}

.login-card .subtitle {
  color: #64748b;
  font-size: 13px;
  margin-bottom: 32px;
}

.google-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 10px;
  width: 100%;
  background: #4285F4;
  color: white;
  border: none;
  border-radius: 6px;
  padding: 10px 16px;
  font-size: 14px;
  font-weight: 500;
  cursor: pointer;
  transition: background 0.15s;
}

.google-btn:hover { background: #3574e3; }

/* ── Dashboard ────────────────────────────── */
.dashboard { display: flex; flex-direction: column; min-height: 100vh; }

.dashboard-header {
  background: #1a1d27;
  border-bottom: 1px solid #2a2d3a;
  padding: 12px 24px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.app-title { font-size: 15px; font-weight: 500; }

.header-right { display: flex; align-items: center; gap: 12px; }

.user-email { color: #64748b; font-size: 13px; }

.signout-btn {
  background: #2a2d3a;
  color: #94a3b8;
  border: none;
  border-radius: 4px;
  padding: 4px 10px;
  font-size: 12px;
  cursor: pointer;
}

.signout-btn:hover { background: #363a4e; }

.dashboard-main { padding: 24px; }

/* ── Event Table ──────────────────────────── */
.event-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 13px;
}

.event-table th {
  text-align: left;
  padding: 8px 12px;
  color: #64748b;
  font-size: 11px;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  font-weight: 500;
  border-bottom: 1px solid #2a2d3a;
}

.event-table td {
  padding: 10px 12px;
  border-bottom: 1px solid #1e2130;
  color: #94a3b8;
}

.event-table td:last-child { color: #e2e8f0; }

.event-table tr:last-child td { border-bottom: none; }

.db-green { color: #10b981; font-weight: 500; }
.db-amber { color: #f59e0b; font-weight: 500; }
.db-red   { color: #ef4444; font-weight: 500; }
```

- [ ] **Step 2: Commit**

```bash
git add src/index.css
git commit -m "feat: add dark theme CSS"
```

---

### Task 3: Firebase config

**Files:**
- Create: `.env.local`
- Create: `src/firebase.js`

**Prerequisites — set up your Firebase project first:**
1. Go to [https://console.firebase.google.com](https://console.firebase.google.com)
2. Click **Add project** — give it a name, disable Google Analytics
3. **Authentication → Get started → Sign-in method** → enable **Google**
4. **Firestore Database → Create database** → choose **Start in test mode**
5. **Project settings → Your apps → Add app → Web** — register a web app and copy the config object

- [ ] **Step 1: Create .env.local**

Create `.env.local` in the project root (already gitignored by Vite's default `.gitignore`):

```
VITE_FIREBASE_API_KEY=your_api_key_here
VITE_FIREBASE_AUTH_DOMAIN=your_project_id.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=your_project_id
VITE_FIREBASE_STORAGE_BUCKET=your_project_id.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=your_messaging_sender_id
VITE_FIREBASE_APP_ID=your_app_id
```

Replace each value with the ones from your Firebase project settings.

- [ ] **Step 2: Create src/firebase.js**

```js
import { initializeApp } from 'firebase/app'
import { getAuth } from 'firebase/auth'
import { getFirestore } from 'firebase/firestore'

const firebaseConfig = {
  apiKey: import.meta.env.VITE_FIREBASE_API_KEY,
  authDomain: import.meta.env.VITE_FIREBASE_AUTH_DOMAIN,
  projectId: import.meta.env.VITE_FIREBASE_PROJECT_ID,
  storageBucket: import.meta.env.VITE_FIREBASE_STORAGE_BUCKET,
  messagingSenderId: import.meta.env.VITE_FIREBASE_MESSAGING_SENDER_ID,
  appId: import.meta.env.VITE_FIREBASE_APP_ID,
}

const app = initializeApp(firebaseConfig)
export const auth = getAuth(app)
export const db = getFirestore(app)
```

- [ ] **Step 3: Commit**

```bash
git add src/firebase.js
git commit -m "feat: add Firebase config and init"
```

Note: `.env.local` is gitignored — do not commit it.

---

### Task 4: EventTable component

**Files:**
- Create: `src/components/EventTable.jsx`
- Create: `src/components/EventTable.test.jsx`

- [ ] **Step 1: Write the failing tests**

Create `src/components/EventTable.test.jsx`:

```jsx
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
```

- [ ] **Step 2: Run tests to verify they fail**

```bash
npm test
```

Expected: FAIL — `Cannot find module './EventTable'`

- [ ] **Step 3: Create src/components/EventTable.jsx**

```jsx
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
```

- [ ] **Step 4: Run tests to verify they pass**

```bash
npm test
```

Expected: All 6 tests PASS

- [ ] **Step 5: Commit**

```bash
git add src/components/EventTable.jsx src/components/EventTable.test.jsx
git commit -m "feat: add EventTable component with dB color coding"
```

---

### Task 5: Login component

**Files:**
- Create: `src/components/Login.jsx`
- Create: `src/components/Login.test.jsx`

- [ ] **Step 1: Write the failing tests**

Create `src/components/Login.test.jsx`:

```jsx
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
```

- [ ] **Step 2: Run tests to verify they fail**

```bash
npm test
```

Expected: FAIL — `Cannot find module './Login'`

- [ ] **Step 3: Create src/components/Login.jsx**

```jsx
import { GoogleAuthProvider, signInWithPopup } from 'firebase/auth'
import { auth } from '../firebase'

function GoogleIcon() {
  return (
    <svg width="18" height="18" viewBox="0 0 24 24" aria-hidden="true">
      <path fill="#fff" d="M22.56 12.25c0-.78-.07-1.53-.2-2.25H12v4.26h5.92c-.26 1.37-1.04 2.53-2.21 3.31v2.77h3.57c2.08-1.92 3.28-4.74 3.28-8.09z"/>
      <path fill="#fff" d="M12 23c2.97 0 5.46-.98 7.28-2.66l-3.57-2.77c-.98.66-2.23 1.06-3.71 1.06-2.86 0-5.29-1.93-6.16-4.53H2.18v2.84C3.99 20.53 7.7 23 12 23z"/>
      <path fill="#fff" d="M5.84 14.09c-.22-.66-.35-1.36-.35-2.09s.13-1.43.35-2.09V7.07H2.18C1.43 8.55 1 10.22 1 12s.43 3.45 1.18 4.93l3.66-2.84z"/>
      <path fill="#fff" d="M12 5.38c1.62 0 3.06.56 4.21 1.64l3.15-3.15C17.45 2.09 14.97 1 12 1 7.7 1 3.99 3.47 2.18 7.07l3.66 2.84c.87-2.6 3.3-4.53 6.16-4.53z"/>
    </svg>
  )
}

export default function Login() {
  function handleSignIn() {
    const provider = new GoogleAuthProvider()
    signInWithPopup(auth, provider)
  }

  return (
    <div className="login-page">
      <div className="login-card">
        <h1>IOT Dashboard</h1>
        <p className="subtitle">Sound Event Monitor</p>
        <button className="google-btn" onClick={handleSignIn}>
          <GoogleIcon />
          Sign in with Google
        </button>
      </div>
    </div>
  )
}
```

- [ ] **Step 4: Run tests to verify they pass**

```bash
npm test
```

Expected: All tests PASS

- [ ] **Step 5: Commit**

```bash
git add src/components/Login.jsx src/components/Login.test.jsx
git commit -m "feat: add Login component with Google sign-in"
```

---

### Task 6: App auth router

**Files:**
- Create/replace: `src/App.jsx`
- Create: `src/App.test.jsx`

- [ ] **Step 1: Write the failing tests**

Create `src/App.test.jsx`:

```jsx
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
```

- [ ] **Step 2: Run tests to verify they fail**

```bash
npm test
```

Expected: FAIL — App.jsx is empty or missing

- [ ] **Step 3: Write src/App.jsx**

```jsx
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
```

- [ ] **Step 4: Run tests to verify they pass**

```bash
npm test
```

Expected: All tests PASS

- [ ] **Step 5: Commit**

```bash
git add src/App.jsx src/App.test.jsx
git commit -m "feat: add App auth state router"
```

---

### Task 7: Dashboard component

**Files:**
- Create: `src/components/Dashboard.jsx`
- Create: `src/components/Dashboard.test.jsx`

- [ ] **Step 1: Write the failing tests**

Create `src/components/Dashboard.test.jsx`:

```jsx
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
```

- [ ] **Step 2: Run tests to verify they fail**

```bash
npm test
```

Expected: FAIL — `Cannot find module './Dashboard'`

- [ ] **Step 3: Create src/components/Dashboard.jsx**

```jsx
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
```

- [ ] **Step 4: Run tests to verify they pass**

```bash
npm test
```

Expected: All tests PASS

- [ ] **Step 5: Commit**

```bash
git add src/components/Dashboard.jsx src/components/Dashboard.test.jsx
git commit -m "feat: add Dashboard with Firestore real-time listener"
```

---

### Task 8: Wire up entry point

**Files:**
- Replace: `src/main.jsx`

- [ ] **Step 1: Update src/main.jsx**

Replace the contents of `src/main.jsx` with:

```jsx
import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import App from './App'

createRoot(document.getElementById('root')).render(
  <StrictMode>
    <App />
  </StrictMode>
)
```

- [ ] **Step 2: Run all tests**

```bash
npm test
```

Expected: All tests PASS

- [ ] **Step 3: Verify in browser**

```bash
npm run dev
```

Open [http://localhost:5173](http://localhost:5173). You should see the dark login screen with the Google sign-in button. Sign in with a Google account and verify the event log loads (empty table is fine — no events yet).

- [ ] **Step 4: Commit**

```bash
git add src/main.jsx
git commit -m "feat: wire up React entry point"
```

---

### Task 9: Deploy to Firebase Hosting

**Files:**
- Create: `firebase.json`
- Create: `.firebaserc`

**Prerequisites — install Firebase CLI if not already installed:**
```bash
npm install -g firebase-tools
firebase login
```

- [ ] **Step 1: Create firebase.json**

```json
{
  "hosting": {
    "public": "dist",
    "ignore": ["firebase.json", "**/.*", "**/node_modules/**"],
    "rewrites": [{ "source": "**", "destination": "/index.html" }]
  }
}
```

- [ ] **Step 2: Create .firebaserc**

Replace `YOUR_PROJECT_ID` with your Firebase project ID (found in Firebase console → Project settings → General):

```json
{
  "projects": {
    "default": "YOUR_PROJECT_ID"
  }
}
```

- [ ] **Step 3: Build and deploy**

```bash
npm run build
firebase deploy --only hosting
```

Expected output ends with:
```
✔  Deploy complete!

Hosting URL: https://YOUR_PROJECT_ID.web.app
```

- [ ] **Step 4: Commit**

```bash
git add firebase.json .firebaserc
git commit -m "chore: add Firebase Hosting config"
```

---

### Task 10: Set Firestore security rules

This step is done in the Firebase console — no local files to edit.

- [ ] **Step 1: Open Firestore rules**

Go to Firebase console → Firestore Database → **Rules** tab.

- [ ] **Step 2: Replace the default rules**

```
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    match /events/{event} {
      allow read: if request.auth != null;
      allow write: if true;
    }
  }
}
```

`read` requires Google sign-in. `write` is open so your IoT device can push events without credentials (acceptable for a single-device demo).

- [ ] **Step 3: Click Publish**

Rules take effect immediately.
