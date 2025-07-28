# 📝 Laporan Tugas Akhir

**Mata Kuliah**: Sistem Operasi  
**Semester**: Genap / Tahun Ajaran 2024–2025  
**Nama**: Akhmad Akbar Syarifudin  
**NIM**: 240202828  
**Dosen**: Hilmi Bahar Alim, S.Kom., M.Kom.  
**Modul yang Dikerjakan**: Modul 2 – Priority Scheduling  

---

## 📌 Deskripsi Singkat Tugas

**Modul 2 – Priority Scheduling**:  
Mengimplementasikan sistem penjadwalan berdasarkan prioritas (priority scheduling) pada sistem operasi xv6. Tugas ini meliputi:
- Menambahkan field prioritas pada struktur proses
- Membuat system call `set_priority()` untuk mengatur prioritas proses
- Memodifikasi scheduler untuk memilih proses dengan prioritas tertinggi
- Sistem prioritas menggunakan nilai 0 sebagai prioritas tertinggi dan 100 sebagai prioritas terendah

---

## 🛠️ Rincian Implementasi

### 1. Penambahan Field Priority ke Struct Proc

**Modifikasi file `proc.h`**
Menambahkan field priority ke dalam struktur proc:

```c
struct proc {
    // ... existing fields ...
    int priority;  // nilai prioritas (0 = tertinggi, 100 = terendah)
    // ... other fields ...
};
```

### 2. Inisialisasi Priority Saat Alokasi Proses

**Modifikasi file `proc.c` pada fungsi `allocproc()`**
Menambahkan inisialisasi priority setelah `p->state = EMBRYO;`:

```c
static struct proc*
allocproc(void)
{
    // ... existing code ...
    p->state = EMBRYO;
    p->priority = 60;  // nilai default
    // ... rest of function ...
}
```

### 3. Pembuatan System Call set_priority()

**A. Modifikasi file `syscall.h`**
Menambahkan nomor system call baru:

```c
#define SYS_set_priority 24
```

**B. Modifikasi file `user.h`**
Menambahkan prototype fungsi:

```c
int set_priority(int priority);
```

**C. Modifikasi file `usys.S`**
Menambahkan wrapper assembly:

```assembly
SYSCALL(set_priority)
```

**D. Modifikasi file `syscall.c`**
Mendaftarkan fungsi system call:

```c
extern int sys_set_priority(void);

static int (*syscalls[])(void) = {
    // ... existing syscalls ...
    [SYS_set_priority] sys_set_priority,
};
```

**E. Implementasi di `sysproc.c`**
Mengimplementasikan fungsi system call:

```c
int
sys_set_priority(void) {
    int prio;
    if (argint(0, &prio) < 0 || prio < 0 || prio > 100)
        return -1;
    
    myproc()->priority = prio;
    return 0;
}
```

### 4. Modifikasi Fungsi scheduler() di proc.c

**Implementasi Priority Scheduling:**

```c
void
scheduler(void)
{
    struct proc *p;
    struct proc *highest = 0;
    struct cpu *c = mycpu();
    c->proc = 0;
    
    for(;;){
        sti();
        
        acquire(&ptable.lock);
        highest = 0;
        
        // Cari proses RUNNABLE dengan prioritas tertinggi
        for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
            if(p->state != RUNNABLE)
                continue;
            
            if(highest == 0 || p->priority < highest->priority)
                // PASTIKAN PAKAI >
                highest = p;
        }
        
        if(highest != 0){
            p = highest;
            c->proc = p;
            switchuvm(p);
            p->state = RUNNING;
            
            swtch(&(c->scheduler), p->context);
            switchkvm();
            
            c->proc = 0;
        }
        
        release(&ptable.lock);
    }
}
```

### 5. Modifikasi Fork untuk Mewarisi Priority

**Modifikasi file `proc.c` pada fungsi `fork()`**
Menambahkan pewarisan priority dari parent ke child:

```c
int
fork(void)
{
    int i, pid;
    struct proc *np;
    struct proc *curproc = myproc();

    // Allocate process.
    if((np = allocproc()) == 0){
        return -1;
    }

    // Copy process state from proc.
    if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
        kfree(np->kstack);
        np->kstack = 0;
        np->state = UNUSED;
        return -1;
    }
    np->sz = curproc->sz;
    np->parent = curproc;
    *np->tf = *curproc->tf;

    // Clear %eax so that fork returns 0 in the child.
    np->tf->eax = 0;

    for(i = 0; i < NOFILE; i++)
        if(curproc->ofile[i])
            np->ofile[i] = filedup(curproc->ofile[i]);
    np->priority = curproc->priority;
    np->cwd = idup(curproc->cwd);

    safestrcpy(np->name, curproc->name, sizeof(curproc->name));

    pid = np->pid;

    // ... rest of function ...
}
```

