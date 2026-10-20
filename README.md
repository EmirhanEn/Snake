# Meyve Bahçesi

C++17 ve raylib 5.5 ile hazırlanmış, şirin çizgi film görselli masaüstü yılan oyunu. İllüstrasyonlar, karakterler, tahta ve arayüz kaplamaları PNG; müzik ve efektler WAV; yazı tipleri TTF dosyalarından yüklenir.

## Oyunu aç

`dist/MeyveBahcesi.exe` dosyasına çift tıkla. `dist/assets` klasörü ve DLL dosyaları uygulamanın yanında kalmalıdır. Başka bir bilgisayara taşırken `dist` klasörünün tamamını kopyala. Dosyalar EXE'nin bulunduğu klasörden okunduğu için oyunu farklı bir çalışma dizininden de açabilirsin.

## Görsel ve ses dosyaları

- `assets/images`: 7 PNG dosyası; maskot, 16 hücreli karakter/nesne sayfası, arka plan, çimenli tahta, arayüz ve kesintisiz yılan derisi kaplaması.
- `assets/audio`: 34 saniyelik özgün stereo müzik ve 5 efekt; toplam 6 WAV dosyası.
- `assets/fonts`: Balsamiq Sans Regular/Bold ve açık font lisansı.

Kaynak dosyaları `assets` içinde düzenlenir. Derleme bunları `dist/assets` içine kopyalar. Hazır oyundaki dosyaları değiştirmek için doğrudan `dist/assets` içindeki dosyaları da aynı adlarla değiştirebilirsin. Atlas düzeni ve dosya açıklamaları `assets/README.md` içindedir.

Çizimler Imagegen ile üretilmiştir. Kaynaklar, lisanslar ve kullanılan görsel istemleri `assets/PROVENANCE.md` içinde açıklanır. Metin, yerleşim, hareket, parçacıklar ve basit arayüz vurguları C++ tarafından yönetilir.

## Modlar ve kontroller

- **Klasik:** duvarlara ve kendi gövdene çarpmadan elma topla.
- **Sonsuz:** kenardan çıkınca karşı taraftan devam et; gövdene dikkat et.
- **Çit Labirenti:** bahçedeki çitlere çarpmadan elmalara ulaş.
- **Yön tuşları / WASD:** hareket.
- **Space / P / Esc:** duraklat veya devam et.
- **F11:** tam ekran.
- **Tab / Enter:** menülerde gezin ve seç. Ana menüde **1 / 2 / 3** ile mod seç.

Ayarlar ekranından hız, müzik ve efekt seviyeleri ile sakin efektler seçeneğini değiştirebilirsin. Rekorlar mod ve zorluk seviyesine göre ayrı tutulur. Ayarlar ve rekorlar `%LOCALAPPDATA%/MeyveBahcesi/preferences.txt` dosyasına kaydedilir.

## Kaynaktan derleme

Visual Studio 2026'nın **C++ ile masaüstü geliştirme** iş yükü ve CMake araçları gereklidir. Proje klasöründe:

```powershell
./build.ps1
```

Derlemek ve kural/ses testlerini çalıştırmak için:

```powershell
./build.ps1 -Test
```

Derleme `build` klasörünü yeniden oluşturur; çalıştırılabilir paketi ve medya dosyalarını `dist` içine yazar. Testler oyun kurallarını, dosya yollarını, PNG saydamlığını, karakter sayfasının hücre sınırlarını, WAV dosyalarını ve Türkçe font karakterlerini kontrol eder. Raylib'in gerekli başlık ve statik kütüphane dosyaları `third_party` içinde bulunur; yeniden indirmek gerekmez. Lisansı `dist/RAYLIB-LICENSE.txt` dosyasındadır.

Geliştirme sırasında ekran önizlemeleri için `dist/MeyveBahcesi.exe --capture` kullanılabilir. Çıktılar komutun çalıştırıldığı dizindeki `artifacts` klasöründe oluşturulur.

Özgün WAV dosyaları ve yardımcı PNG kaplamaları bir defaya mahsus C++ yazarlık aracıyla dışa aktarılmıştır. Bu araç oyunun içine bağlanmaz; oyun açılırken ses veya kaplama üretmez. Bunları yeniden oluşturmak istersen derlemeden sonra `build/Release/asset_export.exe assets` çalıştırabilirsin. Bu komut 6 WAV ve 4 yardımcı PNG dosyasının üzerine yazar; Imagegen illüstrasyonlarını değiştirmez. Sonrasında `./build.ps1` ile güncel dosyaları oyun paketine kopyala.
