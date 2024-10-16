# Awalé Game Project - README

## Overview

This project is an implementation of the classic **Awalé** game, a traditional two-player strategy game originating from Africa. The rules for this version of Awalé are based on the description available [here](http://archi02.free.fr/_pages/awele.html).

This repository provides both **server** and **client** implementations to enable multiplayer games over a network. It leverages **Bazel** for building and managing the project. Below are the instructions on how to compile, run, and manage the game components.

---

## Prerequisites

- **Bazelisk**: This project requires **Bazelisk**, a wrapper for Bazel.  
  Install Bazelisk by following instructions [here](https://github.com/bazelbuild/bazelisk).

---

## Setup Instructions

1. **Initialize the build system:**

   ```bash
   make init
   ```

   This command refreshes all the compilation commands required by the project.

---

## Build Instructions

To compile both the **client** and **server** applications, run:

```bash
make build
```

You can also provide additional arguments using the `ARGS` variable as follows:

```bash
make build ARGS="--some-flag"
```

---

## Running the Game

### Start the Server

To run the server on port **1977**, use the following command:

```bash
make server
```

This will create or connect to the database at `/tmp/awale.db`.

### Run Clients

You can launch multiple clients to connect to the server using the following commands:

```bash
make client1
make client2
make client3
make client4
```

Each client connects to the server running on `127.0.0.1` (localhost) at port **1977** with their respective usernames and passwords.

- **Client 1:** Username: `client1`, Password: `password1`
- **Client 2:** Username: `client2`, Password: `password2`
- **Client 3:** Username: `client3`, Password: `password3`
- **Client 4:** Username: `client4`, Password: `password4`

You can also pass additional arguments with `ARGS`:

```bash
make client1 ARGS="--verbose"
```

### Play the Awalé Game

To launch a standalone instance of the Awalé game, use:

```bash
make awale
```

---

## Cleaning the Build

To remove all build artifacts and clean the workspace, run:

```bash
make clean
```

---

## Project Structure

```
├── src/
│   ├── client/     # Client-side implementation
│   ├── server/     # Server-side implementation
│   └── awale/      # CLI implementation
├── lib/
│   ├── awale/     #
│   ├── client/    #
│   └── cmds/      #
│   └── display/   #
│   └── server/    #
│   └── socket/    #
│   └── valuable/  #
├── Makefile       # Build and run commands using Bazel
└── README.md      # Project documentation
```

---

## Troubleshooting

- **Bazelisk not found:** Ensure that Bazelisk is properly installed and available in your system's `PATH`.
- **Server connection issues:** Ensure the server is running on the correct IP and port before launching clients.
- **Clean build issues:** If you encounter build errors, try running `make clean` followed by `make build`.

---

## License

This project is open-source and available under the terms of the MIT License.

---

## Contributors

- **KILIAN HOUPEURT**
- **NADINE HAJJ-HASSAN**