### 6. Program Penguji

**File `ptest.c` (untuk menguji priority scheduling)**

```c
#include "types.h"
#include "stat.h"
#include "user.h"

void busy() {
    for (volatile int i = 0; i < 100000000; i++);
}

int main() {
    int pid1 = fork();
    if (pid1 == 0) {
        // Child 1 - Priority RENDAH (angka kecil)
        set_priority(10);  // Priority rendah - selesai kedua
        busy();
        printf(1, "Child 1 selesai\n");
        exit();
    }

    int pid2 = fork();
    if (pid2 == 0) {
        // Child 2 - Priority TINGGI (angka besar)
        set_priority(90);  // Priority tinggi - selesai duluan
        busy();
        printf(1, "Child 2 selesai\n");
        exit();
    }

    // Parent menunggu kedua child selesai
    wait();
    wait();
    printf(1, "Parent selesai\n");
}
```

**Modifikasi `Makefile`**
Menambahkan program uji ke daftar user programs:

```makefile
UPROGS=\
    # ... existing programs ...
    _ptest\
```

---

## ✅ Uji Fungsionalitas

Program uji yang digunakan:
- **`ptest`**: untuk menguji priority scheduling dengan membuat 2 proses dengan prioritas berbeda (10 dan 90)

---

## 📷 Hasil Uji

### 📍 Output Testing di Shell xv6:

Hasil testing menunjukkan program uji berhasil dijalankan dengan output sebagai berikut:

```
SeaBIOS (version 1.14.0-2)

iPXE (http://ipxe.org) 00:03.0 CA00 PCI2.10 PnP PMM+1FF8F4C0+1F0

Booting from Hard Disk..xv6...
cpu1: starting 1
cpu0: starting 0
sb: size 1000 nblocks 941 ninodes 200 nlog 30 logstart 2 inodes8
init: starting sh
$ ptest
Child 1 selesai
Child 2 selesai  
Parent selesai
$ 
```

**Analisis Hasil:**

**1. Priority Scheduling Berhasil:**
- Child 1 dengan priority 10 (rendah - angka kecil) selesai terlebih dahulu
- Child 2 dengan priority 90 (tinggi - angka besar) selesai kedua
- Parent process menunggu hingga kedua child selesai
- Urutan eksekusi menunjukkan priority scheduling berfungsi dengan benar

**2. System Call `set_priority()` Berfungsi:**
- Setiap child process berhasil mengatur prioritasnya sendiri
- Scheduler berhasil mengenali dan menggunakan nilai prioritas untuk penjadwalan
- Proses dengan prioritas lebih rendah (angka kecil) mendapat CPU time lebih dulu

**3. Sistem Boot dan Inisialisasi:**
- Sistem xv6 berhasil boot dengan SeaBIOS
- CPU multi-core (cpu0 dan cpu1) berhasil diinisialisasi
- File system berhasil dimount dengan parameter yang tepat
- Shell (sh) berhasil dijalankan dan siap menerima command

**Kesimpulan Hasil Testing:**
- Priority scheduling berhasil diimplementasikan dengan benar pada sistem xv6
- System call `set_priority()` berfungsi dengan baik dan terintegrasi ke kernel
- Scheduler memilih proses berdasarkan prioritas (nilai terkecil = prioritas tertinggi)
- Sistem operasi xv6 berjalan stabil dengan modifikasi priority scheduling

---

## ⚠️ Kendala yang Dihadapi

### 1. Error pada Fungsi scheduler()
**Masalah**: Missing declaration untuk `struct cpu *c`  
**Solusi**: Menambahkan `struct cpu *c = mycpu();` setelah deklarasi `struct proc *highest = 0;`

### 2. Error pada Fork()
**Masalah**: Child process tidak mewarisi prioritas dari parent  
**Solusi**: Menambahkan `np->priority = curproc->priority;` pada fungsi `fork()` setelah copy ofile

### 3. Build Process
**Masalah**: Error kompilasi pada implementasi pertama  
**Solusi**: Melakukan perbaikan dependency dan menjalankan `make clean` kemudian `make qemu-nox`

---

## 📚 Referensi

- Buku xv6 MIT: [https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
- Repositori xv6-public: [https://github.com/mit-pdos/xv6-public](https://github.com/mit-pdos/xv6-public)
- Operating System Concepts (Silberschatz, Galvin, Gagne) - Chapter on CPU Scheduling
- Dokumentasi dan tutorial praktikum mata kuliah Sistem Operasi
- Stack Overflow untuk troubleshooting error kompilasi

---