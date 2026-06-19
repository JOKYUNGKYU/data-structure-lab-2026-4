#include "mainwindow.h"
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QScrollArea>
#include <QDateTime>
#include <QRandomGenerator>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QQueue>
#include <QSet>
#include <QtMath>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════
//  Line colours  (official Seoul Metro palette)
// ═══════════════════════════════════════════════════════════════
static const QMap<QString,QColor> LINE_COLORS = {
    {"1", QColor(0x00,0x52,0xA4)},   // dark blue
    {"2", QColor(0x00,0xA8,0x4F)},   // green
    {"3", QColor(0xFF,0x6E,0x1A)},   // orange
    {"4", QColor(0x00,0xA5,0xDE)},   // sky blue
    {"5", QColor(0x99,0x63,0x9A)},   // purple
    {"6", QColor(0xCD,0x6B,0x3B)},   // brown
    {"7", QColor(0x74,0x7F,0x00)},   // olive
    {"8", QColor(0xE6,0x18,0x6F)},   // pink
    {"9", QColor(0xBF,0xA0,0x00)},   // gold
    {"경의중앙", QColor(0x77,0xC4,0xA3)},
    {"공항", QColor(0x00,0x4B,0x85)},
    {"경춘", QColor(0x00,0x86,0x8B)},
    {"수인분당", QColor(0xFF,0xD1,0x00)},
    {"신분당", QColor(0xD4,0x00,0x3B)},
    {"우이신설", QColor(0xB0,0xCE,0x63)},
};

QColor MainWindow::lineColor(const QString &ln) const {
    return LINE_COLORS.value(ln, Qt::gray);
}
QString MainWindow::lineColorName(const QString &ln) const {
    QColor c = lineColor(ln);
    return c.name();
}

// ═══════════════════════════════════════════════════════════════
//  StationItem
// ═══════════════════════════════════════════════════════════════
StationItem::StationItem(const Station &st, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), m_id(st.id)
{
    // radius scales with number of lines (transfer station = bigger)
    double r = (st.lines.size() > 1) ? 10.0 : 7.0;
    setRect(-r, -r, r*2, r*2);
    setPos(st.x, st.y);

    // colour = first line
    if (!st.lines.isEmpty())
        m_baseColor = LINE_COLORS.value(st.lines.first(), Qt::gray);
    else
        m_baseColor = Qt::gray;

    setBrush(QBrush(m_baseColor));
    setPen(QPen(Qt::white, 2));
    setZValue(2);
    setAcceptHoverEvents(true);
    setToolTip(QString("%1\n%2").arg(st.nameKo, st.nameEn));
    setCursor(Qt::PointingHandCursor);
}

void StationItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsEllipseItem::mousePressEvent(event);
    emit clicked(m_id);
}
void StationItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
    QGraphicsEllipseItem::hoverEnterEvent(e);
    setScale(1.5);
    setPen(QPen(Qt::yellow, 2));
}
void StationItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
    QGraphicsEllipseItem::hoverLeaveEvent(e);
    setScale(1.0);
    setPen(QPen(Qt::white, 2));
}

