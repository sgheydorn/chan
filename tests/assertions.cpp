#include <concepts>
#include <ranges>

#include <chan/mpmc/bounded/Receiver.hpp>
#include <chan/mpmc/bounded/Sender.hpp>
#include <chan/mpmc/unbounded/Receiver.hpp>
#include <chan/mpmc/unbounded/Sender.hpp>
#include <chan/mpmc/unbuffered/Receiver.hpp>
#include <chan/mpmc/unbuffered/Sender.hpp>
#include <chan/mpsc/bounded/Receiver.hpp>
#include <chan/mpsc/bounded/Sender.hpp>
#include <chan/mpsc/unbounded/Receiver.hpp>
#include <chan/mpsc/unbounded/Sender.hpp>
#include <chan/mpsc/unbuffered/Receiver.hpp>
#include <chan/mpsc/unbuffered/Sender.hpp>
#include <chan/spmc/bounded/Receiver.hpp>
#include <chan/spmc/bounded/Sender.hpp>
#include <chan/spmc/unbounded/Receiver.hpp>
#include <chan/spmc/unbounded/Sender.hpp>
#include <chan/spmc/unbuffered/Receiver.hpp>
#include <chan/spmc/unbuffered/Sender.hpp>
#include <chan/spsc/bounded/Receiver.hpp>
#include <chan/spsc/bounded/Sender.hpp>
#include <chan/spsc/unbounded/Receiver.hpp>
#include <chan/spsc/unbounded/Sender.hpp>
#include <chan/spsc/unbuffered/Receiver.hpp>
#include <chan/spsc/unbuffered/Sender.hpp>

// clang-format off
static_assert(std::movable<chan::spsc::bounded::Sender<int>>);
static_assert(!std::copyable<chan::spsc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spsc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::spsc::bounded::Receiver<int>>);
static_assert(!std::copyable<chan::spsc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spsc::bounded::Receiver<int>>);

static_assert(std::movable<chan::spsc::unbounded::Sender<int>>);
static_assert(!std::copyable<chan::spsc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spsc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::spsc::unbounded::Receiver<int>>);
static_assert(!std::copyable<chan::spsc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spsc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::spsc::unbuffered::Sender<int>>);
static_assert(!std::copyable<chan::spsc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::spsc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::spsc::unbuffered::Receiver<int>>);
static_assert(!std::copyable<chan::spsc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spsc::unbuffered::Receiver<int>>);

static_assert(std::movable<chan::mpsc::bounded::Sender<int>>);
static_assert(std::copyable<chan::mpsc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpsc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::mpsc::bounded::Receiver<int>>);
static_assert(!std::copyable<chan::mpsc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpsc::bounded::Receiver<int>>);

static_assert(std::movable<chan::mpsc::unbounded::Sender<int>>);
static_assert(std::copyable<chan::mpsc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpsc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::mpsc::unbounded::Receiver<int>>);
static_assert(!std::copyable<chan::mpsc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpsc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::mpsc::unbuffered::Sender<int>>);
static_assert(std::copyable<chan::mpsc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpsc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::mpsc::unbuffered::Receiver<int>>);
static_assert(!std::copyable<chan::mpsc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpsc::unbuffered::Receiver<int>>);

static_assert(std::movable<chan::spmc::bounded::Sender<int>>);
static_assert(!std::copyable<chan::spmc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spmc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::spmc::bounded::Receiver<int>>);
static_assert(std::copyable<chan::spmc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spmc::bounded::Receiver<int>>);

static_assert(std::movable<chan::spmc::unbounded::Sender<int>>);
static_assert(!std::copyable<chan::spmc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spmc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::spmc::unbounded::Receiver<int>>);
static_assert(std::copyable<chan::spmc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spmc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::spmc::unbuffered::Sender<int>>);
static_assert(!std::copyable<chan::spmc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::spmc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::spmc::unbuffered::Receiver<int>>);
static_assert(std::copyable<chan::spmc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spmc::unbuffered::Receiver<int>>);

static_assert(std::movable<chan::mpmc::bounded::Sender<int>>);
static_assert(std::copyable<chan::mpmc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpmc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::mpmc::bounded::Receiver<int>>);
static_assert(std::copyable<chan::mpmc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpmc::bounded::Receiver<int>>);

static_assert(std::movable<chan::mpmc::unbounded::Sender<int>>);
static_assert(std::copyable<chan::mpmc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpmc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::mpmc::unbounded::Receiver<int>>);
static_assert(std::copyable<chan::mpmc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpmc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::mpmc::unbuffered::Sender<int>>);
static_assert(std::copyable<chan::mpmc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpmc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::mpmc::unbuffered::Receiver<int>>);
static_assert(std::copyable<chan::mpmc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpmc::unbuffered::Receiver<int>>);
// clang-format on

int main() { return 0; }
