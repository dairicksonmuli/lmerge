/*
  lmerge: Merge multiple line-oriented streams into a single stream.

  Copyright (c) 2017 Rian Hunter <rian@alum.mit.edu>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <fcntl.h>
#include <unistd.h>

#include <cstddef>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

namespace lmerge {

const char DELIM = '\n';

std::string getline(std::istream & istream) {
  std::string result;
  std::getline(istream, result, DELIM);
  return result;
}

int open(const std::string & path) {
  auto fd = ::open(path.c_str(), O_RDONLY);
  if (fd < 0) throw std::system_error(errno, std::generic_category());
  return fd;
}

void set_blocking(int fd, bool blocking) {
  int flags = ::fcntl(fd, F_GETFL, 0);
  if (flags < 0) throw std::system_error(errno, std::generic_category());
  flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
  int result = ::fcntl(fd, F_SETFL, flags);
  if (result < 0) throw std::system_error(errno, std::generic_category());
}

int main(int argc, char *argv[]) {
  struct Input {
    bool eof;
    std::size_t num_nls;
    int fd;
    std::stringstream stream;
  };
  std::vector<Input> inputs;

  inputs.push_back({false, 0, 0, std::stringstream()});
  for (int i = 1; i < argc; ++i) {
    inputs.push_back({false, 0, lmerge::open(argv[i]), std::stringstream()});
  }

  for (auto & input : inputs) {
    set_blocking(input.fd, false);
    input.stream.exceptions(std::stringstream::failbit |
                            std::stringstream::badbit |
                            std::stringstream::eofbit);
  }

  while (true) {
    ::fd_set rfds;
    int maxfd = 0;
    FD_ZERO(&rfds);
    for (auto & input : inputs) {
      if (input.eof) continue;
      FD_SET(input.fd, &rfds);
      maxfd = std::max(input.fd, maxfd);
    }
    if (!maxfd) break;

    auto ret = ::select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
    if (ret < 0) {
      if (errno == EINTR) continue;
      throw std::system_error(errno, std::generic_category());
    }

    bool has_data = true;
    while (has_data) {
      has_data = false;
      for (auto & input : inputs) {
        // fill up buffer as much as possible until we get a newline
        while (!input.num_nls && !input.eof) {
          char buf[4096];
          auto n = ::read(input.fd, buf, sizeof(buf));
          if (n < 0) {
            if (errno == EAGAIN) break;
            if (errno == EINTR) continue;
            throw std::system_error(errno, std::generic_category());
          }
          if (!n) {
            input.eof = true;
            break;
          }

          std::ostream_iterator<char> it(input.stream);
          std::copy(buf, buf + n, it);
          input.num_nls += std::count(buf, buf + n, DELIM);
        }

        if (input.num_nls) {
          std::cout << lmerge::getline(input.stream) << std::endl;
          input.num_nls -= 1;
          if (!has_data) has_data = true;
        }
      }
    }
  }

  return 0;
}

}

int main(int argc, char *argv[]) {
  return lmerge::main(argc, argv);
}