// ═══════════════════════════════════════════════════════════════
//  Station data  (representative subset – major stations + transfers)
//  Coordinates are laid out on a 1400×1200 scene
// ═══════════════════════════════════════════════════════════════
void MainWindow::buildStationData()
{
    auto add = [&](const QString &id, const QString &ko, const QString &en,
                   QVector<QString> lines, double x, double y)
    {
        Station s; s.id=id; s.nameKo=ko; s.nameEn=en;
        s.lines=lines; s.x=x; s.y=y;
        m_stations[id] = s;
    };

    // ── Line 1 (north–south spine, dark blue) ──────────────────
    add("1_DORAMO",  "도라산",   "Dorasan",        {"1"},        700, 20);
    add("1_MUSAN",   "문산",     "Munsan",         {"1"},        700, 55);
    add("1_NEUNGGOK","능곡",     "Neunggok",       {"1"},        700, 80);
    add("1_NEUNGGOK2","행신",    "Haengsin",       {"1"},        660, 100);
    add("1_UIJEO",   "의정부",   "Uijeongbu",      {"1"},        700, 130);
    add("1_DOBONG",  "도봉산",   "Dobongsan",      {"1","7"},    730, 155);
    add("1_DOBONGS2","도봉",     "Dobong",         {"1"},        700, 175);
    add("1_BANGHAK", "방학",     "Banghak",        {"1"},        700, 195);
    add("1_CHANGDONG","창동",    "Changdong",      {"1","4"},    700, 215);
    add("1_DOBONGRA","도봉구청", "Dobong-gu Office",{"1"},       700, 235);
    add("1_NOWON",   "녹천",     "Nokcheon",       {"1"},        700, 255);
    add("1_WOLGE",   "월계",     "Wolgye",         {"1"},        700, 275);
    add("1_SEONGJUN","성북",     "Seongbuk",       {"1"},        700, 295);
    add("1_SEOKGYE", "석계",     "Seokgye",        {"1","6"},    700, 315);
    add("1_SINDAP",  "신이문",   "Sinimun",        {"1"},        700, 335);
    add("1_OIRYU",   "외대앞",   "Hankuk Univ.",   {"1"},        700, 355);
    add("1_HOEGI",   "회기",     "Hoegi",          {"1","경의중앙"},700,375);
    add("1_CHEONGNYANGNI","청량리","Cheongnyangni", {"1","경의중앙","경춘","수인분당"}, 700, 400);
    add("1_JEGI",    "제기동",   "Jegi-dong",      {"1"},        700, 425);
    add("1_SINSEOL", "신설동",   "Sinseol-dong",   {"1","2"},    700, 450);
    add("1_DONGMYO", "동묘앞",   "Dongmyo",        {"1","6"},    700, 470);
    add("1_DONGDAEMUN","동대문", "Dongdaemun",     {"1","4"},    700, 490);
    add("1_JONGNO5GA","종로5가", "Jongno 5-ga",    {"1"},        650, 510);
    add("1_JONGNO3GA","종로3가", "Jongno 3-ga",    {"1","3","5"},620, 510);
    add("1_JONGNO1GA","종각",    "Jonggak",        {"1"},        590, 510);
    add("1_CITY_HALL","시청",    "City Hall",      {"1","2"},    560, 510);
    add("1_SEOUL",   "서울역",   "Seoul Station",  {"1","4","경의중앙"}, 560, 560);
    add("1_NAMYEONG","남영",     "Namyeong",       {"1"},        560, 590);
    add("1_YONGSAN", "용산",     "Yongsan",        {"1","경의중앙"}, 560, 620);
    add("1_NORYANGJIN","노량진", "Noryangjin",     {"1","9"},    560, 660);
    add("1_DAEBANG", "대방",     "Daebang",        {"1"},        560, 690);
    add("1_SINDORIM","신도림",   "Sindorim",       {"1","2"},    490, 720);
    add("1_GURO",    "구로",     "Guro",           {"1"},        450, 740);
    add("1_GASAN",   "가산디지털단지","Gasan Digital Complex",{"1","7"},420,770);
    add("1_DOKSAN",  "독산",     "Doksan",         {"1"},        390, 800);
    add("1_GEUMCHEON","금천구청","Geumcheon-gu Office",{"1"},    360, 830);
    add("1_GWANGMYEONG","광명",  "Gwangmyeong",   {"1"},        300, 860);
    add("1_ANYANG",  "안양",     "Anyang",         {"1"},        370, 870);
    add("1_SUWON",   "수원",     "Suwon",          {"1","수인분당"}, 420, 980);
    add("1_INCHEON", "인천",     "Incheon",        {"1"},        100, 1050);

    // ── Line 2 (loop, green) ────────────────────────────────────
    add("2_SINCHON", "신촌",     "Sinchon",        {"2"},        370, 470);
    add("2_HONGIK",  "홍대입구", "Hongik Univ.",   {"2","공항","경의중앙"},340,490);
    add("2_HAPJEONG","합정",     "Hapjeong",       {"2","6"},    310, 510);
    add("2_DANGSAN", "당산",     "Dangsan",        {"2","9"},    290, 540);
    add("2_YEONGDEUNGPO","영등포구청","Yeongdeungpo-gu Off.",{"2"},330,580);
    add("2_MULLAE",  "문래",     "Mullae",         {"2"},        390, 620);
    add("2_DORIMCHEON","도림천", "Dorimcheon",     {"2"},        450, 680);
    add("2_GURO_DIG","구로디지털단지","Guro Digital Complex",{"2"},460,720);
    add("2_SINDAEBANG","신대방", "Sindaebang",     {"2"},        500, 750);
    add("2_BONGCHEON","봉천",    "Bongcheon",      {"2"},        510, 790);
    add("2_SILLIM",  "신림",     "Sillim",         {"2"},        520, 810);
    add("2_NAKSEONG","낙성대",   "Nakseongdae",    {"2"},        550, 830);
    add("2_SADANG",  "사당",     "Sadang",         {"2","4"},    570, 850);
    add("2_BANGBAE", "방배",     "Bangbae",        {"2"},        600, 840);
    add("2_SEOCHO",  "서초",     "Seocho",         {"2"},        630, 840);
    add("2_GANGNAM", "강남",     "Gangnam",        {"2"},        680, 840);
    add("2_YEOKSAM", "역삼",     "Yeoksam",        {"2"},        710, 840);
    add("2_SEOLLEUNG","선릉",    "Seolleung",      {"2","수인분당"},750,840);
    add("2_SAMSUNG", "삼성",     "Samsung",        {"2"},        790, 840);
    add("2_JAMSIL",  "잠실",     "Jamsil",         {"2","8"},    860, 810);
    add("2_JAMSIL_NARU","잠실나루","Jamsil Naru",  {"2"},        890, 790);
    add("2_GUUI",    "구의",     "Guui",           {"2"},        920, 770);
    add("2_KONDAE",  "건대입구", "Konkuk Univ.",   {"2","7"},    960, 750);
    add("2_SEONGSU", "성수",     "Seongsu",        {"2"},        950, 730);
    add("2_WANGSIMNI","왕십리",  "Wangsimni",      {"2","5","경의중앙","수인분당"},900,700);
    add("2_SINDAP2", "신답",     "Sindap",         {"2"},        860, 670);
    add("2_YONGDAP", "용답",     "Yongdap",        {"2"},        830, 650);
    add("2_MAJANG",  "마장",     "Majang",         {"2"},        800, 630);
    add("2_DAPSIMNI","답십리",   "Dapsimni",       {"2"},        770, 610);
    add("2_EULJIRO4","을지로4가","Euljiro 4-ga",   {"2","5"},    700, 490);
    add("2_EULJIRO1","을지로입구","Euljiro 1-ga",  {"2"},        660, 490);
    add("2_EULJIRO3","을지로3가","Euljiro 3-ga",   {"2","3"},    680, 490);

    // ── Line 3 (orange) ─────────────────────────────────────────
    add("3_DAEHWA",  "대화",     "Daehwa",         {"3"},        110, 180);
    add("3_JUYEOP",  "주엽",     "Juyeop",         {"3"},        150, 180);
    add("3_JEONGBALSAN","정발산","Jeongbalsan",     {"3"},        185, 180);
    add("3_MADU",    "마두",     "Madu",           {"3"},        220, 180);
    add("3_BAEKSEOK","백석",     "Baekseok",       {"3"},        250, 180);
    add("3_DAEGOK",  "대곡",     "Daegok",         {"3","경의중앙"},285,180);
    add("3_WHAJEONG","화정",     "Hwajeong",       {"3"},        315, 190);
    add("3_WONHEUNG","원흥",     "Wonheung",       {"3"},        340, 210);
    add("3_SAMSONGJUNGANG","삼송","Samsong",        {"3"},        360, 240);
    add("3_JICHO",   "지축",     "Jichuk",         {"3"},        375, 270);
    add("3_GUPABAL", "구파발",   "Gupabal",        {"3"},        380, 300);
    add("3_YEONSIN", "연신내",   "Yeonsinnae",     {"3","6"},    390, 330);
    add("3_BULGWANG","불광",     "Bulgwang",       {"3"},        400, 355);
    add("3_NOKBEON", "녹번",     "Nokbeon",        {"3"},        410, 380);
    add("3_HONGJEWA","홍제",     "Hongjae",        {"3"},        420, 405);
    add("3_MUAKJAE", "무악재",   "Muakjae",        {"3"},        430, 425);
    add("3_DONGNIMMUN","독립문", "Dongnimmun",     {"3"},        450, 440);
    add("3_GYEONGBOKGUNG","경복궁","Gyeongbokgung",{"3"},        490, 460);
    add("3_ANGUK",   "안국",     "Anguk",          {"3"},        520, 475);
    add("3_CHUNGMURO","충무로",  "Chungmuro",      {"3","4"},    650, 530);
    add("3_DONGGUK", "동국대",   "Dongguk Univ.",  {"3"},        680, 540);
    add("3_YAKSU",   "약수",     "Yaksu",          {"3","6"},    710, 560);
    add("3_GEUMHO",  "금호",     "Geumho",         {"3"},        745, 575);
    add("3_OKSU",    "옥수",     "Oksu",           {"3","경의중앙"},775,585);
    add("3_APGUJEONG","압구정",  "Apgujeong",      {"3"},        760, 640);
    add("3_SINSA",   "신사",     "Sinsa",          {"3"},        750, 680);
    add("3_JAMWON",  "잠원",     "Jamwon",         {"3"},        750, 710);
    add("3_EXPORO",  "고속터미널","Express Bus Ter.",{"3","7","9"},780,730);
    add("3_YUKSAM",  "교대",     "Gyodae",         {"3","2"},    720, 810);
    add("3_YANGJAE", "양재",     "Yangjae",        {"3","신분당"},790,870);
    add("3_MAEHEON", "매헌",     "Maeheon",        {"3"},        810, 905);
    add("3_YANGJAECIT","양재시민의숲","Yangjae (Citizen's Forest)",{"3","신분당"},830,930);
    add("3_CHEONGGYESAN","청계산입구","Cheonggyesan",{"3"},      860, 960);
    add("3_JEONGJA", "정자",     "Jeongja",        {"3","수인분당"},870,1000);

    // ── Line 4 (sky blue) ───────────────────────────────────────
    add("4_DANGGOGAE","당고개",  "Danggogae",      {"4"},        820, 120);
    add("4_SANGGYE", "상계",     "Sanggye",        {"4"},        820, 150);
    add("4_NOWON4",  "노원",     "Nowon",          {"4","7"},    820, 175);
    add("4_JUNGGYE", "중계",     "Junggye",        {"4"},        820, 200);
    add("4_HAGYE",   "하계",     "Hagye",          {"4"},        820, 220);
    add("4_GONGNEUNG","공릉",    "Gongneung",      {"4"},        820, 245);
    add("4_TAENEUNG","태릉입구", "Taereung",       {"4","6"},    820, 265);
    add("4_HWARANG", "화랑대",   "Hwarangdae",     {"4"},        820, 285);
    add("4_DOBONG4", "도봉산",   "Dobongsan",      {"4","1"},    800, 305);
    add("4_SUYU",    "수유",     "Suyu",           {"4"},        800, 325);
    add("4_MIA",     "미아",     "Mia",            {"4"},        790, 345);
    add("4_MIASAMG", "미아사거리","Mia Sageori",   {"4"},        780, 370);
    add("4_GILEUM",  "길음",     "Gireum",         {"4"},        770, 395);
    add("4_SUNGSHIN","성신여대입구","Sungshin Univ.",{"4"},       760, 420);
    add("4_HANSUNG", "한성대입구","Hansung Univ.",  {"4"},        745, 445);
    add("4_HYEHWA",  "혜화",     "Hyehwa",         {"4"},        720, 465);
    add("4_DONGSEO", "동대문역사문화공원","DDP",   {"2","4","5"},720,510);
    add("4_CHUNGMURO2","충무로", "Chungmuro",      {"3","4"},    650, 530);
    add("4_MYEONGDONG","명동",   "Myeongdong",     {"4"},        620, 555);
    add("4_HOEHYEON","회현",     "Hoehyeon",       {"4"},        600, 575);
    add("4_SEOUL4",  "서울역",   "Seoul Station",  {"1","4","경의중앙"},560,560);
    add("4_SOOKMYUNG","숙대입구","Sookmyung Univ.",{"4"},        540, 600);
    add("4_SAMGAKJI","삼각지",   "Samgakji",       {"4","6"},    520, 620);
    add("4_SINGIL",  "신길",     "Singil",         {"4"},        510, 660);
    add("4_YEONGDEUNGPO4","영등포시장","Yeongdeungpo Mkt",{"4"},500,690);
    add("4_GEUMJEONG","금정",    "Geumjeong",      {"1","4"},    475, 930);
    add("4_SURI",    "수리산",   "Surisan",        {"4"},        440, 950);
    add("4_DARIM",   "대림",     "Darim",          {"4"},        430, 970);
    add("4_ANYANGBEOB","안양",   "Anyang",         {"4"},        420, 990);
    add("4_BANYEWOL","반월",     "Banweol",        {"4"},        410, 1010);
    add("4_CHOJI",   "초지",     "Choji",          {"4","수인분당"},405,1030);
    add("4_ANSAN",   "안산",     "Ansan",          {"4"},        400, 1050);
    add("4_SINDURI", "신길온천", "Singil Hot Spring",{"4"},      395, 1070);
    add("4_WONSI",   "원시",     "Wonsi",          {"4"},        390, 1090);

    // ── Line 5 (purple) ─────────────────────────────────────────
    add("5_BANGHWA", "방화",     "Banghwa",        {"5"},        100, 530);
    add("5_GAEHWA",  "개화산",   "Gaehwasan",      {"5"},        130, 530);
    add("5_KIMPO5",  "김포공항", "Gimpo Airport",  {"5","9","공항","김포골드라인"},160,530);
    add("5_SONGJUNG","송정",     "Songjung",       {"5"},        195, 530);
    add("5_MAGOK",   "마곡",     "Magok",          {"5"},        230, 540);
    add("5_BALSAN",  "발산",     "Balsan",         {"5"},        260, 545);
    add("5_UGJUNGBU","우장산",   "Ujangsan",       {"5"},        285, 560);
    add("5_HWAGOK",  "화곡",     "Hwagok",         {"5"},        310, 575);
    add("5_SINJEONG","신정",     "Sinjeong",       {"5"},        340, 590);
    add("5_MOKSAM",  "목동",     "Mok-dong",       {"5"},        370, 605);
    add("5_OMOKGYO", "오목교",   "Omokgyo",        {"5"},        400, 620);
    add("5_YANGPYUNG","양평",    "Yangpyeong",     {"5"},        430, 635);
    add("5_YEONGDEUNGPO5","영등포구청","Yeongdeungpo-gu",{"5"},450,650);
    add("5_DURYU",   "도림천",   "Dorimcheon",     {"5"},        470, 670);
    add("5_SINDAEBANG5","신대방삼거리","Sindaebang",{"5"},       495, 685);
    add("5_YEOUIDO","여의도",    "Yeouido",        {"5","9"},    490, 620);
    add("5_SAGANG",  "샛강",     "Saetgang",       {"5","9"},    510, 600);
    add("5_MAPO",    "마포",     "Mapo",           {"5"},        400, 510);
    add("5_GONGDEOK","공덕",     "Gongdeok",       {"5","6","경의중앙","공항"},380,500);
    add("5_AEROJUNG","애오개",   "Aeogae",         {"5"},        430, 490);
    add("5_CHUNGJEONG","충정로", "Chungjengno",    {"5"},        460, 490);
    add("5_SEODAEMUN","서대문",  "Seodaemun",      {"5"},        490, 490);
    add("5_KWANGHWAMUN","광화문","Gwanghwamun",    {"5"},        530, 490);
    add("5_JONGNO3_5","종로3가", "Jongno 3-ga",    {"1","3","5"},620,510);
    add("5_DONGDAEMUN5","동대문역사문화공원","DDP",{"2","4","5"},720,510);
    add("5_CHEONGGU","청구",     "Cheonggu",       {"5"},        760, 515);
    add("5_SINDAP5", "신답",     "Sindap",         {"5"},        800, 510);
    add("5_YONGDAP5","용답",     "Yongdap",        {"5"},        840, 510);
    add("5_DAPSIMNI5","답십리",  "Dapsimni",       {"5"},        870, 510);
    add("5_MAJANGSTN","마장",    "Majang",         {"5"},        890, 510);
    add("5_WANGSIMNI5","왕십리", "Wangsimni",      {"2","5","경의중앙","수인분당"},900,700);
    add("5_HAENGDANG","행당",    "Haengdang",      {"5"},        920, 680);
    add("5_DAPSIMNI_B","답십리(5)","Dapsimni(5)",  {"5"},        940, 660);
    add("5_GODEOK",  "고덕",     "Godeok",         {"5"},        1020, 710);
    add("5_MACHEON", "마천",     "Macheon",        {"5"},        1080, 790);
    add("5_HANAM",   "하남검단산","Hanam Geomdan", {"5"},        1120, 760);

    // ── Line 6 (brown) ──────────────────────────────────────────
    add("6_EUNGAM",  "응암",     "Eungam",         {"6"},        310, 350);
    add("6_YEOKCHON","역촌",     "Yeokchon",       {"6"},        330, 365);
    add("6_BULGWANG6","불광",    "Bulgwang",       {"6"},        360, 375);
    add("6_DONGJAK6","독바위",   "Dokbawi",        {"6"},        380, 385);
    add("6_YEONSIN6","연신내",   "Yeonsinnae",     {"3","6"},    390, 330);
    add("6_GUSAN",   "구산",     "Gusan",          {"6"},        350, 395);
    add("6_SEOKUISAM","새절",    "Saejeol",        {"6"},        340, 420);
    add("6_SINJEONG6","신정",    "Sinjeong",       {"6"},        340, 450);
    add("6_MAPO6",   "대흥",     "Daeheung",       {"6"},        400, 490);
    add("6_GWANGHEUNG","광흥창", "Gwangheungchang",{"6"},        415, 505);
    add("6_SANGSU",  "상수",     "Sangsu",         {"6"},        375, 510);
    add("6_HAPJEONG6","합정",    "Hapjeong",       {"2","6"},    310, 510);
    add("6_MANGWON", "망원",     "Mangwon",        {"6"},        275, 520);
    add("6_MAPO_NARU","마포구청","Mapo-gu Office",  {"6"},        250, 530);
    add("6_MEONGIL", "망원한강","Mangwon (Hangang)",{"6"},       240, 545);
    add("6_GONGDEOK6","공덕",    "Gongdeok",       {"5","6","경의중앙","공항"},380,500);
    add("6_SAMGAKJI6","삼각지",  "Samgakji",       {"4","6"},    520, 620);
    add("6_NOKSAPYEONG","녹사평","Noksapyeong",    {"6"},        560, 645);
    add("6_ITAEWON", "이태원",   "Itaewon",        {"6"},        600, 655);
    add("6_HANGANG","한강진",    "Hangang Bridge",  {"6"},        630, 660);
    add("6_BUGAM",   "버티고개", "Beortigo-gae",   {"6"},        680, 660);
    add("6_YAKSU6",  "약수",     "Yaksu",          {"3","6"},    710, 560);

    // ── Line 7 (olive) ──────────────────────────────────────────
    add("7_JANGHANG","장암",     "Jangam",         {"7"},        800, 80);
    add("7_DOBONGSAN7","도봉산", "Dobongsan",      {"1","7"},    730, 155);
    add("7_SURAKSAN","수락산",   "Suraksan",       {"7"},        750, 185);
    add("7_MADEUNG", "마들",     "Madeul",         {"7"},        770, 205);
    add("7_NOWON7",  "노원",     "Nowon",          {"4","7"},    820, 175);
    add("7_JUNGGYE7","중계",     "Junggye",        {"7"},        845, 200);
    add("7_HAGYE7",  "하계",     "Hagye",          {"7"},        865, 220);
    add("7_GONGNEUNG7","공릉",   "Gongneung",      {"7"},        885, 240);
    add("7_TAENEUNG7","태릉입구","Taereung",       {"4","6"},    820, 265);
    add("7_먹골",    "먹골",     "Meokgol",        {"7"},        900, 275);
    add("7_JUNGHWA", "중화",     "Junghwa",        {"7"},        920, 295);
    add("7_SANGBONG", "상봉",    "Sangbong",       {"7","경춘"}, 950, 315);
    add("7_MYEONMOK","면목",     "Myeonmok",       {"7"},        960, 355);
    add("7_SAGANGNA","사가정",   "Sagajeong",      {"7"},        970, 385);
    add("7_YONGMA",  "용마산",   "Yongmasan",      {"7"},        980, 410);
    add("7_KONDAE7", "건대입구", "Konkuk Univ.",   {"2","7"},    960, 750);
    add("7_TTUKSEOM","뚝섬유원지","Ttukseom Resort",{"7"},       950, 780);
    add("7_CHEONGDAM","청담",    "Cheongdam",      {"7"},        940, 800);
    add("7_GANGNAM7","강남구청", "Gangnam-gu Office",{"7"},      920, 820);
    add("7_HAKDONG", "학동",     "Hakdong",        {"7"},        900, 840);
    add("7_NONHYEON","논현",     "Nonhyeon",       {"7"},        875, 855);
    add("7_BANYPO",  "반포",     "Banpo",          {"7"},        835, 855);
    add("7_NAEBANG", "내방",     "Naebang",        {"7"},        805, 865);
    add("7_GOBUK",   "고속터미널","Express Bus Ter.",{"3","7","9"},780,730);
    add("7_SINGIL7", "이수",     "Isu",            {"4","7"},    580, 880);
    add("7_SANGDO",  "상도",     "Sangdo",         {"7"},        560, 900);
    add("7_JANGSEUNG","장승배기","Jangseungbaegi", {"7"},        545, 920);
    add("7_SHINDAEBANG7","신대방삼거리","Sindaebang Sageori",{"7"},530,940);
    add("7_BORA",    "보라매",   "Boramae",        {"7"},        515, 960);
    add("7_SINPUNG", "신풍",     "Sinpung",        {"7"},        500, 975);
    add("7_DAELIM",  "대림",     "Daelim",         {"2","7"},    490, 995);
    add("7_GASAN7",  "가산디지털단지","Gasan",     {"1","7"},    420, 770);
    add("7_CHEOLSAN","철산",     "Cheolsan",       {"7"},        345, 800);
    add("7_GWANGMYUNG7","광명사거리","Gwangmyeong Sageori",{"7"},320,820);
    add("7_CHEONWANG","천왕",    "Cheonwang",      {"7"},        295, 840);
    add("7_ONSU",    "온수",     "Onsu",           {"1","7"},    270, 860);
    add("7_BUCHEN",  "부천종합운동장","Bucheon Stadium",{"7"},   240,880);
    add("7_BUCHEONSI","부천시청","Bucheon City Hall",{"7"},      215, 900);
    add("7_SINTANJIN","신중동",  "Sinjung-dong",   {"7"},        195, 920);
    add("7_BOKSA",   "복사골",   "Boksagol",       {"7"},        180, 940);
    add("7_BUCHEON","부평구청",  "Bupyeong-gu Office",{"7"},    160,960);
    add("7_SOSA",    "소사",     "Sosa",           {"7"},        140, 980);
    add("7_SEOKSO",  "석남",     "Seongnam",       {"7"},        120, 1000);

    // ── Line 8 (pink) ───────────────────────────────────────────
    add("8_AMSA",    "암사",     "Amsa",           {"8"},        980, 650);
    add("8_CHEONHO", "천호",     "Cheonho",        {"5","8"},    1000, 670);
    add("8_GANGILSUN","강일",    "Gangil",         {"8"},        1010, 695);
    add("8_HANAM_A", "하남풍산", "Hanam Pungsan",  {"8"},        1050, 710);
    add("8_HANAM_B", "하남시청", "Hanam City Hall",{"8"},        1075, 730);
    add("8_HANAM_C", "하남검단산","Hanam Geomdan", {"8"},        1100, 745);
    add("8_MONGCHON","몽촌토성", "Mongchontoseong",{"8"},        960, 700);
    add("8_JAMSIL8", "잠실",     "Jamsil",         {"2","8"},    860, 810);
    add("8_SEOKCHON","석촌",     "Seokchon",       {"8"},        920, 830);
    add("8_SONGPA",  "송파",     "Songpa",         {"8"},        940, 850);
    add("8_GARAKSI", "가락시장", "Garak Market",   {"8"},        950, 870);
    add("8_MUNJEONG","문정",     "Munjeong",       {"8"},        960, 890);
    add("8_JANGJI",  "장지",     "Jangji",         {"8"},        970, 910);
    add("8_BOKJEONG","복정",     "Bokjeong",       {"8","수인분당"},980,930);
    add("8_MORAN",   "모란",     "Moran",          {"8","수인분당"},990,960);
    add("8_BYEOLLAE","별내",     "Byeollae",       {"8"},        1060, 600);

    // ── Line 9 (gold) ───────────────────────────────────────────
    add("9_GAEHWA9", "개화",     "Gaehwa",         {"9"},        130, 600);
    add("9_GIMPO9",  "김포공항", "Gimpo Airport",  {"5","9","공항"},160,530);
    add("9_SINBANGHWA","신방화", "Sinbanghwa",     {"9"},        165, 600);
    add("9_MAGOK9",  "마곡나루", "Magok Naru",     {"9"},        215, 590);
    add("9_YANGHWA", "양화",     "Yanghwa",        {"9"},        275, 555);
    add("9_SEONYU",  "선유도",   "Seonyudo",       {"9"},        300, 560);
    add("9_DANGSAN9","당산",     "Dangsan",        {"2","9"},    290, 540);
    add("9_GUKHOE",  "국회의사당","National Assembly",{"9"},     430, 600);
    add("9_YEOUIDO9","여의도",   "Yeouido",        {"5","9"},    490, 620);
    add("9_SAGANG9", "샛강",     "Saetgang",       {"5","9"},    510, 600);
    add("9_NORYANGJIN9","노량진","Noryangjin",     {"1","9"},    560, 660);
    add("9_NODEUL",  "노들",     "Nodeul",         {"9"},        575, 680);
    add("9_HEUKSEOK","흑석",     "Heukseok",       {"9"},        585, 710);
    add("9_DONGJAK9","동작",     "Dongjak",        {"4","9"},    590, 735);
    add("9_BANPO9",  "구반포",   "Gubanpo",        {"9"},        620, 750);
    add("9_SINBANPO","신반포",   "Sinbanpo",       {"9"},        650, 755);
    add("9_EXPRESS9","고속터미널","Express Bus Ter.",{"3","7","9"},780,730);
    add("9_SEOCHO9", "사평",     "Sapyeong",       {"9"},        750, 790);
    add("9_SINNONHYEON","신논현","Sinnonhyeon",    {"9"},        780, 800);
    add("9_EONJU",   "언주",     "Eonju",          {"9"},        820, 820);
    add("9_SEOLLEUNG9","선정릉", "Seonjeongneung", {"9"},        850, 825);
    add("9_SAMSEONG9","삼성중앙","Samseong(Jungng)",{"9"},       885, 830);
    add("9_BONGEUN", "봉은사",   "Bongeunsa",      {"9"},        915, 835);
    add("9_MONGCHON9","종합운동장","Sports Complex",{"9"},       945, 840);
    add("9_SPORTS",  "삼전",     "Samjeon",        {"9"},        960, 850);
    add("9_SEOKCHON9","석촌고분","Seokchon Gobin", {"9"},        975, 860);
    add("9_SONGPA9", "올림픽공원","Olympic Park",  {"5","9"},    990, 870);
    add("9_HANCHUN", "한성백제",  "Hanseong Baekje",{"9"},       995, 885);
    add("9_DUNCHI",  "둔촌오륜", "Dunchon Oryun",  {"9"},        1000,900);
    add("9_MACHEON9","중앙보훈병원","Veterans Hospital",{"9"},   1010,915);

    // ── Airport Line (공항철도, dark navy) ──────────────────────
    add("AP_INCHEON2","인천공항2터미널","ICN T2",  {"공항"},     30,  540);
    add("AP_INCHEON1","인천공항1터미널","ICN T1",  {"공항"},     60,  540);
    add("AP_UNSEO",  "운서",     "Unseok",         {"공항"},     90,  540);
    add("AP_YONGYU", "용유",     "Yongyu",         {"공항"},     115, 540);
    add("AP_YEONGONG","영종",    "Yeongjong",      {"공항"},     135, 550);
    add("AP_CHEONGNA","청라국제도시","Cheongna Intl City",{"공항"},140,580);
    add("AP_GEOMAM", "검암",     "Geomam",         {"공항"},     160, 600);
    add("AP_GIMPO",  "김포공항", "Gimpo Airport",  {"5","9","공항"},160,530);
    add("AP_GYEYANG","계양",     "Gyeyang",        {"공항"},     175, 615);
    add("AP_BUPYUNG","부평",     "Bupyeong",       {"1","공항"}, 170, 640);
    add("AP_MAJEMUL","마전",     "Majeon",         {"공항"},     200, 630);
    add("AP_SEOULDONGNO","서울역","Seoul Station",  {"1","4","경의중앙","공항"},560,560);
    add("AP_HONGIK", "홍대입구", "Hongik Univ.",   {"2","경의중앙","공항"},340,490);
    add("AP_GONGDEOK2","공덕",   "Gongdeok",       {"5","6","경의중앙","공항"},380,500);
    add("AP_DIGITAL","디지털미디어시티","DMC",     {"공항","경의중앙"},290,400);

    // ── 수인·분당선 (yellow) ────────────────────────────────────
    add("SB_INCHEON_S","인천",   "Incheon",        {"수인분당"}, 100, 1050);
    add("SB_SUIN_SO", "소래포구","Sorae Pogu",     {"수인분당"}, 160, 1020);
    add("SB_ANSAN_S", "안산",    "Ansan",          {"4","수인분당"},400,1050);
    add("SB_SUWON_S", "수원",    "Suwon",          {"1","수인분당"},420,980);
    add("SB_METAN",   "매탄권선","Maetan Gwonseon",{"수인분당"}, 440, 960);
    add("SB_YATAP",   "야탑",    "Yatap",          {"수인분당"}, 870, 990);
    add("SB_JEONGJA2","정자",    "Jeongja",        {"3","수인분당"},870,1000);
    add("SB_MIGEUM",  "미금",    "Migeum",         {"수인분당"}, 880, 1010);
    add("SB_ORI",     "오리",    "Ori",            {"수인분당"}, 890, 1020);
    add("SB_BOJEONG", "보정",    "Bojeong",        {"수인분당"}, 900, 1030);
    add("SB_GIHEUNG", "기흥",    "Giheung",        {"수인분당"}, 910, 1050);
    add("SB_EVERLINE","에버라인환승","Everline",   {"수인분당"}, 920, 1060);
    add("SB_WANGSIMNI_S","왕십리","Wangsimni",     {"2","5","경의중앙","수인분당"},900,700);
    add("SB_SEOLLEUNG_S","선릉", "Seolleung",      {"2","수인분당"},750,840);
    add("SB_BOKJEONG_S","복정",  "Bokjeong",       {"8","수인분당"},980,930);
    add("SB_MORAN_S", "모란",    "Moran",          {"8","수인분당"},990,960);
}

