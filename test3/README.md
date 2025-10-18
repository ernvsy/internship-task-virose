# DOKUMENTASI TASK 3 SAPA MENYAPA ESP NOW
---
## Tujuan
membuat program menggunakan ESP-now communication sehingga ESP 32 dapat menerima perintah dari serial dan dari ESP lain

---
## TODO 1
```
const int mac_index_ku = 5;
```
mengganti index dengan mac index sesuai dengan apa yang sudah tertera di  `const char *mac_names[MAC_ADDRESS_TOTAL]`

---
## TODO 2
fungsi ini membaca header dalam bentuk byte yang dikirimkan dengan format paket `0xFF 0xFF 0x00`
```
static uint8_t buffer[BUFFER_SIZE];
```
menyimpan buffer untuk menampung data dari serial dengan nilai yang sudah didefinisikan '#define BUFFER_SIZE 250'
```
 while (Serial.available()) {
        if (Serial.read() == 0xFF) { 
            if (Serial.available() && Serial.read() == 0xFF) { 
                if (Serial.available() && Serial.read() == 0x00) { 
```
mengecek apakah benar ada data yang masuk di port serial, jika ada maka membaca satu byte data jika benar '0XFF' maka akan dilanjutkan pengecekkan sampai `0xFF 0XFF 0X00`
```
if (Serial.available()) {
    uint8_t data_len = Serial.read();
         if (data_len > 0 && data_len < BUFFER_SIZE && Serial.available() >= data_len) {
```
Jika header yang masuk sesuai maka program akan mengecek lagi apakah ada data yang masuk setelah header, Jika ada maka panjang data akan disimpan di `data_len` lalu program hanya akan lanjut ketika panjang data tidak nol dan masih masuk kedalam batas buffer serta jumlah data setelah `panjang data` sesuai dengan `data_len`
```
 size_t baca_lanjut = Serial.readBytes(buffer, data_len);
    if (baca_lanjut == data_len) {
        if (callback) {
             callback(buffer, (int)data_len);
```
Program membaca data sebanyak `data_len` byte lalu disimpan ke dalam buffer setelah itu, lalu program memanggil fungsi callback yang akan memanggil fungsi `process_perintah`

---
## TODO 3
```
uint8_t kren = data[0];
```
mengambil byte pertama daei data dan menyimpannya di variabel `kren`

### Perintah melalui Serial

```
if (index_mac_address_asal == -1) {
```
Jika data yang diterima berasal dari Serial mac adress asal bernilai -1 sesuai yang tercantum di main.h `void process_perintah(const uint8_t *data, int len, int index_mac_address_asal = -1);`

```
uint8_t tujuan_index = data[1];
String nama_tujuan = mac_index_to_names(tujuan_index);
String namaku = mac_index_to_names(mac_index_ku);
```
`data[1]` adalah byte kedua yang berisi index tujuan mana yang ingin dikirimkan pesan

lalu fungsi `mac_index_to_names(tujuan_index)` mengubah index menjadi nama ESP

lalu `namaku` berisi nama ESP pengirim berdasarkan `mac_index_ku` yang sudah tertera di TODO 1

```
uint8_t simpen[BUFFER_SIZE];
        size_t simpen_len=0;
```
untuk menyimpan data byte ke dalam array 

#### HALO dari Serial

```
if (kren==HALO){
            String msg= "Halo "+ nama_tujuan + "Aku " + namaku;
            simpen[0]= HALO;
            simpen_len = 1+ msg.length()+ 1;
```

Jika perintah yang diterima pertama adalah `HALO`maka string msg akan membuat sebuah pesan balasan dengan format `"Halo "+ nama_tujuan + "Aku " + namaku` 

Byte pertama dari `simpen` diisi dengan kode perintah HALO 

lalu menghitung panjang total data yang akan dikirim `1+ msg.length()+ 1;`
ditambah 1 diawal karena simpen 0 sudah diisi untuk kode HALO dan ditambah 1 di akhir untuk karakter akhir `\0` penanda akhir stirng

```
 memcpy(simpen +1 , msg.c_str(), msg.length()+1);

```
menyalin seluruh isi teks `msg` ke dalam array `simpen` agar data yang dikirim dalam bentuk byte

```
 esp_now_send(mac_addresses[tujuan_index], simpen, simpen_len);
            Serial.printf("Halo dikirim ke %s: %s\n", nama_tujuan.c_str(), msg.c_str());
```
mengirim data sebanyak `simpen_len` byte dari array `simpen` ke dalam ESP dengan alamat tujuan `mac_addresses[tujuan_index]`

lalu memberikan informasi ke layar serial jika pesan berhasil dikirim 

