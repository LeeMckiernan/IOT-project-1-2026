# IOT Dashboard — Design Spec
**Date:** 2026-04-16

## Overview

A minimal, single-purpose web dashboard for monitoring sound sensor events. Built with React + Vite, backed by Firebase (Firestore + Auth). Two screens: a Google sign-in page and a real-time log viewer.

**Cost constraint:** Zero cost. Everything runs on the Firebase Spark (free) plan. The dashboard is built for a single demo — no scaling, no production hardening needed. Firestore free tier (50k reads/day, 20k writes/day) is more than sufficient.

---

## Tech Stack

| Concern | Choice |
|---|---|
| Framework | React + Vite |
| Auth | Firebase Authentication (Google provider) |
| Database | Firebase Firestore |
| Hosting | Firebase Hosting |
| Styling | Plain CSS (no CSS framework) |

---

## Data Model

### Firestore Collection: `events`

Each document represents a single sound event logged by the sensor:

| Field | Type | Description |
|---|---|---|
| `id` | string (doc ID) | Auto-generated Firestore document ID |
| `time` | Timestamp | When the event occurred |
| `db` | number | Peak decibel level recorded |
| `duration` | number | Duration of the event in seconds (e.g. `5` = 5s) |
| `deviceName` | string | Name of the sensor device (e.g. `garage-sensor`) |

Events are written by the IoT device directly to Firestore (or via a Cloud Function/API if auth is needed on the write side — out of scope for this spec).

---

## Screens

### Screen 1 — Login

- Full-screen centered card on a dark (`#0f1117`) background
- App title: **IOT Dashboard** + subtitle: *Sound Event Monitor*
- Single button: **Sign in with Google** (Firebase Google Auth popup)
- On successful auth, redirect to the log viewer
- If already signed in on load, skip directly to log viewer

### Screen 2 — Log Viewer

- **Header bar**: app name left, signed-in user email + "Sign out" button right
- **Event table**: full-width, newest events at top
  - Columns: Time | dB | Duration | Device
  - dB is color-coded:
    - Green: < 80 dB
    - Amber: 80–99 dB
    - Red: ≥ 100 dB
- **Real-time updates**: Firestore `onSnapshot` listener — new events appear instantly without any user action
- **Sign out**: clears Firebase auth session, returns to login screen

---

## Architecture

```
src/
  main.jsx          # React entry point
  App.jsx           # Auth state router (Login vs Dashboard)
  firebase.js       # Firebase app init + exports (auth, db)
  components/
    Login.jsx       # Google sign-in screen
    Dashboard.jsx   # Header + EventTable
    EventTable.jsx  # Firestore real-time log table
  index.css         # Global dark theme styles
```

- `App.jsx` listens to `onAuthStateChanged` — renders `<Login>` or `<Dashboard>` based on auth state
- `EventTable.jsx` uses `onSnapshot` on the `events` collection, ordered by `time` descending, limited to the last 100 events
- No routing library needed — auth state drives the single view switch

---

## Visual Design

- **Theme:** Dark — `#0f1117` page background, `#1a1d27` card/header background, `#2a2d3a` borders
- **Text:** `#e2e8f0` primary, `#94a3b8` secondary, `#64748b` muted
- **dB colors:** `#10b981` green / `#f59e0b` amber / `#ef4444` red
- **Font:** System sans-serif stack
- No external UI libraries — all styling is hand-written CSS

---

## Out of Scope

- Filtering or searching events
- Pagination (Firestore limit of 100 handles this)
- Multiple device management
- Write access / event submission from the dashboard
- Admin roles or multi-user access control
- Error boundaries, loading skeletons, or production-grade UX polish
- Any paid Firebase features (Functions, Blaze plan, etc.)