// ═══════════════════════════════════════════════════════════════
//  Edge data (simplified – major connections)
// ═══════════════════════════════════════════════════════════════
void MainWindow::buildEdgeData()
{
    auto link = [&](const QString &a, const QString &b, const QString &ln, int sec=120){
        m_edges.push_back({a, b, ln, sec});
    };
    // Line 1
    link("1_UIJEO","1_DOBONG","1"); link("1_DOBONGS2","1_BANGHAK","1");
    link("1_BANGHAK","1_CHANGDONG","1"); link("1_CHANGDONG","1_SEONGJUN","1");
    link("1_SEOKGYE","1_SINDAP","1"); link("1_SINDAP","1_OIRYU","1");
    link("1_OIRYU","1_HOEGI","1"); link("1_HOEGI","1_CHEONGNYANGNI","1");
    link("1_CHEONGNYANGNI","1_JEGI","1"); link("1_JEGI","1_SINSEOL","1");
    link("1_SINSEOL","1_DONGMYO","1"); link("1_DONGMYO","1_DONGDAEMUN","1");
    link("1_DONGDAEMUN","1_JONGNO5GA","1"); link("1_JONGNO5GA","1_JONGNO3GA","1");
    link("1_JONGNO3GA","1_JONGNO1GA","1"); link("1_JONGNO1GA","1_CITY_HALL","1");
    link("1_CITY_HALL","1_SEOUL","1"); link("1_SEOUL","1_NAMYEONG","1");
    link("1_NAMYEONG","1_YONGSAN","1"); link("1_YONGSAN","1_NORYANGJIN","1");
    link("1_NORYANGJIN","1_DAEBANG","1"); link("1_DAEBANG","1_SINDORIM","1");
    link("1_SINDORIM","1_GURO","1"); link("1_GURO","1_GASAN","1");
    link("1_GASAN","1_DOKSAN","1"); link("1_DOKSAN","1_GEUMCHEON","1");
    // Line 2 (loop)
    link("2_SINCHON","2_HONGIK","2"); link("2_HONGIK","2_HAPJEONG","2");
    link("2_HAPJEONG","2_DANGSAN","2"); link("2_DANGSAN","2_MULLAE","2");
    link("2_MULLAE","2_GURO_DIG","2"); link("2_GURO_DIG","2_SINDAEBANG","2");
    link("2_SINDAEBANG","2_BONGCHEON","2"); link("2_BONGCHEON","2_SILLIM","2");
    link("2_SILLIM","2_NAKSEONG","2"); link("2_NAKSEONG","2_SADANG","2");
    link("2_SADANG","2_BANGBAE","2"); link("2_BANGBAE","2_SEOCHO","2");
    link("2_SEOCHO","2_GANGNAM","2"); link("2_GANGNAM","2_YEOKSAM","2");
    link("2_YEOKSAM","2_SEOLLEUNG","2"); link("2_SEOLLEUNG","2_SAMSUNG","2");
    link("2_SAMSUNG","2_JAMSIL","2"); link("2_JAMSIL","2_JAMSIL_NARU","2");
    link("2_JAMSIL_NARU","2_GUUI","2"); link("2_GUUI","2_KONDAE","2");
    link("2_KONDAE","2_SEONGSU","2"); link("2_SEONGSU","2_WANGSIMNI","2");
    link("2_WANGSIMNI","2_SINDAP2","2"); link("2_SINDAP2","2_YONGDAP","2");
    link("2_YONGDAP","2_DAPSIMNI","2");
    link("2_EULJIRO1","2_EULJIRO3","2"); link("2_EULJIRO3","2_EULJIRO4","2");
    link("2_EULJIRO4","1_SINSEOL","2"); // meeting point
    link("1_CITY_HALL","2_SINCHON","2");
    // Line 3
    link("3_DAEHWA","3_JUYEOP","3"); link("3_JUYEOP","3_JEONGBALSAN","3");
    link("3_JEONGBALSAN","3_MADU","3"); link("3_MADU","3_BAEKSEOK","3");
    link("3_BAEKSEOK","3_DAEGOK","3"); link("3_DAEGOK","3_WHAJEONG","3");
    link("3_WHAJEONG","3_WONHEUNG","3"); link("3_WONHEUNG","3_SAMSONGJUNGANG","3");
    link("3_SAMSONGJUNGANG","3_JICHO","3"); link("3_JICHO","3_GUPABAL","3");
    link("3_GUPABAL","3_YEONSIN","3"); link("3_YEONSIN","3_BULGWANG","3");
    link("3_BULGWANG","3_NOKBEON","3"); link("3_NOKBEON","3_HONGJEWA","3");
    link("3_HONGJEWA","3_MUAKJAE","3"); link("3_MUAKJAE","3_DONGNIMMUN","3");
    link("3_DONGNIMMUN","3_GYEONGBOKGUNG","3"); link("3_GYEONGBOKGUNG","3_ANGUK","3");
    link("3_ANGUK","1_JONGNO3GA","3"); link("1_JONGNO3GA","2_EULJIRO3","3");
    link("2_EULJIRO3","3_CHUNGMURO","3"); link("3_CHUNGMURO","3_DONGGUK","3");
    link("3_DONGGUK","3_YAKSU","3"); link("3_YAKSU","3_GEUMHO","3");
    link("3_GEUMHO","3_OKSU","3"); link("3_OKSU","3_APGUJEONG","3");
    link("3_APGUJEONG","3_SINSA","3"); link("3_SINSA","3_JAMWON","3");
    link("3_JAMWON","3_EXPORO","3"); link("3_EXPORO","3_YUKSAM","3");
    link("3_YUKSAM","2_GANGNAM","3"); link("2_GANGNAM","3_YANGJAE","3");
    link("3_YANGJAE","3_MAEHEON","3"); link("3_MAEHEON","3_YANGJAECIT","3");
    link("3_YANGJAECIT","3_CHEONGGYESAN","3"); link("3_CHEONGGYESAN","3_JEONGJA","3");
    // Line 4
    link("4_DANGGOGAE","4_SANGGYE","4"); link("4_SANGGYE","4_NOWON4","4");
    link("4_NOWON4","4_JUNGGYE","4"); link("4_JUNGGYE","4_HAGYE","4");
    link("4_HAGYE","4_GONGNEUNG","4"); link("4_GONGNEUNG","4_TAENEUNG","4");
    link("4_TAENEUNG","4_HWARANG","4"); link("4_HWARANG","4_SUYU","4");
    link("4_SUYU","4_MIA","4"); link("4_MIA","4_MIASAMG","4");
    link("4_MIASAMG","4_GILEUM","4"); link("4_GILEUM","4_SUNGSHIN","4");
    link("4_SUNGSHIN","4_HANSUNG","4"); link("4_HANSUNG","4_HYEHWA","4");
    link("4_HYEHWA","4_DONGSEO","4"); link("4_DONGSEO","3_CHUNGMURO","4");
    link("3_CHUNGMURO","4_MYEONGDONG","4"); link("4_MYEONGDONG","4_HOEHYEON","4");
    link("4_HOEHYEON","1_SEOUL","4"); link("1_SEOUL","4_SOOKMYUNG","4");
    link("4_SOOKMYUNG","4_SAMGAKJI","4"); link("4_SAMGAKJI","4_SINGIL","4");
    link("4_SINGIL","4_YEONGDEUNGPO4","4"); link("4_YEONGDEUNGPO4","4_GEUMJEONG","4");
    link("4_GEUMJEONG","4_SURI","4"); link("4_SURI","4_DARIM","4");
    link("4_DARIM","4_ANYANGBEOB","4"); link("4_ANYANGBEOB","4_BANYEWOL","4");
    link("4_BANYEWOL","4_CHOJI","4"); link("4_CHOJI","4_ANSAN","4");
    // Line 5
    link("5_BANGHWA","5_GAEHWA","5"); link("5_GAEHWA","5_KIMPO5","5");
    link("5_KIMPO5","5_SONGJUNG","5"); link("5_SONGJUNG","5_MAGOK","5");
    link("5_MAGOK","5_BALSAN","5"); link("5_BALSAN","5_UGJUNGBU","5");
    link("5_UGJUNGBU","5_HWAGOK","5"); link("5_HWAGOK","5_SINJEONG","5");
    link("5_SINJEONG","5_MOKSAM","5"); link("5_MOKSAM","5_OMOKGYO","5");
    link("5_OMOKGYO","5_YANGPYUNG","5"); link("5_YANGPYUNG","5_MAPO","5");
    link("5_MAPO","5_GONGDEOK","5"); link("5_GONGDEOK","5_AEROJUNG","5");
    link("5_AEROJUNG","5_CHUNGJEONG","5"); link("5_CHUNGJEONG","5_SEODAEMUN","5");
    link("5_SEODAEMUN","5_KWANGHWAMUN","5"); link("5_KWANGHWAMUN","5_JONGNO3_5","5");
    link("5_JONGNO3_5","5_DONGDAEMUN5","5"); link("5_DONGDAEMUN5","5_CHEONGGU","5");
    link("5_CHEONGGU","5_HAENGDANG","5"); link("5_HAENGDANG","5_WANGSIMNI5","5");
    link("5_WANGSIMNI5","5_GODEOK","5"); link("5_GODEOK","5_MACHEON","5");
    // Line 9
    link("9_GAEHWA9","9_SINBANGHWA","9"); link("9_SINBANGHWA","9_MAGOK9","9");
    link("9_MAGOK9","9_GIMPO9","9"); link("9_GIMPO9","9_YANGHWA","9");
    link("9_YANGHWA","9_SEONYU","9"); link("9_SEONYU","9_DANGSAN9","9");
    link("9_DANGSAN9","9_GUKHOE","9"); link("9_GUKHOE","9_YEOUIDO9","9");
    link("9_YEOUIDO9","9_SAGANG9","9"); link("9_SAGANG9","9_NORYANGJIN9","9");
    link("9_NORYANGJIN9","9_NODEUL","9"); link("9_NODEUL","9_HEUKSEOK","9");
    link("9_HEUKSEOK","9_DONGJAK9","9"); link("9_DONGJAK9","9_BANPO9","9");
    link("9_BANPO9","9_SINBANPO","9"); link("9_SINBANPO","9_EXPRESS9","9");
    link("9_EXPRESS9","9_SEOCHO9","9"); link("9_SEOCHO9","9_SINNONHYEON","9");
    link("9_SINNONHYEON","9_EONJU","9"); link("9_EONJU","9_SEOLLEUNG9","9");
    link("9_SEOLLEUNG9","9_SAMSEONG9","9"); link("9_SAMSEONG9","9_BONGEUN","9");
    link("9_BONGEUN","9_MONGCHON9","9"); link("9_MONGCHON9","9_SPORTS","9");
    link("9_SPORTS","9_SEOKCHON9","9"); link("9_SEOKCHON9","9_SONGPA9","9");
    link("9_SONGPA9","9_HANCHUN","9"); link("9_HANCHUN","9_DUNCHI","9");
    link("9_DUNCHI","9_MACHEON9","9");
    // Airport line
    link("AP_INCHEON2","AP_INCHEON1","공항",300);
    link("AP_INCHEON1","AP_UNSEO","공항"); link("AP_UNSEO","AP_YONGYU","공항");
    link("AP_YONGYU","AP_YEONGONG","공항"); link("AP_YEONGONG","AP_CHEONGNA","공항");
    link("AP_CHEONGNA","AP_GEOMAM","공항"); link("AP_GEOMAM","AP_GIMPO","공항");
    link("AP_GIMPO","AP_GYEYANG","공항"); link("AP_GYEYANG","AP_MAJEMUL","공항");
    link("AP_MAJEMUL","AP_BUPYUNG","공항"); link("AP_BUPYUNG","AP_HONGIK","공항",180);
    link("AP_HONGIK","AP_GONGDEOK2","공항"); link("AP_GONGDEOK2","AP_DIGITAL","공항");
    link("AP_DIGITAL","AP_SEOULDONGNO","공항",240);
    // 8호선
    link("8_BYEOLLAE","8_AMSA","8",300); link("8_AMSA","8_CHEONHO","8");
    link("8_CHEONHO","8_MONGCHON","8"); link("8_MONGCHON","8_JAMSIL8","8");
    link("8_JAMSIL8","8_SEOKCHON","8"); link("8_SEOKCHON","8_SONGPA","8");
    link("8_SONGPA","8_GARAKSI","8"); link("8_GARAKSI","8_MUNJEONG","8");
    link("8_MUNJEONG","8_JANGJI","8"); link("8_JANGJI","8_BOKJEONG","8");
    link("8_BOKJEONG","8_MORAN","8"); link("8_MORAN","8_HANAM_A","8");
    // 수인분당선
    link("SB_WANGSIMNI_S","SB_SEOLLEUNG_S","수인분당",180);
    link("SB_SEOLLEUNG_S","SB_BOKJEONG_S","수인분당",240);
    link("SB_BOKJEONG_S","SB_MORAN_S","수인분당");
    link("SB_MORAN_S","SB_YATAP","수인분당"); link("SB_YATAP","SB_JEONGJA2","수인분당");
    link("SB_JEONGJA2","SB_MIGEUM","수인분당"); link("SB_MIGEUM","SB_ORI","수인분당");
    link("SB_ORI","SB_BOJEONG","수인분당"); link("SB_BOJEONG","SB_GIHEUNG","수인분당");
    link("SB_GIHEUNG","SB_SUWON_S","수인분당",300);
    link("SB_SUWON_S","SB_ANSAN_S","수인분당",600);
    link("SB_ANSAN_S","SB_SUIN_SO","수인분당",600);
    link("SB_SUIN_SO","SB_INCHEON_S","수인분당",300);
    // 신분당선
    link("3_YANGJAE","3_YANGJAECIT","신분당",180);
    link("3_YANGJAECIT","3_JEONGJA","신분당",240);
}

