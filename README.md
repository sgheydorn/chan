# Chan

A C++ channel implementation similar to `std::sync::mpsc` from the Rust programming language.

## Example

```c++
#include "chan/spsc/bounded/channel.hpp"
#include <iostream>
#include <thread>

int main() {
  // Create a bounded channel of `int`s with a buffer of 10 items.
  auto [tx, rx] = chan::spsc::bounded::channel<int>(10);

  // Spawn a thread that sends 100 items to the channel.
  std::thread tx_thread([tx = std::move(tx)] {
    for (int i = 0; i < 100; ++i) {
      tx.send(i);
    }
  });

  // Spawn a thread receives all items from the channel.
  std::thread rx_thread([rx = std::move(rx)] {
    for (int i : rx) {
      std::cout << "Got " << i << '\n';
    }
  });

  tx_thread.join();
  rx_thread.join();
}
```

## Requirements

- C++23

## Installation

This is a header-only library. Add the contents of [include](./include) to your include path.

## Overview

Channels are a way of streaming items between threads.

You create a channel by calling the `chan::<channel_variant>::<channel_type>::channel` function, which returns a `Sender` and `Receiver` for the new channel.

### Channel types

#### Bounded

Channel that buffers items in a fixed-sized array.
`send` will block until the channel is not full.
`recv` will block until the channel is not empty.

#### Unbounded

Channel that buffers items in expandable storage.
`send` will not block.
`recv` will block until the channel is not empty.

When using unbounded channels, make sure the receiving thread(s) can keep up with the sending thread(s).
If there are more `send`s than `recv`s over a long period of time, the channel will consume all available memory.

#### Unbuffered

Channel that passes items from sender to receiver without buffering.
`send` will block until there is a `recv` to pair with.
`recv` will block until there is a `send` to pair with.

### Channel variants

For best performance, use the most restrictive variant that meets your needs.

#### spsc (**s**ingle **p**roducer **s**ingle **c**onsumer)

- Single sending thread. `Sender` is not copyable.
- Single receiving thread. `Receiver` is not copyable.

#### mpsc (**m**ultiple **p**roducer **s**ingle **c**onsumer)

- Multiple sending threads. `Sender` is copyable.
- Single receiving thread. `Receiver` is not copyable.

#### spmc (**s**ingle **p**roducer **m**ultiple **c**onsumer)

- Single sending thread. `Sender` is not copyable.
- Multiple receiving threads. `Receiver` is copyable.

#### mpmc (**m**ultiple **p**roducer **m**ultiple **c**onsumer)

- Multiple sending threads. `Sender` is copyable.
- Multiple receiving threads. `Receiver` is copyable.

## Compile-time flags (macros)

#### CHAN_REPLACE_SEMAPHORE_WITH_CONDITION_VARIABLE

Use `std::condition_variable` instead of `std::counting_semaphore`.

This flag exists because some standard library implementations of `std::counting_semaphore` have bugs.
It is a workaround for these bugs.

Enabling this flag will decrease performance, so you should only use if needed.

Only the existence of this macro matters. Its value is not used.

## Things to watch out for

Do not share a `Sender` or `Receiver` between threads. All `Sender` and `Receiver` types are **not** thread-safe. You must give each thread its own copy of the `Sender` or `Receiver`.

When capacity is `0`, `chan::mpsc::bounded::channel` is different from Rust's `std::sync::mpsc::sync_channel`.
`chan::mpsc::bounded::channel` will create a bounded channel with capacity `0`.
`std::sync::mpsc::sync_channel` will create an unbuffered channel.
