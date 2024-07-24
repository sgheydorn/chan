# Chan
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
    for (auto i : rx) {
      std::cout << "Got " << i << '\n';
    }
  });

  tx_thread.join();
  rx_thread.join();
}
```

## Summary
Channels are a way of streaming items between threads.

This channel implementation uses the concept of senders and receivers.
- A channel is only accessible via a `Sender` or `Receiver`.
- A `Sender` can insert items into a channel.
- A `Receiver` can consume items from a channel.
- When all `Sender`s are destroyed, any threads blocking on a `recv` will unblock.
- When all `Receiver`s are destroyed, any threads blocking on a `send` will unblock.
- Channels do **not** need to be explicitly closed.

Every item sent on the channel will be received by at most one `Receiver`.
This library does not provide "broadcast" channels.

## Requirements
- C++23

## Installation
This is a header-only library. Add the contents of [src](./src) to your include path.

## Detailed description
### Channel types
#### Bounded
Items are stored in a fixed-sized buffer.
`send` will block until the buffer is not full.
`recv` will block until the buffer is not empty.

This is the fastest channel type.

#### Unbounded
Items are stored in a growable buffer.
`send` will not block.
`recv` will block until the buffer is not empty.

When using unbounded channels, make sure the receiving thread(s) can keep up with the sending thread(s).
If there are more `send`s than `recv`s over a long period of time, the channel will consume all available memory.

#### Unbuffered
Items are passed from sender to receiver without buffering.
`send` will block until there is a `recv` to pair with.
`recv` will block until there is a `send` to pair with.

This is the slowest channel type.

### Channel variants
Use the most restrictive variant for best performance.

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

Where possible, avoid having the same thread own a `Sender` and a `Receiver` for the same channel. This can easily lead to deadlock.

When capacity is `0`, `chan::mpsc::bounded::channel` is different from Rust's `std::sync::mpsc::sync_channel`.
`chan::mpsc::bounded::channel` will create a bounded channel with capacity `0`.
`std::sync::mpsc::sync_channel` will create an unbuffered channel.