// ═══════════════════════════════════════════════════════════════
//  MainWindow constructor
// ═══════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("서울 지하철 노선도  Seoul Metro Graph");
    resize(1600, 980);

    buildStationData();
    buildEdgeData();

    // central widget = graphics view
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 1400, 1150);
    m_scene->setBackgroundBrush(QColor(0x1a,0x1a,0x2e));

    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_view->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    m_view->setInteractive(true);
    setCentralWidget(m_view);

    setupDock();
    buildScene();

    // Zoom with wheel
    m_view->installEventFilter(this);

    // Auto-refresh timer (every 30 s)
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(30000);
    connect(m_refreshTimer, &QTimer::timeout, this, &MainWindow::refreshArrivals);
    m_refreshTimer->start();

    // status bar
    statusBar()->showMessage("마우스 휠로 확대/축소  |  역을 클릭하면 도착 정보 표시  |  드래그로 이동");
}

// ─── Event filter (zoom) ──────────────────────────────────────
bool MainWindow::eventFilter(QObject *obj, QEvent *ev) {
    if (obj == m_view && ev->type() == QEvent::Wheel) {
        auto *we = static_cast<QWheelEvent*>(ev);
        double factor = (we->angleDelta().y() > 0) ? 1.15 : 1.0/1.15;
        m_view->scale(factor, factor);
        return true;
    }
    return QMainWindow::eventFilter(obj, ev);
}

