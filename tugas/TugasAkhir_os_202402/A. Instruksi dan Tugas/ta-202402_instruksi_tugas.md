# **Tugas Akhir Praktikum Sistem Operasi: Modifikasi Kernel xv6**

# **A. Deskripsi Umum**

Tugas Akhir ini dirancang untuk memberikan pengalaman praktis dalam memodifikasi kernel sistem operasi. Anda akan bekerja dengan **xv6**, sebuah sistem operasi pengajaran modern yang dikembangkan oleh MIT sebagai re-implementasi dari Unix Versi 6 pada arsitektur x86 dan RISC-V. Melalui tugas ini, Anda diharapkan mampu mengaplikasikan konsep-konsep teoretis yang telah dipelajari di kelas ke dalam sebuah sistem operasi yang fungsional.

Anda akan memilih salah satu dari tiga modul yang ditawarkan, masing-masing dengan tingkat kesulitan yang berbeda, untuk diimplementasikan di atas *source code* asli xv6.

# **B. Tujuan Pembelajaran**

Setelah menyelesaikan tugas ini, mahasiswa diharapkan mampu:

 1.  **Memahami Struktur Kernel**: Mendeskripsikan dan memodifikasi komponen-komponen inti dari kernel sistem operasi xv6.
 2.  **Manajemen Proses & System Call**: Mengimplementasikan *system call* baru dan memahami alur eksekusinya dari *user space* ke *kernel space*.
 3.  **Penjadwalan CPU**: Mengimplementasikan dan menganalisis algoritma penjadwalan CPU alternatif.
 4.  **Manajemen Memori**: Memodifikasi cara sistem operasi mengelola memori untuk proses.
 5.  **Praktik Rekayasa Perangkat Lunak**: Menggunakan sistem kontrol versi (Git/GitHub) dan alat pengembangan standar (Docker, QEMU, GDB) secara efektif dalam proyek pengembangan sistem.

-----

# **C. Lingkungan Pengerjaan dan Persiapan**

Seluruh proses kompilasi, eksekusi, dan *debugging* xv6 akan dilakukan di dalam lingkungan yang terisolasi dan terstandarisasi menggunakan **Docker** dan **QEMU**.

-----

# **D. Mekanisme Pengumpulan dan Penyerahan**

### Pengumpulan tugas sepenuhnya dilakukan melalui **GitHub**.

1. **Fork Repositori**: Lakukan *fork* dari repositori utama yang telah disediakan: `https://github.com/mhbahara/os-202402`.
2. **Commit Berkala**: Lakukan `commit` untuk setiap perubahan signifikan yang Anda buat. Gunakan pesan *commit* yang jelas dan deskriptif (contoh: `feat: add getpinfo syscall skeleton`, `fix: resolve kernel panic in scheduler`).
3. **Struktur Repositori Akhir**: Pastikan repositori *fork* Anda yang akan dinilai memiliki struktur sebagai berikut:
      * Seluruh *source code* xv6 yang telah Anda modifikasi.
      * Sebuah file `LAPORAN.md` di direktori utama.
      * Sebuah direktori `/screenshots` yang berisi bukti gambar dari fungsionalitas yang telah Anda implementasikan.
4. **Push Final**: Lakukan `push` untuk semua perubahan Anda ke repositori *fork* Anda sebelum tenggat waktu yang ditentukan. Tautan ke repositori Anda adalah bukti pengumpulan.

5. **Format Laporan (`LAPORAN.md`)**:
  Laporan berisi bagian-bagian berikut:
    1.  **Nama dan NIM**: Identitas Anda.
    2.  **Deskripsi Implementasi**: Jelaskan secara ringkas namun teknis perubahan yang Anda lakukan.
      * File mana saja yang Anda modifikasi dan mengapa.
      * Struktur data baru atau modifikasi struktur data yang ada.
      * Logika utama dari implementasi Anda (misalnya, bagaimana algoritma penjadwalan Anda bekerja atau bagaimana Anda menangani *page fault*).
    3.  **Cara Pengujian**: Jelaskan bagaimana Anda menguji fungsionalitas yang telah dibuat dan bagaimana cara menjalankan program pengujian Anda.
    4.  **Analisis Singkat**: Jelaskan hasil dari implementasi Anda. Apakah sesuai ekspektasi? Apa tantangan yang dihadapi?

-----

# **E. Detail Soal**

## **1. System Call dan Instrumentasi Kernel (Tingkat Kesulitan: Mudah)**

### **Deskripsi**

Modul ini berfokus pada penambahan *system call* untuk mendapatkan informasi dari kernel dan melakukan instrumentasi dasar untuk memonitor aktivitas sistem.

### **Tugas yang Harus Dilakukan**
 1. **Implementasikan `getpinfo()`**: Buat *system call* `int getpinfo(struct pinfo *ptable)` yang mengisi sebuah array dengan informasi semua proses aktif (PID, ukuran memori, nama).

 2. **Implementasikan `getReadCount()`**: Buat *system call* `int getReadCount(void)` yang mengembalikan total berapa kali *system call* `read()` telah dieksekusi sejak sistem boot.
    * **Petunjuk**: Buat sebuah *counter* global di kernel. Gunakan *spinlock* untuk melindungi *counter* ini dari *race condition* saat di-increment di dalam `sys_read()`.
 3. Buat program *user-level* untuk menguji kedua *system call* tersebut.

### **Output yang Diharapkan**
* Screenshot output program `ps.c` (dari `getpinfo`).
* Screenshot output program yang memanggil `getReadCount()` beberapa kali setelah menjalankan perintah `cat README`.

----

## **2. Penjadwalan CPU Lanjutan (Tingkat Kesulitan: Sedang)**

