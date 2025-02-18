set(HEADER_FILES
  include/test_util/disconnect_sender.hpp
  include/test_util/unbuffered_disconnect_sender.hpp
  include/test_util/disconnect_receiver.hpp
  include/test_util/unbounded_disconnect_receiver.hpp
  include/test_util/one_item.hpp
  include/test_util/two_seperate.hpp
  include/test_util/two_consecutive.hpp
  include/test_util/leftover.hpp
  include/test_util/bounded_try.hpp
  include/test_util/unbounded_try.hpp
  include/test_util/unbuffered_try.hpp
  include/test_util/one_to_one_disconnect_sender.hpp
  include/test_util/one_to_one_disconnect_receiver.hpp
  include/test_util/many_to_one_disconnect_sender.hpp
  include/test_util/many_to_one_disconnect_receiver.hpp
  include/test_util/one_to_many_disconnect_sender.hpp
  include/test_util/one_to_many_disconnect_receiver.hpp
  include/test_util/many_to_many_disconnect_sender.hpp
  include/test_util/many_to_many_disconnect_receiver.hpp
  include/test_util/add_remove_sender.hpp
  include/test_util/add_remove_receiver.hpp
  include/test_util/add_remove_sender_receiver.hpp
)

add_library(test_util INTERFACE)
foreach(header IN LISTS HEADER_FILES)
  target_sources(test_util INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES ${header}
  )
endforeach()
target_include_directories(test_util INTERFACE include)
target_link_libraries(test_util INTERFACE chan)
