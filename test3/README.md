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