### **Deskripsi**
Penjadwal *default* di xv6 menggunakan algoritma *Round Robin*. Pada modul ini, Anda akan menggantinya dengan algoritma penjadwalan berbasis prioritas.

### **Tugas yang Harus Dilakukan**
1.  Tambahkan sebuah atribut `priority` pada `struct proc`. Berikan nilai prioritas *default* (misalnya, 60) untuk setiap proses baru.
2.  Buat *system call* baru, `int set_priority(int priority)`, yang memungkinkan sebuah proses untuk mengubah prioritasnya sendiri. Validasi nilai prioritas (misal 0-100).
3.  Modifikasi fungsi `scheduler()` untuk mengimplementasikan *Priority Scheduling* (Non-Preemptive): selalu pilih proses `RUNNABLE` dengan nilai prioritas terendah (tertinggi) untuk dijalankan.
4.  Buat program pengujian yang menunjukkan bahwa proses berprioritas tinggi mendapatkan lebih banyak waktu CPU atau selesai lebih cepat.

### **Output yang Diharapkan**
* Screenshot program pengujian dan outputnya yang membuktikan efektivitas penjadwalan prioritas.

----

## **3. Manajemen Memori Tingkat Lanjut (Tingkat Kesulitan: Sulit)**

### **Deskripsi**

Modul ini memberikan tantangan mendalam pada subsistem manajemen memori.

 1. **Copy-on-Write (CoW) Fork:** Modifikasi `fork()` agar tidak lagi menyalin seluruh ruang memori. Sebaliknya, proses anak dan induk awalnya berbagi halaman memori yang ditandai *read-only*. Salinan fisik baru dibuat hanya ketika ada upaya penulisan (ditangani melalui *page fault*). Anda perlu mengimplementasikan *reference counting* untuk halaman memori fisik.

 2. **Memori Bersama (Shared Memory)** Implementasikan mekanisme *shared memory* ala System V.`void* shmget(int key)`: Mendapatkan atau membuat satu halaman memori yang dapat dibagikan, diidentifikasi oleh sebuah `key`.

 3. **Memori Bersama (Shared Memory):** Implementasikan mekanisme *shared memory* ala System V. `int shmrelease(int key)`: Melepaskan memori bersama dari ruang alamat proses. Gunakan *reference counting* untuk menentukan kapan memori fisik tersebut benar-benar harus dibebaskan.

### **Output yang Diharapkan**
* Screenshot program pengujian dan `LAPORAN.md` yang menjelaskan desain dan implementasi secara detail, termasuk penanganan *edge cases*.
----

## **4: Subsistem Kernel Alternatif (Tingkat Kesulitan: Sedang)**

### **Deskripsi**
Modul ini memungkinkan Anda untuk menjelajahi area kernel selain manajemen proses dan memori.

### **Tugas yang Harus Dilakukan.**
* ####  **Perizinan Sistem Berkas (File Permissions):** Implementasikan sistem perizinan dasar ala Unix.
  1. Modifikasi `struct inode` untuk menyimpan bit perizinan (misalnya, mode baca/tulis).
  2. Implementasikan *system call* `int chmod(char* path, int mode)` untuk mengubah bit perizinan pada sebuah file.
  3.  Pastikan *system call* seperti `open()`, `read()`, dan `write()` mematuhi perizinan ini.

* #### **Driver Perangkat I/O Semu** : Implementasikan sebuah *character device driver* untuk perangkat semu `/dev/random`.
  1.  Daftarkan perangkat baru pada tabel `devsw` kernel.
  2.  Implementasikan fungsi *read* untuk perangkat ini. Saat dibaca, perangkat harus menghasilkan urutan angka *pseudo-acak*.
  3.  Buat program *user-level* yang dapat membaca dari `/dev/random` dan menampilkan hasilnya.

### **Output yang Diharapkan**
* Screenshot yang jelas menunjukkan fungsionalitas yang telah diimplementasikan (misalnya, gagal menulis ke file *read-only*, atau output dari `cat /dev/random`).
-----

## **5: Audit dan Keamanan Sistem (Tingkat Kesulitan: Sedang)**

### **Deskripsi**
Modul ini berfokus pada penambahan fitur keamanan dan monitoring dengan mengimplementasikan sistem audit untuk *system call*.

### **Tugas yang Harus Dilakukan**
  1. Modifikasi kernel untuk mencatat setiap eksekusi *system call* ke dalam sebuah log internal kernel (misalnya, *circular buffer*). Informasi yang dicatat minimal: PID proses, nomor *syscall*, dan stempel waktu (`ticks`).
  2. Implementasikan *system call* baru `int get_audit_log(char* buffer, int size)`.
  3. **Aspek Keamanan**: Pastikan `get_audit_log` hanya dapat berhasil dieksekusi oleh proses dengan hak istimewa (misalnya, hanya proses `init` dengan PID 1). Proses lain yang mencoba memanggilnya harus menerima *error*.
  4. Buat program penguji yang dapat memanggil `get_audit_log` (jika dijalankan sebagai proses pertama) dan mencetak isinya.

### **Output yang Diharapkan**
* Screenshot output program audit Anda.
* Screenshot yang menunjukkan proses non-privilege gagal saat memanggil `get_audit_log`.


# **F. Referensi Tambahan**

  * **xv6, a simple Unix-like teaching operating system**: Buku dokumentasi resmi xv6 yang menjelaskan arsitekturnya secara detail. [buku xv6](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
  * **Repositori GitHub xv6 MIT**: Sumber kode asli xv6. [GitHub xv6](https://github.com/mit-pdos/xv6-public)
  * **Dokumentasi QEMU**: Berguna untuk memahami opsi-opsi emulator.
  * **Dokumentasi GDB**: Sangat direkomendasikan untuk melakukan *debugging* pada kernel.