#### CEK dari Serial
```
 else if(kren==CEK){
     String msg= nama_tujuan+ "Aku "+ namaku + " apa kamu disana?";
     simpen[0]= CEK;
```
Jika perintah yang diterima adalah `CEK` maka msg akan membuat pesan balasan dengan format `String msg= nama_tujuan+ "Aku "+ namaku + " apa kamu disana?";` lalu byte pertama yang disimpan di `simpen` adalah `CEK` 
untuk penjelasan program selanjutnya sama persis dengan yang tertulis di `HALO`

#### JAWAB dari Serial

```
 else if(kren==JAWAB){
            Serial.println("Hmm nggaada perintah Jawab dari serial");
        }
        else{
            Serial.println("Perintah anomali tidak dikenal");
        }
        return;
```
Jika kren menerima jawab maka serial akan menuliskan `Hmm nggaada perintah Jawab dari serial` dan jika kren tidak menerima salah satu dari `HALO,CEK, dan JAWAB` makan serial akan menuliskan `Perintah anomali tidak dikenal`

### Perintah melalui ESP NOW
```
String nama_pengirim= mac_index_to_names(index_mac_address_asal);
String namaku = mac_index_to_names(mac_index_ku);
uint8_t simpen[BUFFER_SIZE];
```
`nama_pengirim` mengambil nama pengirim yang tertera sesuai index di `mac_address_asal` ini menunjukkan darimana pesan berasal 
lalu `namaku` berisi nama ESP pengirim berdasarkan `mac_index_ku` yang sudah tertera di TODO 1
menyimpan array dengan nama `simpen` dan dengan ukuran yang tertera di `BUFFER_SIZE`

#### HALO dari ESP NOW
```
 if(kren==HALO){
    String msg= "Halo Juga "+ nama_pengirim + "Aku "+ namaku;
    simpen[0]= JAWAB;
```
Jika kren menerima perintah ` HALO` maka msg akan menuliskan pesan jawaban dengan format `"Halo Juga "+ nama_pengirim + "Aku "+ namaku;` dan `JAWAB` akan disimpan kedalam `simpen[0]` karena ini adalah pesan jawaban
untuk penjelasan program selanjutnya sama persis dengan yang tertulis di `HALO dari Serial`
```
 Serial.printf("Balas JAWAB ke %s\n", nama_pengirim.c_str());
```
Jika balasan sudah berhasil terkirim maka serial akan menampilkan `"Balas JAWAB ke %s\n", nama_pengirim.c_str()`

#### CEK dari ESP NOW

```
 else if (kren==CEK){
    String msg = "Iya aku "+ nama_pengirim + "Disini -" + namaku;
    simpen[0]=JAWAB;
```
Jika kren menerima perintah `CEK` maka msg akan menuliskan pesan jawaban dengan format `"Iya aku "+ nama_pengirim + "Disini -" + namaku;`dan `JAWAB` akan disimpan kedalam `simpen[0]` karena ini adalah pesan jawaban
untuk penjelasan program selanjutnya sama persis dengan yang tertulis di `HALO dari Serial

#### JAWAB dari ESP NOW
```
 else if(kren==JAWAB){
    if(len>1){
        size_t copylen= (size_t)min((int)(BUFFER_SIZE-1), len-1);

```
Jika kren menerima perintah `JAWAB` maka akan dicek apakah ada pesan selain byte pertama 

Jika ada `copylen` menghitung berapa banyak byte pesan yang mau di salin ke buffer msg `len-1` karena byte pertama adalah kode perintah `BUFFER_SIZE -1`agar tidak melebihi ukuran array dan fungsi `min()` memastikan kita menyalin data sepanjang `len-1` dengan batas `BUFFER_SIZE -1`

```
 char msg[BUFFER_SIZE];
        memcpy(msg, data +1 , copylen);
        msg[copylen]='\0';
        Serial.printf("Jawab dari %s: %s\n", nama_pengirim.c_str(), msg);

    }else{
        Serial.printf("Jawab tanpa pesan.\n", nama_pengirim.c_str());
    }
 } 
 ```
 membuat karakter array `char msg` untuk menyimpan pesan salinan

 lalu `memcpy` menyalin isi pesan ke `msg` 
 
 `msg[copylen]=\0` menambahkan karakter akhir string supaya `msg` bisa dibaca sebagai `char`

lalu serial akan menampilkan hasil balasan dengan format `("Jawab dari %s: %s\n", nama_pengirim.c_str(), msg);`  
bagian else jika `len<1` yang artinya tidak ada isi pesan maka akan menampilkan `Jawab tanpa pesan.\n", nama_pengirim.c_str()`









