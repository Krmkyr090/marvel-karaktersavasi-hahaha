// Süper Kahramanlar - Ultimate Single File
// Özellikler: isim/sayı/rastgele seçim, AI modu, 3 tur cooldown, stun, hava, özel güçler, heal, kritik, istatistikler
// Derle: g++ -std=c++17 arena.cpp -o arena

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <cctype>
using namespace std;

// --- Yardımcı fonksiyonlar ---
void bekle_ms(int ms){ this_thread::sleep_for(chrono::milliseconds(ms)); }
void noktaAnim(int n=3,int ms=250){ for(int i=0;i<n;i++){ cout<<'.'; cout.flush(); bekle_ms(ms);} cout<<endl; }

string trim(const string &s){
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
string toLowerNoSpace(const string &s){
    string t;
    for(char c: s) if(!isspace((unsigned char)c)) t += char(tolower((unsigned char)c));
    else continue;
    for(auto &ch : t) ch = char(tolower((unsigned char)ch));
    return t;
}

// Güvenli satır okuma
string safeLine(){
    string line;
    if(!std::getline(cin, line)) {
        // temizle ve tekrar dene
        cin.clear();
        std::getline(cin, line);
    }
    return line;
}

// Kullanıcıdan bir tamsayı almak (hata kontrolü)
int safeIntFromLine(const string &prompt){
    while(true){
        cout << prompt;
        string line = safeLine();
        line = trim(line);
        try {
            int x = stoi(line);
            return x;
        } catch(...) {
            cout << "❌ Geçersiz sayı, tekrar dene.\n";
        }
    }
}

// --- ASCII / efektler ---
void asciiIntro(){
    cout << "=====================================\n";
    cout << "     SÜPER KAHRAMANLAR ARENASI       \n";
    cout << "           ULTIMATE EDITION          \n";
    cout << "=====================================\n";
    bekle_ms(600);
    cout << "Previously on... Superhero Arena"; noktaAnim(3,300);
}

void efektSpiderman(){
    cout << R"(  .--.
 (    )   THWIP!
  '--'   )" << "\n";
    bekle_ms(500);
    cout << "🕸️ Spiderman: Ağ tuzağı aktif!" << endl;
    bekle_ms(400);
}
void efektBatman(){
    cout << R"(   /\ 
  /__\   BOOM!
 (|  |) )" << "\n";
    bekle_ms(500);
    cout << "💣 Batman: Batarang + bomba!" << endl;
    bekle_ms(400);
}
void efektIronMan(){
    cout << R"(   /-\
  |o o|  PEW PEW!
   \_/ )" << "\n";
    bekle_ms(500);
    cout << "⚡ Iron Man: Lazer & Roket!" << endl;
    bekle_ms(400);
}
void efektHulk(){
    cout << R"(  .----.
 /      \  RAAAGH!
 \      / )" << "\n";
    bekle_ms(500);
    cout << "💪 Hulk: SMASH!" << endl;
    bekle_ms(400);
}

// --- Kahraman sınıfı ---
class Kahraman {
public:
    string ad;
    int can, hasar, maxCan;
    int ozelCooldown;    // 0 hazır, >0 beklemede
    bool stunlu;

    // istatistikler
    int toplamHasar;
    int kritikSayisi;
    int yenilemeSayisi;
    int vurusSayisi;
    int ozelKullanmaSayisi;
    int selfHasar;
    int toplamAldigiHasar;

    Kahraman(){}
    Kahraman(const string &isim, int c, int h){
        ad = isim; can=c; hasar=h; maxCan=c;
        ozelCooldown = 0; stunlu = false;
        toplamHasar = kritikSayisi = yenilemeSayisi = vurusSayisi = ozelKullanmaSayisi = selfHasar = toplamAldigiHasar = 0;
    }

