# P2P File Transfer System — Qt GUI Edition

A peer-to-peer file sharing system with a tracker server and client, both with full Qt6 desktop GUIs. Files transfer directly between peers; the tracker only coordinates metadata (who has what, where to find them).

Built on top of the original [P2P-File-Transfer-System](https://github.com/AmalAgarkar/P2P-File-Transfer-System), rewritten in Qt6/C++17 with security fixes, parallel chunked downloads, SHA-256 integrity verification, and a native desktop UI for both the tracker and the client.

---

## Features

- **Native Qt6 GUI** for both Tracker and Client — no terminal commands needed at runtime
- **Parallel chunked downloads** — files split into 1 MB chunks, downloaded concurrently from the seeder, merged automatically
- **SHA-256 integrity verification** — every download is hash-checked after merging; corrupted files are deleted and reported
- **Hashed passwords** — credentials are never stored or transmitted in plaintext
- **Path traversal protection** — the peer file server rejects any filename containing `..` or path separators
- **Live tracker dashboard** — real-time stat cards (users, groups, files, connected peers), event log, connected-peer table
- **Group-based sharing** — create groups, send/accept join requests, upload/download/stop-sharing files per group

---

## Architecture

```
┌──────────────────┐         TCP (control)            ┌─────────────────┐
│  Tracker GUI     │◄─────────────────────────────►   │  Client GUI A   │
│  (TrackerBackend)│         port 9000                │  (MainWindow)   │
└──────────────────┘                                  └────────┬────────┘
                                                               │
                                                     PeerServer listens
                                                      on port 10000
                                                               │
 ┌──────────────────┐         TCP (control)                    │
 │  Client GUI B    │◄─────────────────────────────────────────┘
 │  (MainWindow)    │         direct P2P file transfer
 └──────────────────┘         (TransferManager ↔ PeerServer)
```

- **Tracker** never touches file bytes — it only stores filename, size, SHA-256 hash, and `peer_ip:peer_port` for each shared file.
- **Client A** (uploader) runs a `PeerServer` that serves files directly to other peers.
- **Client B** (downloader) uses `TransferManager` to fetch chunks in parallel directly from Client A, then verifies the merged file's hash.

---

## Project Structure

```
P2Pgui/
├── CMakeLists.txt              # single top-level build config
│
├── shared/                     # code shared by both binaries
│   ├── crypto.h / crypto.cpp       SHA-256 hashing, safe filename check
│   └── protocol.h / protocol.cpp   wire-protocol structs & parsing
│
├── client/                     # p2p_client binary
│   ├── main.cpp                    QApplication entry point
│   ├── Style.h                     shared dark theme stylesheet
│   ├── LoginDialog.h/.cpp           startup login popup
│   ├── TrackerSocket.h/.cpp         QTcpSocket wrapper for tracker protocol
│   ├── PeerServer.h/.cpp            serves local files to other peers
│   ├── TransferManager.h/.cpp       parallel chunked download + integrity check
│   └── MainWindow.h/.cpp            main client window
│
└── tracker/                    # p2p_tracker binary
    ├── main.cpp                    QApplication entry point
    ├── TrackerBackend.h/.cpp       full tracker server logic & state
    └── TrackerWindow.h/.cpp        tracker dashboard window
```

---

## Requirements

- Ubuntu/Debian Linux (tested on Ubuntu 24.04)
- CMake ≥ 3.16
- Ninja build system
- Qt6 (Core, Widgets, Network)
- OpenSSL (libssl-dev)
- A C++17 compiler (g++ or clang++)

---

## Installation

```bash
sudo apt update
sudo apt install -y cmake ninja-build qt6-base-dev libssl-dev build-essential
```

Verify Qt6 is installed:
```bash
qmake6 --version
```

---

## Build

From the project root:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

A successful build produces two binaries:
```
build/p2p_tracker
build/p2p_client
```

> **Note:** If you change `CMakeLists.txt`, delete the `build/` folder first and reconfigure from scratch:
> ```bash
> rm -rf build
> cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
> cmake --build build --parallel
> ```

---

## Running

Open the **Tracker** first, then one or more **Clients**.

### 1. Start the Tracker

```bash
./build/p2p_tracker
```

In the window that opens:
- Port `9000` is pre-filled
- Click **▶ Start**
- The status bar turns green: *Tracker running on port 9000*

### 2. Start a Client

In a new terminal:

```bash
./build/p2p_client
```

A login dialog appears automatically. Fill in:

| Field | Example |
|---|---|
| Tracker IP | `127.0.0.1` |
| Port | `9000` |
| Username | `alice` |
| Password | `pass123` |

Click **Sign in**.

### 3. Start a second Client (to test transfers)

In another terminal:

```bash
./build/p2p_client
```

Log in as a different user, e.g. `bob`.

---

## Walkthrough — Sharing and Downloading a File

**Client A (alice) — upload:**
1. Click **+ Group** → name it `mygroup`
2. Click **⬆ Upload** → choose any file
3. A `PeerServer` automatically starts on port `10000` to serve it

**Client B (bob) — join:**
1. Click **Join** → type `mygroup`

**Client A (alice) — accept:**
1. Click **📋 Requests** → select `bob` → accept

**Client B (bob) — download:**
1. Click `mygroup` in the sidebar — the file appears in the table
2. Select it → click **⬇ Download** → choose a save folder
3. Watch the progress bar fill in the **Transfers** panel
4. On completion, the log shows `Integrity OK` confirming the SHA-256 hash matched

---

## Running Across Two Machines

On the **tracker/uploader machine**:
```bash
./build/p2p_tracker   # start, click ▶ Start
./build/p2p_client     # connect to 127.0.0.1:9000, upload a file
```

On the **downloader machine**:
```bash
./build/p2p_client     # connect to <tracker-machine-LAN-IP>:9000
```

Open the required ports in the firewall on the tracker/uploader machine:
```bash
sudo ufw allow 9000/tcp        # tracker control port
sudo ufw allow 10000:10010/tcp # P2P file-serving ports
```

---

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `cmake: command not found` | CMake not installed | `sudo apt install cmake` |
| `Could not find Qt6` | Qt6 dev package missing | `sudo apt install qt6-base-dev` |
| `fatal error: openssl/sha.h` | OpenSSL headers missing | `sudo apt install libssl-dev` |
| `fatal error: PeerServer.h: No such file or directory` | Stale build cache or missing include path | `rm -rf build` then reconfigure |
| `Could NOT find XKB` warning | Optional keyboard extension, not required on desktop Linux | Safe to ignore |
| `Tracker: failed to listen on port 9000` | Port already in use | Change the port number in the tracker window, or `lsof -i :9000` to find the conflicting process |
| Client login dialog does nothing after Sign in | Tracker not started yet | Start the tracker first, click ▶ Start before logging in |
| Download stuck at 0% | Uploader's P2P port is firewalled | `sudo ufw allow 10000/tcp` on the uploader's machine |
| `Integrity check failed` in log | File corrupted in transit (rare network issue) | Retry the download |

---

## Security Notes

- Passwords are hashed with SHA-256 before storage and before being sent over the wire — plaintext passwords never touch disk or network.
- The peer file server validates every requested filename and rejects anything containing `..`, `/`, or `\` to prevent path traversal attacks.
- Every completed download is verified against the uploader's SHA-256 hash; mismatched files are deleted automatically rather than handed to the user.

This project is intended for local networks and learning purposes. It does not implement TLS/encryption-in-transit, peer authentication tokens, or rate limiting — do not expose the tracker or peer ports directly to the public internet without adding these.

---

## License
This project is open-source and available under the [MIT License](LICENSE).
