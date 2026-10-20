#include "game.hpp"
#include "audio.hpp"
#include "art.hpp"
#include "raylib.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace orchard;
using namespace orchard::art;
namespace fs=std::filesystem;
namespace {
constexpr float W=1280,H=800,cell=30,bx=54,by=174;
const char* modes[]={"Klasik","Sonsuz","Çit Labirenti"};
const char* descriptions[]={"Bir elma, bir adım daha.","Duvar biter, bahçe bitmez.","Çitlerin arasından ustaca geç."};
const char* difficulties[]={"Rahat","Normal","Hızlı"};
enum class Page { Menu,Play,Pause,Over,Settings };
struct Particle { Vector2 pos,velocity; float life,maxLife,size,rotation; Color color; };
struct Pop { Vector2 pos; float life; std::string text; };

struct Preferences {
    std::array<int,9> records{};
    float music=.48f,effects=.65f;
    bool reduced=false,saveOk=true;
    fs::path path;
    void locate() {
#ifdef _WIN32
        wchar_t* local=nullptr;std::size_t localSize=0;
        _wdupenv_s(&local,&localSize,L"LOCALAPPDATA");
        if(local) path=fs::path(local)/L"MeyveBahcesi"/L"preferences.txt";
        std::free(local);
#endif
        if(path.empty()) { std::error_code ec; auto folder=fs::temp_directory_path(ec); if(!ec) path=folder/"MeyveBahcesi-preferences.txt"; }
    }
    void load() {
        std::ifstream f(path); int version=0,reducedValue=0; float m=0,e=0; std::array<int,9> r{};
        if(!(f>>version) || version!=1 || !(f>>m>>e>>reducedValue)) return;
        for(auto& score:r) if(!(f>>score) || score<0 || score>100000) return;
        if(!std::isfinite(m)||!std::isfinite(e)||(reducedValue!=0 && reducedValue!=1)) return;
        music=std::clamp(m,0.f,1.f); effects=std::clamp(e,0.f,1.f); reduced=reducedValue!=0; records=r;
    }
    void save() {
        std::error_code ec;
        if(path.empty()) {saveOk=false;return;}
        fs::create_directories(path.parent_path(),ec);
        std::ofstream f(path);
        if(!f) { saveOk=false;return; }
        f<<"1\n"<<music<<' '<<effects<<' '<<reduced<<'\n';
        for(int score:records) f<<score<<' ';
        f<<'\n';f.flush();saveOk=bool(f);
    }
};

class App {
public:
    Type type;
    Audio audio;
    AssetFiles assets{AssetFiles::executableDirectory()/L"assets"};
    Preferences prefs;
    Game game{static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};
    RenderTexture2D surface{};
    Page page=Page::Menu,settingsReturn=Page::Menu;
    Mode selectedMode=Mode::Classic;
    Difficulty selectedDifficulty=Difficulty::Normal;
    Vector2 mouse{};
    float time=0,accumulator=0,countdown=0,elapsed=0,shake=0,foodPulse=0,deathAge=0;
    int focus=0,buttonCount=0,lastButtonCount=1,lastCountdown=3;
    bool quit=false,fullscreen=false,newRecord=false,showcase=false;
    std::vector<Particle> particles;
    std::vector<Pop> pops;

