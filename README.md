# 🚂 Railway Freight Management System

A full-stack web application for generating railway freight quotations, built with a **C++ backend** (Crow framework) and a plain **HTML/CSS/JS frontend**. Demonstrates OOP concepts including inheritance, polymorphism, and association in C++.

---

## 📁 Project Structure

```
freight-system/
├── backend/
│   ├── freight.hpp         # Abstract base class — baseFreight(), pure virtual handlingCharge() & routeRules()
│   ├── perishable.hpp      # Derived class — temperature control, expiry window
│   ├── nonperishable.hpp   # Derived class — packaging type, fragile flag
│   ├── hazardous.hpp       # Derived class — UN hazard class (1–9), escort requirement
│   ├── shipment.hpp        # Association class — has-a Freight*, generates quotation
│   ├── main.cpp            # Crow HTTP server — serves frontend + REST API
│   └── CMakeLists.txt      # Build configuration, fetches Crow automatically
└── frontend/
    ├── index.html          # Single-page UI with dynamic form fields
    ├── style.css           # Dark industrial railway aesthetic
    └── app.js              # Form logic, API calls, PDF generation via jsPDF
```

---

## ⚙️ Prerequisites

### Arch Linux
```bash
sudo pacman -S cmake gcc boost asio
```

### Ubuntu / Debian
```bash
sudo apt install cmake g++ libboost-all-dev libasio-dev
```

### Windows (WSL recommended)
Install WSL with Ubuntu, then follow the Ubuntu steps above.

> **Note:** Crow itself is downloaded automatically by CMake during the build step. You do not need to install it manually.

---

## 🔨 Build & Run

### Step 1 — Clone the repo
```bash
git clone https://github.com/yourusername/freight-system.git
cd freight-system
```

### Step 2 — Build the backend
```bash
cd backend
mkdir build && cd build
cmake ..
make -j$(nproc)
```

CMake will automatically fetch **Crow** from GitHub during this step. First build takes 2–3 minutes. Subsequent builds are fast.

### Step 3 — Run
```bash
./freight_server
```

You should see:
```
[INFO] Crow/master server is running at http://0.0.0.0:8080 using 12 threads
```

### Step 4 — Open in browser
```
http://localhost:8080
```

That's it. The C++ server serves both the frontend files and the API on the same port. No separate frontend server needed.

---

## 🧠 How It Works

### OOP Design

```
Freight (abstract base class)
│
│   baseFreight() = weight × distance × ₹0.85       ← concrete, shared by all
│   handlingCharge() = 0                             ← pure virtual
│   routeRules()     = 0                             ← pure virtual
│
├── Perishable
│       handlingCharge() → 15% of base + ₹200 if temp controlled
│       routeRules()     → priority routing, refrigerated wagon rules
│
├── NonPerishable
│       handlingCharge() → 8% of base + ₹150 if fragile
│       routeRules()     → standard routing, fragile/loose cargo rules
│
└── Hazardous
        handlingCharge() → 25% of base + ₹500 if escort required
        routeRules()     → per UN hazard class (1–9) specific rules

Shipment  ──has-a──▶  Freight*   (association, not inheritance)
    generateQuotation() calls handlingCharge() and routeRules()
    through the base pointer — this is runtime polymorphism
```

### Polymorphism in action

In `main.cpp`, regardless of which freight type the user selects, the code calls the same methods through a base class pointer:

```cpp
std::shared_ptr<Freight> freight;          // base pointer

if (type == "perishable")
    freight = std::make_shared<Perishable>(...);
else if (type == "nonperishable")
    freight = std::make_shared<NonPerishable>(...);
else if (type == "hazardous")
    freight = std::make_shared<Hazardous>(...);

Shipment shipment(freight);
// handlingCharge() and routeRules() resolve to the correct
// derived class implementation at runtime — polymorphism
```

### Request flow

