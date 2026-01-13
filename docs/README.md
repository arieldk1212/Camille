# Camille Web Framework

## Overview

Web framework written in modern c++.

## More Products - Camille

Camille's Web Framework is the first child, but as always i strive for more, therefore there's also (not yet :P ):

1. Camille ORM
2. Camille Proxy - Fast, Optimized Proxy, Reverse Proxy in modern C++.
3. Camille Agenty - C++ LLM Agent Framework, made for the C++ Community.
4. Camille Itch - Database analytics tracker.

## Inspirations

1. cpp-httplib for simplicity and header-only style, oat++.
2. DHH for Ruby On Rails, all in one.
3. FastAPI for architecture, async, starlette, asgi.

## Deep Dive

In order to create Camille, similar to modern async frameworks i had to use several libraries that are open sourced.
Asio: Camille is built above the asio C++ framework.
Json: nlohmann/json or something custom made.
Router: custom made, radix, trie, regex, params.
Parsing: custom made with help from open source implementations.