// ─── Setup dock ───────────────────────────────────────────────
void MainWindow::setupDock()
{
    m_dock = new QDockWidget("역 정보 / Station Info", this);
    m_dock->setMinimumWidth(320);
    m_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    QWidget *w = new QWidget;
    QVBoxLayout *vl = new QVBoxLayout(w);
    vl->setSpacing(8);

    // Search bar
    QLabel *searchLbl = new QLabel("역 검색:");
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("역 이름 입력...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, [this](){
        onSearchActivated(m_searchEdit->text());
    });

    // Line filter
    QLabel *filterLbl = new QLabel("노선 필터:");
    m_lineFilter = new QComboBox;
    m_lineFilter->addItem("전체", "");
    for (auto &k : LINE_COLORS.keys())
        m_lineFilter->addItem(k + "호선 / " + k, k);
    connect(m_lineFilter, &QComboBox::currentTextChanged, this, &MainWindow::onLineFilterChanged);

    // Station header
    m_stationLbl = new QLabel("역을 클릭하세요");
    m_stationLbl->setStyleSheet("font-size:16px; font-weight:bold; color:#00d4ff;");
    m_stationLbl->setWordWrap(true);

    m_linesLbl = new QLabel("");
    m_linesLbl->setStyleSheet("font-size:12px; color:#aaa;");

    QLabel *arrLbl = new QLabel("실시간 도착 정보:");
    arrLbl->setStyleSheet("font-size:13px; font-weight:bold; color:#fff; margin-top:10px;");

    m_arrivalList = new QListWidget;
    m_arrivalList->setStyleSheet(
        "QListWidget { background:#12122a; color:#eee; border:1px solid #333; font-size:12px; }"
        "QListWidget::item { padding:6px; border-bottom:1px solid #2a2a4a; }"
        "QListWidget::item:selected { background:#1a3a6a; }");

    QPushButton *refreshBtn = new QPushButton("새로고침");
    refreshBtn->setStyleSheet(
        "QPushButton { background:#0055a5; color:white; border-radius:4px; padding:6px; }"
        "QPushButton:hover { background:#0077cc; }");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshArrivals);

    vl->addWidget(searchLbl);
    vl->addWidget(m_searchEdit);
    vl->addWidget(filterLbl);
    vl->addWidget(m_lineFilter);
    vl->addWidget(m_stationLbl);
    vl->addWidget(m_linesLbl);
    vl->addWidget(arrLbl);
    vl->addWidget(m_arrivalList);
    vl->addWidget(refreshBtn);

    m_dock->setWidget(w);
    addDockWidget(Qt::RightDockWidgetArea, m_dock);
}

