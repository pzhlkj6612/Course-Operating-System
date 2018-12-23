#include <unistd.h>
#include <sys/mman.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

// https://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"
//

extern int errno;

int fd_log;
int page_size;
char *logA;

void init_log(const char *filename)
{
  printf("init log.\n");
  if ((fd_log = creat(filename, 00644)) == -1)
  {
    printf("create log error: %s\n", strerror(errno));
    exit(-1);
  }
  printf("File descriptor of log: %d\n", fd_log);
}

void close_log()
{
  // No!
  printf("close log %d.\n", fd_log);
  // if (close(fd_log) == -1)
  // {
  //   printf("close log error: %s\n", strerror(errno));
  //   exit(-1);
  // }
  close(fd_log);
}

int loglog()
{
  int count = printf("%s", logA);
  // printf("char count: %d\n", count);
  if (write(fd_log, logA, count) != count)
  {
    printf("write error: %s\n", strerror(errno));
    exit(-1);
  }
  memset(logA, '\0', count);
  return count;
}

// int loglog(const char *str)
// {
//   int count = printf("%s", str);
//   printf("char count: %d\n", count);
//   // char *log = (char *)malloc(count);
//   // strcpy(log, str);
//   if (write(fd_log, str, count) != count)
//   {
//     printf("write error: %s\n", strerror(errno));
//     // free(log);
//     exit(-1);
//   }
//   // free(log);
//   return count;
// }

int main(int argc, char const *argv[])
{
  if (argc < 4)
  {
    printf("Not enough arguments.\n");
    exit(1);
  }

  // https://stackoverflow.com/questions/15301495/get-first-char-from-char-variable-in-c
  char theOne = *(argv[1]);
  char theOtherOne = *(argv[2]);
  int repetition = atoi(argv[3]);
  int time = atoi(argv[4]);
  if (time < 0)
  {
    time = INT_MAX;
  }

  int fd_input;

  logA = (char *)malloc(page_size);
  memset(logA, '\0', page_size);

  init_log("./log");

  // ⑴显示当前系统的页面大小;
  page_size = getpagesize();
  printf("⑴Page size: %d\n\n", page_size);
  //

  char *theOther = malloc(repetition);
  memset(theOther, theOtherOne, repetition * sizeof(char));

  if (time > 1)
  {
    printf("%sI want to replace \'%c\' to \'%s\' %d times.%s\n\n", BLU, theOne, theOther, time, RESET);
  }
  else
  {
    printf("%sI want to replace \'%c\' to \'%s\' %d time.%s\n\n", BLU, theOne, theOther, time, RESET);
  }

  free(theOther);

  fd_input = open("./f1", O_RDWR);
  if (fd_input == -1)
  {
    printf("error open file.\n");
    exit(1);
  }
  char *src = (char *)mmap(NULL, page_size, PROT_WRITE, MAP_SHARED, fd_input, 0);
  if (src == MAP_FAILED)
  {
    printf("error\n");
    exit(1);
  }
  // ⑵显示src的内容及其内存起始地址;
  printf("⑵The content of %ssrc%s is:\n%s%s%s\nAddress of src: %p\n\n", RED, RESET, RED, src, RESET, src);
  //

  pid_t pid = fork();
  if (pid == 0)
  {
    /*子进程空间*/

    //
    int i = 1;
    while (time > 0)
    {

      // ⑹使用memchr()将src中第一个W的地址赋给tmp;
      char *tmp = memchr(src, theOne, page_size);
      if (NULL == tmp)
      {
        printf("⑹\'%c\' was not found in the first %d characters of src.\n\n", theOne, page_size);
        exit(1);
      }
      //

      printf("Time %sNo.%d%s:\n", BLU, i, RESET);

      // ⑺显示tmp的内容及其内存起始地址;
      printf("⑺The content of %stmp%s is:\n%s%s%s\nAddress of tmp: %p\n\n", YEL, RESET, YEL, tmp, RESET, tmp);
      //

      // ⑻将tmp中前2个字符用$theOtherOne替换;
      memset(tmp, theOtherOne, repetition);
      printf("⑻The content of %stmp%s is:\n%s%s%s\n\n", YEL, RESET, YEL, tmp, RESET);
      //

      time--;
      i++;
    }
    //

    // ⑽显示src的内容(包含PID及PPID)；
    printf("⑽PID is: %d, PPID is: %d, the content of %ssrc%s:\n%s%s%s\n\n", getpid(), getppid(), RED, RESET, RED, src, RESET);
    //
  }
  else if (pid > 0)
  {
    /*父进程空间*/

    wait(NULL);
    // ⑿使用memset()将共享存储区src中前2个字符用QQ替换，并显示替换后的内容(包含PID及PPID);
    memset(src, 'Q', 2);
    printf("⑿PID is: %d, PPID is: %d, the content of %ssrc%s:\n%s%s%s\n\n", getpid(), getppid(), RED, RESET, RED, src, RESET);
    //

    // ⒀显示经2次修改后的f1的最终内容;
    int n;
    char buf[page_size];
    printf("⒀" CYN "f1" RESET ":\n" CYN);
    while ((n = read(fd_input, buf, page_size)) > 0)
      printf("%s", buf);
    if (n < 0)
      printf("⒀read error\n");
    printf("\n\n" RESET);
    //
  }
  if (munmap(src, page_size) == 0) /*释放共享存储区*/
  {
    printf("PID is: %d, PPID is: %d, munmap src success\n", getpid(), getppid());
  }
  else
  {
    printf("PID is: %d, PPID is: %d, munmap src failed: %s\n", getpid(), getppid(), strerror(errno));
  }

  // ⑾关闭之前打开的文件f1;
  if (close(fd_input) == 0)
  {
    printf("⑾PID is: %d, PPID is: %d, close(fd_input) success\n", getpid(), getppid());
  }
  else
  {
    printf("⑾PID is: %d, PPID is: %d, close(fd_input) failed: %s\n", getpid(), getppid(), strerror(errno));
  }
  //

  if (close(fd_log) == 0)
  {
    printf("PID is: %d, PPID is: %d, close(fd_log) success\n", getpid(), getppid());
  }
  else
  {
    printf("PID is: %d, PPID is: %d, close(fd_log) failed: %s\n", getpid(), getppid(), strerror(errno));
  }

  close_log();
  free(logA);

  printf("\n<END>\n");
  return 0;
}