```
Browser → GET /              → Crow reads & serves index.html
Browser → GET /style.css     → Crow reads & serves style.css
Browser → GET /app.js        → Crow reads & serves app.js

User submits form
Browser → POST /api/quotation (JSON body)
        → Crow parses JSON
        → Instantiates correct Freight subclass
        → Wraps in Shipment
        → Calls handlingCharge() + routeRules() polymorphically
        → Returns quotation JSON
Browser renders quotation card
User clicks Download → jsPDF generates PDF in browser
```

---

## 🌐 API Reference

### POST `/api/quotation`

**Common fields (all types):**
```json
{
  "type":     "perishable | nonperishable | hazardous",
  "sender":   "Ravi Logistics Pvt Ltd",
  "receiver": "Delhi Warehousing Co.",
  "origin":   "Mumbai CST",
  "dest":     "New Delhi",
  "distance": 1400,
  "weight":   500
}
```

**Perishable — extra fields:**
```json
{ "tempControl": true, "expiryHours": 36 }
```

**Non-Perishable — extra fields:**
```json
{ "packaging": "crate | pallet | loose", "fragile": false }
```

**Hazardous — extra fields:**
```json
{ "hazardClass": 3, "escort": true }
```

**Response:**
```json
{
  "shipmentId":     "RFM-303256",
  "date":           "04-04-2026",
  "freightType":    "Hazardous",
  "sender":         "Ravi Logistics Pvt Ltd",
  "receiver":       "Delhi Warehousing Co.",
  "origin":         "Mumbai CST",
  "destination":    "New Delhi",
  "weightKg":       "500.00",
  "distanceKm":     "1400.00",
  "baseFreight":    "595.00",
  "handlingCharge": "648.75",
  "totalCost":      "1243.75",
  "routeRules":     "Hazard Class 3 — Flammable liquids..."
}
```

### GET `/api/health`
```json
{ "status": "ok", "service": "Railway Freight Management System" }
```

---

## 💰 Pricing Model

| Freight Type   | Handling Charge Formula                        |
|----------------|------------------------------------------------|
| Base (all)     | weight (kg) × distance (km) × ₹0.85           |
| Perishable     | 15% of base freight + ₹200 if temp controlled |
| Non-Perishable | 8% of base freight + ₹150 if fragile           |
| Hazardous      | 25% of base freight + ₹500 if escort required  |

---

## 📦 Dependencies

| Package | What it does | How it is installed |
|---------|-------------|-------------------|
| `gcc`   | C++ compiler | `pacman -S gcc` (usually pre-installed) |
| `cmake` | Build system generator — creates the Makefile | `pacman -S cmake` |
| `boost` | C++ utility libraries that Crow depends on | `pacman -S boost` |
| `asio`  | Async networking library Crow uses under the hood | `pacman -S asio` |
| `Crow`  | C++ web framework — HTTP routing, JSON, middleware | Auto-fetched by CMake from GitHub |
| `jsPDF` | PDF generation in the browser | Loaded from CDN in index.html, no install needed |

---

## 🐛 Troubleshooting

**`Could NOT find asio`**
```bash
sudo pacman -S asio
```

**`crow_all.h: No such file or directory`**
Make sure `main.cpp` uses `#include <crow.h>` not `#include "crow_all.h"`.

**`ends_with` not found**
Your compiler is in C++17 mode. Replace `ends_with` with the rfind approach or switch to C++20 in CMakeLists.txt.

**Browser says "Could not connect to backend"**
Make sure `./freight_server` is running. Test with:
```bash
curl http://localhost:8080/api/health
```

**Frontend shows 404**
The server cannot find the frontend folder. Edit `main.cpp` and set FRONTEND to the absolute path:
```cpp
const std::string FRONTEND = "/home/yourusername/freight-system/frontend";
```
Then rebuild.

---

## 🛠️ Built With

- **C++17** — backend language
- **Crow** (master branch) — C++ HTTP framework
- **CMake** — build system
- **Boost + Asio** — Crow dependencies
- **HTML / CSS / Vanilla JS** — frontend
- **jsPDF** — client-side PDF generation
- **IBM Plex Mono & Bebas Neue** — fonts

---

## 📄 License

MIT License — feel free to use, modify, and distribute.
