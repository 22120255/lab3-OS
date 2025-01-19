#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"


uint64
sys_trace(void)
{
    int mask;
    if(argint(0, &mask) < 0)
        return -1;

    struct proc *p = myproc();
    p->tracemask = mask;
    return 0;
}


uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


int sys_sysinfo(void) {
    struct sysinfo info;
    uint64 user_addr;

    // Lấy địa chỉ tham số từ user space
    if (argaddr(0, &user_addr) < 0)
        return -1;

    // Tính toán số byte bộ nhớ trống
    info.freemem = kfreemem(); // Hàm này sẽ được thêm vào kalloc.c

    // Đếm số lượng tiến trình không UNUSED
    info.nproc = proc_count(); // Hàm này sẽ được thêm vào proc.c

    // Tính toán load average trong 1 phút, 5 phút và 15 phút
    info.loadavg[0] = calc_loadavg(1);  // Load average trong 1 phút
    info.loadavg[1] = calc_loadavg(5);  // Load average trong 5 phút
    info.loadavg[2] = calc_loadavg(15); // Load average trong 15 phút

    // Sao chép dữ liệu `sysinfo` sang user space
    if (copyout(myproc()->pagetable, user_addr, (char *)&info, sizeof(info)) < 0)
        return -1;

    return 0;
}