// ─── Build scene ──────────────────────────────────────────────
void MainWindow::buildScene()
{
    drawEdges();
    drawStations();
    drawLegend();
}

void MainWindow::drawEdges()
{
    for (const auto &e : m_edges) {
        auto itA = m_stations.find(e.fromId);
        auto itB = m_stations.find(e.toId);
        if (itA == m_stations.end() || itB == m_stations.end()) continue;

        QColor c = lineColor(e.lineNo);
        c.setAlpha(180);
        QPen pen(c, 3, Qt::SolidLine, Qt::RoundCap);
        auto *li = m_scene->addLine(itA->x, itA->y, itB->x, itB->y, pen);
        li->setZValue(1);
    }
}

void MainWindow::drawStations()
{
    for (const auto &st : m_stations) {
        auto *item = new StationItem(st);
        connect(item, &StationItem::clicked, this, &MainWindow::onStationClicked);
        m_scene->addItem(item);
        m_items[st.id] = item;

        // label – only for transfer/major stations to avoid clutter
        if (st.lines.size() > 1 || st.nameKo.length() <= 3) {
            QGraphicsTextItem *txt = m_scene->addText(st.nameKo);
            txt->setDefaultTextColor(Qt::white);
            QFont f; f.setPointSize(6); txt->setFont(f);
            txt->setPos(st.x + 8, st.y - 8);
            txt->setZValue(3);
        }
    }
}

