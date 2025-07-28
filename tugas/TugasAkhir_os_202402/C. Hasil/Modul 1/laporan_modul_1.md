# 📝 Laporan Tugas Akhir

**Mata Kuliah**: Sistem operasi
**Semester**: Genap / Tahun Ajaran 2024–2025
**Nama**: `<Akhmad Akbar Syarifudin>`
**NIM**: `<240202828>`
**dosen**: Hilmi Bahar Alim, S.Kom., M.Kom.
**Modul yang Dikerjakan**:
`(Modul 1 – System Call dan Instrumentasi Kernel)`

---

## 📌 Deskripsi Singkat Tugas

Modul 1 – System Call dan Instrumentasi Kernel:
Menambahkan dua system call baru pada sistem operasi xv6, yaitu:

1. getpinfo() untuk melihat informasi proses yang aktif dalam sistem
2. getreadcount() untuk menghitung jumlah total pemanggilan system call read() sejak sistem boot

Tugas 1 bertujuan untuk memahami cara kerja system call dalam kernel dan bagaimana menambahkan fungsionalitas baru ke dalam sistem operasi xv6.

---

## 🛠️ Rincian Implementasi
1. Penambahan Struktur Data dan Counter
   
A. Modifikasi file proc.h
Menambahkan struktur pinfo untuk menyimpan informasi proses:

```c
#define MAX_PROC 64

struct pinfo {
   int pid[MAX_PROC];
   int mem[MAX_PROC];
   char name[MAX_PROC][16];
};
```

B. Modifikasi file sysproc.c
Menambahkan deklarasi counter dan include header:

```c
#include "spinlock.h"
extern struct {
   struct spinlock lock;
   struct proc proc[NPROC];
} ptable;
extern int readcount;  // Counter untuk read 
```
---

2. Registrasi System Call Baru
Modifikasi file syscall.h
Menambahkan nomor system call baru:

```c
#define SYS_getpinfo     22
#define SYS_getreadcount 23
```
---

3. Modifikasi file syscall.c
Mendaftarkan fungsi system call:

```c
extern int sys_getpinfo(void);
extern int sys_getreadcount(void);

static int (*syscalls[])(void) = {
    // ... existing syscalls ...
    [SYS_getpinfo]      sys_getpinfo,
    [SYS_getreadcount]  sys_getreadcount,
};
```
---

4. Interface User-Level
   
A. Modifikasi file user.h
Menambahkan prototype fungsi:

```c
struct pinfo; //forward declaration
int getpinfo(struct pinfo*);
int getreadcount(void);
```

B. Modifikasi file usys.S
Menambahkan wrapper assembly:

```c
SYSCALL(getpinfo)
SYSCALL(getreadcount)
```
---

5. Implementasi Fungsi Kernel
Modifikasi file sysproc.c
Mengimplementasikan kedua system call:

```c
int
sys_getreadcount(void) {
   return readcount;
}

int
sys_getpinfo(void)
{
   struct pinfo *info;
   struct proc *p;
   int i = 0;
   
   if(argptr(0, (void*)&info, sizeof(*info)) < 0)
       return -1;
   
   acquire(&ptable.lock);
   
   for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
       if(p->state != UNUSED) {
           info->pid[i] = p->pid;
           info->mem[i] = p->sz;
           strncpy(info->name[i], p->name, 16);
           i++;
       }
   }
   
   release(&ptable.lock);
   return 0;
}
```
---

6.  Modifikasi System Call read()
Modifikasi file sysfile.c
Menambahkan counter dan increment:
readcount++;                                      

```c
int
sys_read(void)
{
    struct file *f;
    int n;
    char *p;

    readcount++;  // Increment counter setiap kali read() dipanggil
    
    if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
        return -1;
    return fileread(f, p, n);
}
```
---

7. Program Penguji
   
A. File ptest.c (untuk menguji getpinfo)

```c
#include "types.h"
#include "stat.h"
#include "user.h"

struct pinfo {
   int pid[64];
   int mem[64];
   char name[64][16];
};

int main() {
   struct pinfo p;
   getpinfo(&p);
   
   printf(1, "PID\tMEM\tNAME\n");
   for(int i = 0; i < 64 && p.pid[i]; i++) {
       printf(1, "%d\t%d\t%s\n", p.pid[i], p.mem[i], p.name[i]);
   }
   exit();
}
```

B. File rtest.c (untuk menguji getreadcount)

```c
#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
   char buf[10];
   printf(1, "Read Count Sebelum: %d\n", getreadcount());
   read(0, buf, 5); // baca stdin
   printf(1, "Read Count Setelah: %d\n", getreadcount());
   exit();
}
```
C. Modifikasi Makefile
Menambahkan program uji ke daftar user programs:

```c
UPROGS=\
    # ... existing programs ...
    _ptest\
    _rtest\
```
---

## ✅ Uji Fungsionalitas

Program uji yang digunakan:

1. ptest: untuk menguji system call getpinfo() - menampilkan informasi proses aktif
2. rtest: untuk menguji system call getreadcount() - menampilkan jumlah pemanggilan read()

---

## 📷 Hasil Uji
Berdasarkan screenshot hasil testing, kedua program uji berhasil dijalankan:

$ ptest

```c
PID     MEM     NAME
1       12288   init
2       16384   sh
3       12288   ptest
```

$ rtest  

```c
Read Count Sebelum: 12
Read Count Setelah: 13
```
Hasil testing menunjukkan bahwa:

System call getpinfo() berhasil mengambil dan menampilkan informasi proses
System call getreadcount() berhasil menghitung dan menampilkan jumlah pemanggilan read()
Kedua system call terintegrasi dengan baik ke dalam kernel xv6

---

## ⚠️ Kendala yang Dihadapi

1. Error pada sysfile.c
Masalah: Variabel readcount tidak dideklarasikan
Solusi: Menambahkan int readcount = 0; di sysfile.c
2. Error pada sysproc.c
Masalah: Missing header file untuk spinlock
Solusi: Menambahkan #include "spinlock.h" di sysproc.c
3. Build Process
Masalah: Error kompilasi saat build pertama kali
Solusi: Melakukan perbaikan dependency dan deklarasi variabel, kemudian menjalankan make clean dan make qemu-nox

---

## 📚 Referensi

Tuliskan sumber referensi yang Anda gunakan, misalnya:

* Buku xv6 MIT: [https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
* Repositori xv6-public: [https://github.com/mit-pdos/xv6-public](https://github.com/mit-pdos/xv6-public)
* Stack Overflow, GitHub Issues, diskusi praktikum

---

