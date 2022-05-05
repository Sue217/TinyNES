#ifndef _LOG_
#define _LOG_

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

#ifndef _FILENAME_
#define _FILENAME_ _FILE_
#endif
/*
#define LOG(level)                                                             \
  if (level <= Log::get().getLevel())                                          \
  Log::get().getStream() << '[' << __FILENAME__ << ":" << std::dec << __LINE__ \
                         << "] "
*/
//! For test
#define LOG(level) std::cout

#define LOG_CPU \
  if (CpuTrace == Log::get().getLevel()) Log::get().getCpuTraceStream()

enum Level {
  None,
  Error,
  Info,
  InfoVerbose,
  CpuTrace
};

class Log {
 public:
  ~Log();
  static Log& get();

  void setLogStream(std::ostream& stream);
  std::ostream& getLogStream();
  
  void setCpuTraceStream(std::ostream& stream);
  std::ostream& getCpuTraceStream();

  Log& setLevel(Level level);
  Level getLevel();

 private:
  Level m_logLevel;
  std::ostream* m_logStream;
  std::ostream* m_cpuTrace;
};

// Courtesy of http://wordaligned.org/articles/cpp-streambufs#toctee-streams
class TeeBuf : public std::streambuf {
 public:
  /* Construct an streambuf which tees output to both input streambufs.*/
  TeeBuf(std::streambuf* sb1, std::streambuf* sb2);
 private:
  /*
   * This tee buffer has no buffer. So every character "overflows"
   * and can be put directly into the teed buffers.
  */
  virtual int overflow(int c);
  // Sync both teed buffers.
  virtual int sync();
 private:
  std::streambuf* m_sb1;
  std::streambuf* m_sb2;
};

class TeeStream : public std::ostream {
 public:
  /* Construct an ostream which tees output to the supplied ostreams. */
  TeeStream(std::ostream& o1, std::ostream& o2);
 private:
  TeeBuf m_tbuf;
};

#endif  // _LOG_