void MainWindow::drawLegend()
{
    double lx = 10, ly = 10;
    QGraphicsRectItem *bg = m_scene->addRect(lx-5, ly-5, 200, (int)LINE_COLORS.size()*18+15,
                                              QPen(Qt::NoPen), QBrush(QColor(0,0,0,160)));
    bg->setZValue(10);

    int i=0;
    for (auto it=LINE_COLORS.cbegin(); it!=LINE_COLORS.cend(); ++it, ++i) {
        double y = ly + i*18;
        auto *rect = m_scene->addRect(lx, y, 20, 12, QPen(Qt::NoPen), QBrush(it.value()));
        rect->setZValue(11);
        QGraphicsTextItem *t = m_scene->addText(it.key() + "호선");
        QFont f; f.setPointSize(7); t->setFont(f);
        t->setDefaultTextColor(Qt::white);
        t->setPos(lx+24, y-1);
        t->setZValue(11);
    }
}

// ─── Click handler ────────────────────────────────────────────
void MainWindow::onStationClicked(const QString &stationId)
{
    // De-select previous
    if (!m_selectedStation.isEmpty()) {
        auto it = m_items.find(m_selectedStation);
        if (it != m_items.end()) {
            it.value()->setPen(QPen(Qt::white, 2));
        }
    }
    m_selectedStation = stationId;
    auto it = m_items.find(stationId);
    if (it != m_items.end())
        it.value()->setPen(QPen(Qt::yellow, 3));

    updateInfoPanel(stationId);
}

