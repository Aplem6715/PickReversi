// #ifndef BUFFER_SINK_H
// #define BUFFER_SINK_H

// #include <spdlog/details/fmt_helper.h>
// #include <spdlog/fmt/fmt.h>
// #include <spdlog/sinks/base_sink.h>
// #include <spdlog/details/log_msg_buffer.h>
// #include <string>
// #include <vector>

// // class DummyFormatter : public spdlog::formatter
// // {
// // public:
// //     virtual ~DummyFormatter() = default;
// //     virtual void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override
// //     {
// //         spdlog::details::fmt_helper::append_string_view(msg.payload, dest);
// //         spdlog::details::fmt_helper::append_string_view(spdlog::details::os::default_eol, dest);
// //     }
// //     virtual std::unique_ptr<spdlog::formatter> clone() const override
// //     {
// //         return nullptr;
// //     }
// // };

// template <typename Mutex>
// class BufferSink : public spdlog::sinks::base_sink<Mutex>
// {
// public:
//     BufferSink(std::shared_ptr<spdlog::sinks::sink> sink)
//         : subSink_(sink)
//     {
//     }

// protected:
//     void sink_it_(const spdlog::details::log_msg& msg) override
//     {
//         buffer_.push_back(spdlog::details::log_msg_buffer{msg});
//     }

//     void flush_() override
//     {
//         for (auto& msg : buffer_)
//         {
//             if (subSink_->should_log(msg.level))
//             {
//                 subSink_->log(msg);
//             }
//         }
//         buffer_.clear();
//     }

// private:
//     std::vector<spdlog::details::log_msg_buffer> buffer_;
//     std::shared_ptr<spdlog::sinks::sink> subSink_;
// };

// #include "spdlog/details/null_mutex.h"
// #include <mutex>

// using BufferSink_mt = BufferSink<std::mutex>;
// using BufferSink_st = BufferSink<spdlog::details::null_mutex>;

// #endif