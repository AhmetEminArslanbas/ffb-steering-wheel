<div align="center">

# 🏎️ Force-Feedback Direksiyon Seti

**Arduino ile kendi yaptığım, oyunlarda gerçek direksiyon gibi tanınan ve kuvvet geri bildirimi (FFB) veren direksiyon + pedal seti ve Unity ile yazılmış ayar uygulaması**

![Arduino](https://img.shields.io/badge/Arduino-Leonardo%20(ATmega32U4)-00979D?logo=arduino&logoColor=white)
![USB HID](https://img.shields.io/badge/USB-HID%20PID%20(FFB)-blue)
![Unity](https://img.shields.io/badge/Unity-2021.3-000000?logo=unity&logoColor=white)
![C#](https://img.shields.io/badge/C%23-239120?logo=csharp&logoColor=white)

<img src="docs/images/app.png" height="420" alt="Direksiyon ayar uygulaması">

</div>

## Proje hakkında
Piyasadaki FFB direksiyonlar pahalı olduğu için kendi direksiyon setimi tasarladım. Arduino Leonardo, USB üzerinden bilgisayara **HID oyun kumandası** olarak tanıtılıyor. Bu sayede Windows ve yarış oyunları (ör. Euro Truck Simulator 2) cihazı ek sürücü olmadan direksiyon olarak görüyor.

Oyunun gönderdiği kuvvet geri bildirimi komutları (USB HID PID) çözülüyor ve H-köprü üzerinden DC motora PWM olarak uygulanıyor. Böylece virajda ya da çarpışmada direksiyon gerçekten direnç gösteriyor.

## Özellikler
- **900° direksiyon:** Potansiyometre ile mutlak açı okunur, ayarlanabilir merkez ofseti vardır.
- **Kuvvet geri bildirimi:**
  - `Joystick.getForce()` ile oyundan gelen kuvvet okunur.
  - Ölü bölge filtresi uygulanır, ardından çift yönlü PWM motor sürüşü yapılır.
  - Yay (spring) efekti parametreleri gerçek zamanlı güncellenir.
- **Mekanik sınır koruması:** Direksiyon açı sınırını aşınca motor, direksiyonu güvenli aralığa geri iten bir kuvvet uygular.
- **3 pedal:** Gaz, fren ve debriyaj için ayrı eksenler ve her biri için kalibre edilmiş aralık.
- **8 buton:** 2×4 buton matrisi taraması (pull-up girişli).
- **1 kHz USB raporlama:** TIMER3 kesmesi (CTC modu) ile FFB raporları düzenli olarak işlenir.
- **Ayar uygulaması (Unity)**
  - Direksiyon açısını ve pedal doluluklarını canlı gösterir.
  - Maksimum direksiyon derecesini ayarlamayı sağlar.
  - Seri portları tarayıp `WHO_ARE_YOU` el sıkışmasıyla doğru Arduino'yu **otomatik bulur**.

## Mimari
```mermaid
flowchart LR
    P[Potansiyometre<br>direksiyon açısı] --> L
    PD[Gaz / Fren / Debriyaj<br>pedalları] --> L
    BM[2×4 buton matrisi] --> L
    L[Arduino Leonardo<br>USB HID + PID] <-- "USB HID<br>eksenler ↔ FFB kuvveti" --> PC[Oyun<br>ETS2 vb.]
    L -- PWM --> H[H-köprü] --> M[DC motor<br>dişli aktarma]
    L <-- "USB seri" --> APP[Unity ayar<br>uygulaması]
```

## Nasıl denenir
- [Releases](../../releases) sayfasından ayar uygulamasını (`SteeringWheelSettings-Windows.zip`) indirip çalıştırabilirsin. Uygulama açılırken COM portlarını taradığı için ilk açılış birkaç saniye sürebilir.
- Herhangi bir oyun kumandası takılıysa direksiyon ve pedal göstergeleri o kumandanın eksenleriyle de hareket eder.
- Donanımı kurmak için [`showcase/firmware/ffbDireksiyon`](showcase/firmware/ffbDireksiyon) klasöründeki kodu bir Arduino Leonardo'ya ya da Pro Micro'ya [ArduinoJoystickWithFFBLibrary](https://github.com/YukMingLaw/ArduinoJoystickWithFFBLibrary) ile yükle.

## Kod örnekleri
- [`firmware/ffbDireksiyon/ffbDireksiyon.ino`](showcase/firmware/ffbDireksiyon/ffbDireksiyon.ino): HID tanımı, FFB kuvvet → PWM dönüşümü, sınır koruması, buton matrisi.
- [`unity/arduinoConnect.cs`](showcase/unity/arduinoConnect.cs): otomatik ve manuel seri port bağlantısı.
- [`unity/joyManager.cs`](showcase/unity/joyManager.cs): eksenlerin arayüze yansıtılması ve derece ayarı.

## Neler öğrendim
- USB HID ve PID (Physical Interface Device) raporlarının mantığı.
- AVR zamanlayıcı kesmeleri (TIMER3, CTC modu) ile düzenli görev çalıştırma.
- Motor sürüşünde ölü bölge, doyma ve güvenlik sınırları.

---
<sub>© Ahmet Emin Arslanbaş — Tüm hakları saklıdır. Kod inceleme amaçlı paylaşılmıştır.</sub>