void MainWindow::updateInfoPanel(const QString &stationId)
{
    auto it = m_stations.find(stationId);
    if (it == m_stations.end()) return;
    const Station &st = it.value();

    m_stationLbl->setText(QString("🚇  %1\n%2").arg(st.nameKo, st.nameEn));

    // Build lines string with coloured HTML
    QString linesHtml;
    for (const QString &ln : st.lines) {
        QColor c = lineColor(ln);
        linesHtml += QString("<span style='background:%1;color:white;padding:2px 6px;"
                             "border-radius:3px;margin-right:4px;font-weight:bold;'>%2</span>")
                         .arg(c.name(), ln);
    }
    m_linesLbl->setText(linesHtml);
    m_linesLbl->setTextFormat(Qt::RichText);

    // Populate arrivals
    m_arrivalList->clear();
    QVector<ArrivalInfo> arrivals = generateArrivals(stationId);
    for (const auto &a : arrivals) {
        QColor c = lineColor(a.lineNo);
        QString icon = (a.minutesLeft == 0) ? "🟢 곧 도착" :
                       (a.minutesLeft <= 1) ? QString("🟡 %1분 후").arg(a.minutesLeft) :
                                              QString("⬜ %1분 후").arg(a.minutesLeft);
        QString text = QString("[%1호선] %2 방면  %3\n열차번호: %4")
                           .arg(a.lineNo, a.destination, icon, a.trainId);

        auto *lwi = new QListWidgetItem(text);
        lwi->setForeground(c.lighter(150));
        m_arrivalList->addItem(lwi);
    }

    statusBar()->showMessage(QString("선택: %1 (%2)  |  환승노선: %3개")
                                 .arg(st.nameKo, st.nameEn)
                                 .arg(st.lines.size()));
}

// ─── Arrival simulation ───────────────────────────────────────
QVector<ArrivalInfo> MainWindow::generateArrivals(const QString &stationId)
{
    auto it = m_stations.find(stationId);
    if (it == m_stations.end()) return {};

    const Station &st = it.value();
    QVector<ArrivalInfo> result;

    // Map line → typical destinations
    static QMap<QString,QStringList> destinations = {
        {"1",{"소요산","인천","광운대","서동탄","신창","연천"}},
        {"2",{"성수","신설동","까치산","용답"}},
        {"3",{"대화","오금","수서","일산"}},
        {"4",{"당고개","오이도","안산","인천"}},
        {"5",{"방화","마천","하남검단산","강동"}},
        {"6",{"응암","봉화산","신내"}},
        {"7",{"장암","석남","인천","부평구청"}},
        {"8",{"암사","별내","모란"}},
        {"9",{"개화","중앙보훈병원","김포공항"}},
        {"공항",{"인천공항2터미널","서울역","김포공항"}},
        {"수인분당",{"인천","수원","왕십리","청량리"}},
        {"신분당",{"강남","광교","신사"}},
        {"경의중앙",{"문산","지평","서울역","가좌"}},
        {"경춘",{"청량리","춘천","상봉"}},
    };

    auto rng = QRandomGenerator::global();
    for (const QString &ln : st.lines) {
        QStringList dests = destinations.value(ln, {"상행","하행"});
        // 2 upcoming trains per line
        for (int i=0; i<2; i++) {
            ArrivalInfo a;
            a.lineNo = ln;
            a.destination = dests[rng->bounded(dests.size())];
            a.minutesLeft = (i==0) ? rng->bounded(6) : rng->bounded(4)+3;
            a.trainId = QString("%1%2").arg(ln).arg(1000 + rng->bounded(999));
            result.push_back(a);
        }
    }
    // sort by time
    std::sort(result.begin(), result.end(),
              [](const ArrivalInfo &a, const ArrivalInfo &b){ return a.minutesLeft < b.minutesLeft; });
    return result;
}

void MainWindow::refreshArrivals()
{
    if (!m_selectedStation.isEmpty())
        updateInfoPanel(m_selectedStation);
}

// ─── Search ───────────────────────────────────────────────────
void MainWindow::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        // restore all
        for (auto *item : m_items) {
            item->setOpacity(1.0);
        }
        return;
    }
    for (auto jt = m_items.begin(); jt != m_items.end(); ++jt) {
        const Station &st = m_stations[jt.key()];
        bool match = st.nameKo.contains(text, Qt::CaseInsensitive) ||
                     st.nameEn.contains(text, Qt::CaseInsensitive);
        jt.value()->setOpacity(match ? 1.0 : 0.2);
    }
}

void MainWindow::onSearchActivated(const QString &text)
{
    if (text.isEmpty()) return;
    for (auto jt = m_stations.cbegin(); jt != m_stations.cend(); ++jt) {
        const Station &st = jt.value();
        if (st.nameKo.contains(text, Qt::CaseInsensitive) ||
            st.nameEn.contains(text, Qt::CaseInsensitive))
        {
            onStationClicked(st.id);
            m_view->centerOn(m_items[st.id]);
            return;
        }
    }
}

void MainWindow::onLineFilterChanged(const QString &text)
{
    QString lineKey = m_lineFilter->currentData().toString();
    for (auto jt = m_items.begin(); jt != m_items.end(); ++jt) {
        const Station &st = m_stations[jt.key()];
        bool show = lineKey.isEmpty() || st.lines.contains(lineKey);
        jt.value()->setOpacity(show ? 1.0 : 0.15);
    }
}

// ─── BFS shortest path (bonus) ────────────────────────────────
QVector<QString> MainWindow::shortestPath(const QString &fromId, const QString &toId)
{
    QMap<QString,QString> prev;
    QQueue<QString> q;
    QSet<QString> visited;
    q.enqueue(fromId);
    visited.insert(fromId);
    while (!q.isEmpty()) {
        QString cur = q.dequeue();
        if (cur == toId) break;
        for (const auto &e : m_edges) {
            QString nb;
            if (e.fromId == cur) nb = e.toId;
            else if (e.toId == cur) nb = e.fromId;
            if (!nb.isEmpty() && !visited.contains(nb)) {
                visited.insert(nb);
                prev[nb] = cur;
                q.enqueue(nb);
            }
        }
    }
    QVector<QString> path;
    QString cur = toId;
    while (!cur.isEmpty()) {
        path.prepend(cur);
        cur = prev.value(cur, "");
        if (cur == fromId) { path.prepend(fromId); break; }
    }
    return path;
}
