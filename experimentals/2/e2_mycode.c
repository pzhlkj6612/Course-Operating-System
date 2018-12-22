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

int main()
{
  int fd, page_size;
  pid_t pid;
  // ⑴显示当前系统的页面大小;
  page_size = getpagesize();
  printf("⑴Page size: %d\n\n", page_size);
  //

  fd = open("./f1", O_RDWR);
  if (fd == -1)
  {
    printf("error open file.\n");
    exit(1);
  }
  char *src = (char *)mmap(NULL, page_size, PROT_WRITE, MAP_SHARED, fd, 0);
  if (src == MAP_FAILED)
  {
    printf("error\n");
    exit(1);
  }
  // ⑵显示src的内容及其内存起始地址;
  printf("⑵The content of %ssrc%s is:\n%s%s%s\nAddress of src: %p\n\n", RED, RESET, RED, src, RESET, src);
  //

  pid = fork();
  if (pid == 0)
  {
    /*子进程空间*/

    // ⑹使用memchr()将src中第一个W的地址赋给temp;
    char *temp = memchr(src, 'W', page_size);
    if (NULL == temp)
    {
      printf("⑹\'W\' was not found in the first %d characters of src.\n", page_size);
      exit(1);
    }
    //

    // ⑺显示temp的内容及其内存起始地址;
    printf("⑺The content of %stemp%s is:\n%s%s%s\nAddress of temp: %p\n\n", YEL, RESET, YEL, temp, RESET, temp);
    //

    // ⑻将temp中前2个字符用@替换;
    memset(temp, '@', 2);
    printf("⑻The content of %stemp%s is:\n%s%s%s\n", YEL, RESET, YEL, temp, RESET);
    //

    // ⑽显示src的内容(包含PID及PPID)；
    printf("⑽PID is: %d, PPID is: %d, the content of %ssrc%s:\n%s%s%s\n", getpid(), getppid(), RED, RESET, RED, src, RESET);
    //
  }
  else if (pid > 0)
  {
    /*父进程空间*/

    wait(NULL);
    // ⑿使用memset()将共享存储区src中前2个字符用QQ替换，并显示替换后的内容(包含PID及PPID);
    memset(src, 'Q', 2);
    printf("⑿PID is: %d, PPID is: %d, the content of %ssrc%s:\n%s%s%s\n", getpid(), getppid(), RED, RESET, RED, src, RESET);
    //

    // ⒀显示经2次修改后的f1的最终内容;
    int n;
    char buf[page_size];
    printf("⒀f1:\n" CYN);
    while ((n = read(fd, buf, page_size)) > 0)
      printf("%s", buf);
    if (n < 0)
      printf("⒀read error\n");
    printf("\n" RESET);
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
  if (close(fd) == 0)
  {
    printf("⑾PID is: %d, PPID is: %d, close(fd) success\n", getpid(), getppid());
  }
  else
  {
    printf("⑾PID is: %d, PPID is: %d, close(fd) failed: %s\n", getpid(), getppid(), strerror(errno));
  }
  //

  printf("\n");
  return 0;
}