    void durumGoster(){
        int bar = (can * 20) / maxCan;
        if(bar < 0) bar = 0;
        cout << ad << " [";
        for(int i=0;i<bar;i++) cout << "▓";
        for(int i=bar;i<20;i++) cout << "░";
        cout << "] " << can << "/" << maxCan << " HP";
        if(ozelCooldown > 0) cout << " | Özel CD: " << ozelCooldown << " tur";
        else cout << " | Özel: HAZIR";
        if(stunlu) cout << " | STUNLU";
        cout << endl;
    }

    void saldir(Kahraman &rakip, const string &hava, int kritikSans){
        int base = hasar;
        if(hava == "Gunesli") base += 10;
        else if(hava == "Sisli") base = base * 75 / 100;

        int kritik = rand() % 100;
        int dmg = base;
        if(kritik < kritikSans){
            dmg *= 2;
            kritikSayisi++;
            cout << "🔥 Kritik vuruş! ";
        }
        cout << ad << " saldırdı: " << dmg << " hasar verdi.\n";

        rakip.can -= dmg; if(rakip.can < 0) rakip.can = 0;
        toplamHasar += dmg;
        vurusSayisi++;
        rakip.toplamAldigiHasar += dmg;
    }

    void ozelGuc(Kahraman &rakip){
        if(ozelCooldown > 0){
            cout << ad << " özel gücü beklemede (CD: " << ozelCooldown << " tur).\n";
            return;
        }
        cout << ad << " özel gücünü aktive ediyor"; noktaAnim();
        if(ad == "Spiderman") efektSpiderman();
        else if(ad == "Batman") efektBatman();
        else if(ad == "Iron Man") efektIronMan();
        else if(ad == "Hulk") efektHulk();

        int ozelDmg = hasar * 2;
        if(ad == "Hulk") ozelDmg = hasar * 3;
        if(ad == "Iron Man") ozelDmg += 20;

        if(ad == "Spiderman"){
            rakip.can -= ozelDmg; if(rakip.can < 0) rakip.can = 0;
            rakip.stunlu = true;
            cout << "🕸️ " << ad << " ağ tuzağı kurdu! " << rakip.ad << " 1 tur stunlu ve " << ozelDmg << " hasar aldı.\n";
        } else if(ad == "Batman"){
            int self = 10;
            rakip.can -= ozelDmg; if(rakip.can < 0) rakip.can = 0;
            can -= self; if(can < 0) can = 0;
            selfHasar += self;
            toplamAldigiHasar += self;
            cout << "💣 " << ad << " bomba fırlattı! " << rakip.ad << " " << ozelDmg << " hasar aldı. (" << ad << " kendine " << self << " zarar aldı)\n";
        } else {
            rakip.can -= ozelDmg; if(rakip.can < 0) rakip.can = 0;
            cout << "💥 " << ad << " özel saldırdı! " << rakip.ad << " " << ozelDmg << " hasar aldı.\n";
        }

        toplamHasar += ozelDmg;
        ozelKullanmaSayisi++;
        vurusSayisi++;
        rakip.toplamAldigiHasar += ozelDmg;

        // cooldown 3 tur
        ozelCooldown = 3;
    }

    void heal(const string &hava){
        if(hava == "Firtinali"){ cout << "🌩️ Fırtınada dinlenilemez!\n"; return; }
        int yenile = 20 + rand()%11;
        if(ad == "Hulk") yenile = max(0, yenile - 10);
        can += yenile; if(can > maxCan) can = maxCan;
        yenilemeSayisi++;
        cout << "💚 " << ad << " dinlendi ve " << yenile << " can yeniledi! (Yeni: " << can << ")\n";
    }

    void turSonuGuncelle(){
        if(ozelCooldown > 0) ozelCooldown--;
        // stun hamle sırasında kaldırılır (1 tur)
    }

