cmake_minimum_required(VERSION 3.23)

project(chan VERSION 0.1.0)

option(CHAN_BUILD_TESTS "Build chan tests." OFF)

set(HEADER_FILES
  include/chan/detail/BoundedChannel.hpp
  include/chan/detail/CvarSemaphore.hpp
  include/chan/detail/expected_to_optional.hpp
  include/chan/detail/SemaphoreType.hpp
  include/chan/detail/UnboundedChannel.hpp
  include/chan/detail/UnbufferedChannel.hpp
  include/chan/mpmc/bounded/Chan.hpp
  include/chan/mpmc/bounded/channel.hpp
  include/chan/mpmc/bounded/Receiver.hpp
  include/chan/mpmc/bounded/Sender.hpp
  include/chan/mpmc/unbounded/Chan.hpp
  include/chan/mpmc/unbounded/channel.hpp
  include/chan/mpmc/unbounded/PacketChunk.hpp
  include/chan/mpmc/unbounded/Receiver.hpp
  include/chan/mpmc/unbounded/Sender.hpp
  include/chan/mpmc/unbuffered/Chan.hpp
  include/chan/mpmc/unbuffered/channel.hpp
  include/chan/mpmc/unbuffered/Receiver.hpp
  include/chan/mpmc/unbuffered/Sender.hpp
  include/chan/mpmc/Packet.hpp
  include/chan/mpsc/bounded/Chan.hpp
  include/chan/mpsc/bounded/channel.hpp
  include/chan/mpsc/bounded/Receiver.hpp
  include/chan/mpsc/bounded/Sender.hpp
  include/chan/mpsc/unbounded/Chan.hpp
  include/chan/mpsc/unbounded/channel.hpp
  include/chan/mpsc/unbounded/PacketChunk.hpp
  include/chan/mpsc/unbounded/Receiver.hpp
  include/chan/mpsc/unbounded/Sender.hpp
  include/chan/mpsc/unbuffered/Chan.hpp
  include/chan/mpsc/unbuffered/channel.hpp
  include/chan/mpsc/unbuffered/Receiver.hpp
  include/chan/mpsc/unbuffered/Sender.hpp
  include/chan/mpsc/Packet.hpp
  include/chan/spmc/bounded/Chan.hpp
  include/chan/spmc/bounded/channel.hpp
  include/chan/spmc/bounded/Receiver.hpp
  include/chan/spmc/bounded/Sender.hpp
  include/chan/spmc/unbounded/Chan.hpp
  include/chan/spmc/unbounded/channel.hpp
  include/chan/spmc/unbounded/PacketChunk.hpp
  include/chan/spmc/unbounded/Receiver.hpp
  include/chan/spmc/unbounded/Sender.hpp
  include/chan/spmc/unbuffered/Chan.hpp
  include/chan/spmc/unbuffered/channel.hpp
  include/chan/spmc/unbuffered/Receiver.hpp
  include/chan/spmc/unbuffered/Sender.hpp
  include/chan/spmc/Packet.hpp
  include/chan/spsc/bounded/Chan.hpp
  include/chan/spsc/bounded/channel.hpp
  include/chan/spsc/bounded/Receiver.hpp
  include/chan/spsc/bounded/Sender.hpp
  include/chan/spsc/unbounded/Chan.hpp
  include/chan/spsc/unbounded/channel.hpp
  include/chan/spsc/unbounded/ItemChunk.hpp
  include/chan/spsc/unbounded/Receiver.hpp
  include/chan/spsc/unbounded/Sender.hpp
  include/chan/spsc/unbuffered/Chan.hpp
  include/chan/spsc/unbuffered/channel.hpp
  include/chan/spsc/unbuffered/Receiver.hpp
  include/chan/spsc/unbuffered/Sender.hpp
  include/chan/DEFAULT_CHUNK_SIZE.hpp
  include/chan/RecvError.hpp
  include/chan/RecvIter.hpp
  include/chan/SendError.hpp
  include/chan/SendIter.hpp
  include/chan/TryRecvError.hpp
  include/chan/TryRecvErrorKind.hpp
  include/chan/TrySendError.hpp
  include/chan/TrySendErrorKind.hpp
)

add_library(chan INTERFACE)
foreach(header IN LISTS HEADER_FILES)
  target_sources(chan INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${header}>
      $<INSTALL_INTERFACE:${header}>
  )
endforeach()
target_include_directories(chan INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(chan INTERFACE cxx_std_23)

install(TARGETS chan EXPORT chan-config FILE_SET HEADERS DESTINATION include)
install(EXPORT chan-config DESTINATION lib/cmake/chan)

if(CHAN_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
