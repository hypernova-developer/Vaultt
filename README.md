# Vaultt

Vaultt is a high-performance, lightweight, and dependency-free CLI password manager written in C++. It isolates sensitive data directly at the filesystem level by enforcing root-only privileges on the database file.

## Features

- **Strict Security:** The flat-file database uses chmod 000 permissions. Only root or sudo processes can read or write the credentials.
- **Optimized Compilation:** Built with aggressive optimization (-O3) and full static linking (-static) for maximal performance and portability.
- **Index-Based Architecture:** View stored accounts mapped against incremental indices, making modifications and deletions completely straightforward.
- **Dynamic Generation:** Instantly generate secure passwords with specific, user-defined length constraints.

## Installation

Do NOT run the installation script as root. Execute it as a standard user so the system maps the storage scope inside your correct home directory profile:

```bash
chmod +x init.sh
./init.sh
```
