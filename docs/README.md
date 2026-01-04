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