    void istatistikGoster(){
        cout << "\n📊 " << ad << " - İstatistikler 📊\n";
        cout << "Toplam Verilen Hasar: " << toplamHasar << "\n";
        cout << "Vuruş Sayısı: " << vurusSayisi << "\n";
        cout << "Kritik Vuruş Sayısı: " << kritikSayisi << "\n";
        cout << "Can Yenileme Sayısı: " << yenilemeSayisi << "\n";
        cout << "Özel Kullanım Sayısı: " << ozelKullanmaSayisi << "\n";
        cout << "Kendine Alınan Toplam Hasar: " << selfHasar << "\n";
        cout << "Toplam Aldığı Hasar: " << toplamAldigiHasar << "\n";
    }
};

// --- Hava seçimi ---
string havaSec(){
    int r = rand() % 4;
    switch(r){
        case 0: return "Gunesli";
        case 1: return "Yagmurlu";
        case 2: return "Firtinali";
        default: return "Sisli";
    }
}

// --- Karakter seçimi (isim/sayı/r) ---
Kahraman karakterSec(int oyuncu){
    while(true){
        cout << "\n🎮 Oyuncu " << oyuncu << " kahramanını seç!\n";
        cout << "1) Spiderman 🕷️  (Can:90  Hasar:40)\n";
        cout << "2) Batman 🦇      (Can:120 Hasar:30)\n";
        cout << "3) Iron Man ⚡   (Can:80  Hasar:50)\n";
        cout << "4) Hulk 💪       (Can:150 Hasar:60)\n";
        cout << "Rastgele için (r) yaz veya numara/isim gir: ";
        string girdi;
        girdi = safeLine();
        girdi = trim(girdi);
        string key = toLowerNoSpace(girdi);

        if(key == "1" || key == "spiderman" || key == "spider-man" || key == "spider") {
            cout << "Seçildi: Spiderman 🕷️\n";
            return Kahraman("Spiderman", 90, 40);
        } else if(key == "2" || key == "batman") {
            cout << "Seçildi: Batman 🦇\n";
            return Kahraman("Batman", 120, 30);
        } else if(key == "3" || key == "ironman" || key == "ironman" /* no-space handled */) {
            cout << "Seçildi: Iron Man ⚡\n";
            return Kahraman("Iron Man", 80, 50);
        } else if(key == "4" || key == "hulk") {
            cout << "Seçildi: Hulk 💪\n";
            return Kahraman("Hulk", 150, 60);
        } else if(key == "r") {
            int rast = rand() % 4;
            if(rast == 0){ cout << "🎲 Rastgele: Spiderman 🕷️\n"; return Kahraman("Spiderman",90,40); }
            if(rast == 1){ cout << "🎲 Rastgele: Batman 🦇\n"; return Kahraman("Batman",120,30); }
            if(rast == 2){ cout << "🎲 Rastgele: Iron Man ⚡\n"; return Kahraman("Iron Man",80,50); }
            cout << "🎲 Rastgele: Hulk 💪\n"; return Kahraman("Hulk",150,60);
        } else {
            cout << "❌ Geçersiz seçim, isim (ör. hulk), numara (1-4) ya da 'r' yazmayı dene.\n";
        }
    }
}

// --- Basit AI kararı ---
int kararAI(Kahraman &ai, Kahraman &rakip, const string &hava, int kritikSans){
    int ol = rand() % 100;
    int tahmini = ai.hasar * 2;
    if(ai.ad == "Hulk") tahmini = ai.hasar * 3;
    if(ai.ad == "Iron Man") tahmini += 20;
    if(hava == "Gunesli") tahmini += 10;
    if(hava == "Sisli") tahmini = tahmini * 75 / 100;

    if(ai.ozelCooldown == 0 && tahmini >= rakip.can && ol < 95) return 2; // bitirirse özel
    if(ai.can < ai.maxCan * 30 / 100 && hava != "Firtinali") {
        if(ol < 75) return 3; // heal
    }
    if(ai.ozelCooldown == 0 && ol < 25) return 2; // bazen özel
    if(ol < 90) return 1; // çoğunlukla saldır
    return 4;
}

