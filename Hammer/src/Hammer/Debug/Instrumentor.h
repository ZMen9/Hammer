#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>
#include "Hammer/Core/Log.h"

namespace hammer {
using FloatingPointMicrosecends = std::chrono::duration<double, std::micro>;

struct ProfileResult {
  std::string name;
  FloatingPointMicrosecends start;
  std::chrono::microseconds elapsed_time;
  std::thread::id thread_id;
};

struct InstrumentationSession {
  std::string name;
};

class Instrumentor {
 public:
  Instrumentor(const Instrumentor&) = delete;
  Instrumentor& operator=(const Instrumentor&) = delete;
  Instrumentor(Instrumentor&&) = delete;

  static Instrumentor& GetInstance() {
    static Instrumentor instance;
    return instance;
  }

  void BeginSession(const std::string& name,
                    const std::string& file_path = "result.json") {
    std::lock_guard<std::mutex> lock(mtx_);
    if (current_session_) {
      // If there is already a current session, then close it before beginning
      // new one. Subsequent profiling output meant for the original session
      // will end up in the newly opened session instead. That's better than
      // having badly formatted profiling output.
      if (Log::core_logger()) {  // Edge case: BeginSession() might be before
                                 // Log::Init()
        HM_CORE_ERROR(
            "Instrumentor::BeginSession('{0}') when session '{1}' already "
            "open.",
            name, current_session_->name);
      }
      InternalEndSession();
    }
    output_stream_.open(file_path);
    if (output_stream_.is_open()) {
      current_session_ = new InstrumentationSession{name};
      WriteHead();
    } else {
      if (Log::core_logger()) {  // Edge case: BeginSession() might be before
                                 // Log::Init()
        HM_CORE_ERROR("Instrumentor could not open results file '{0}'.", name);
      }
    }
  }

  void WriteProfile(const ProfileResult& result) {
    std::stringstream json;

    json << std::setprecision(3) << std::fixed;
    json << ",{";
    json << "\"cat\":\"function\",";
    json << "\"dur\":" << result.elapsed_time.count() << ",";
    json << "\"name\":\"" << result.name << "\",";
    json << "\"ph\":\"X\",";
    json << "\"pid\":0,";
    json << "\"tid\":" << result.thread_id << ',';
    json << "\"ts\":" << result.start.count();
    json << "}";

    std::lock_guard<std::mutex> lock(mtx_);
    if (current_session_) {
      output_stream_ << json.str();
      output_stream_.flush();
    }
  }

  void EndSession() {
    std::lock_guard<std::mutex> lock(mtx_);
    InternalEndSession();
  }

 private:
  // Using RAII
  // If a session is begun using HZ_PROFILE_BEGIN_SESSION(...) but
  // HZ_PROFILE_END_SESSION() is not called before the program ends then the heap
  // allocated InstrumentationSession is not deleted and the footer is not added
  // to the file which creates malformed JSON.
  Instrumentor() : current_session_(nullptr) {}
  ~Instrumentor() { EndSession(); }
  void WriteHead() {
    output_stream_ << "{\"otherData\": {},\"traceEvents\":[{}";
    output_stream_.flush();
  }

  // Note: you must already own lock on mtx_ before calling InternalEndSession()
  void InternalEndSession() {
    if (current_session_) {
      WriteFooter();
      output_stream_.close();
      delete current_session_;
      current_session_ = nullptr;
    }
  }

  void WriteFooter() {
    output_stream_ << "]}";
    output_stream_.flush();
  }

 private:
  std::ofstream output_stream_;
  InstrumentationSession* current_session_;
  std::mutex mtx_;
};

class InstrumentationTimer {
 public:
  InstrumentationTimer(const char* name) : name_(name), is_stop_(false) {
    start_time_point_ = std::chrono::steady_clock::now();
  }

  ~InstrumentationTimer() {
    if (!is_stop_) Stop();
  }

  void Stop() {
    auto end_time_point = std::chrono::high_resolution_clock::now();

    auto high_res_start =
        FloatingPointMicrosecends{start_time_point_.time_since_epoch()};

    auto elapsed_time =
        std::chrono::time_point_cast<std::chrono::microseconds>(end_time_point)
            .time_since_epoch() -
        std::chrono::time_point_cast<std::chrono::microseconds>(
            start_time_point_)
            .time_since_epoch();

    Instrumentor::GetInstance().WriteProfile(
        {name_, high_res_start, elapsed_time, std::this_thread::get_id()});

    is_stop_ = true;
  }

 private:
  const char* name_;
  bool is_stop_;
  std::chrono::time_point<std::chrono::steady_clock> start_time_point_;
};

namespace InstrumentorUtils {

template <size_t N>
struct ChangeResult {
  char data[N];
};

template <size_t N, size_t K>
constexpr auto CleanupOutputString(const char (&expr)[N],
                                   const char (&remove)[K]) {
  ChangeResult<N> result = {};

  size_t src_idx = 0;
  size_t dst_idx = 0;
  while (src_idx < N) {
    size_t match_idx = 0;
    while (match_idx < K - 1 && src_idx + match_idx < N - 1 &&
           expr[src_idx + match_idx] == remove[match_idx])
      match_idx++;
    if (match_idx == K - 1) src_idx += match_idx;
    result.data[dst_idx++] = expr[src_idx] == '"' ? '\'' : expr[src_idx];
    src_idx++;
  }
  return result;
}

}


}  // namespace hammer

#define HM_PROFILE 0
#if HM_PROFILE

// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || \
    (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define HM_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define HM_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define HM_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || \
    (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define HM_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define HM_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define HM_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define HM_FUNC_SIG __func__
#else
#define HM_FUNC_SIG "HM_FUNC_SIG unknown!"
#endif

#define HM_PROFILE_BEGIN_SESSION(name, file_path) \
  ::hammer::Instrumentor::GetInstance().BeginSession(name, file_path)

#define HM_PROFILE_END_SESSION() \
  ::hammer::Instrumentor::GetInstance().EndSession()

#define HM_PROFILE_SCOPE_LINE2(name, line) \
  constexpr auto fixed_name##line = \
    ::hammer::InstrumentorUtils::CleanupOutputString(name, "__cdecl"); \
    ::hammer::InstrumentationTimer timer##line(fixed_name##line.data)
#define HM_PROFILE_SCOPE_LINE1(name, line) HM_PROFILE_SCOPE_LINE2(name, line)
#define HM_PROFILE_SCOPE(name) HM_PROFILE_SCOPE_LINE1(name, __LINE__)

#define HM_PROFILE_FUNCTION() HM_PROFILE_SCOPE(HM_FUNC_SIG)

#else
#define HM_PROFILE_BEGIN_SESSION(name, file_path)
#define HM_PROFILE_END_SESSION()
#define HM_PROFILE_SCOPE(name)
#define HM_PROFILE_FUNCTION()

#endif  // HM_PROFILE