    bool assetError(const std::string& reason) {
        TraceLog(LOG_ERROR,"ASSET ERROR: %s",reason.c_str());
        if(showcase) return false;
        while(!WindowShouldClose()&&!IsKeyPressed(KEY_ESCAPE)) {
            BeginDrawing();ClearBackground(cream);
            DrawText("Oyun dosyalari yuklenemedi.",45,65,30,ink);
            DrawText(reason.c_str(),45,130,20,ink);
            DrawText("assets klasorunu EXE'nin yanina koyup tekrar ac. Cikmak icin ESC.",45,220,20,ink);
            EndDrawing();
        }
        return false;
    }
    bool init(bool capture,bool checkAudio=false) {
        showcase=capture;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_MSAA_4X_HINT|FLAG_VSYNC_HINT|(capture?FLAG_WINDOW_HIDDEN:0));
        SetTraceLogLevel(LOG_WARNING);
        InitWindow(1280,800,"Meyve Bahcesi | Bir lokma mutluluk");
        SetWindowMinSize(960,600);SetExitKey(KEY_NULL);SetTargetFPS(capture?0:120);
        surface=LoadRenderTexture(1280,800);SetTextureFilter(surface.texture,TEXTURE_FILTER_BILINEAR);
        auto missing=assets.missing();
        if(!missing.empty()) return assetError("Eksik dosya: "+missing.front());
        if(!loadAssets(assets)||!type.load(assets)) return assetError("PNG veya TTF dosyasi okunamiyor.");
        auto iconBytes=assets.read("images/mascot.png");
        Image icon=LoadImageFromMemory(".png",iconBytes.data(),static_cast<int>(iconBytes.size()));
        SetWindowIcon(icon);UnloadImage(icon);
        prefs.locate(); if(!capture) prefs.load();
        if(!capture||checkAudio) {
            bool soundOk=audio.init(assets);
            if(audio.ready()&&!soundOk) return assetError("WAV dosyalari okunamiyor.");
        }
        audio.setMusic(prefs.music);audio.setEffects(prefs.effects);
        game.reset(selectedMode,selectedDifficulty);
        return true;
    }
    void go(Page p) {page=p;focus=0;lastButtonCount=1;}
    int recordIndex() const {return static_cast<int>(selectedMode)*3+static_cast<int>(selectedDifficulty);}
    void start() {
        game.reset(selectedMode,selectedDifficulty);go(Page::Play);accumulator=0;elapsed=0;
        countdown=2.7f;lastCountdown=3;deathAge=0;newRecord=false;particles.clear();pops.clear();
        audio.effect(Cue::Countdown);
    }
    void pause() {if(page==Page::Play) {go(Page::Pause);audio.effect(Cue::Click);}}
    void resume() {go(Page::Play);countdown=1.5f;lastCountdown=2;audio.effect(Cue::Countdown);}
    void settings() {settingsReturn=page;go(Page::Settings);}
    void burst(Vector2 at,int count,bool victory=false) {
        if(prefs.reduced) return;
        for(int i=0;i<count;i++) {
            float angle=GetRandomValue(0,628)/100.f,force=GetRandomValue(35,190)*1.f;
            float life=GetRandomValue(45,100)/100.f;
            const Color colors[]={coral,orange,lime,Color{249,212,94,255},Color{255,249,220,255}};
            particles.push_back({at,{std::cos(angle)*force,std::sin(angle)*force-(victory?80.f:0.f)},life,life,GetRandomValue(3,7)*1.f,angle,colors[i%5]});
        }
    }
    void input() {
        if(IsKeyPressed(KEY_F11)) {ToggleBorderlessWindowed();fullscreen=!fullscreen;}
        if(IsKeyPressed(KEY_TAB)) {
            focus=(focus+(IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT)?lastButtonCount-1:1))%std::max(1,lastButtonCount);
        }
        if(page==Page::Menu) {
            if(IsKeyPressed(KEY_ONE)) selectedMode=Mode::Classic;
            if(IsKeyPressed(KEY_TWO)) selectedMode=Mode::Endless;
            if(IsKeyPressed(KEY_THREE)) selectedMode=Mode::Challenge;
            if(IsKeyPressed(KEY_LEFT)) selectedMode=static_cast<Mode>((static_cast<int>(selectedMode)+2)%3);
            if(IsKeyPressed(KEY_RIGHT)) selectedMode=static_cast<Mode>((static_cast<int>(selectedMode)+1)%3);
        }
        if(page==Page::Play) {
            if(!IsWindowFocused()&&!showcase) {pause();return;}
            if(IsKeyPressed(KEY_ESCAPE)||IsKeyPressed(KEY_SPACE)||IsKeyPressed(KEY_P)) {pause();return;}
            // Read the press queue in arrival order, rather than testing four booleans.
            int key=0;
            while((key=GetKeyPressed())!=0) {
                if(key==KEY_RIGHT||key==KEY_D) game.turn(Direction::Right);
                if(key==KEY_DOWN||key==KEY_S) game.turn(Direction::Down);
                if(key==KEY_LEFT||key==KEY_A) game.turn(Direction::Left);
                if(key==KEY_UP||key==KEY_W) game.turn(Direction::Up);
            }
        } else if(page==Page::Pause && (IsKeyPressed(KEY_ESCAPE)||IsKeyPressed(KEY_SPACE)||IsKeyPressed(KEY_P))) resume();
        else if(page==Page::Settings && IsKeyPressed(KEY_ESCAPE)) {prefs.save();go(settingsReturn);}
        else if(page==Page::Over && IsKeyPressed(KEY_R)) start();
        else if(page==Page::Over && IsKeyPressed(KEY_ESCAPE)) go(Page::Menu);
        // Drain menu presses, so the key that starts a game cannot become a turn.
        if(page!=Page::Play) while(GetKeyPressed()!=0) {}
    }
    void update(float dt) {
        time+=dt;foodPulse=std::max(0.f,foodPulse-dt*3);shake=std::max(0.f,shake-dt*24);
        audio.duck(page==Page::Pause||page==Page::Settings||page==Page::Over);
        audio.update(dt);
        for(auto& p:particles) {p.life-=dt;p.velocity.y+=100*dt;p.pos.x+=p.velocity.x*dt;p.pos.y+=p.velocity.y*dt;p.rotation+=dt*2;}
        particles.erase(std::remove_if(particles.begin(),particles.end(),[](auto& p){return p.life<=0;}),particles.end());
        for(auto& p:pops){p.life-=dt;p.pos.y-=dt*32;}
        pops.erase(std::remove_if(pops.begin(),pops.end(),[](auto& p){return p.life<=0;}),pops.end());
        if(page==Page::Over) deathAge+=dt;
        if(page!=Page::Play) return;
        if(countdown>0) {
            countdown=std::max(0.f,countdown-dt);
            int c=static_cast<int>(std::ceil(countdown));
            if(c!=lastCountdown) {audio.effect(c?Cue::Countdown:Cue::Eat);lastCountdown=c;}
            return;
        }
        elapsed+=dt;accumulator+=dt;
        while(accumulator>=game.interval() && page==Page::Play) {
            accumulator-=game.interval();
            Cell fruit=game.food;auto event=game.step();
            if(event==Event::Ate||event==Event::Won) {
                Vector2 pos{bx+fruit.x*cell+15,by+fruit.y*cell+15};
                burst(pos,18);pops.push_back({pos,1.f,"+10"});foodPulse=1;audio.effect(Cue::Eat);
            }
            if(event==Event::Died||event==Event::Won) {
                go(Page::Over);accumulator=game.interval();deathAge=0;shake=prefs.reduced?0.f:5.f;
                int& best=prefs.records[recordIndex()];newRecord=game.score>best;
                best=std::max(best,game.score);if(!showcase) prefs.save();
                audio.effect(game.won?Cue::Win:Cue::Lose);
                if(game.won) burst({640,300},90,true);
            }
        }
    }
    bool button(Rectangle r,const std::string& label,bool primary=false,bool selected=false,float size=21) {
        int id=buttonCount++;
        bool hot=CheckCollisionPointRec(mouse,r),focused=focus==id;
        bool pressed=(hot&&IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        Color fill=primary?green:selected?Color{230,239,196,255}:paper;
        if(hot) fill=primary?Color{53,113,65,255}:Color{237,241,212,255};
        if(focused) rounded({r.x-3,r.y-3,r.width+6,r.height+6},17,orange);
        Rectangle face={r.x,r.y+(pressed?2.f:0),r.width,r.height};
        buttonImage(face,fill);
        type.center(label,face.x+face.width/2,face.y+(face.height-size)/2-2,size,primary?paper:ink,true);
        if(hot) SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        bool clicked=(hot&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) || (focused&&IsKeyPressed(KEY_ENTER));
        if(clicked) audio.effect(Cue::Click);
        return clicked;
    }
    void badge(const std::string& s,Rectangle r,Color fill,Color textColor=ink) {
        rounded(r,r.height/2,fill);type.center(s,r.x+r.width/2,r.y+7,16,textColor,true);
    }
    void backdrop() {
        ClearBackground(cream);
        backdropImage();
    }
    void logo(float x,float y) {
        DrawCircleV({x+16,y+19},17,green);leaf({x+8,y+20},22,-.4f,lime);
        type.text("MEYVE BAHÇESİ",x+44,y,27,ink,true);
    }
    void menu() {
        logo(58,34);
        badge("KÜÇÜK BİR BAHÇE. BÜYÜK BİR İŞTAH.",{827,34,393,34},Color{231,236,201,255},muted);
        badge("TAPTAZE BİR KLASİK",{62,121,208,34},Color{255,225,164,255});
        type.heading("Bir lokma",56,169,76);
        type.heading("mutluluk.",56,249,76,green);
        type.text("Elmanı kap, uzadıkça uza!",64,351,24,muted);
        type.text("Bahçenin en kıvrak sakini sen ol.",64,385,24,muted);
        if(button({64,446,253,60},"Hadi oynayalım!",true)) start();
        type.text("ENTER ile başla",78,521,16,muted);
        // PNG illustrations share the same soft, storybook palette.
        DrawEllipse(930,409,231,145,Color{218,230,168,255});
        sprite(Sprite::Sun,{1125,139},{98,98});
        float motion=prefs.reduced?0:std::sin(time*.5f)*10;
        cloud({775+motion,174},73,Color{255,253,239,255});cloud({1155-motion,269},60,Color{255,253,239,255});
        for(int i=0;i<13;i++) {
            float x=753.f+(i*61)%365,y=408.f+(i*31)%96;
            sprite(Sprite::Grass,{x,y},{17,17});
        }
        mascot({946,321},174,prefs.reduced?0:time);
        apple({771,425},31);apple({1117,426},23);
        flower({1125,476},13,paper);flower({760,466},10,Color{248,194,151,255});
        star({772,270},13,orange,-.2f);star({1121,333},9,orange,.3f);
        type.text("BİR MOD SEÇ",64,574,16,muted,true);
        const char* tags[]={"01 / KLASİK TARİF","02 / ÖZGÜR RUH","03 / BİRAZ MACERA"};
        for(int i=0;i<3;i++) {
            Rectangle r{64.f+i*391,610,370,114};bool selected=static_cast<int>(selectedMode)==i;
            if(button(r,"",false,selected)) selectedMode=static_cast<Mode>(i);
            type.text(tags[i],r.x+22,r.y+15,14,selected?green:muted,true);
            type.text(modes[i],r.x+22,r.y+39,27,ink,true);
            type.text(descriptions[i],r.x+22,r.y+77,17,muted);
            if(selected) {DrawCircleV({r.x+r.width-26,r.y+30},10,green);DrawLineEx({r.x+r.width-31,r.y+30},{r.x+r.width-27,r.y+34},2,paper);DrawLineEx({r.x+r.width-27,r.y+34},{r.x+r.width-21,r.y+26},2,paper);}
        }
        if(button({925,750,133,32},"Ayarlar",false,false,16)) settings();
        if(button({1080,750,137,32},"Çıkış",false,false,16)) quit=true;
        type.text("Yön tuşları / WASD  ·  1–3 mod seç  ·  F11 tam ekran",64,757,17,muted);
    }
    void drawSnake() {
        float alpha=std::clamp(accumulator/game.interval(),0.f,1.f);
        if(page==Page::Menu || countdown>0) alpha=1;
        std::vector<Vector2> positions;
        for(std::size_t i=0;i<game.snake.size();i++) {
            Cell now=game.snake[i],old=i<game.previous.size()?game.previous[i]:game.previous.back();
            float x=static_cast<float>(now.x),y=static_cast<float>(now.y);
            if(std::abs(now.x-old.x)<=1 && std::abs(now.y-old.y)<=1) {x=old.x+(now.x-old.x)*alpha;y=old.y+(now.y-old.y)*alpha;}
            positions.push_back({bx+(x+.5f)*cell,by+(y+.5f)*cell});
        }
        snakeBody(positions);
        if(positions.empty()) return;
        Sprite expression=Sprite::Head;
        if(page==Page::Over&&!game.won) expression=Sprite::Dizzy;
        else if(foodPulse>.1f) expression=Sprite::Munch;
        else if(!prefs.reduced&&std::fmod(time,5.8f)>5.65f) expression=Sprite::Blink;
        float size=38.f+foodPulse*3;
        sprite(expression,positions[0],{size,size},static_cast<int>(game.direction)*90.f);
    }
    void board() {
        panel({42,158,744,568},Color{214,228,174,255});
        rounded({49,169,730,550},12,Color{129,163,96,255});
        boardImage({bx,by,720,540});
        if(selectedMode==Mode::Endless) {
            for(int y=0;y<Game::rows;y+=3) {
                DrawCircleV({bx-3,by+y*cell+15},3,orange);DrawCircleV({bx+724,by+y*cell+15},3,orange);
            }
        }
        for(Cell hedgeCell:game.hedges) hedge({bx+hedgeCell.x*cell+15,by+hedgeCell.y*cell+15},cell);
        if(!game.won) {
            float bounce=prefs.reduced?0:std::sin(time*3)*1.4f;
            apple({bx+game.food.x*cell+15,by+game.food.y*cell+14+bounce},11.5f+foodPulse*2);
        }
        drawSnake();
        for(auto& p:particles) {
            Color c=Fade(p.color,std::min(1.f,p.life*3));
            DrawRectanglePro({p.pos.x,p.pos.y,p.size,p.size*1.4f},{p.size/2,p.size*.7f},p.rotation*RAD2DEG,c);
        }
        for(auto& p:pops) type.center(p.text,p.pos.x,p.pos.y,25,Fade(ink,p.life),true);
    }
    void gameUI() {
        logo(45,33);
        badge(modes[static_cast<int>(selectedMode)],{365,34,174,34},Color{227,235,191,255});
        type.text("Küçük adımlar. Kocaman bir iştah.",46,96,23,muted);
        if(page==Page::Play) {if(button({1086,32,149,43},"Duraklat",false,false,18)) pause();}
        else badge("BİR MOLA",{1086,34,149,34},Color{255,225,164,255});
        float sx=0,sy=0;
        if(shake>0) {sx=std::sin(time*90)*shake;sy=std::cos(time*77)*shake;}
        // Only the board shakes; text and controls stay still.
        Camera2D camera{{sx,sy},{0,0},0,1};BeginMode2D(camera);board();EndMode2D();
        panel({820,158,414,194});
        type.text("TOPLADIĞIN MUTLULUK",847,180,15,muted,true);
        type.heading(std::to_string(game.score),845,207,65,green);
        apple({1182,241},26);
        DrawLineEx({847,288},{1207,288},1,border);
        type.text("EN İYİ",847,309,16,muted,true);
        type.text(std::to_string(prefs.records[recordIndex()]),952,301,27,ink,true);
        type.text(std::string(difficulties[static_cast<int>(selectedDifficulty)]),1107,307,18,muted);
        panel({820,371,414,159});
        type.text("BAHÇE NOTLARI",847,393,15,muted,true);
        if(selectedMode==Mode::Classic) {
            type.text("Elmayı ye, biraz daha büyü.",847,429,21,ink,true);
            type.text("Duvarlara ve kendine dikkat!",847,466,20,muted);
        } else if(selectedMode==Mode::Endless) {
            type.text("Öbür tarafta görüşürüz!",847,429,21,ink,true);
            type.text("Duvarlardan geç. Kuyruğa dikkat.",847,466,19,muted);
        } else {
            type.text("Kestirmeler her zaman iyi değil.",847,429,20,ink,true);
            type.text("Çitlere çarpmadan elmalara ulaş.",847,466,19,muted);
        }
        rounded({820,550,414,176},22,Color{231,235,199,255});
        type.text("ACELE ETMEDEN, KIVRILA KIVRILA.",844,568,14,muted,true);
        type.text(std::to_string(game.eaten)+" elma",847,603,25,ink,true);
        int seconds=static_cast<int>(elapsed);char clock[24];std::snprintf(clock,sizeof(clock),"%02d:%02d",seconds/60,seconds%60);
        type.text(clock,1120,603,25,ink,true);
        DrawLineEx({847,647},{1207,647},1,Color{201,213,165,255});
        type.text("Her elmayla tempo biraz artar.",847,673,18,muted);
        type.text("Yön tuşları / WASD  Hareket",47,754,18,muted);
        type.text("SPACE / P  Mola",425,754,18,muted);
        type.text("F11  Tam ekran",1034,754,18,muted);
        if(!prefs.saveOk) type.text("Rekor bu oturumda tutuluyor.",824,735,15,coral);
        if(page==Page::Play && countdown>0) {
            rounded({bx,by,720,540},1,Fade(cream,.57f));
            type.center(countdown>1.5f?"Hazır mısın?":"Hadi bakalım!",414,326,27,ink,true);
            type.center(std::to_string(static_cast<int>(std::ceil(countdown))),414,370,95,green,true);
            type.center("Yönünü şimdiden seçebilirsin.",414,490,20,muted);
        }
    }
    void overlay() {
        DrawRectangle(0,0,1280,800,Fade(Color{38,62,45,255},.42f));
        panel({386,121,508,559});
        bool over=page==Page::Over;
        if(!over) {
            mascot({622,241},82,prefs.reduced?0:time,true);
            type.center("Küçük bir mola",640,345,38,ink,true);
            type.center("Bahçe seni bekliyor.",640,399,22,muted);
            if(button({442,456,396,52},"Devam et",true)) resume();
            if(button({442,523,188,49},"Ayarlar",false,false,19)) settings();
            if(button({650,523,188,49},"Ana menü",false,false,19)) go(Page::Menu);
            type.center("SPACE veya ESC ile devam et",640,615,17,muted);
        } else {
            if(newRecord||game.won) {
                star({640,215},54,orange,0);star({540,210},13,orange,.2f);star({739,230},17,lime,-.3f);
            } else mascot({620,210},64,prefs.reduced?0:time);
            type.center(game.won?"Bahçe senin!":"Bir tur daha?",640,305,38,ink,true);
            type.center(newRecord?"YENİ REKOR! HARİKA İŞTAH.":game.won?"Bütün elmaları topladın!":"Her kıvrım yeni bir başlangıç.",640,358,18,newRecord?green:muted,true);
            type.center(std::to_string(game.score),640,390,70,green,true);
            type.center(std::to_string(game.eaten)+" elma  ·  "+modes[static_cast<int>(selectedMode)],640,474,20,muted);
            if(button({442,525,396,53},"Yeniden oyna",true)) start();
            if(button({442,596,396,47},"Ana menü",false,false,20)) go(Page::Menu);
        }
    }
    void settingsUI() {
        logo(58,34);
        type.heading("Tam sana göre.",64,115,55);
        type.text("Biraz daha sakin, biraz daha hareketli. Seçim senin.",67,193,24,muted);
        panel({64,250,704,445});
        type.text("OYUN TEMPOSU",94,278,17,muted,true);
        for(int i=0;i<3;i++) if(button({94.f+i*214,318,194,53},difficulties[i],false,static_cast<int>(selectedDifficulty)==i,21)) {
            if(settingsReturn==Page::Menu) selectedDifficulty=static_cast<Difficulty>(i);
        }
        type.text(settingsReturn==Page::Menu?"Yeni oyununa uygun bir hız seç.":"Tempoyu değiştirmek için önce ana menüye dön.",94,389,19,muted);
        type.text("MÜZİK",94,446,19,ink,true);
        type.text("Özgün, neşeli bir bahçe melodisi.",94,480,18,muted);
        if(button({490,443,47,44},"-")) prefs.music=std::max(0.f,prefs.music-.15f);
        type.center(std::to_string(static_cast<int>(prefs.music*100+.5f))+"%",586,451,23,ink,true);
        if(button({637,443,47,44},"+")) prefs.music=std::min(1.f,prefs.music+.15f);
        type.text("SES EFEKTLERİ",94,549,19,ink,true);
        type.text("Her elmaya küçük bir kutlama.",94,583,18,muted);
        if(button({490,548,47,44},"-")) prefs.effects=std::max(0.f,prefs.effects-.15f);
        type.center(std::to_string(static_cast<int>(prefs.effects*100+.5f))+"%",586,556,23,ink,true);
        if(button({637,548,47,44},"+")) {prefs.effects=std::min(1.f,prefs.effects+.15f);audio.setEffects(prefs.effects);audio.effect(Cue::Eat);}
        if(!audio.ready()) type.text(showcase?"Tanıtım çekiminde ses kapalı.":"Ses aygıtı bulunamadı; sessiz oynayabilirsin.",94,645,16,muted);
        panel({794,250,422,445},Color{233,239,205,255});
        mascot({995,364},88,prefs.reduced?0:time);
        type.center("Daha sakin bir bahçe",1005,486,25,ink,true);
        type.center("Parçacıkları ve sarsıntıyı azalt.",1005,528,19,muted);
        if(button({836,579,338,52},prefs.reduced?"Sakin efektler: Açık":"Sakin efektler: Kapalı",false,prefs.reduced,19)) prefs.reduced=!prefs.reduced;
        if(button({64,731,228,48},"Kaydet ve dön",true,false,20)) {prefs.save();go(settingsReturn);}
        if(button({973,731,243,48},"Tam ekran / F11",false,false,19)) {ToggleBorderlessWindowed();fullscreen=!fullscreen;}
        type.text("TAB ile gezin  ·  ENTER ile seç  ·  ESC ile dön",340,748,18,muted);
        audio.setMusic(prefs.music);audio.setEffects(prefs.effects);
    }
    void frame() {
        buttonCount=0;SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        BeginTextureMode(surface);backdrop();
        if(page==Page::Menu) menu();
        else if(page==Page::Settings) settingsUI();
        else {gameUI();if(page==Page::Pause||page==Page::Over) overlay();}
        EndTextureMode();lastButtonCount=std::max(1,buttonCount);
        float scale=std::min(GetScreenWidth()/W,GetScreenHeight()/H);
        float ox=(GetScreenWidth()-W*scale)/2,oy=(GetScreenHeight()-H*scale)/2;
        BeginDrawing();ClearBackground(Color{40,59,44,255});
        DrawTexturePro(surface.texture,{0,0,W,-H},{ox,oy,W*scale,H*scale},{0,0},0,WHITE);EndDrawing();
    }
    void screenshot(const char* path) {
        frame();Image image=LoadImageFromTexture(surface.texture);ImageFlipVertical(&image);ExportImage(image,path);UnloadImage(image);
    }
    void captureGallery() {
        fs::create_directories("artifacts");mouse={-100,-100};time=2.3f;
        screenshot("artifacts/01-menu.png");
        selectedMode=Mode::Challenge;start();countdown=0;game.food={13,7};
        game.snake={{12,9},{11,9},{10,9},{10,10},{10,11},{9,11},{8,11},{7,11},{7,10},{7,9},{7,8}};
        game.previous=game.snake;game.score=80;game.eaten=8;elapsed=93;accumulator=game.interval();
        screenshot("artifacts/02-game.png");
        go(Page::Pause);screenshot("artifacts/03-pause.png");
        go(Page::Over);newRecord=true;screenshot("artifacts/04-results.png");
        settingsReturn=Page::Menu;go(Page::Settings);screenshot("artifacts/05-settings.png");
        selectedMode=Mode::Classic;start();countdown=0;game.food={20,5};game.snake.clear();
        for(int x=19;x>=3;x--) game.snake.push_back({x,9});
        game.previous=game.snake;game.score=140;game.eaten=14;accumulator=game.interval();
        screenshot("artifacts/06-long-snake.png");
    }
    void run() {
        while(!quit&&!WindowShouldClose()) {
            float scale=std::min(GetScreenWidth()/W,GetScreenHeight()/H);
            Vector2 raw=GetMousePosition();mouse={(raw.x-(GetScreenWidth()-W*scale)/2)/scale,(raw.y-(GetScreenHeight()-H*scale)/2)/scale};
            input();update(std::min(GetFrameTime(),.05f));frame();
        }
    }
    void close() {if(!showcase) prefs.save();audio.shutdown();type.unload();unloadAssets();UnloadRenderTexture(surface);CloseWindow();}
};
}
int main(int argc,char** argv) {
    bool verify=argc>1&&std::string(argv[1])=="--verify-media";
    bool capture=verify||(argc>1&&std::string(argv[1])=="--capture");
    App app;if(!app.init(capture,verify)) {app.close();return 1;}
    if(verify) {bool ok=app.audio.mediaLoaded();app.audio.update(.1f);app.close();return ok?0:2;}
    if(capture) app.captureGallery();else app.run();
    app.close();return 0;
}
