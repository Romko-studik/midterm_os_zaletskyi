#include "options_parser.h"
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <iostream>

char buf[4 * 1024 * 1024];
// output buffer for capitalized characters
char out_buf[4 * 1024 * 1024 ];

int readbuffer(int fd, char *buffer, ssize_t size, int *status) {
  ssize_t read_bytes = 0;

  while (read_bytes < size) {
    ssize_t read_now = read(fd, buffer + read_bytes, size - read_bytes);
    if (read_now == -1) {
      if (errno == EINTR)
        continue;
      else {
        *status = errno;
        return -1;
      }
    } else if (read_now == 0)
      break;
    else
      read_bytes += read_now;
  }
  return read_bytes;
}

int writebuffer(int fd, const char *buffer, ssize_t size, int *status) {
  ssize_t written_bytes = 0;

  while (written_bytes < size) {
    ssize_t written_now =
        write(fd, buffer + written_bytes, size - written_bytes);
    if (written_now == -1) {
      if (errno == EINTR)
        continue;
      else {
        *status = errno;
        return -1;
      }
    } else
      written_bytes += written_now;
  }
  return 0;
}


void capitilize(int fd_in,int fd_out) {
  ssize_t n;
  int status;

  while ((n = readbuffer(fd_in, buf, sizeof(buf), &status)) > 0) {
    //capitalise the buffer
    for (int i = 0; i < n; i++) {
      if (isalpha(buf[i])) {
        out_buf[i] = toupper(buf[i]);
      } else {
        out_buf[i] = buf[i];
      }
    }
    if (writebuffer(fd_out,out_buf , n, &status) != 0) {
      std::string s("capitilize: write error\n");      
      write(2, s.c_str(), s.size());
      std::cerr << strerror(status) << std::endl;
      return;
    }
  }
  if (n < 0) {
    return;
  }
}

int main(int argc, char *argv[]) {
  pid_t pid = getpid();

  command_line_options_t command_line_options{argc, argv};
  std::vector<int> fds;
  //open files 1 in R_ONLY mode and 2 in W_ONLY mode
  auto filenames = command_line_options.get_filenames();
  if(filenames.size() != 2){
    std::cerr << "Error: Expected 2 filenames, got " << filenames.size() << std::endl;
  }
  //open files with error handling
  int fd1 = open(filenames[0].c_str(), O_RDONLY);
  int fd2 = open(filenames[1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd2 == -1 || fd1 == -1) {
    std::cerr << "Error: Could not open file" << std::endl;
    return 1;
  }
  //output information about file descriptors
  // std::cout << "fd1: " << fd1 << " fd2: " << fd2 << std::endl;
  // //output information about files
  // char stat_buf[1024*1024*4];
  // fstat(fd1, (struct stat *)stat_buf);
  // struct stat *stat1 = (struct stat *)stat_buf;
  // std::cout << "fd1: " << stat1->st_mode << std::endl;
  // fstat(fd2, (struct stat *)stat_buf);
  // struct stat *stat2 = (struct stat *)stat_buf;
  // std::cout << "fd2: " << stat2->st_size << std::endl;
  capitilize(fd1,fd2);
  close(fd1);
  close(fd2);
  return 0;
}