// --- Main ---
int main(){
    srand((unsigned)time(0));
    asciiIntro();

    cout << "\nMod seç: 1) 1P vs 1P    2) 1P vs AI\nSeçim: ";
    string modline = safeLine();
    modline = trim(modline);
    bool vsAI = (modline == "2" || toLowerNoSpace(modline) == "2" || toLowerNoSpace(modline) == "ai");

    Kahraman p1 = karakterSec(1);
    Kahraman p2;
    if(!vsAI) p2 = karakterSec(2);
    else {
        cout << "\nAI için kahraman seç (AI oyuncu):\n";
        p2 = karakterSec(2);
    }

    cout << "\nBaşlıyor... İyi şanslar!"; noktaAnim(3,300);
    int siradaki = 1;
    int tur = 1;
    bool oyun = true;

    while(oyun){
        cout << "\n========== TUR " << tur << " ==========\n";
        string hava = havaSec();
        int kritik = 30;
        if(hava == "Yagmurlu") kritik = 20;
        cout << "🌦️ Hava: " << hava << " (Kritik: %" << kritik << ")\n";

        Kahraman *aktif = (siradaki == 1) ? &p1 : &p2;
        Kahraman *rakip  = (siradaki == 1) ? &p2 : &p1;

        cout << "\n-- Ön Durum --\n";
        p1.durumGoster();
        p2.durumGoster();

        if(aktif->stunlu){
            cout << "\n💥 " << aktif->ad << " STUNLU! Bu tur hareket edemiyor.\n";
            aktif->stunlu = false; // stun 1 tur sürer
        } else {
            if(vsAI && siradaki == 2){
                int karar = kararAI(*aktif, *rakip, hava, kritik);
                bekle_ms(700);
                if(karar == 1){
                    cout << "\n[AI] " << aktif->ad << " saldırıyor...\n";
                    aktif->saldir(*rakip, hava, kritik);
                } else if(karar == 2){
                    cout << "\n[AI] " << aktif->ad << " özel kullanmayı seçti...\n";
                    aktif->ozelGuc(*rakip);
                } else if(karar == 3){
                    cout << "\n[AI] " << aktif->ad << " heal seçti...\n";
                    aktif->heal(hava);
                } else {
                    cout << "\n[AI] " << aktif->ad << " pas geçti...\n";
                }
            } else {
                cout << "\nSıra: " << aktif->ad << "  (1=Attack  2=Special  3=Heal  4=Pass)\n";
                int sec = safeIntFromLine("Seçimin: ");
                if(sec == 1) aktif->saldir(*rakip, hava, kritik);
                else if(sec == 2) aktif->ozelGuc(*rakip);
                else if(sec == 3) aktif->heal(hava);
                else if(sec == 4) cout << aktif->ad << " pas geçti.\n";
                else cout << "❌ Geçersiz seçim! Tur pas sayıldı.\n";
            }
        }

        // Tur sonu güncellemeleri
        p1.turSonuGuncelle();
        p2.turSonuGuncelle();

        cout << "\n-- Son Durum --\n";
        p1.durumGoster();
        p2.durumGoster();

        if(p1.can <= 0 || p2.can <= 0){
            cout << "\n🏁 Maç Bitti! Kazanan: " << (p1.can > 0 ? p1.ad : p2.ad) << " 🎉\n";
            oyun = false;
            break;
        }

        // sıra değişir, tam tur kontrolü
        siradaki = (siradaki == 1) ? 2 : 1;
        if(siradaki == 1) tur++;
    }

    // İstatistikler
    cout << "\n------------------------\n";
    cout << "🏆 MAÇ SONU - DETAYLAR\n";
    cout << "------------------------\n";
    p1.istatistikGoster();
    p2.istatistikGoster();

    cout << "\nTeşekkürler Kanka — tekrar oynamak istersen programı yeniden başlat!\n";
    return 0;
}
