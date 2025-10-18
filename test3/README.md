# DOKUMENTASI TASK 3 SAPA MENYAPA ESP NOW
---
## Tujuan
membuat program menggunakan ESP-now communication sehingga ESP 32 dapat menerima perintah dari serial dan dari ESP lain
---
## TODO 1
'''
const int mac_index_ku = 5;
'''
mengganti index dengan mac index sesuai dengan apa yang sudah tertera di 'const char *mac_names[MAC_ADDRESS_TOTAL]'
---
## TODO 2
fungsi ini membaca header dalam bentuk byte yang dikirimkan dengan format paket '0xFF 0xFF 0x00'
'''
static uint8_t buffer[BUFFER_SIZE];
'''
menyimpan buffer untuk menampung data dari serial dengan nilai yang sudah didefinisikan '#define BUFFER_SIZE 250'
'''
 while (Serial.available()) {
        if (Serial.read() == 0xFF) { 
            if (Serial.available() && Serial.read() == 0xFF) { 
                if (Serial.available() && Serial.read() == 0x00) { 
'''
mengecek apakah benar ada data yang masuk di port serial, jika ada maka membaca satu byte data jika benar '0XFF' maka akan dilanjutkan pengecekkan sampai '0xFF 0XFF 0X00'
'''
if (Serial.available()) {
    uint8_t data_len = Serial.read();
         if (data_len > 0 && data_len < BUFFER_SIZE && Serial.available() >= data_len) {
'''
Jika header yang masuk sesuai maka program akan mengecek lagi apakah ada data yang masuk setelah header, Jika ada maka panjang data akan disimpan di 'data_len' lalu program hanya akan lanjut ketika panjang data tidak nol dan masih masuk kedalam batas buffer serta jumlah data setelah 'panjang data' sesuai dengan 'data_len'
'''
 size_t baca_lanjut = Serial.readBytes(buffer, data_len);
    if (baca_lanjut == data_len) {
        if (callback) {
             callback(buffer, (int)data_len);
'''
Program membaca data sebanyak 'data_len' byte lalu disimpan ke dalam buffer setelah itu, lalu program memanggil fungsi callback yang akan memanggil fungsi 'process_perintah'
---
## TODO 3



