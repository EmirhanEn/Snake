# Meyve Bahçesi medya dosyaları

Bu klasördeki dosyalar oyunun gerçek çalışma kaynaklarıdır. C++ uygulaması PNG ve TTF dosyalarını belleğe yükler; müziği WAV dosyasından okur. Dosyalar EXE içine gömülü değildir ve eksik olduklarında uygulama eksik dosyayı bildirir.

| Dosya | Kullanım |
| --- | --- |
| `images/mascot.png` | Menü, mola ve ayarlar ekranlarındaki saydam çizgi film yılanı |
| `images/sprites.png` | Saydam 4×4 karakter ve bahçe nesnesi sayfası |
| `images/backdrop.png` | Suluboya dokulu, çiçekli krem arka plan |
| `images/grass-board.png` | 24×18 hücreli çimenli oyun tahtası |
| `images/snake-skin.png` | Gövde boyunca kesintisiz akan, tekrarlanabilir deri kaplaması |
| `images/ui-card.png` | Kenarları korunarak ölçeklenen panel |
| `images/ui-button.png` | Durumuna göre renklendirilen düğme kaplaması |
| `audio/orchard-loop.wav` | Yaklaşık 34,29 saniyelik döngülü stereo müzik |
| `audio/eat.wav` | Elma toplama |
| `audio/click.wav` | Menü seçimi |
| `audio/countdown.wav` | Geri sayım |
| `audio/lose.wav` | Oyun sonu |
| `audio/win.wav` | Kazanma |
| `fonts/BalsamiqSans-Regular.ttf` | Gövde yazıları |
| `fonts/BalsamiqSans-Bold.ttf` | Başlıklar ve vurgular |

## Sprite sayfası

Dört eşit sütun, dört eşit satır; sıra soldan sağa okunur. Her hücrenin çevresi saydam kalmalıdır. Başların varsayılan yönü sağdır; oyun bunları hareket yönüne döndürür.

| Satır | 1 | 2 | 3 | 4 |
| --- | --- | --- | --- | --- |
| 1 | Normal baş | Göz kırpma | Elma yeme | Şaşkın baş |
| 2 | Gövde | Köşe | Kuyruk | Elma |
| 3 | Çalı/çit | Beyaz çiçek | Pembe çiçek | Yaprak |
| 4 | Bulut | Güneş | Yıldız | Çimen |

Sayfadaki gövde, köşe ve kuyruk çizimleri tasarım setinin parçasıdır. Oyun içi gövde, yumuşatılmış tek bir şerit üzerinde `snake-skin.png` kaplaması kullanılarak çizilir. Kaplama her hücrede başa dönmez; deri ve karın çizgisi köşelerden kesintisiz geçer. Son 38 piksel uca doğru incelen kuyruktur. Baş ifadeleri ve bahçe nesneleri doğrudan sprite sayfasından alınır.

## Değiştirme

Aynı dosya adını, biçimini ve sprite hücre düzenini koru. PNG saydamlığını kaybetme; tahta oranı 4:3, arka plan oranı 16:10 olmalıdır. WAV dosyaları standart PCM olmalıdır. Kaynakta değişiklikten sonra `./build.ps1` ile dağıtım kopyasını güncelle. Oyun bütün dosyaları ilk açılışta yüklediği için değişiklikleri görmek üzere yeniden aç.
