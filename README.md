<p align="center">
  <img src="https://img.icons8.com/fluency/96/000000/city-buildings.png" alt="Municipal System Logo" width="80">
</p>

# 🏛️ Municipal Reporting System
### *[PSD] Project 2025-2026 | Track 3*

---

<p align="center">
  <img src="https://img.shields.io/badge/Language-C99-blue.svg" alt="C99">
  <img src="https://img.shields.io/badge/Build-Makefile-orange.svg" alt="Makefile">
  <img src="https://img.shields.io/badge/Memory-Optimized-brightgreen.svg" alt="Memory Optimized">
  <img src="https://img.shields.io/badge/License-AGPL--3.0-red.svg" alt="AGPL-3.0 License">
</p>

---

## 📋 Project Overview

> **Transforming urban chaos into structured workflows.**

The **Municipal Reporting System** is a centralized software solution designed to streamline the management of citizen-reported urban issues. The primary goal is to provide municipal operators with an efficient tool to monitor, organize, and resolve territorial problems—such as *utility failures*, *waste management*, or *public safety concerns*—transforming a raw stream of events into a structured and prioritized workflow.

---

## 🛠️ Architecture & Design

The project is built on modern software engineering principles, focusing on **scalability** and **high performance**:

* **⚡ Multi-Dimensional Navigation:** The system offers more than just a single view of the data. Thanks to a multi-index architecture, operators can instantly browse reports by category, chronological order, progress status, or urgency level.
* **💾 Efficient Memory Management:** At its core, the system is designed to eliminate data redundancy. Each report exists as a unique entity in memory; various indices act as "**maps**" pointing to the same data, ensuring a **minimal memory footprint** even with large databases.
* **🧩 Modular Organization:** The software follows a modular approach (using *Opaque Pointers*), strictly separating the user interface from the internal logic. This ensures that the data structures are protected and easily extensible for future updates.
* **🛠️ Industry Standards:** The codebase adheres to the **C99** standard and is managed via an automated build system (**Makefile**), ensuring portability and ease of compilation across different environments.

---

## ⚙️ Features

- Interactive terminal dashboard
- Binary database loading and persistence
- Multi-index navigation by category, date, status and urgency
- Live incremental search by ID/category prefix
- Controlled report removal with persistent database update
- Status update workflow for municipal reports
- Statistical report generation
- Valgrind-tested heap deallocation with 0 memory leaks

---

## 🧪 Testing & Memory Safety

The project includes an automated test executable that validates the core ADT operations:

- report registration and structural consistency
- search on existing and non-existing records
- priority management
- filtered navigation by category prefix
- status update across orthogonal indices
- statistical report generation
- full heap deallocation

The test suite was executed under Valgrind, reporting:

```text
==5726== HEAP SUMMARY:
==5726==     in use at exit: 0 bytes in 0 blocks
==5726==   total heap usage: 25,013 allocs, 25,013 frees, 30,206,376 bytes allocated
==5726==
==5726== All heap blocks were freed -- no leaks are possible
==5726==
==5726== For lists of detected and suppressed errors, rerun with: -s
==5726== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
---

## 🧭 Roadmap & Planned Improvements

After the official university submission, the project may be extended with additional development-oriented features, including:

- **Doxygen documentation generation**, to automatically produce structured technical documentation from the annotated header files.
- **Python database generator**, used to create large synthetic datasets of municipal reports for testing, benchmarking and demonstration purposes.
- **Extended test coverage**, with additional edge cases for invalid input, corrupted records and stress testing.
- **Improved reporting module**, with optional export formats and more detailed statistical summaries.

These improvements are planned as post-submission enhancements and are not required to run the current stable version of the project.

---
## 👤 Developer

**Salvatore Pisu** (*sandie69p*)
> **Individual Developer** | *Track 3*
> **Portfolio:** https://web3solnet.sol.site

---

## 📜 Licensing

This project is released under the **GNU AFFERO GENERAL PUBLIC LICENSE**.

---
*Created for the University Project - Digital Systems Design course.*