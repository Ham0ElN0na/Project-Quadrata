# Quadrata GUI

A beautiful Qt-based GUI for the Quadrata Travel Booking System.

## Design

- **Dark space theme** matching the Quadrata logo aesthetic
- **Cyan/teal accent color** (`#00c8ff`) throughout
- **Animated logo** with pulsing glow effect on the splash screen
- **Sidebar navigation** for both Admin and Customer dashboards
- **Glassmorphism-style cards** with drop shadows

## Screens

| Screen | Description |
|--------|-------------|
| Splash | Animated logo with glow pulse, Enter button |
| Login  | Tabbed Login / Sign Up card, supports Admin & Customer |
| Admin Dashboard | Stats overview, Inventory CRUD, System Logs viewer |
| Customer Dashboard | Search & Book travel, Profile with balance/loyalty points |

## Building

### Option A — Qt Creator (recommended)

1. Install [Qt 5.15+](https://www.qt.io/download) or Qt 6.x
2. Open `QuadrataGUI.pro` in Qt Creator
3. Configure a kit (MSVC 2019/2022 x64 recommended)
4. Build & Run (Ctrl+R)

### Option B — CMake + Visual Studio

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
cmake --build . --config Release
```

### Option C — qmake from command line

```bash
# In a Qt-enabled command prompt (e.g. Qt 6.x MSVC 2022 64-bit)
cd QuadrataGUI
qmake QuadrataGUI.pro
nmake          # or: mingw32-make
```

## Data Files

The app reads/writes the same JSON files as the console app:
- `Users.json` — user accounts
- `Inventory.json` — travel inventory
- `session.json` — session config
- `log.txt` — system log (read-only in GUI)

The app looks for these files in this order:
1. Next to the executable
2. Current working directory
3. `C:/Users/Lenovo/Downloads/Project Quadrata/Project Quadrata/Project Quadrata/` (hardcoded fallback)

For development, run the executable from the `Project Quadrata` source folder,
or copy the JSON files next to the built `.exe`.

## Notes

- Passwords are stored in plain text (same as the console app — not changed)
- Admin verification code is `adminReJion` (same as console app)
- The GUI directly reads/writes the same JSON files — no separate database
