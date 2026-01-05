# Camille Web Framework

## Overview

Camille's Web Framework - first of the Camille product list.
Inspired by fastapi, cpp-httplib, with a touch of simplicity.
Will never want to write backend application in another languages/framework after finding THIS.

## More Products - Camille

Camille's Web Framework is the first child, but as always i strive for more, therefore there's also:

1. Camille Proxy - Fast, Optimized Proxy, Reverse Proxy, its time to get our own proxy in C++!
2. Camille ORM - ORM That's built right. No more to say.
3. Camille Agenty - C++ LLM Agent Framework, Only for the C++ Community.
4. Camille's Itch - I had an itch to create a DB Proxy for analytics.. no more extensions that are haradcoded in your database.

## Inspirations

1. cpp-httplib for simplicity and header-only style, oat++.
2. DHH for Ruby On Rails, all in one.
3. FastAPI for architecture, async, starlette, asgi.

## Deep Dive

In order to create Camille, similar to modern async frameworks i had to use several libraries that are open sourced.
Asio: Camille is built above the asio C++ framework.
Json: nlohmann/json or something made by me.
Router: By me using trie, regex, params (template metaprogramming)
HTTP Parsing: By me.

## Me

1. we need to an asgi server, which is what im building that will sit above asio.
2. implement modern backend requirements: routing, middlewares, res/req and more.
3. implement the ui for api usage: openapi, json, how to use it and more.

The Socket Selector: Uvicorn uses a non-blocking socket. It asks the OS (via epoll on Linux or kqueue on BSD/macOS) to notify it only when data arrives.

The Event Loop: While waiting for network I/O, the CPU isn't "blocked." The loop moves to the next task in the queue.

The Starlette Task: When a full HTTP request is buffered, Uvicorn "schedules" a Coroutine. This coroutine runs the Starlette middleware stack and eventually your FastAPI route.

The Await Point: If your code hit an await, the execution pauses, the state is saved, and the loop goes back to checking the sockets.

1. The Network Layer (Your "Uvicorn")
Boost.Asio is perfect for this. It provides the I/O execution context (the event loop).

The Acceptor Loop: You need a listener that accepts TCP connections and spawns a "Session" object for each one.

The Parser: Asio gives you raw bytes. You need an HTTP parser (like llhttp or nghttp2) to turn those bytes into a Request object.

The Event Loop: You’ll be running io_context.run(). This is your engine.

1. The Toolkit Layer (Your "Starlette")
This is the hardest part to write from scratch. Starlette isn't just a server; it's a set of data structures and logic. You will need to implement:

Routing Tree: You need a high-performance Radix Tree or Regex-based router to map GET /users/{id} to a specific C++ lambda or function pointer.

Request/Response Objects: High-level abstractions that wrap the raw HTTP data so the user doesn't have to deal with headers and status codes manually.

Middleware Stack: A way to "wrap" the main handler with logic like Logging, CORS, or GZIP compression. In C++, this is often done using a "Chain of Responsibility" pattern or template-based wrapping.

1. The Framework Layer (Your "FastAPI")
FastAPI's "magic" is how it handles data. To replicate this in C++, you need:

Serialization/Validation: Instead of Pydantic, you’ll likely use nlohmann/json or Glaze for JSON, and potentially Reflect-cpp for compile-time reflection to automatically validate types.

Dependency Injection: A system to "inject" database connections or auth-users into your route functions.

OpenAPI Generator: You need code that scans your routes and types at compile-time (or startup) to generate a openapi.json file.
